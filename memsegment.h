#ifndef MEMSEGMENT_H
#define MEMSEGMENT_H

#include <cstdint>
#include <string>

enum SegmentType {CODE_SEGMENT, DATA_SEGMENT, CALL_TABLE_SEGMENT};

class MemSegment
{
    public:
    uint16_t start;
    uint16_t pointer;
    uint16_t limit;
    std::string name;
    SegmentType type;

    MemSegment(std::string label, SegmentType segtype, uint16_t strt, uint16_t pntr, uint16_t lmt)
        : name(label)
        , type(segtype)
        , start(strt)
        , pointer(pntr)
        , limit(lmt)
        {}
    MemSegment(std::string label, SegmentType segtype, uint16_t strt, uint16_t pntr)
        : name(label)
        , type(segtype)
        , start(strt)
        , pointer(pntr)
        , limit(0xFFFF)
        {}
    MemSegment():MemSegment("", CODE_SEGMENT, -1, -1, 0xFFFF){}
    
    void print();
    bool isDissassembled();
};
 
#endif