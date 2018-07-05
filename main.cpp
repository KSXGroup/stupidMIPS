#include "mips_define.hpp"
#include "mips_global.hpp"
#include "mips_text_parser.hpp"
#include "mips_simulator.hpp"
#include "mips_pipeline.hpp"
int main(int argc, char *argv[]){
        MIPSPipeline p(argv[1]);
        p.preProcess();
        p.runPipeline();
        return 0;
}
