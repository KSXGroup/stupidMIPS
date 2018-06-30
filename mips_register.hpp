#ifndef _MIPS_REGISTER_HPP_
#define _MIPS_REGISTER_HPP_
#include "mips_define.h"
using std::map;
using std::string;

class MIPSRegister{
friend class byteOperator;
private:
    BYTE *data;
    map<string, int> registerMapper;
public:
    MIPSRegister(){
        data = new BYTE[34];
        for(int i = 0; i < 34; ++i) data[i] = 0;
        registerMapper["$0"] = registerMapper["$zero"] = 0;
        registerMapper["$1"] = registerMapper["$at"] = 1;
        registerMapper["$2"] = registerMapper["$v0"] = 2;
        registerMapper["$3"] = registerMapper["$v1"] = 3;
        registerMapper["$4"] = registerMapper["$a0"] = 4;
        registerMapper["$5"] = registerMapper["$a1"] = 5;
        registerMapper["$6"] = registerMapper["$a2"] = 6;
        registerMapper["$7"] = registerMapper["$a3"] = 7;
        registerMapper["$8"] = registerMapper["$t0"] = 8;
        registerMapper["$9"] = registerMapper["$t1"] = 9;
        registerMapper["$10"] = registerMapper["$t2"] = 10;
        registerMapper["$11"] = registerMapper["$t3"] = 11;
        registerMapper["$12"] = registerMapper["$t4"] = 12;
        registerMapper["$13"] = registerMapper["$t5"] = 13;
        registerMapper["$14"] = registerMapper["$t6"] = 14;
        registerMapper["$15"] = registerMapper["$t7"] = 15;
        registerMapper["$16"] = registerMapper["$s0"] = 16;
        registerMapper["$17"] = registerMapper["$s1"] = 17;
        registerMapper["$18"] = registerMapper["$s2"] = 18;
        registerMapper["$19"] = registerMapper["$s3"] = 19;
        registerMapper["$20"] = registerMapper["$s4"] = 20;
        registerMapper["$21"] = registerMapper["$s5"] = 21;
        registerMapper["$22"] = registerMapper["$s6"] = 22;
        registerMapper["$23"] = registerMapper["$s7"] = 23;
        registerMapper["$24"] = registerMapper["$t8"] = 24;
        registerMapper["$25"] = registerMapper["$t9"] = 25;
        registerMapper["$26"] = registerMapper["$k0"] = 26;
        registerMapper["$27"] = registerMapper["$k1"] = 27;
        registerMapper["$28"] = registerMapper["$gp"] = 28;
        registerMapper["$29"] = registerMapper["$sp"] = 29;
        registerMapper["$30"] = registerMapper["$fp"] = 30;
        registerMapper["$31"] = registerMapper["$ra"] = 31;
        registerMapper["$lo"] = 32;
        registerMapper["$hi"] = 33;
    }

    ~MIPSRegister(){
        delete[] data;
        data = nullptr;
    }

};

#endif
