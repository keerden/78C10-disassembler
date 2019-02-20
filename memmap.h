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
  std::list<MemSegment *> memlist;
  std::stack<uint16_t> pointerStack;
  std::list<MemSegment *>::iterator iterator;
  int labelCounter;

  bool insertSegment(MemSegment *seg);


public:
  MemMap() : labelCounter(0), iterator(memlist.begin()){};
  ~MemMap();

  bool load(std::string file);

  bool stackEmpty();
  MemSegment *popSegment();
  void pushSegment(MemSegment *seg);

  MemSegment *changeSegment(MemSegment * current, uint16_t addr);
  MemSegment *lookup(uint16_t addr);
  std::string getLabel(uint16_t addr);

  MemSegment *iterate(bool first = false);
  void print();
};

#endif