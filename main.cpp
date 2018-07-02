#include "mips_define.hpp"
#include "mips_global.hpp"
#include "mips_text_parser.hpp"
#include "mips_simulator.hpp"
int main(){
        MIPSMapper mp;
        MIPSTextParser p("MipsTest/testsuit-1/array_test1-mahaojun.s", mp);
        char ipt;
        while(1){
            std::cin >> ipt;
            if(ipt == 'g') std::cout << p.getNextLine() << "\n";
            else if(ipt == 'p') std::cout << p.peekNextLine() << "\n";
        }
        p.display();
}
