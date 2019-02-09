#ifndef SEGMENT_H
#define SEGMENT_H
 
#include <cstdint>
#include <string>


enum SegmentType {CODE_SEGMENT, DATA_SEGMENT, CALL_TABLE_SEGMENT};

class Segment
{
    public:
    uint16_t start;
    std::string name;
    enum SegmentType type;
    bool translated;

    MemSegment(string label, SegmentType segtype, uint16_t strt, bool trans)
        : name(label)
        , type(segtype)
        , start(strt)
        , translated(trans)
        {}

};

#endif