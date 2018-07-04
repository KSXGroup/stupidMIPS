#ifndef _MIPS_PIPLINED_INSTRUCTION_HPP_
#define _MIPS_PIPLINED_INSTRUCTION_HPP_
#include "mips_define.hpp"
#include "mips_global.hpp"
#include "mips_text_parser.hpp"

class MIPSPipeline{
private:
    MIPSTextParser *parser = nullptr;
public:
    MIPSPipeline(const char* fileName){
        parser =  new MIPSTextParser(fileName);
        //parser->display();
    }

    void processSourceCode(MIPSMemory &mem){
        parser->MIPSTextPreProcess(mem);
        std::cerr << parser->codeLength << "\n";
    }

};
#endif
