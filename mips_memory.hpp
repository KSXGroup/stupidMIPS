#ifndef _MIPS_MEMORY_HPP_
#define _MIPS_MEMORY_HPP_
#include "mips_define.hpp"
class MIPSMemory{
friend class byteOperator;
private:
    BYTE *data;
    OFFSETTYPE staticPosition = 0;
    OFFSETTYPE dynamicPosition = 0;
    OFFSETTYPE textPosition = 0;
    OFFSETTYPE stackPosition = 0;
public:
    MIPSMemory(){
        data = new BYTE[MAX_MEMORY_SIZE];
        for(int i = 0; i < MAX_MEMORY_SIZE; ++i) data[i] = 0;
    }

    ~MIPSMemory(){
        delete data;
        data = nullptr;
    }

    /*void insertWord(const int32_t &d){
        byteOperator::setByte(data, position - 4, d);
        position -= 4;
    }
    inline void insertHalf(const int16_t &d){
        byteOperator::setHalf(data, position - 2, d);
        position -= 2;
    }
    inline void insertBYTE(const int8_t &d){
        byteOperator::setByte(data, position - 1, d);
        position -= 1;
    }
    inline int32_t getWord(const OFFSETTYPE &pos){
        return byteOperator::getWord(data, pos);
    }
    inline int16_t getHalf(const OFFSETTYPE &pos){
        return byteOperator::getHalf(data, pos);
    }
    inline int8_t getByte(const OFFSETTYPE &pos){
        return byteOperator::getByte(data, pos);
    }
    inline void setWord(const OFFSETTYPE &pos, const int32_t &d){
        byteOperator::setByte(data, pos, d);
    }
    inline void setHalf(const OFFSETTYPE &pos, const int16_t &d){
        byteOperator::setHalf(data, pos, d);
    }
    inline void setBYTE(const OFFSETTYPE &pos, const int8_t &d){
        byteOperator::setByte(data, pos, d);
    }*/
};

#endif
