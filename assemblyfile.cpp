#include <cstdint>
#include <string>
#include <fstream>
#include "assemblyfile.h"

using namespace std;

void AssemblyFile::add(AssemblyLine line)
{
  list<AssemblyLine>::iterator it;

    if (this->lines.empty())
    {
        this->lines.push_front(line);
    }
    else
    {
        for (it = this->lines.begin(); it != this->lines.end(); it++)
        {
            if(line.location < (*it).location)
              break;
        } 
        this->lines.insert(it, line);
    }
}

void AssemblyFile::matchLabels(MemMap *mm)
{
  size_t pos;
  string label;
  list<AssemblyLine>::iterator it;

  for(it = this->lines.begin(); it != this->lines.end(); it++){
    if((*it).address != -1){
      pos = (*it).line.find("$label");
      if(pos != string::npos){
        label = mm->getLabel((*it).address);
        if(label == "")
          label = "??UNDEFINED_LABEL??";
          (*it).line.replace(pos, 6, label);
      }
    }
  }
}

bool AssemblyFile::write(std::string filename, MemMap *mm)
{
  ofstream outfile;
  MemSegment* seg;
  outfile.open(filename);

  if(!outfile.is_open())
    return false;


  seg = mm->iterate(true);

  for(auto aline : this->lines){

    if(seg != NULL){
      if(aline.location >= seg->start){
        outfile << seg->name << ":" << endl;
        seg = mm->iterate();
      }
    }

    outfile << hex <<  int(aline.location) << "\t" << aline.line << "\ta: " << aline.address << endl;
  }


  outfile.close();
  return true;
}
