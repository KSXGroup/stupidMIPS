#ifndef _MIPS_TEXT_PARSER_HPP_
#define _MIPS_TEXT_PARSER_HPP_
#include "mips_define.hpp"
#include "mips_global.hpp"
#include "mips_memory.hpp"
using std::string;
using std::cerr;

class MIPSTextParser{
friend class MIPSPipeline;
private:
    char *sourceCode = nullptr;
    uint32_t codeLength = 0;
    uint32_t pos = 0;
    uint32_t mainPos = 0;
    uint32_t lineNumber = 0;
    MIPSMapper mapper;
    BYTE status = STATUS_DATA;
    vector<MIPSInstruction*> inst;
    map<string, int32_t> labelToAddress, labelToIndex;
    vector<string> labels;

public:
    MIPSTextParser(const char *fileName){
        //READ SOURCE FILE
        std::ifstream fin(fileName);
        fin.seekg(0, std::ios::end);
        codeLength = fin.tellg();
        sourceCode = new char[codeLength + 1];
        fin.seekg(0, std::ios::beg);
        fin.read(sourceCode, codeLength);
        fin.close();
        sourceCode[codeLength] = '\0';
    }

    ~MIPSTextParser(){
        delete[] sourceCode;
        int sz = inst.size();
        for(int i = 0; i < sz; ++i) delete inst[i];
        inst.clear();
    }
private:
    string getNextLine(){
        if(pos == codeLength) return string();
        string tmp = "";
        while(pos < codeLength && (sourceCode[pos] == ' ' || sourceCode[pos] == '\t')) pos++;
        while(pos < codeLength && sourceCode[pos] != '\n') tmp += sourceCode[pos++];
        if(sourceCode[pos] == '\n') lineNumber++;
        if(pos >= codeLength) return string();
        else pos++;
        if(tmp == "") return getNextLine();
        else return tmp;
    }

    string peekNextLine(uint32_t st = 0){
        if(st == 0) st = pos;
        if(st == codeLength) return string();
        string tmp = "";
        while(st < codeLength && sourceCode[st] == ' ') st++;
        while(st < codeLength && sourceCode[st] != '\n') tmp += sourceCode[st++];
        if(st >= codeLength) return string();
        else st++;
        if(tmp == "") return peekNextLine(st);
        else return tmp;
    }

    inline uint8_t getLabelFromString(const string &s, uint8_t st, string &res){
        res = "";
        while(st < s.length() && (s[st] == ' ' || s[st] == '\t' || s[st] == ',')) ++st;
        while(st < s.length() && !(s[st] == ' ' || s[st] == '\t' || s[st] == ',')) res += s[st++];
        return st;
    }

    inline uint8_t stringSkipForNumberAndRegister(const string &s, uint8_t st){
        while(1){
            if(st >= s.length() || s[st] == '$' || (s[st] <= '9' && s[st] >= '0') ) break;
            st++;
        }
        return st;
    }

    inline uint8_t getNumberFromString(const string &s, uint8_t st, int16_t &res){
        string tmp = "";
        bool ifMinus = 0;
        if(s[st - 1] == '-') ifMinus = 1;
        while(st < s.length() && s[st] >= '0' && s[st] <= '9') tmp += s[st++];
        res = byteConvert::stringToInt16(tmp);
        if(ifMinus) res = -res;
        return st;
    }

