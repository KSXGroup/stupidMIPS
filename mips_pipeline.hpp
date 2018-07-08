#ifndef _MIPS_PIPLINED_INSTRUCTION_HPP_
#define _MIPS_PIPLINED_INSTRUCTION_HPP_
#include "mips_define.hpp"
#include "mips_global.hpp"
#include "mips_text_parser.hpp"
#include "mips_register.hpp"
#include "mips_memory.hpp"
using std::cin;
using std::cout;
using std::cerr;
class MIPSPipeline{
private:
    MIPSTextParser *parser = nullptr;
    MIPSRegister *reg = nullptr;
    MIPSMemory *mem = nullptr;
    //PIPELINE REGISTERS
    //refer to 'Computer Architecture (A Quantitive Approach)'
    BYTE PC_STALL = 0;
    BYTE DT_STALL = 0;
    struct{
        MIPSInstruction *ins = nullptr;
        uint32_t NPC = 0;
        BYTE STALL = 0;
    }IFID;

    struct{
        MIPSInstruction *ins = nullptr;
        uint32_t NPC = 0;
        int32_t dataRs = 0;
        int32_t dataRt = 0;
        int32_t sysA1 = 0;
        BYTE STALL = 0;
    }IDEX;

    struct{
        MIPSInstruction *ins = nullptr;
        uint32_t NPC = 0;
        int64_t aluOutput = 0;
        uint8_t cond = 0;
        int32_t dataRs = 0;
        int32_t dataRt = 0;
        int32_t sysA1 = 0;
        BYTE STALL = 0;
    }EXMEM;

    struct{
        MIPSInstruction *ins = nullptr;
        uint32_t NPC = 0;
        int64_t aluOutput = 0;
        int32_t lmd = 0;
        int32_t dataRs = 0;
        int32_t dataRt = 0;
        uint32_t sysV0 = 0;
        BYTE STALL = 0;
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
        reg->setWord((uint32_t)(4 * 1024 * 1024), parser->mapper.registerMapper["$sp"]);
        reg->setWord((uint32_t)(0), parser->mapper.registerMapper["$fp"]);
        reg->setWord(uint32_t(0), 0);
    }

    void runPipeline(){
        while(1){
            WB();MA();EX();ID();IF();
#ifdef LOOP_PAUSE
          cerr << "\n\n";
          cerr << "PC_STALL: " << (int)PC_STALL<< "\t" << "DT_STALL: " << (int)DT_STALL << "\n";
          getchar();
#endif
            //IF();ID();EX();MA();WB();
        }
    }
private:

    void IF(){
        //TODO CHECK BRANCH
#ifdef PIPELINE_DEBUG
        cerr <<"IF START" << "\n";
#endif
        if(DT_STALL){
            DT_STALL = 0;
            return;
        }
        if(PC_STALL){
            IFID.STALL = 1;
            return;
        }
        else IFID.STALL = 0;
        uint32_t InstPos = reg->getWord(34);
        reg->setWord(InstPos + 1, 34);
        IFID.ins = parser->inst[InstPos];
        IFID.STALL = 0;
        IFID.NPC = InstPos + 1;

#ifdef PIPELINE_DEBUG
        cerr <<"Line"<<parser->inst[InstPos]->lineNumer<<": "<< parser->inst[InstPos]->dispName<< " IF\n";
        reg->dispRegInt();
#ifdef PIPELINE_PAUSE
        getchar();
#endif
#endif

    }

