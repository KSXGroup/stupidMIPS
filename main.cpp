#include "mips_define.hpp"
#include "mips_global.hpp"
#include "mips_text_parser.hpp"
#include "mips_simulator.hpp"
#include "mips_pipeline.hpp"
int main(int argc, char *argv[]){
        std::ios::sync_with_stdio(0);
        std::cin.tie(0);
        std::cout.tie(0);
        MIPSSimulator smult(argv[1]);
        smult.run();
        return 0;
}
