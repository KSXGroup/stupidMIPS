#ifndef _MIPS_REGISTER_HPP_
#define _MIPS_REGISTER_HPP_
#include "mips_define.hpp"
#include "mips_global.hpp"
class MIPSRegister{
friend class byteOperator;
friend class MIPSPipeline;
private:
    BYTE **data;
public:
    MIPSRegister(){
        data = new BYTE*[35];
        for(int i = 0; i < 35; ++i){
            data[i] = new BYTE[4];
            for(int j = 0; j < 4; ++j) data[i][j] = 0;
        }
    }

    ~MIPSRegister(){
        for(int i = 0; i < 35; ++i) delete [] data[i];
        data = nullptr;
    }

    inline void setWord(const int32_t &d, const int32_t registerId){
        byteOperator::setWord(data[registerId], 0, d);
    }

    inline void setHalf(const int16_t &d, const int32_t registerId){
        byteOperator::setHalf(data[registerId], 0, d);
        data[registerId][2] = data[registerId][3] = 0;
    }

    inline void setByte(const int8_t &d, const int32_t registerId){
        byteOperator::setByte(data[registerId], 0, d);
        data[registerId][1] = data[registerId][2] = data[registerId][3] = 0;
    }

    inline int32_t getWord(const int32_t &registerId){
        return byteOperator::getWord(data[registerId], 0);
    }

    inline int16_t getHalf(const int32_t &registerId){
        return byteOperator::getHalf(data[registerId], 0);
    }

    inline int8_t getByte(const int32_t &registerId){
        return byteOperator::getByte(data[registerId], 0);
    }

    void dispRegInt(){
        std::cerr << "\n";
        for(int i = 0; i < 35; ++i){
            std::cerr <<"$"<< i << ":" << getWord(i) << "\t";
        }
        std::cerr << "\n";
    }
};

#endif
