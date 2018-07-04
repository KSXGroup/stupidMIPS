#ifndef _MIPS_PIPLINED_INSTRUCTION_HPP_
#define _MIPS_PIPLINED_INSTRUCTION_HPP_
#include "mips_define.hpp"
#include "mips_global.hpp"
#include "mips_text_parser.hpp"
#include "mips_register.hpp"
#include "mips_memory.hpp"

class MIPSPipeline{
private:
    MIPSTextParser *parser = nullptr;
    MIPSRegister *reg = nullptr;
    MIPSMemory *mem = nullptr;
    BYTE exit = 0;
    int32_t rtv = 0;
    //PIPELINE REGISTERS
    //refer to 'Computer Architecture (A quantitive approach)'
    struct{
        MIPSInstruction *ins;
        uint32_t NPC = 0;
    }IFID;

    struct{
        MIPSInstruction *ins;
        uint32_t NPC = 0;
        int32_t dataRs = 0;
        int32_t dataRt = 0;
    }IDEX;

    struct{
        MIPSInstruction *ins;
        uint32_t NPC = 0;
        int64_t aluOutput = 0;
        uint8_t cond = 0;
        int32_t dataRs = 0;
        int32_t dataRt = 0;
    }EXMEM;

    struct{
        MIPSInstruction *ins;
        uint32_t NPC = 0;
        int64_t aluOutput = 0;
        int32_t lmd = 0;
        int32_t dataRs = 0;
        int32_t dataRt = 0;
        uint32_t sysA1 = 0;
        uint32_t sysV0 = 0;
    }MEMWB;


public:
    MIPSPipeline(const char* fileName){
        parser =  new MIPSTextParser(fileName);
        reg = new MIPSRegister;
        mem = new MIPSMemory;
        //parser->display();
    }

    ~MIPSPipeline(){        //TO CHECK BRANCH
        delete parser;
        delete reg;
        delete mem;
        parser = nullptr;
        reg = nullptr;
        mem = nullptr;
    }

    void preProcess(){
        parser->MIPSTextPreProcess(*mem);
        reg->setWord((uint32_t)parser->mainPos, 34);
        reg->setWord((uint32_t)(4 * 1024 * 1024 - 1), parser->mapper.registerMapper["$sp"]);
        reg->setWord((uint32_t)(0), parser->mapper.registerMapper["$fp"]);
    }

private:

    void IF(){
        //TODO CHECK BRANCH
        uint32_t tmp = 0;
        uint32_t InstPos = (uint32_t)(reg->getWord(34));
        IFID.ins = parser->inst[InstPos];
        IFID.NPC = InstPos + 1;
        switch (EXMEM.ins->name){
            case B: case BEQ: case BNE: case BGE: case BLE: case BGT: case BLT: case BEQZ: case BNEZ: case BLEZ: case BGEZ: case BGTZ:
            case BLTZ: case J: case JR: case JAL: case JALR:
                if(EXMEM.cond == 1){
                    reg->setWord((uint32_t)EXMEM.aluOutput, 34);
                }
                else{
                    reg->setWord(InstPos + 1,34);
                }
                break;
            default:
                reg->setWord(InstPos + 1,34);
                break;
        }
    }

    void ID(){
        IDEX.ins = IFID.ins;
        IDEX.dataRs = reg->getWord(IFID.ins->Rsrc);
        if(IFID.ins->srcType == 1) IDEX.dataRt = reg->getWord(IFID.ins->Src);
        IDEX.NPC = IFID.NPC;
        if(IFID.ins == SYSCALL){
            IDEX.dataRs = reg->getWord(2);
            IDEX.dataRt = reg->getWord(4);
        }
    }

