#ifndef ASSEMBLY_FILE_H
#define ASSEMBLY_FILE_H

#include <cstdint>
#include <string>
#include <list>
#include "assemblyline.h"
#include "memmap.h"

class AssemblyFile
{
  private:
    std::list<AssemblyLine> lines;

  public:
    void add(AssemblyLine line);
    void matchLabels(MemMap *mm);

    bool write(std::string filename, MemMap *mm);
};

#endif
