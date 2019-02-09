#ifndef ASSEMBLY_H
#define ASSEMBLY_H
 
#include <cstdint>
#include <string>
#include "registerset.h"
#include "instruction.h"


class AssemblyLine {

    public:
        Instruction instruct;
        std::string line;
        int         location;
        int         address;
        AssemblyLine (  Instruction i, 
                        std::string l, 
                        int loc, 
                        int addr
                    ) : 
                       instruct(i),
                       line(l),
                       location(loc),
                       address(addr)
                    {};
        AssemblyLine () :AssemblyLine(Instruction(),"",-1,-1){};

};


#endif