    void ID(){
#ifdef PIPELINE_DEBUG
        cerr << "ID START" << "\n";
#endif
        if(IFID.STALL == 1){
            IDEX.STALL = 1;
            return;
        }
        else IDEX.STALL = 0;
        if(IFID.ins == nullptr) return;
#ifdef TEXT_DEBUG
        //if(IFID.ins->lineNumer == 428){
            //int a = 0;
            //reg->dispRegInt();
            //for(int i = 0; i < 30; ++i) {cerr <<"[DBG] ";cerr << mem->getByte(reg->getWord(30) - 4 + i) << " ";}
        //}
#endif
        INSTRUCTION currentInst = IFID.ins->name;
        //TODO LOCK REGISTER
        /*if(currentInst == MFHI && reg->locker[33]){
            DT_STALL = 1;
        }
        else if(currentInst == MFLO && reg->locker[32]){
            DT_STALL = 1;
        }
        else if((currentInst >= ADD  && currentInst <= REMU) || (currentInst >= BEQZ && currentInst <= BLTZ) || currentInst == JR || currentInst == JALR || currentInst == MOVE){
            if(reg->locker[IFID.ins->Rsrc]){
                DT_STALL = 1;
            }
            if(IFID.ins->srcType == 1 && reg->locker[IFID.ins->Src]){
                DT_STALL = 1;
            }
        }
        else if((currentInst >= MUL && currentInst <= DIVU) || (currentInst >= SEQ && currentInst <= BLT)){
            if(IFID.ins->argCount == 3){
                if(reg->locker[IFID.ins->Rsrc]){
                    DT_STALL = 1;
                }
                if(IFID.ins->srcType && reg->locker[IFID.ins->Src]){
                    DT_STALL = 1;
                }
            }
            else{
                if(reg->locker[IFID.ins->Rsrc] == 1) DT_STALL = 1;
                if(!DT_STALL && IFID.ins->srcType == 1 && reg->locker[IFID.ins->Src]){
                    DT_STALL = 1;
                }
                if(!DT_STALL &&  currentInst >= MUL && currentInst <= DIVU){
                    reg->locker[32]++;
                    reg->locker[33]++;
                }
            }
        }
        else if(currentInst >= LA && currentInst <= LW){
            if(IFID.ins->srcType && reg->locker[IFID.ins->Src]){
                DT_STALL = 1;
            }
        }
        else if(currentInst >= SB && currentInst <= SW){
            if(reg->locker[IFID.ins->Rsrc]){
                DT_STALL = 1;
            }
            if(IFID.ins->srcType && reg->locker[IFID.ins->Src]){
                DT_STALL = 1;
            }
        }
        else if(currentInst == SYSCALL){
            if(reg->locker[2]){
                DT_STALL = 1;
            }
            else{
                int s = reg->getWord(2);
                switch(s){
                    case 5:
                        reg->locker[2]++;
                        break;
                    case 9:
                        if(reg->locker[4]){
                            DT_STALL = 1;
                        }
                        else reg->locker[2]++;
                        break;
                    case 8:
                        if(reg->locker[4] || reg->locker[5]){
                            DT_STALL = 1;
                        }
                        break;
                    default:break;
                }
            }
        }*/
        if(IFID.ins->Rsrc != 0 && reg->locker[IFID.ins->Rsrc] > 0) DT_STALL = 1;
        if(IFID.ins->srcType == 1 && IFID.ins->Src != 0 && reg->locker[IFID.ins->Src] > 0) DT_STALL = 1;
        if(currentInst == SYSCALL){
            if(reg->locker[2]){
                DT_STALL = 1;
            }
            else{
                int s = reg->getWord(2);
                switch(s){
                    case 1:
                    case 4:
                        if(reg->locker[4]) DT_STALL = 1;
                        break;
                    case 5:
                        reg->locker[2]++;
                        break;
                    case 9:
                        if(reg->locker[4]){
                            DT_STALL = 1;
                        }
                        else reg->locker[2]++;
                        break;
                    case 8:
                        if(reg->locker[4] || reg->locker[5]){
                            DT_STALL = 1;
                        }
                        break;
                    default:break;
                }
            }
        }
        if(DT_STALL){
            IDEX.STALL = 1;
            return;
        }
        else IDEX.STALL = 0;
        IDEX.NPC = IFID.NPC;
        IDEX.ins = IFID.ins;
        IDEX.dataRs = reg->getWord(IFID.ins->Rsrc);
        if(IFID.ins->srcType == 1) IDEX.dataRt = reg->getWord(IFID.ins->Src);
        if(currentInst == SYSCALL){
            IDEX.dataRs = reg->getWord(2);
            IDEX.dataRt = reg->getWord(4);
            if(IDEX.dataRs == 8) IDEX.sysA1 = reg->getWord(5);
        }
        if(IFID.ins->Rdest != 0) reg->locker[IFID.ins->Rdest]++;
        if(currentInst == JAL || currentInst == JALR) reg->locker[31]++;
        if(!DT_STALL && currentInst >= MUL && currentInst <= DIVU && IFID.ins->argCount == 2){
            reg->locker[32]++;
            reg->locker[33]++;
        }
        if(currentInst >= BEQ && currentInst <= JALR) PC_STALL = 1;
#ifdef PIPELINE_DEBUG
        cerr <<"Line"<<IFID.ins->lineNumer<<": "<< IFID.ins->dispName <<" ID\n";
        reg->dispRegInt();
#ifdef PIPELINE_PAUSE
        getchar();
#endif
#endif
    }