    void EX(){
        int32_t tmpLo, tmpHi;
        uint32_t tmpA, tmpB, quotient, remainder;
        EXMEM.ins = IDEX.ins;
        EXMEM.dataRs = IDEX.dataRs;
        EXMEM.dataRt = IDEX.dataRt;
        EXMEM.NPC = IDEX.NPC;
        INSTRUCTION currentInst = IFID.ins->name;
        switch(currentInst){
            case ADD:
            case ADDU:
            case ADDIU:
                if(IFID.ins->srcType == 0) EXMEM.aluOutput = IDEX.dataRs + IDEX.ins->Src;
                else EXMEM.aluOutput = IDEX.dataRs + IDEX.dataRt;
                break;
            case SUB:
            case SUBU:
                if(IDEX.ins->srcType == 0) EXMEM.aluOutput = IDEX.dataRs - IDEX.ins->Src;
                else EXMEM.aluOutput = IDEX.dataRs - IDEX.dataRt;
                break;
            case MUL:
                if(IDEX.ins->srcType == 0){
                    EXMEM.aluOutput = IDEX.dataRs * IDEX.ins->Src;
                }
                else{
                    EXMEM.aluOutput = IDEX.dataRs * IDEX.dataRt;
                }
                break;
            case DIV:
                if(IDEX.ins->argCount == 2){
                    if(IDEX.ins->srcType == 0){
                        tmpLo = IDEX.dataRs / IDEX.ins->Src;
                        tmpHi = IDEX.dataRs % IDEX.ins->Src;
                    }
                    else{
                        tmpLo = IDEX.dataRs / IDEX.dataRt;
                        tmpHi = IDEX.dataRs % IDEX.dataRt;
                    }
                    EXMEM.aluOutput = ((((int64_t)tmpHi) << 32) | (tmpLow));
                }
                else{
                    if(IFID.ins->srcType == 0){
                       EXMEM.aluOutput = IDEX.dataRs / IDEX.ins->Src;
                    }
                    else{
                       EXMEM.aluOutput = IDEX.dataRs / IDEX.dataRt;
                    }
                }
                break;
            case MULU:
                if(IDEX.ins->srcType == 0){
                    tmpA = IDEX.dataRs;
                    tmpB = IDEX.ins->Src;
                    EXMEM.aluOutput = (uint32_t)(tmpA * tmpB);
                }
                else{
                    tmpA = IDEX.dataRs;
                    tmpB = IDEX.dataRt;
                    EXMEM.aluOutput = (uint32_t)(tmpA * tmpB);
                }
                break;
            case DIVU:
                if(IDEX.ins->argCount == 2){
                    if(IDEX.ins->srcType == 0){
                        tmpA = IDEX.dataRs;
                        tmpB = IDEX.ins->Src;
                        quotient = tmpA / tmpB;
                        remainder = tmpA % tmpB;
                        EXMEM.aluOutput =  ((((uint64_t)remainder) << 32) | (quotient));
                    }
                    else{
                        tmpA = IDEX.dataRs;
                        tmpB = IDEX.dataRt;
                        quotient = tmpA / tmpB;
                        remainder = tmpA % tmpB;
                        EXMEM.aluOutput =  ((((uint64_t)remainder) << 32) | (quotient));
                    }
                }
                else{
                    if(IDEX.ins->srcType == 0){
                        tmpA = IDEX.dataRs;
                        tmpB = IDEX.ins->Src;
                        EXMEM.aluOutput = (uint32_t)(tmpA / tmpB);
                    }
                    else{
                        tmpA = IDEX.dataRs;
                        tmpB = IDEX.dataRt;
                        EXMEM.aluOutput = (uint32_t)(tmpA / tmpB);
                    }
                }
                break;
            case NEG:
                EXMEM.aluOutput = -IDEX.dataRs;
                break;
            case NEGU:
                EXMEM.aluOutput = ~((uint32_t)(IDEX.dataRs));
                break;
            case XOR:
            case XORU:
                if(IDEX.ins->srcType == 0){
                    EXMEM.aluOutput = (IDEX.dataRs ^ IDEX.ins->Src);
                }
                else{
                    EXMEM.aluOutput = (IDEX.dataRs ^ IDEX.dataRt);
                }
                break;
            case REM:
            case REMU:
                if(IDEX.ins->srcType == 0){
                    EXMEM.aluOutput = (IDEX.dataRs % IDEX.ins->Src);
                }
                else{
                    EXMEM.aluOutput = (IDEX.dataRs % IDEX.dataRt);
                }
                break;
            case SEQ:
                if(IDEX.ins->srcType == 0) EXMEM.aluOutput = (IDEX.dataRs == IDEX.ins->Src);
                else EXMEM.aluOutput = (IDEX.dataRs == IDEX.dataRt);
                break;
            case SGE:
                if(IDEX.ins->srcType == 0) EXMEM.aluOutput = (IDEX.dataRs >= IDEX.ins->Src);
                else EXMEM.aluOutput = (IDEX.dataRs >= IDEX.dataRt);
                break;
            case SGT:
                if(IDEX.ins->srcType == 0) EXMEM.aluOutput = (IDEX.dataRs > IDEX.ins->Src);
                else EXMEM.aluOutput = (IDEX.dataRs > IDEX.dataRt);
                break;
            case SLE:
                if(IDEX.ins->srcType == 0) EXMEM.aluOutput = (IDEX.dataRs <= IDEX.ins->Src);
                else EXMEM.aluOutput = (IDEX.dataRs <= IDEX.dataRt);
                break;
            case SLT:
                if(IDEX.ins->srcType == 0) EXMEM.aluOutput = (IDEX.dataRs < IDEX.ins->Src);
                else EXMEM.aluOutput = (IDEX.dataRs < IDEX.dataRt);
                break;
            case SNE:
                if(IDEX.ins->srcType == 0) EXMEM.aluOutput = (IDEX.dataRs != IDEX.ins->Src);
                else EXMEM.aluOutput = (IDEX.dataRs != IDEX.dataRt);
                break;
            case BEQ:
                if(IDEX.ins->srcType == 0) EXMEM.cond = (IDEX.dataRs == IDEX.ins->Src);
                else EXMEM.cond = (IDEX.dataRs == IDEX.dataRt);
                EXMEM.aluOutput = IDEX.ins->addressedLabel;
                break;
            case BNE:
                if(IDEX.ins->srcType == 0) EXMEM.cond = (IDEX.dataRs != IDEX.ins->Src);
                else EXMEM.cond = (IDEX.dataRs != IDEX.dataRt);
                EXMEM.aluOutput = IDEX.ins->addressedLabel;
                break;
            case BGE:
                if(IDEX.ins->srcType == 0) EXMEM.cond = (IDEX.dataRs >= IDEX.ins->Src);
                else EXMEM.cond = (IDEX.dataRs >= IDEX.dataRt);
                EXMEM.aluOutput = IDEX.ins->addressedLabel;
                break;
            case BLE:
                if(IDEX.ins->srcType == 0) EXMEM.cond = (IDEX.dataRs <= IDEX.ins->Src);
                else EXMEM.cond = (IDEX.dataRs <= IDEX.dataRt);
                EXMEM.aluOutput = IDEX.ins->addressedLabel;
                break;
            case BGT:
                if(IDEX.ins->srcType == 0) EXMEM.cond = (IDEX.dataRs > IDEX.ins->Src);
                else EXMEM.cond = (IDEX.dataRs > IDEX.dataRt);
                EXMEM.aluOutput = IDEX.ins->addressedLabel;
                break;
// 0 for immediate number, 1 for register
            case BLT:
                if(IDEX.ins->srcType == 0) EXMEM.cond = (IDEX.dataRs < IDEX.ins->Src);
                else EXMEM.cond = (IDEX.dataRs < IDEX.dataRt);
                EXMEM.aluOutput = IDEX.ins->addressedLabel;
                break;
            case BEQZ:
                EXMEM.cond = (IDEX.dataRs == 0);
                EXMEM.aluOutput = IDEX.ins->addressedLabel;
                break;
            case BNEZ:
                EXMEM.cond = (IDEX.dataRs != 0);
                EXMEM.aluOutput = IDEX.ins->addressedLabel;
                break;
            case BLEZ:
                EXMEM.cond = (IDEX.dataRs <= 0);
                EXMEM.aluOutput = IDEX.ins->addressedLabel;
                break;
            case BGEZ:
                EXMEM.cond = (IDEX.dataRs >= 0);
                EXMEM.aluOutput = IDEX.ins->addressedLabel;
                break;
            case BGTZ:
                EXMEM.cond = (IDEX.dataRs > 0);
                EXMEM.aluOutput = IDEX.ins->addressedLabel;
                break;
            case BLTZ:
                EXMEM.cond = (IDEX.dataRs < 0);
                EXMEM.aluOutput = IDEX.ins->addressedLabel;
                break;
            case J:
            case B:
            case JAL:
                EXMEM.aluOutput = IDEX.ins->addressedLabel;
                EXMEM.cond = 1;
                if(EXMEM.aluOutput == -1){
#ifdef PIPELINE_DEBUG
                    cerr << "\n\nB FUCK!\n\n";
#endif
                    exit(0);
                }
                break;
            case JR:
            case JALR:
                EXMEM.aluOutput = IDEX.ins->addressedLabel;
                EXMEM.cond = 1;
                break;
            case LA:
            case LB:
            case LH:
            case LW:
            case SB:
            case SH:
            case SW:
                if(IDEX.ins->addressedLabel == -1){
                    EXMEM.aluOutput = IDEX.dataRs + IDEX.ins->offset;
                }
                else{
                    EXMEM.aluOutput = IDEX.ins->addressedLabel;
                }
                break;
            case SYSCALL:
                switch(IDEX.dataRs){
                   case 1:
                        cout << IDEX.dataRt;
                        break;

                   case 5:
                        cin >> EXMEM.aluOutput;
                        break;
                    case 10:
                        exit = 1;
                        break;
                    case 17:
                        exit = 1;
                        rtv = IDEX.dataRt; // rt = $a0
                        break;
                    default:
                        break;
                }

             default:
                    break;
            }
    }

