#ifndef _MIPS_TEXT_PARSER_HPP_
#define _MIPS_TEXT_PARSER_HPP_
#include "mips_define.hpp"
#include "mips_global.hpp"
#include "mips_memory.hpp"
using std::string;
class MIPSTextParser{
private:
    char *sourceCode = nullptr;
    uint32_t codeLength = 0;
    uint32_t pos = 0;
    const MIPSMapper *mapper = nullptr;
    BYTE status = STATUS_DATA;
    vector<instructionTemp*> inst;
    vector<int> idxToAddress;
    map<string, int> labelToIdx;

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
        mapper = &mp;
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
        string tmp = "";        std::cerr << codeLength;
        while(st < codeLength && sourceCode[st] == ' ') st++;
        while(st < codeLength && sourceCode[st] != '\n') tmp += sourceCode[st++];
        if(st >= codeLength) return string();
        else st++;
        if(tmp == "") return peekNextLine(st);
        else return tmp;
    }

    void MIPSTextPreProcess(MIPSMemory &mem){
        string tmpLine = "", tmpToken = "", currentLabel = "";
        pos = 0;
        uint32_t linePos = 0, lineLength = 0;
        while(pos < codeLength){
            tmpLine = getNextLine();
            linePos = 0;
            lineLength = tmpLine.length();
            while(tmpLine[linePos] == ' ' || tmpLine[linePos] == '\t') linePos++;
            while(linePos < lineLength &&  tmpLine[linePos] != ' ' && tmpLine[linePos] != '\t') tmpToken += tmpLine[linePos++];
            if(status == STATUS_DATA){
                if(mapper->instructionMapper.count(tmpToken)){
                    INSTRUCTION currentInst = mapper->instructionMapper[tmpToken];
                    switch(currentInst){
                        case DOTWORD:
                        case DOTHALF:
                        case DOTBYTE:
                        case DOTASCII:
                        case DOTASCIIZ:
                        case DOTSPACE:
                        default:
                            cerr << "COMPILE ERROR";
                            exit(0);
                    }
                }
                else{
                    //NEW LABEL
                }
            }
            if(status == STATUS_TEXT){}
            linePos = 0;
            tmpToken = "";
        }
    }

    void display(){
      std::cout << sourceCode;
    }
};
#endif





















