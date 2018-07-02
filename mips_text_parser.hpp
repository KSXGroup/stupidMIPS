#ifndef _MIPS_TEXT_PARSER_HPP_
#define _MIPS_TEXT_PARSER_HPP_
#include "mips_define.hpp"
#include "mips_global.hpp"
#include "mips_memory.hpp"
using std::string;
using std::cerr;

class MIPSTextParser{
private:
    char *sourceCode = nullptr;
    uint32_t codeLength = 0;
    uint32_t pos = 0;
    MIPSMapper mapper;
    BYTE status = STATUS_DATA;
    vector<instructionTemp*> inst;
    map<string, int> labelToAddress, labelToLineNumber;

public:
    MIPSTextParser(const char *fileName, const MIPSMapper &mp){
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

    string getNextLine(){
        if(pos == codeLength) return string();
        string tmp = "";
        while(pos < codeLength && sourceCode[pos] == ' ') pos++;
        while(pos < codeLength && sourceCode[pos] != '\n') tmp += sourceCode[pos++];
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

    void MIPSTextPreProcess(MIPSMemory &mem){
        std::string tmpLine = "", tmpToken = "";
        pos = 0;
        uint32_t linePos = 0, lineLength = 0;
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
                else labelToAddress[tmpToken] = mem.staticPosition;
            }
            else if(status == STATUS_TEXT){
                if(mapper.instructionMapper.count(tmpToken)){
                    INSTRUCTION currentInst = mapper.instructionMapper[tmpToken];
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
                        case SNE:
                        //--TODO--
                        //Rdes, Rsrc1, Src2 AND Rdest, Src2 TYPE
                        case MUL:
                        case MULU:
                        case DIV:
                        case DIVU:
                            //TODO
                        case NEG:
                        case NEGU:
                            //TODO
                        case LI:
                        default:
                            break;
                    }
                }
                else{
                }
            }
            linePos = 0;
            tmpToken = "";
        }
#ifdef STATIC_DATA_DEBUG
                std::cerr << "[Dump Static Memory From Low To High]\n";
                for(int32_t i = 0; i < mem.staticPosition; i+=4) std::cerr << mem.getWord(i) << ' ';
                std::cerr << "\n";
                std::cerr << "[Dump Data Labels In Code Squence]\n";
                for(auto i = labelToAddress.begin(); i != labelToAddress.end(); ++i) std::cerr << i->first << " : "  << i -> second << "\n";
#endif
        mem.dynamicPosition = mem.staticPosition;
    }


    void display(){
      std::cout << sourceCode;
    }
};
#endif





















