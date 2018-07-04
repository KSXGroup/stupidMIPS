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
        int32_t sysA1 = 0;
    }IDEX;

    struct{
        MIPSInstruction *ins;
        int64_t aluOutput = 0;
        int32_t lmd = 0;
        uint8_t cond = 0;
        int32_t dataRs = 0;
        int32_t dataRt = 0;
        int32_t sysA1 = 0;
    }EXMEM;

    struct{
        MIPSInstruction *ins;
        int64_t aluOutput = 0;
        int32_t lmd = 0;
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
        reg->setWord((uint32_t)parser->mainPos, parser->mapper.registerMapper["$pc"]);
        reg->setWord((uint32_t)(4 * 1024 * 1024 - 1), parser->mapper.registerMapper["$sp"]);
        reg->setWord((uint32_t)(0), parser->mapper.registerMapper["$fp"]);
    }

private:

    void IF(){
        //TODO CHECK BRANCH
        uint32_t InstPos = (uint32_t)(reg->getWord(34));
        IFID.ins = parser->inst[InstPos];
        reg->setWord(InstPos + 1, 34);
        IFID.NPC = InstPos + 1;
    }

    void ID(){
        IDEX.ins = IFID.ins;
        IDEX.dataRs = reg->getWord(IFID.ins->Rsrc);
        if(IFID.ins->srcType == 1) IDEX.dataRt = reg->getWord(IFID.ins->Src);
        IDEX.NPC = IFID.NPC;
        if(IFID.ins == SYSCALL){
            IDEX.dataRs = reg->getWord(parser->mapper.registerMapper["$v0"]);
            IDEX.dataRt = reg->getWord(parser->mapper.registerMapper["$a0"]);
            IDEX.sysA1 = reg->getWord(parser->mapper.registerMapper["$a1"]); //FOR SYSCALL PARAMATER IN $a1
        }
    }

    void EX(){
        string ipt = "";
        EXMEM.ins = IDEX.ins;
        EXMEM.dataRs = IDEX.dataRs;
        EXMEM.dataRt = IDEX.dataRt;
        EXMEM.sysA1 = IDEX.sysA1;
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
                        int32_t tmpLo = IDEX.dataRs / IDEX.ins->Src;
                        int32_t tmpHi = IDEX.dataRs % IDEX.ins->Src;
                    }
                    else{
                        int32_t tmpLo = IDEX.dataRs / IDEX.dataRt;
                        int32_t tmpHi = IDEX.dataRs % IDEX.dataRt;
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
                    uint32_t tmpA = IDEX.dataRs;
                    uint32_t tmpB = IDEX.ins->Src;
                    EXMEM.aluOutput = (uint32_t)(tmpA * tmpB);
                }
                else{
                    uint32_t tmpA = IDEX.dataRs;
                    uint32_t tmpB = IDEX.dataRt;
                    EXMEM.aluOutput = (uint32_t)(tmpA * tmpB);
                }
                break;
            case DIVU:
                if(IDEX.ins->argCount == 2){
                    if(IDEX.ins->srcType == 0){
                        uint32_t tmpA = IDEX.dataRs;
                        uint32_t tmpB = IDEX.ins->Src;
                        uint32_t quotient = tmpA / tmpB;
                        uint32_t remainder = tmpA % tmpB;
                        EXMEM.aluOutput =  ((((uint64_t)remainder) << 32) | (quotient));
                    }
                    else{
                        uint32_t tmpA = IDEX.dataRs;
                        uint32_t tmpB = IDEX.dataRt;
                        uint32_t quotient = tmpA / tmpB;
                        uint32_t remainder = tmpA % tmpB;
                        EXMEM.aluOutput =  ((((uint64_t)remainder) << 32) | (quotient));
                    }
                }
                else{
                    if(IDEX.ins->srcType == 0){
                        uint32_t tmpA = IDEX.dataRs;
                        uint32_t tmpB = IDEX.ins->Src;
                        EXMEM.aluOutput = (uint32_t)(tmpA / tmpB);
                    }
                    else{
                        uint32_t tmpA = IDEX.dataRs;
                        uint32_t tmpB = IDEX.dataRt;
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
                break;
            case BNE:
                if(IDEX.ins->srcType == 0) EXMEM.cond = (IDEX.dataRs != IDEX.ins->Src);
                else EXMEM.cond = (IDEX.dataRs != IDEX.dataRt);
                break;
            case BGE:
                if(IDEX.ins->srcType == 0) EXMEM.cond = (IDEX.dataRs >= IDEX.ins->Src);
                else EXMEM.cond = (IDEX.dataRs >= IDEX.dataRt);
                break;
            case BLE:
                if(IDEX.ins->srcType == 0) EXMEM.cond = (IDEX.dataRs <= IDEX.ins->Src);
                else EXMEM.cond = (IDEX.dataRs <= IDEX.dataRt);
                break;
            case BGT:
                if(IDEX.ins->srcType == 0) EXMEM.cond = (IDEX.dataRs > IDEX.ins->Src);
                else EXMEM.cond = (IDEX.dataRs > IDEX.dataRt);
                break;
// 0 for immediate number, 1 for register
            case BLT:
                if(IDEX.ins->srcType == 0) EXMEM.cond = (IDEX.dataRs < IDEX.ins->Src);
                else EXMEM.cond = (IDEX.dataRs < IDEX.dataRt);
                break;
            case BEQZ:
                EXMEM.cond = (IDEX.dataRs == 0);
                break;
            case BNEZ:
                EXMEM.cond = (IDEX.dataRs != 0);
                break;
            case BLEZ:
                EXMEM.cond = (IDEX.dataRs <= 0);
                break;
            case BGEZ:
                EXMEM.cond = (IDEX.dataRs >= 0);
                break;
            case BGTZ:
                EXMEM.cond = (IDEX.dataRs > 0);
                break;
            case BLTZ:
                EXMEM.cond = (IDEX.dataRs < 0);
                break;
            case J:
            case B:
            case JAL:
            case JALR:
                EXMEM.aluOutput = IDEX.ins->addressedLabel;
                if(EXMEM.aluOutput == -1){
#ifdef PIPELINE_DEBUG
                    cerr << "\n\nB FUCK!\n\n";
#endif
                    exit(0);
                }
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
#ifdef PIPELINE_DEBUG
                        cerr << "\n\nSYSCALL ERROR\n\n";
                        exit(0);
#endif
                }

                default:
#ifdef PIPELINE_DEBUG
                    cerr << "\n\nRUN TIME ERROR!\n\n";
#endif
                    exit(0);
                    break;
            }
    }

    void MA(){
        string str = "";
        uint32_t pos = 0;
        MEMWB.ins = EXMEM.ins;
        MEMWB.aluOutput = EXMEM.aluOutput;
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
                        ipt = "";

                    case 8:
                    case 9:
                }
        }
    }

    void WB(){}

};
#endif














































