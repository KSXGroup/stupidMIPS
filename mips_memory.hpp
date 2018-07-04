#ifndef _MIPS_MEMORY_HPP_
#define _MIPS_MEMORY_HPP_
#include "mips_define.hpp"
#include "mips_text_parser.hpp"
class MIPSMemory{
friend class byteOperator;
friend class MIPSTextParser;
private:
    BYTE *data;
    OFFSETTYPE staticPosition = 0;
    OFFSETTYPE dynamicPosition = 0;
    OFFSETTYPE stackPosition = 4 * 1024 * 1024 - 1;
public:

    MIPSMemory(){
        data = new BYTE[MAX_MEMORY_SIZE];
        for(int i = 0; i < MAX_MEMORY_SIZE; ++i) data[i] = 0;
    }

    ~MIPSMemory(){
        delete[] data;
        data = nullptr;
    }

    inline void insertWordToStatic(const int32_t &d){
        byteOperator::setWord(data, staticPosition, d);
        staticPosition += 4;
    }

    inline void insertHalfToStatic(const int16_t &d){
        byteOperator::setHalf(data, staticPosition, d);
        staticPosition += 2;
    }

    inline void insertByteToStatic(const int8_t &d){
        byteOperator::setByte(data, staticPosition, d);
        staticPosition += 1;
    }

    inline void insertWordToHeap(const int32_t &d){
        byteOperator::setWord(data, dynamicPosition, d);
        dynamicPosition += 4;
    }

    inline void insertHalfToHeap(const int16_t &d){
        byteOperator::setHalf(data, dynamicPosition, d);
        dynamicPosition += 2;
    }

    inline void insertByteToHeap(const int8_t &d){
        byteOperator::setByte(data, dynamicPosition, d);
        dynamicPosition += 1;
    }

    inline void insertWordToStack(const int32_t &d){
        stackPosition -= 4;
        byteOperator::setByte(data, stackPosition, d);
    }

    inline void insertHalfToStack(const int16_t &d){
        stackPosition -= 2;
        byteOperator::setHalf(data, stackPosition, d);
    }

    inline void insertBYTE(const int8_t &d){
        stackPosition -= 1;
        byteOperator::setByte(data, stackPosition, d);
    }

    inline uint32_t getWordUnsigned(const OFFSETTYPE &pos){
        return byteOperator::getWordUnsigned(data, pos);
    }

    inline uint16_t getHalfUnsigned(const OFFSETTYPE &pos){
        return byteOperator::getHalfUnsigned(data, pos);
    }

    inline uint8_t getByteUnsigned(const OFFSETTYPE &pos){
        return byteOperator::getByteUnsigned(data, pos);
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

    inline void setByte(const OFFSETTYPE &pos, const int8_t &d){
        byteOperator::setByte(data, pos, d);
    }
};

#endif
