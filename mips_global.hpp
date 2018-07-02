#ifndef _MIPS_GLOBAL_HPP_
#define _MIPS_GLOBAL_HPP_
#include "mips_define.hpp"

struct instructionTemp{
    INSTRUCTION name;
    int rs, rt, rd, shamt, funct, immediate, addressIdx;
};

class MIPSMapper{
public:
    map<string, char> registerMapper, instructionMapper;
    MIPSMapper(){
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
        registerMapper["$pc"] = 34; //PROGRAM COUNTER

        //INIT INSTRUCTION MAPPER
        instructionMapper["add"] = ADD;
        instructionMapper["addu"] =ADDU;
        instructionMapper["addiu"] = ADDIU;
        instructionMapper["sub"] = SUB;
        instructionMapper["subu"] =SUBU;
        instructionMapper["mul"] = MUL;
        instructionMapper["mulu"] = MULU;
        instructionMapper["div"] = DIV;
        instructionMapper["divu"] = DIVU;
        instructionMapper["xor"] = XOR;
        instructionMapper["xoru"] = XORU;
        instructionMapper["neg"] = NEG;
        instructionMapper["negu"] = NEGU;
        instructionMapper["rem"] = REM;
        instructionMapper["remu"] = REMU;
        instructionMapper["li"] = LI;
        instructionMapper["seq"] = SEQ;
        instructionMapper["sge"] = SGE;
        instructionMapper["sle"] = SLE;
        instructionMapper["slt"] = SLT;
        instructionMapper["sne"] = SNE;
        instructionMapper["b"] = B;
        instructionMapper["beq"] = BEQ;
        instructionMapper["bne"] = BNE;
        instructionMapper["bge"] = BGE;
        instructionMapper["ble"] = BLE;
        instructionMapper["bgt"] = BGT;
        instructionMapper["blt"] = BLT;
        instructionMapper["beqz"] = BEQZ;
        instructionMapper["bnez"] = BNEZ;
        instructionMapper["blez"] = BLEZ;
        instructionMapper["bgtz"] = BGTZ;
        instructionMapper["bltz"] = BLTZ;
        instructionMapper["j"] = J;
        instructionMapper["jr"] = JR;
        instructionMapper["jal"] = JAL;
        instructionMapper["jalr"] = JALR;
        instructionMapper["sb"] = SB;
        instructionMapper["sh"] = SH;
        instructionMapper["sw"] = SW;
        instructionMapper["move"] = MOVE;
        instructionMapper["mfhi"] = MFHI;
        instructionMapper["mflo"] = MFLO;
        instructionMapper["nop"] = NOP;
        instructionMapper["syscall"] = SYSCALL;
        instructionMapper[".ascii"] = DOTASCII;
        instructionMapper[".asciiz"] = DOTASCIIZ;
        instructionMapper[".byte"] = DOTBYTE;
        instructionMapper[".half"] = DOTHALF;
        instructionMapper[".word"] = DOTWORD;
        instructionMapper[".space"] = DOTSPACE;
        instructionMapper[".data"] = DOTDATA;
        instructionMapper[".text"] = DOTTEXT;
    }
};

class byteOperator{
public:
    static inline int32_t getWord(const BYTE target[], const OFFSETTYPE &pos){
        return ((int32_t)(target[pos + 3] << 24) & 0xff000000) | ((target[pos + 2] << 16) & 0xff0000) | ((target[pos + 1] << 8) & 0xff00) | (target[pos] & 0xff);
    }
    static inline int16_t getHalf(const BYTE target[], const OFFSETTYPE &pos){
        return ((int16_t)(target[pos + 1] << 8) & 0xff00) | (target[pos] & 0xff);
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
