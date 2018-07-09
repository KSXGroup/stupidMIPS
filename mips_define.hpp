#ifndef _MIPS_DEFINE_H_
#define _MIPS_DEFINE_H_
//#define PIPELINE_DEBUG
//#define LINE_PAUSE
//#define LOOP_PAUSE
//#define PIPELINE_PAUSE
//#define STATIC_DATA_DEBUG
//#define TEXT_DEBUG
//#define PARSED_PROGRAM_DEBUG
//#define REG_DEBUG
//#define PREDICTION_DEBUG
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <vector>
using std::string;
using std::vector;
using std::map;
typedef unsigned char BYTE;
typedef unsigned int OFFSETTYPE;
const unsigned int MAX_MEMORY_SIZE = 1024 * 1024 * 4;
const unsigned int BRANCH_PREDICTOR_CACHE = 12000;
const unsigned char PREDICT_BYTE = 8;
const BYTE TYPE_R = 0;
const BYTE TYPE_L = 1;
const BYTE TYPE_J = 2;
const BYTE STATUS_DATA = 3;
const BYTE STATUS_TEXT = 4;

enum INSTRUCTION{
    ADD = 0, ADDU, ADDIU, SUB, SUBU, XOR, XORU, NEG, NEGU, REM, REMU ,MUL, MULU, DIV, DIVU,
    LI,
    SEQ, SGE, SGT, SLE, SLT, SNE,BEQ, BNE, BGE,BLE, BGT, BLT, BEQZ, BNEZ, BLEZ, BGEZ, BGTZ, BLTZ, B,
    J, JR, JAL, JALR,
    LA, LB, LH, LW,
    SB, SH, SW,
    MOVE, MFHI, MFLO,
    NOP,
    SYSCALL,
    DOTALIGN,
    DOTASCII, DOTASCIIZ, DOTBYTE, DOTHALF, DOTWORD,
    DOTSPACE,
    DOTDATA, DOTTEXT, DOTLABEL
};

#endif
