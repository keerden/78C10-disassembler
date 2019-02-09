#ifndef INSTRUCTIONSET_H
#define INSTRUCTIONSET_H
 
#include <cstdint>
#include <string>
#include <map>
#include <list>
#include "instruction.h"
#include "registerset.h"
#include "assembly.h"



class InstructionSet
{
    private:
        std::list<Instruction> instrList;
        RegisterSet* regs;
    public:
        InstructionSet(RegisterSet* r) : regs(r) {};
        void add(Instruction i);
        bool load(std::string file);
        void print();
        AssemblyLine match(uint8_t* ptr, int* len, int* offset);
};

#endif