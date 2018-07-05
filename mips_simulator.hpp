#ifndef _MIPS_SIMULATOR_HPP_
#define _MIPS_SIMULATOR_HPP_
#include "mips_define.hpp"
#include "mips_pipeline.hpp"
class MIPSSimulator{
private:
    MIPSPipeline *p;
public:
    MIPSSimulator(const char *s){
        p = new MIPSPipeline(s);
    }

    ~MIPSSimulator(){
        delete p;
        p = nullptr;
    }

    void run(){
        p->preProcess();
        p->runPipeline();
    }
};
#endif