    inline uint8_t getRegisterFromString(const string &s, uint8_t st, string &res){
        res = "";
        if(s[st] != '$') return st;
        else{
            res = "";
            res += '$';
            ++st;
            while(st < s.length() && (s[st] != ' ' && s[st] != '\t' && s[st] != '\n' && s[st] != ')' && s[st] != ',')) res += s[st++];
            return st;
        }
    }
public:
    void MIPSTextPreProcess(MIPSMemory &mem){
        std::string tmpLine = "", tmpToken = "";
        pos = 0;
        uint8_t linePos = 0, lineLength = 0;
        bool ifMinus = 0;
        while(pos < codeLength){
            tmpLine = getNextLine();
            linePos = 0;
            lineLength = tmpLine.length();
            while(tmpLine[linePos] == ' ' || tmpLine[linePos] == '\t') linePos++;
            while(linePos < lineLength &&  tmpLine[linePos] != ' ' && tmpLine[linePos] != '\t') tmpToken += tmpLine[linePos++];
            if(status == STATUS_DATA){
                if(mapper.instructionMapper.count(tmpToken)){
                    INSTRUCTION currentInst = mapper.instructionMapper[tmpToken];
                    int32_t alignTmp = 0, powRes = 0, modRes = 0, spaceTmp = 0;
                    vector<int32_t> arg32;
                    vector<int16_t> arg16;
                    vector<int8_t> arg8;
                    string argTmpLine = "";
                    switch(currentInst){
                        case DOTTEXT:
#ifdef STATIC_DATA_DEBUG
                        std::cerr << "[Change To Text Mode From Data Mode]\n";
#endif
                            status = STATUS_TEXT;
                            break;
                        case DOTDATA:
#ifdef STATIC_DATA_DEBUG
                        std::cerr << "[Change To Data Mode From Data Mode]\n";
#endif
                            status = STATUS_DATA;
                            break;
                        case DOTALIGN:
                            while(linePos < lineLength && !(tmpLine[linePos] <= '9' && tmpLine[linePos] >= '0')) linePos++;
                            while(linePos < lineLength && tmpLine[linePos] <= '9' && tmpLine[linePos] >= '0') alignTmp = alignTmp * 10 + tmpLine[linePos++] - '0';
#ifdef STATIC_DATA_DEBUG
                        std::cerr << "[.align]:" << alignTmp << "\n";
#endif
                            powRes = (1 << alignTmp);
                            modRes = mem.staticPosition % powRes;
                            if(modRes != 0) mem.staticPosition += powRes - modRes;
                            break;
                        case DOTWORD:
                            while(linePos < lineLength){
                                int32_t argtmp32 = 0;
                                while(linePos < lineLength && !(tmpLine[linePos] >= '0' && tmpLine[linePos] <= '9')) linePos++;
                                if(tmpLine[linePos - 1] == '-') ifMinus = 1;
                                while(linePos < lineLength && tmpLine[linePos] >= '0' && tmpLine[linePos] <= '9'){
                                    argtmp32 = argtmp32 * 10 + tmpLine[linePos] - '0';
                                    linePos++;
                                }
                                if(ifMinus){
                                    argtmp32 = -argtmp32;
                                    ifMinus = 0;
                                }
                                arg32.push_back(argtmp32);
                            }
                            for(int32_t i = 0; i < arg32.size(); ++i){
#ifdef STATIC_DATA_DEBUG
                        std::cerr << "[.word]:" << arg32[i] << "\n";
#endif
                                mem.insertWordToStatic(arg32[i]);
                            }
                            break;
                        case DOTHALF:
                            while(linePos < lineLength){
                                int16_t argtmp16 = 0;
                                while(linePos < lineLength && !(tmpLine[linePos] >= '0' && tmpLine[linePos] <= '9')) linePos++;
                                if(tmpLine[linePos - 1] == '-') ifMinus = 1;
                                while(linePos < lineLength && tmpLine[linePos] >= '0' && tmpLine[linePos] <= '9'){
                                    argtmp16 = argtmp16 * 10 + tmpLine[linePos] - '0';
                                    linePos++;
                                }
                                if(ifMinus){
                                    argtmp16 = -argtmp16;
                                    ifMinus = 0;
                                }
                                arg16.push_back(argtmp16);
                            }
                            for(int32_t i = 0; i < arg16.size(); ++i){
#ifdef STATIC_DATA_DEBUG
                        std::cerr << "[.half]:" << arg16[i] << "\n";
#endif
                                mem.insertHalfToStatic(arg16[i]);
                            }
                            break;
                        case DOTBYTE:
                            while(linePos < lineLength){
                                int8_t argtmp8 = 0;
                                while(linePos < lineLength && !(tmpLine[linePos] >= '0' && tmpLine[linePos] <= '9')) linePos++;
                                if(tmpLine[linePos - 1] == '-') ifMinus = 1;
                                while(linePos < lineLength && tmpLine[linePos] >= '0' && tmpLine[linePos] <= '9'){
                                    argtmp8 = argtmp8 * 10 + tmpLine[linePos] - '0';
                                    linePos++;
                                }
                                if(ifMinus){
                                    argtmp8 = -argtmp8;
                                    ifMinus = 0;
                                }
                                arg8.push_back(argtmp8);
                            }
                            for(int32_t i = 0; i < arg8.size(); ++i){
#ifdef STATIC_DATA_DEBUG
                        std::cerr << "[.byte]:" << arg8[i] << "\n";
#endif
                                mem.insertByteToStatic(arg8[i]);
                            }
                            break;
                        case DOTASCII:
                            argTmpLine = "";
                            while(linePos < lineLength){
                                while(linePos < lineLength && tmpLine[linePos] != '\"') linePos++;
                                linePos++;
                                while(linePos < lineLength && tmpLine[linePos] != '\"'){
                                    if(tmpLine[linePos] == '\\'){
                                        linePos++;
                                        char ch = tmpLine[linePos];
                                        switch (ch) {
                                        case 'n':
                                            argTmpLine += '\n';
                                            linePos++;
                                            break;
                                        case '\"':
                                            argTmpLine += '\"';
                                            linePos++;
                                        default:
                                            linePos++;
                                            break;
                                        }
                                        //TODO MORE
                                    }
                                    else argTmpLine += tmpLine[linePos++];
                                }
                            }
                            for(int32_t i = 0; i < argTmpLine.length(); ++i){
#ifdef STATIC_DATA_DEBUG
                        std::cerr << "[.ascii]:" << argTmpLine[i] << "\n";
#endif
                                mem.insertByteToStatic(argTmpLine[i]);
                            }
                            break;
                        case DOTASCIIZ:
                            argTmpLine = "";
                            while(linePos < lineLength){
                                while(linePos < lineLength && tmpLine[linePos] != '\"') linePos++;
                                linePos++;
                                while(linePos < lineLength && tmpLine[linePos] != '\"'){
                                    if(tmpLine[linePos] == '\\'){
                                        linePos++;
                                        char ch = tmpLine[linePos];
                                        switch (ch) {
                                        case 'n':
                                            argTmpLine += '\n';
                                            linePos++;
                                            break;
                                        case '\"':
                                            argTmpLine += '\"';
                                            linePos++;
                                            break;
                                        case '0':
                                            argTmpLine += '\0';
                                            linePos++;
                                            break;
                                        case 't':
                                            argTmpLine += '\t';
                                            linePos++;
                                            break;
                                        case '\'':
                                            argTmpLine += '\'';
                                            linePos++;
                                            break;
                                        default:
                                            linePos++;
                                            break;
                                        }
                                        //TODO MORE
                                    }
                                    else argTmpLine += tmpLine[linePos++];
                                }
                            }
                            for(int32_t i = 0; i < argTmpLine.length(); ++i){
#ifdef STATIC_DATA_DEBUG
                        std::cerr << "[.asciiz]:" << argTmpLine[i] << "\n";
#endif
                                mem.insertByteToStatic(argTmpLine[i]);
                            }
#ifdef STATIC_DATA_DEBUG
                        std::cerr << "[.asciiz]:\\0\n";
#endif
                            mem.insertByteToStatic('\0');
                            break;
                        case DOTSPACE:
                            while(linePos < lineLength && !(tmpLine[linePos] <= '9' && tmpLine[linePos] >= '0')) linePos++;
                            while(linePos < lineLength && tmpLine[linePos] <= '9' && tmpLine[linePos] >= '0'){
                                spaceTmp = spaceTmp * 10 + tmpLine[linePos] - '0';
                            }
#ifdef STATIC_DATA_DEBUG
                        std::cerr << "[.space]:" << spaceTmp << "\n";
#endif
                            for(int32_t i = 0; i < spaceTmp; ++i) mem.insertByteToStatic(0);
                            break;
                        default:
                            cerr << "COMPILE ERROR";
                            getchar();
                            exit(0);
                            break;
                    }
                }
                else{
                    string tmpLabel = "";
                    for(uint8_t i = 0; i < tmpToken.length() - 1; ++i) tmpLabel += tmpToken[i];
                    labels.push_back(tmpLabel);
                    labelToIndex[tmpLabel] = labels.size() - 1;
                    labelToAddress[tmpLabel] = mem.staticPosition;
                }
            }
            else if(status == STATUS_TEXT){
                if(mapper.instructionMapper.count(tmpToken)){
                    INSTRUCTION currentInst = mapper.instructionMapper[tmpToken];
                    string Rdest = "", Rsrc1 = "", Src2 = "", label = "";
                    MIPSInstruction *tmpPtr = nullptr;
                    switch(currentInst){
                        case DOTTEXT:
                            status = STATUS_TEXT;
                            break;
                        case DOTDATA:
                            status = STATUS_DATA;
                            break;
                        //Rdest, Rsrc1, Src2 TYPE
                        case ADD:
                        case ADDU:
                        case ADDIU:
                        case SUB:
                        case SUBU:
                        case XOR:
                        case REM:
                        case REMU:
                        case SEQ:
                        case SGE:
                        case SGT:
                        case SLE:
                        case SLT:
                        case SNE:
                            tmpPtr = new MIPSInstruction;
                            tmpPtr->name = currentInst;
                            tmpPtr->argCount = 3;
                            while(linePos < lineLength && tmpLine[linePos] != '$') ++linePos;
                            linePos = getRegisterFromString(tmpLine, linePos, Rdest);
                            tmpPtr->Rdest = mapper.registerMapper[Rdest];
                            while(linePos < lineLength && tmpLine[linePos] != '$') ++linePos;
                            linePos = getRegisterFromString(tmpLine, linePos, Rsrc1);
                            tmpPtr->Rsrc = mapper.registerMapper[Rsrc1];
                            linePos = stringSkipForNumberAndRegister(tmpLine, linePos);
                            if(tmpLine[linePos] == '$'){
                                tmpPtr->srcType = 1;
                                linePos = getRegisterFromString(tmpLine, linePos, Src2);
                                tmpPtr->Src = mapper.registerMapper[Src2];
                            }
                            else{
                                tmpPtr->Src = 0;
                                linePos = getNumberFromString(tmpLine, linePos, tmpPtr->Src);
                            }
#ifdef TEXT_DEBUG
                        tmpPtr->dispName = tmpToken;
                        cerr << "[" << tmpToken << "]:" ;
                        cerr << "Rdest: $" << (int)tmpPtr->Rdest << " ";
                        cerr << "Rsrc1: $" << (int)tmpPtr->Rsrc << " ";
                        cerr << "SrcType:" << ((tmpPtr->srcType == 1) ? "Register: $" : "ImmediateNumber:" );
                        cerr << tmpPtr->Src << "\n";
#endif
                            inst.push_back(tmpPtr);
                            tmpPtr = nullptr;
                            break;

                        //Rdest, Rsrc1, Src2 AND Rdest, Src2 TYPE
                        case MUL:
                        case MULU:
                        case DIV:
                        case DIVU:
                            tmpPtr = new MIPSInstruction;
                            tmpPtr->name = currentInst;
                            linePos = stringSkipForNumberAndRegister(tmpLine, linePos);
                            linePos = getRegisterFromString(tmpLine, linePos, Rdest);
                            tmpPtr->Rdest = mapper.registerMapper[Rdest];
                            linePos = stringSkipForNumberAndRegister(tmpLine, linePos);
                            if(tmpLine[linePos] == '$'){
                                linePos = getRegisterFromString(tmpLine, linePos, Rsrc1);
                                linePos = stringSkipForNumberAndRegister(tmpLine, linePos);
                                if(linePos == lineLength){
                                    tmpPtr->argCount = 2;
                                    tmpPtr->Src = mapper.registerMapper[Rsrc1];
                                }
                                else{
                                    tmpPtr->argCount = 3;
                                    tmpPtr->Rsrc = mapper.registerMapper[Rsrc1];
                                    if(tmpLine[linePos] == '$'){
                                        linePos = getRegisterFromString(tmpLine, linePos, Src2);
                                        tmpPtr->srcType = 1;
                                        tmpPtr->Src = mapper.registerMapper[Src2];
                                    }
                                    else{
                                        tmpPtr->srcType = 0;
                                        linePos = getNumberFromString(tmpLine, linePos, tmpPtr->Src);
                                    }
                                }
                            }
                            else{
                               tmpPtr->srcType = 0;
                               tmpPtr->argCount = 2;
                               linePos = getNumberFromString(tmpLine, linePos, tmpPtr->Src);
                            }
#ifdef TEXT_DEBUG
                            tmpPtr->dispName = tmpToken;
                            cerr << "[" << tmpToken << "]:" ;
                            cerr << "Rdest: $" << (int)tmpPtr->Rdest << " ";
                            if(tmpPtr->argCount == 3) cerr << "Rsrc1: $" << (int)tmpPtr->Rsrc << " ";
                            cerr << "SrcType:" << ((tmpPtr->srcType == 1) ? "Register: $" : "ImmediateNumber:" );
                            cerr << (int)tmpPtr->Src << "\n";
#endif
                            inst.push_back(tmpPtr);
                            tmpPtr = nullptr;
                            break;
                        case NEG:
                        case NEGU:
                        case MOVE:
                            tmpPtr = new MIPSInstruction;
                            tmpPtr->argCount = 2;
                            tmpPtr->name = currentInst;
                            Rdest = Rsrc1 = "";
                            linePos = stringSkipForNumberAndRegister(tmpLine, linePos);
                            linePos = getRegisterFromString(tmpLine, linePos, Rdest);
                            linePos = stringSkipForNumberAndRegister(tmpLine, linePos);
                            linePos = getRegisterFromString(tmpLine, linePos, Rsrc1);
                            tmpPtr->Rdest = mapper.registerMapper[Rdest];
                            tmpPtr->Rsrc = mapper.registerMapper[Rsrc1];
#ifdef TEXT_DEBUG
                            tmpPtr->dispName = tmpToken;
                            cerr << "[" << tmpToken << "]:" ;
                            cerr << "Rdest: $" << (int)tmpPtr->Rdest << " ";
                            cerr << "Rsrc1: $" << (int)tmpPtr->Rsrc << "\n";
#endif
                            inst.push_back(tmpPtr);
                            tmpPtr = nullptr;
                            break;
                        case LI:
                            tmpPtr = new MIPSInstruction;
                            tmpPtr->argCount = 2;
                            tmpPtr->srcType = 0;
                            tmpPtr->name = currentInst;
                            linePos = stringSkipForNumberAndRegister(tmpLine, linePos);
                            linePos = getRegisterFromString(tmpLine, linePos, Rdest);
                            tmpPtr->Rdest = mapper.registerMapper[Rdest];
                            linePos = stringSkipForNumberAndRegister(tmpLine, linePos);
                            linePos = getNumberFromString(tmpLine, linePos, tmpPtr->Src);
#ifdef TEXT_DEBUG
                            tmpPtr->dispName = tmpToken;
                            cerr << "[" << tmpToken << "]:" ;
                            cerr << "Rdest: $" << (int)tmpPtr->Rdest << " ";
                            cerr << " (immediate)Src:" << tmpPtr->Src << "\n";
#endif
                            inst.push_back(tmpPtr);
                            tmpPtr = nullptr;
                            break;
                        case BEQ:
                        case BNE:
                        case BGE:
                        case BLE:
                        case BGT:
                        case BLT:
                            tmpPtr = new MIPSInstruction;
                            tmpPtr->argCount = 3;
                            tmpPtr->name = currentInst;
                            linePos = stringSkipForNumberAndRegister(tmpLine, linePos);
                            linePos = getRegisterFromString(tmpLine, linePos, Rdest);
                            tmpPtr->Rdest = mapper.registerMapper[Rdest];
                            linePos = stringSkipForNumberAndRegister(tmpLine, linePos);
                            if(tmpLine[linePos] == '$'){
                                linePos = getRegisterFromString(tmpLine, linePos, Rsrc1);
                                tmpPtr->srcType = 1;
                                tmpPtr->Src = mapper.registerMapper[Rsrc1];
                            }
                            else{
                                tmpPtr->srcType = 0;
                                linePos = getNumberFromString(tmpLine, linePos, tmpPtr->Src);
                            }
                            linePos = getLabelFromString(tmpLine, linePos, label);
                            if(!labelToIndex.count(label)){
                                labels.push_back(label);
                                labelToIndex[label] = labels.size() - 1;
                                labelToAddress[label] = -1;
                            }
                            tmpPtr->addressedLabel = labelToIndex[label];
#ifdef TEXT_DEBUG
                            tmpPtr->dispName = tmpToken;
                            cerr << "[" << tmpToken << "]:" ;
                            cerr << "Rdest: $" << (int)tmpPtr->Rdest << " ";
                            cerr << "Src: ";
                            if(tmpPtr->srcType) cerr << "Register: $";
                            else cerr << "Immediate:";
                            cerr << tmpPtr->Src << " ";
                            cerr << "Label Address: ";
                            if(tmpPtr->addressedLabel == -1) cerr << "<NOT DECIDED YET>\n";
                            else cerr << tmpPtr->addressedLabel << "\n";
#endif
                            inst.push_back(tmpPtr);
                            tmpPtr = nullptr;
                            break;
                        case BEQZ:
                        case BNEZ:
                        case BLEZ:
                        case BGEZ:
                        case BGTZ:
                        case BLTZ:
                            tmpPtr = new MIPSInstruction;
                            tmpPtr->argCount = 2;
                            tmpPtr->name = currentInst;
                            linePos = stringSkipForNumberAndRegister(tmpLine, linePos);
                            linePos = getRegisterFromString(tmpLine, linePos, Rdest);
                            tmpPtr->Rdest = mapper.registerMapper[Rdest];
                            //while(tmpLine[linePos] != ' ' && tmpLine[linePos] != '\t') ++linePos;
                            linePos = getLabelFromString(tmpLine, linePos, label);
                            if(!labelToIndex.count(label)){
                                labels.push_back(label);
                                labelToIndex[label] = labels.size() - 1;
                                labelToAddress[label] = -1;
                            }
                            tmpPtr->addressedLabel = labelToIndex[label];
#ifdef TEXT_DEBUG
                            tmpPtr->dispName = tmpToken;
                            cerr << "[" << tmpToken << "]:" ;
                            cerr << "Rdest: $" << (int)tmpPtr->Rdest << " ";
                            cerr << "Label Address: ";
                            if(tmpPtr->addressedLabel == -1) cerr << "<NOT DECIDED YET>\n";
                            else cerr << tmpPtr->addressedLabel << "\n";
#endif
                            inst.push_back(tmpPtr);
                            tmpPtr = nullptr;
                            break;
                        case JR:
                        case JALR:
                        case MFHI:
                        case MFLO:
                             tmpPtr = new MIPSInstruction;
                             tmpPtr->name = currentInst;
                             tmpPtr->argCount = 1;
                             linePos = stringSkipForNumberAndRegister(tmpLine, linePos);
                             linePos = getRegisterFromString(tmpLine, linePos, Rsrc1);
                             tmpPtr->Rsrc = mapper.registerMapper[Rsrc1];
#ifdef TEXT_DEBUG
                            tmpPtr->dispName = tmpToken;
                            cerr << "[" << tmpToken << "]:" ;
                            cerr << "Rsrc: $" << (int)tmpPtr->Rsrc << "\n";
#endif
                            inst.push_back(tmpPtr);
                            tmpPtr = nullptr;
                            break;
                        case B:
                        case J:
                        case JAL:
                            tmpPtr = new MIPSInstruction;
                            tmpPtr->name = currentInst;
                            tmpPtr->argCount = 1;
                            while(linePos < lineLength && tmpLine[linePos] != ' ' && tmpLine[linePos] != '\t') ++linePos;
                            linePos = getLabelFromString(tmpLine, linePos, label);
                            if(!labelToIndex.count(label)){
                                labels.push_back(label);
                                labelToIndex[label] = labels.size() - 1;
                                labelToAddress[label] = -1;
                            }
                            tmpPtr->addressedLabel = labelToIndex[label];
#ifdef TEXT_DEBUG
                            tmpPtr->dispName = tmpToken;
                            cerr << "[" << tmpToken << "]:" ;
                            cerr << "Label Address: ";
                            if(tmpPtr->addressedLabel == -1) cerr << "<NOT DECIDED YET>\n";
                            else cerr << tmpPtr->addressedLabel << "\n";
#endif
                            inst.push_back(tmpPtr);
                            tmpPtr = nullptr;
                            break;
                        case LA:
                        case LB:
                        case LH:
                        case LW:
                        case SB:
                        case SH:
                        case SW:
                            tmpPtr = new MIPSInstruction;
                            tmpPtr->name = currentInst;
                            tmpPtr->argCount = 2;
                            linePos = stringSkipForNumberAndRegister(tmpLine, linePos);
                            linePos = getRegisterFromString(tmpLine, linePos, Rsrc1);
                            if(currentInst == SB || currentInst == SH || currentInst == SW) tmpPtr->Rsrc = mapper.registerMapper[Rsrc1];
                            else tmpPtr->Rdest = mapper.registerMapper[Rsrc1];
                            while(linePos < lineLength && tmpLine[linePos] != ',') ++linePos;
                            ++linePos;
                            while(linePos < lineLength && tmpLine[linePos] == ' ') ++linePos;
                            if(tmpLine[linePos] == '-' || (tmpLine[linePos] <= '9' && tmpLine[linePos] >= '0')){
                                linePos++;
                                int16_t num = 0;
                                linePos = getNumberFromString(tmpLine, linePos, num);
                                tmpPtr->offset = num;
                                linePos = stringSkipForNumberAndRegister(tmpLine, linePos);
                                linePos = getRegisterFromString(tmpLine, linePos, Rdest);
                                tmpPtr->argCount = 3;
                                tmpPtr->srcType = 1;
                                tmpPtr->Src = mapper.registerMapper[Rdest];
                            }
                            else{
                                linePos = getLabelFromString(tmpLine, linePos, label);
                                if(!labelToIndex.count(label)){
                                    labels.push_back(label);
                                    labelToIndex[label] = labels.size() - 1;
                                    labelToAddress[label] = -1;
                                }
                                tmpPtr->addressedLabel = labelToIndex[label];
                            }
#ifdef TEXT_DEBUG
                            tmpPtr->dispName = tmpToken;
                            cerr << "[" << tmpToken << "]:" ;
                            cerr << "Rdest: $" << (int)tmpPtr->Rdest << " ";
                            if(tmpPtr->argCount == 2){
                                cerr << "Label Address: ";
                                if(tmpPtr->addressedLabel == -1) cerr << "<NOT DECIDED YET>\n";
                                else cerr << tmpPtr->addressedLabel << "\n";
                            }
                            else{
                                cerr << "Rsrc: $"<< (int)tmpPtr->Rsrc << " ";
                                cerr << "Offset:" << tmpPtr->offset << "\n";
                            }
#endif
                            inst.push_back(tmpPtr);
                            tmpPtr = nullptr;
                            break;
                        case NOP:
                        case SYSCALL:
                            tmpPtr = new MIPSInstruction;
                            tmpPtr->name = currentInst;
                            tmpPtr->argCount = 0;
#ifdef TEXT_DEBUG
                            tmpPtr->dispName = tmpToken;
                            cerr << "[" << tmpToken << "]:\n";
#endif
                            inst.push_back(tmpPtr);
                            tmpPtr = nullptr;
                            break;
                        default:
                            cerr << "COMPILE ERROR\n";
                            getchar();
                            exit(0);
                            break;
                    }
                }
                else{
                    string tmpLabel = "";
                    for(uint8_t i = 0; i < tmpToken.length() - 1; ++i) tmpLabel += tmpToken[i];
                    labelToAddress[tmpLabel] = inst.size();
                    if(labelToIndex.count(tmpLabel) == 0){
                        labels.push_back(tmpLabel);
                        labelToIndex[tmpLabel] = labels.size() - 1;
                    }
#ifdef TEXT_DEBUG
                            cerr << "[[" << tmpLabel << "]]:\n";
#endif
                }
            }
            linePos = 0;
            tmpToken = "";
        }
        for(int i = 0; i < inst.size(); ++i){
            if(inst[i]->addressedLabel != -1){
                inst[i]->addressedLabel = labelToAddress[labels[inst[i]->addressedLabel]];
            }
        }
#ifdef STATIC_DATA_DEBUG
                std::cerr << "[Dump Static Memory From Low To High]\n";
                for(int32_t i = 0; i < mem.staticPosition; i+=4) std::cerr << mem.getWord(i) << ' ';
                std::cerr << "\n";
                std::cerr << "[Dump Data Labels In Code Squence]\n";
                for(auto i = labelToAddress.begin(); i != labelToAddress.end(); ++i) std::cerr << i->first << " : "  << i -> second << "\n";
#endif
        mem.dynamicPosition = mem.staticPosition;
        while(mem.dynamicPosition % 4 != 0) ++mem.dynamicPosition;
        mainPos = labelToAddress["main"];
#ifdef TEXT_DEBUG
        for(int i = 0; i < inst.size(); ++i){
            cerr << inst[i]->dispName << " : " << inst[i]->addressedLabel << "\n";
        }
        cerr << inst.size() << "\n";
#endif
    }

    void display(){
      std::cout << sourceCode;
    }
};



#endif





















