#ifndef _MIPS_TEXT_PARSER_HPP_
#define _MIPS_TEXT_PARSER_HPP_
#include "mips_define.h"
using std::string;
class MIPSTextParser{
private:
    char *sourceCode;
    uint32_t codeLength;
    const MIPSMapper *mapper = nullptr;
    BYTE status;
    vector<instructionTemp*> inst;
    vector<int> idxToAddress;
    map<string, int> labelToIdx;

public:
    MIPSTextParser(const char *fileName, const MIPSMapper &map){
        //READ SOURCE FILE
        std::ifstream fin(fileName);
        fin.seekg(0, std::ios::end);
        codeLength = fin.tellg();
        sourceCode = new char[codeLength + 1];
        fin.seekg(0, std::ios::beg);
        fin.read(sourceCode, codeLength);
        sourceCode[codeLength] = '/0';
        mapper = &map;
    }

    ~MIPSTextParser(){
        delete[] sourceCode;
        int sz = inst.size();
        for(int i = 0; i < sz; ++i) delete inst[i];
        inst.clear();
    }

    void MIPSTextToInstructionTemp(){
        int pos = 0;
        tmpInstPtr = new instructionTemp;
        while(pos != codeLength){
            if(sourceCode[i] == ''){
                pos++;
                continue;
            }
        }
    }

    void MIPSTextPreProcess(MIPSMemory &mem){
    }

    void display(){
        for(int i = 0; i < codeLength; ++i) std::cout << sourceCode[i];
    }
};
#endif
