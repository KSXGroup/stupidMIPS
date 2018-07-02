#include "mips_define.hpp"
#include "mips_global.hpp"
#include "mips_text_parser.hpp"
#include "mips_simulator.hpp"
int main(){
        MIPSMapper mp;
        MIPSMemory mem;
        MIPSTextParser p("MipsTest/testsuit-1/spill2-5100379110-daibo.s", mp);
        p.MIPSTextPreProcess(mem);
}