    void EX(){
#ifdef PIPELINE_DEBUG
        cerr << "EX START" << "\n";
#endif
        if(IDEX.STALL == 1){
           EXMEM.STALL = 1;
           return;
        }
        else EXMEM.STALL = 0;
        if(IDEX.ins == nullptr) return;
        int32_t tmpLo = 0, tmpHi = 0;
        uint32_t tmpA = 0, tmpB = 0, quotient = 0, remainder = 0;
        EXMEM.ins = IDEX.ins;
        EXMEM.dataRs = IDEX.dataRs;
        EXMEM.dataRt = IDEX.dataRt;
        EXMEM.NPC = IDEX.NPC;
        EXMEM.sysA1 = IDEX.sysA1;
        EXMEM.cond = 0;
        INSTRUCTION currentInst = IDEX.ins->name;
        switch(currentInst){
            case ADD:
            case ADDU:
            case ADDIU:
                if(IDEX.ins->srcType == 0) EXMEM.aluOutput = IDEX.dataRs + IDEX.ins->Src;
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
                    EXMEM.aluOutput = ((((int64_t)tmpHi) << 32) | (tmpLo & 0xffffffff));
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
                        EXMEM.aluOutput =  ((((uint64_t)remainder) << 32) | (quotient & 0xffffffff));
                    }
                    else{
                        tmpA = IDEX.dataRs;
                        tmpB = IDEX.dataRt;
                        quotient = tmpA / tmpB;
                        remainder = tmpA % tmpB;
                        EXMEM.aluOutput =  ((((uint64_t)remainder) << 32) | (quotient & 0xffffffff));
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
                if(IDEX.ins->srcType == 0){
                    EXMEM.aluOutput = (IDEX.dataRs % IDEX.ins->Src);
                }
                else{
                    EXMEM.aluOutput = (IDEX.dataRs % IDEX.dataRt);
                }
                break;

            case REMU:
                if(IDEX.ins->srcType == 0){
                    EXMEM.aluOutput = ((uint32_t)(IDEX.dataRs)) % ((uint32_t)(IDEX.ins->Src));
                }
                else{
                    EXMEM.aluOutput = ((uint32_t)(IDEX.dataRs) % (uint32_t)(IDEX.dataRt));
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
                    cerr << "\n\nB FUCK! NO LABEL!\n\n";
#ifdef PIPELINE_PAUSE
        getchar();
#endif
#endif
                    exit(0);
                }
                break;
            case JR:
            case JALR:
                EXMEM.aluOutput = IDEX.dataRs;
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
                    EXMEM.aluOutput = IDEX.dataRt + IDEX.ins->offset;
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
                        //exit = 1;
                        exit(0);
                        break;
                    case 17:
                        exit(IDEX.dataRt);
                        //exit = 1;
                        //rtv = IDEX.dataRt; // rt = $a0
                        break;
                    default:
                        break;
                }

             default: break;
            }
        if(EXMEM.cond == 1) reg->setWord((uint32_t)EXMEM.aluOutput, 34);
        PC_STALL = 0;
#ifdef PIPELINE_DEBUG
         cerr <<"Line"<<IDEX.ins->lineNumer<<": "<< IDEX.ins->dispName<< " EX\n";
        reg->dispRegInt();
#ifdef PIPELINE_PAUSE
        getchar();
#endif
#endif
    }

    void MA(){
#ifdef PIPELINE_DEBUG
        cerr << "MA START" << "\n";
#endif
        if(EXMEM.STALL == 1){
             MEMWB.STALL = 1;
             return;
        }
        else MEMWB.STALL = 0;
        if(EXMEM.ins == nullptr) return;
        string str = "";
        uint32_t pos = 0;
        MEMWB.ins = EXMEM.ins;
        MEMWB.aluOutput = EXMEM.aluOutput;
        MEMWB.dataRs = EXMEM.dataRs;
        MEMWB.dataRt = EXMEM.dataRt;
        MEMWB.NPC = EXMEM.NPC;
        MEMWB.STALL = EXMEM.STALL;
        INSTRUCTION currentInst = EXMEM.ins->name;
        switch (currentInst){
            case LB:
                MEMWB.lmd =  mem->getByte(EXMEM.aluOutput);
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
                        pos = EXMEM.dataRt;
                        while(mem->getByte(pos) != '\0'){
                            str += (char)(mem->getByte(pos));
                            ++pos;
                        }
                        cout << str;
                        break;
                    case 8:
                        cin >> str;
                        pos = EXMEM.dataRt;
                        for(int32_t i = 0; i < str.length(); ++i) mem->setByte(pos++, (int8_t)str[i]);
                        mem->setByte(pos, (int8_t)'\0');
                        break;
                    case 9:
                        while(mem->dynamicPosition % 4 != 0) ++mem->dynamicPosition;
                        MEMWB.sysV0 = mem->dynamicPosition;
                        mem->dynamicPosition += EXMEM.dataRt;
                        break;
                    default:
                        break;
                }
            default: break;
        }
