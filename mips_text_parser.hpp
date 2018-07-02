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
    BYTE status = 0;
    vector<instructionTemp*> inst;
    vector<int> idxToAddress;
    map<string, int> labelToIdx;

public:
    MIPSTextParser(const char *fileName, const MIPSMapper &map){
        //READ SOURCE FILE
        std::ifstream fin(fileName);
        fin.seekg(0, std::ios::end);
        codeLength = fin.tellg();
        std::cerr << codeLength;
        sourceCode = new char[codeLength + 1];
        fin.seekg(0, std::ios::beg);
        fin.read(sourceCode, codeLength);
        fin.close();
        sourceCode[codeLength] = '\0';
        mapper = &map;
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

    void MIPSTextToInstructionTemp(){
    }

    void MIPSTextPreProcess(MIPSMemory &mem){
    }

    void display(){
      std::cout << sourceCode;
    }
};
#endif