    void MA(){
        string str = "";
        uint32_t pos = 0;
        MEMWB.ins = EXMEM.ins;
        MEMWB.aluOutput = EXMEM.aluOutput;
        MEMWB.dataRs = EXMEM.dataRs;
        MEMWB.dataRt = EXMEM.dataRt;
        MEMWB.NPC = EXMEM.NPC;
        INSTRUCTION currentInst = EXMEM.ins->name;
        switch (currentInst){
            case LB:
                MEMWB.lmd =  mem->getBYTE(EXMEM.aluOutput);
                break;
            case LH:
                MEMWB.lmd = mem->getHalf(EXMEM.aluOutput);
                break;
            case LW:
                MEMWB.lmd = mem->getWord(EXMEM.aluOutput);
                break;
            case SB:
                mem->setByte(EXMEM.aluOutput, (int8_t)EXMEM.dataRs);
                break;
            case SH:
                mem->setHalf(EXMEM.aluOutput, (int16_t)EXMEM.dataRs);
                break;
            case SW:
                mem->setWord(EXMEM.aluOutput, (int32_t)EXMEM.dataRs);
                break;
            case SYSCALL:
                switch(EXMEM.dataRs){
                    case 4:
                        str = "";
                        pos = EXMEM.aluOutput;
                        while(mem->getByte(pos) != '\0'){
                            str += (char)(mem->getByte(pos));
                            ++pos;
                        }
                        cout << str;
                        break;
                    case 8:
                        cin >> str;
                        pos = EXMEM.aluOutput;
                        for(int32_t i = 0; i < str.length(); ++i) mem->setByte(pos++, (int8_t)str[i]);
                        mem->setByte(pos, (int8_t)'\0');
                        MEMWB.sysA1 = str.length() + 1;
                        break;
                    case 9:
                        while(mem->dynamicPosition % 4 != 0) ++mem->dynamicPosition;
                        MEMWB.sysV0 = mem->dynamicPosition;
                        mem->dynamicPosition += EXMEM.dataRt;
                        break;
                    default:
                        break;
                }
        }
    }

