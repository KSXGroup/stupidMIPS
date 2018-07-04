#include "mips_define.hpp"
#include "mips_global.hpp"
#include "mips_text_parser.hpp"
#include "mips_simulator.hpp"
#include "mips_pipeline.hpp"
int main(){
        MIPSPipeline p("MipsTest/testsuit-1/class_test-mahaojun.s");
        p.preProcess();
}
