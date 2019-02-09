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
    std::string name;
    SegmentType type;

    MemSegment(std::string label, SegmentType segtype, uint16_t strt, uint16_t pntr)
        : name(label)
        , type(segtype)
        , start(strt)
        , pointer(pntr)
        {}
    MemSegment():MemSegment("", CODE_SEGMENT, -1, -1){}
    
    void print();
};
 
#endif