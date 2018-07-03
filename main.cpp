#include "mips_define.hpp"
#include "mips_global.hpp"
#include "mips_text_parser.hpp"
#include "mips_simulator.hpp"
int main(){
        MIPSMapper mp;
        MIPSMemory mem;
        MIPSTextParser p("MipsTest/testsuit-1/array_test1-mahaojun.s", mp);
        p.MIPSTextPreProcess(mem);
}
