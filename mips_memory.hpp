#ifndef _MIPS_MEMORY_HPP_
#define _MIPS_MEMORY_HPP_;
#include "mips_define.h"
class MIPSMemory{
friend class byteOperator;
private:
    BYTE *data;
    OFFSETTYPE position = MAX_MEMORY_SIZE;
public:
    MIPSMemory(){
        data = new BYTE[MAX_MEMORY_SIZE];
        for(int i = 0; i < MAX_MEMORY_SIZE; ++i) BYTE[i] = 0;
    }

    ~MIPSMemory(){
        delete data;
        data = nullptr;
    }
};

#endif
