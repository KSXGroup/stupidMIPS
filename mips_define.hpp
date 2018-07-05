#ifndef _MIPS_DEFINE_H_
#define _MIPS_DEFINE_H_
//#define PIPELINE_DEBUG
//#define LINE_PAUSE
//#define PIPELINE_PAUSE
//#define STATIC_DATA_DEBUG
//#define TEXT_DEBUG
//#define PARSED_PROGRAM_DEBUG
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
const BYTE TYPE_R = 0;
const BYTE TYPE_L = 1;
const BYTE TYPE_J = 2;
const BYTE STATUS_DATA = 3;
const BYTE STATUS_TEXT = 4;

enum INSTRUCTION{
    ADD = 0, ADDU, ADDIU, SUB, SUBU, MUL, MULU, DIV, DIVU, XOR, XORU, NEG, NEGU, REM, REMU,
    LI,
    SEQ, SGE, SGT, SLE, SLT, SNE, B, BEQ, BNE, BGE,BLE, BGT, BLT, BEQZ, BNEZ, BLEZ, BGEZ, BGTZ, BLTZ,
    J, JR, JAL,JALR,
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