    void WB(){
        uint32_t tmpHi = 0, tmpLo = 0;
        INSTRUCTION currentInst = MEMWB.ins->name;
        switch(currentInst){
            case ADD:
            case ADDU:
            case ADDIU:
            case SUB:
            case SUBU:
            case XOR:
            case XORU:
            case NEGU:
            case NEG:
            case REM:
            case REMU:
            case SEQ:
            case SGE:
            case SGT:
            case SLE:
            case SNE:
                reg->setWord(MEMWB.aluOutput,MEMWB.ins->Rdest);
                break;
            case MUL:
            case MULU:
            case DIV:
            case DIVU:
                if(MEMWB.ins->argCount == 2){
                    tmpLo = MEMWB.aluOutput & ((int64_t)(0xffff));
                    tmpHi = (MEMWB.aluOutput >> 32);
                    reg->setWord(tmpHi, 33);
                    reg->setWord(tmpLo, 32);
                }
                else{
                    reg->setWord(MEMWB.aluOutput, MEMWB.ins->Rdest);
                }
                break;
            case LA:
                reg->setWord(MEMWB.aluOutput, MEMWB.ins->Rdest);
                break;
            case LB:
                reg->setByte(MEMWB.lmd, MEMWB.ins->Rdest);
                break;
            case LH:
                reg->setHalf(MEMWB.lmd, MEMWB.ins->Rdest);
                break;
            case LW:
                reg->setWord(MEMWB.lmd, MEMWB.ins->Rdest);
                break;
            case MOVE:
                reg->setWord(MEMWB.dataRs, MEMWB.ins->Rdest);
                break;
                //HI 33 LO 32
            case MFHI:
                reg->setWord(reg->getWord(33), MEMWB.ins->Rdest);
                break;
            case MFLO:
                reg->setWord(reg->getWord(32), MEMWB.ins->Rdest);
                break;
            case JAL:
            case JALR:
                reg->setWord(MEMWB.NPC, 31);
                break;
            case SYSCALL:

            default:
                break;
        }
    }

};
#endif














































