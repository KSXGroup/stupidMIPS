#ifndef _MIPS_GLOBAL_HPP_
#define _MIPS_GLOBAL_HPP_
#include "mips_define.h"
class byteOperator{
public:
    static int32_t getWord(const BYTE target[], const OFFSETTYPE &pos){}
    static int16_t getHalf(const BYTE target[], const OFFSETTYPE &pos){}
    static int8_t getByte(const BYTE target[], const OFFSETTYPE &pos){}
    static void setWord(BYTE target[], const OFFSETTYPE &pos,const int32_t &p){}
    static void setHalf(BYTE target[], const OFFSETTYPE &pos,const int32_t &p){}
    static void setByte(BYTE target[], const OFFSETTYPE &pos,const int32_t &p){}
};

#endif
