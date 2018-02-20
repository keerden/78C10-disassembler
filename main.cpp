#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cctype>
#include <map>
#include <regex>
#include "registerset.h"
#include "utils.h"
#include "instructionset.h"
#include "memmap.h"
#include "assembly.h"

using namespace std;


int readfile(char* name, uint8_t*& rawdata);
int main(int argc, char** argv) {
	uint8_t* program;
	uint8_t* ptr;
	uint16_t current_addr;
	int size;
	int len;

	uint16_t offset = 0;

	ofstream outfile;

	RegisterSet regset;
	std::vector<std::string>* result;
	regset.load("definitions/upc7810.regs");

	InstructionSet iset(&regset);
	iset.load("definitions/upc7810.instr");

	MemMap mmap;
	mmap.load("definitions/upc7810.mmap");

	MemSegment *seg;
	AssemblyLine aline;


    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " <binary file>  <output file>" << endl;
        return 1;
    }
	
	outfile.open(argv[2]);

	size = readfile(argv[1], program);

	while(mmap.stackSize()) {
		//pop address
		current_addr = mmap.popPointer();

		//lookup segment
		seg = lookupSegment(current_addr);

		//check if segment pointer is not past current pointer
		if(seg->pointer > current_addr)
			continue;

		//check if current pointer is not past end of segment

		//disassemble until address is found

		//if adress exists continue disassembling
		//set current segment's pointer to current pointer
		//push pointer 
		//push address and loop



		seg = mmap.pop();
		offset = seg.start;
		len = mmap.getLenght(offset);
		seg.translated = true;
		mmap.updateSegment(seg);

		if(len == -1 || (offset + len) > size)
			len = size - offset;

		cout << seg.name << " len: " << int(len) << endl;
		do{
			aline = iset.match(program + offset, &len, &offset);
			if(aline.address != -1){
				if(mmap.addNewLabel(aline.address))
					cout << "new segment! @" << aline.address <<endl;
				else
					cout << "known segment! @" << aline.address <<endl;	
			}

			cout << int(aline.location) << aline.line << " a: " << aline.address << endl;
			
		} while (aline.location != -1);

	}



//	cout << .line <<endl;
//	cout <<"len " << len << "  offset: " <<offset;

	outfile.close();
}



int readfile(char* name, uint8_t*& rawdata) {
  streamoff size;

  ifstream file (name, ios::in|ios::binary|ios::ate);
  if (file.is_open())
  {
    size = file.tellg();
    rawdata = new uint8_t [(size + 3) * sizeof(char) / sizeof(uint8_t)];
    
    file.seekg (0, ios::beg);
    file.read ((char*) rawdata, size);
    file.close();

	rawdata[size] = 0xFF;
	rawdata[size+1] = 0xFF;
	rawdata[size+2] = 0xFF;
    return int(size);
  }
  else cout << "Unable to open file";
  return -1;
}
	














