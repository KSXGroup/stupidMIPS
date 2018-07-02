#ifndef _MIPS_REGISTER_HPP_
#define _MIPS_REGISTER_HPP_
#include "mips_define.hpp"

class MIPSRegister{
friend class byteOperator;
private:
    BYTE *data;
public:
    MIPSRegister(){
        data = new BYTE[35];
        for(int i = 0; i < 34; ++i) data[i] = 0;
    }

    ~MIPSRegister(){
        delete[] data;
        data = nullptr;
    }


};

#endif
