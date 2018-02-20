#ifndef MEMMAP_H
#define MEMMAP_H
 
#include <list>
#include <cstdint>
#include <string>
#include <stack>
#include "memsegment.h"

class MemMap
{
private:
    std::list<MemSegment*> memlist;
    std::stack<uint16_t> pointerStack;
    int labelCounter;

    bool insertSegment(MemSegment* seg);
 
public:
    MemMap(): labelCounter(0) {};
    ~MemMap();

    uint16_t popPointer();
    void pushPointer(uint16_t pntr);
    int stackSize();

    bool load(std::string file);


    MemSegment* lookupSegment(uint16_t address);
    bool isSegment(uint16_t address);


    int getLenght(uint16_t address);
    bool addNewLabel(uint16_t address);
    void print();


     
};
 
#endif