#ifdef PIPELINE_DEBUG
        cerr <<"Line"<<EXMEM.ins->lineNumer<<": "<<EXMEM.ins->dispName<<" MA\n";
#ifdef PIPELINE_PAUSE
        getchar();
#endif
#endif
    }

    void WB(){
#ifdef PIPELINE_DEBUG
        cerr << "WB START" << "\n";
#endif
        if(MEMWB.STALL == 1) return;
        if(MEMWB.ins == nullptr) return;
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
            case SLT:
            case SNE:
                reg->setWord(MEMWB.aluOutput, MEMWB.ins->Rdest);
                reg->locker[MEMWB.ins->Rdest]--;
                break;
            case MUL:
            case MULU:
            case DIV:
            case DIVU:
                if(MEMWB.ins->argCount == 2){
                    tmpLo = MEMWB.aluOutput & ((int64_t)(0xffffffff));
                    tmpHi = (MEMWB.aluOutput >> 32);
                    reg->setWord(tmpHi, 33);
                    reg->setWord(tmpLo, 32);
                    reg->locker[32]--;
                    reg->locker[33]--;
                }
                else{
                    reg->setWord(MEMWB.aluOutput, MEMWB.ins->Rdest);
                    reg->locker[MEMWB.ins->Rdest]--;
                }
                break;
            case LA:
                reg->setWord(MEMWB.aluOutput, MEMWB.ins->Rdest);
                reg->locker[MEMWB.ins->Rdest]--;
                break;
            case LB:
                reg->setByte(MEMWB.lmd, MEMWB.ins->Rdest);
                 reg->locker[MEMWB.ins->Rdest]--;
                break;
            case LH:
                reg->setHalf(MEMWB.lmd, MEMWB.ins->Rdest);
                 reg->locker[MEMWB.ins->Rdest]--;
                break;
            case LW:
                reg->setWord(MEMWB.lmd, MEMWB.ins->Rdest);
                 reg->locker[MEMWB.ins->Rdest]--;
                break;
            case MOVE:
                reg->setWord(MEMWB.dataRs, MEMWB.ins->Rdest);
                 reg->locker[MEMWB.ins->Rdest]--;
                break;
            case MFHI:
                reg->setWord(reg->getWord(33), MEMWB.ins->Rdest);
                 reg->locker[MEMWB.ins->Rdest]--;
                break;
            case MFLO:
                reg->setWord(reg->getWord(32), MEMWB.ins->Rdest);
                 reg->locker[MEMWB.ins->Rdest]--;
                break;
            case JAL:
            case JALR:
                 reg->setWord(MEMWB.NPC, 31);
                 reg->locker[31]--;
                break;
            case LI:
                reg->setWord(MEMWB.ins->Src, MEMWB.ins->Rdest);
                reg->locker[MEMWB.ins->Rdest]--;
                break;
            case SYSCALL:
                switch(MEMWB.dataRs){
                    case 5:
                        reg->setWord(MEMWB.aluOutput, 2);
                        reg->locker[2]--;
                        break;
                    case 9:
                        reg->setWord(MEMWB.sysV0, 2);
                        reg->locker[2]--;
                        break;
                    default: break;
                }
                break;
            default:
                break;
        }
#ifdef PIPELINE_DEBUG
        cerr <<"Line"<<MEMWB.ins->lineNumer<<": "<<MEMWB.ins->dispName<<" WB\n";
        reg->dispRegInt();
#ifdef PIPELINE_PAUSE
        getchar();
#endif
#ifdef LINE_PAUSE
        getchar();
#endif
#endif
#ifdef REG_DEBUG
       cout << MEMWB.ins->lineNumer << ": " << MEMWB.ins->dispName << "\n";
       reg->dispRegIntCout();
        /*cout << MEMWB.ins->lineNumer << "\n";
        cout << std::flush;*/
#endif
    }

};
#endif














































