#ifndef _MIPS_TEXT_PARSER_HPP_
#define _MIPS_TEXT_PARSER_HPP_
#include "mips_define.h"
using std::string;
class MIPSTextParser{
private:
    char *sourceCode;
    uint32_t codeLength;
public:
    MIPSTextParser(const char *fileName){
        std::ifstream fin(fileName);
        fin.seekg(0, std::ios::end);
        codeLength = fin.tellg();
        sourceCode = new char[codeLength + 1];
        fin.seekg(0, std::ios::beg);
        fin.read(sourceCode, codeLength);
    }

    ~MIPSTextParser(){
        delete[] sourceCode;
    }

    void MIPSTextPreProcess(MIPSMemory &mem){}

    void display(){
        for(int i = 0; i < codeLength; ++i) std::cout << sourceCode[i];
    }
};
#endif
