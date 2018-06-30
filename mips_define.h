#ifndef _MIPS_DEFINE_H_
#define _MIPS_DEFINE_H_
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include "mips_memory.hpp"
#include "mips_register.hpp"
#include "mips_text_parser.hpp"
#include "mips_global.hpp"

const int MAX_MEMORY_SIZE = 1024 * 1024 * 4;
using std::string;
typedef char BYTE;
typedef unsigned int OFFSETTYPE;

enum OPCODE{
    NONE = -1, ADD, ADDU, ADDIU, SUB, SUBU, MUL, MULU, DIV, DIVU, XOR, XORU, NEG, NEGU, REM, REMU,
    LI,
    SEQ, SGE, SGT, SLE, SLT, SNE, B, BEQ, BNE, BGE,BLE, BGT, BLT, BEQZ, BNEZ, BLEZ, BGTZ, BLTZ,
    J, JR, JAL,JALR,
    LA, LB, LH, LW,
    SB, SH, SW,
    MOVE, MFHI, MFLO,
    NOP,
    SYSCALL
};

class MIPSMemory;
class MIPSSimulator;
class MIPSTextParser;
class byteOperator;

#endif
