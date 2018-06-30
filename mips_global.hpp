#ifndef _MIPS_GLOBAL_HPP_
#define _MIPS_GLOBAL_HPP_
#include "mips_define.h"
class byteOperator{
public:
    static inline int32_t getWord(const BYTE target[], const OFFSETTYPE &pos){
        return ((int32_t)(target[pos + 3] << 24) & 0xff000000) | ((target[pos + 2] << 16) & 0xff0000) | ((target[pos + 1] << 8) & 0xff00) | (target[pos] & 0xff);
    }
    static inline int16_t getHalf(const BYTE target[], const OFFSETTYPE &pos){
        return ((int16_t)(target[pos] << 8) & 0xff00) | (target[pos + 1] & 0xff);
    }
    static inline int8_t getByte(const BYTE target[], const OFFSETTYPE &pos){
        return (int8_t)(target[pos] & 0xff);
    }
    static inline void setWord(BYTE target[], const OFFSETTYPE &pos,const int32_t &p){
        int32_t tmp = p;
        target[pos] = (BYTE)(tmp & 0xff);
        target[pos + 1] = (BYTE)((tmp & 0xff00) >> 8);
        target[pos + 2] = (BYTE)((tmp & 0xff0000) >> 16);
        target[pos + 3] = (BYTE)((tmp & 0xff000000) >> 24);
    }
    static inline void setHalf(BYTE target[], const OFFSETTYPE &pos,const int16_t &p){
        int16_t tmp = p;
        target[pos] = (BYTE)(tmp & 0xff);
        target[pos + 1] = (BYTE)((tmp & 0xff00) >> 8);
    }
    static inline void setByte(BYTE target[], const OFFSETTYPE &pos,const int8_t &p){
        int8_t tmp = p;
        target[pos] = (BYTE)(tmp & 0xff);
    }
};

#endif
