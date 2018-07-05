#include "mips_define.hpp"
#include "mips_global.hpp"
#include "mips_text_parser.hpp"
#include "mips_simulator.hpp"
#include "mips_pipeline.hpp"
int main(int argc, char *argv[]){
        MIPSPipeline p("MipsTest/testsuit-1/basicopt1-5100309127-hetianxing.s");
        p.preProcess();
        p.runPipeline();
}
