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
#include "assemblyline.h"
#include "assemblyfile.h"

using namespace std;

int readfile(char *name, uint8_t *&rawdata);
int main(int argc, char **argv)
{
	uint8_t *program;		//array containing raw bytes of program
	int size;						//size of program

	RegisterSet regset;
	std::vector<std::string> *result;
	if(!regset.load("definitions/upc7810.regs")){
		cout << "Parse error in register definition" << endl;
		return 1;
	}

	InstructionSet iset(&regset);
	if(!iset.load("definitions/upc7810.instr")){
		cout << "Parse error in instructions definition" << endl;
		return 1;
	}

	MemMap mmap;
	if(!mmap.load("definitions/upc7810.mmap")){
		cout << "Parse error in memory map definition" << endl;
		return 1;
	}

	AssemblyFile afile;
	MemSegment *seg;
	AssemblyLine aline;

	if (argc < 3)
	{
		cerr << "Usage: " << argv[0] << " <binary file>  <output file>" << endl;
		return 1;
	}
	size = readfile(argv[1], program);

	while(!mmap.stackEmpty()){
		seg = mmap.popSegment();
		while(!seg->isDissassembled() && seg->pointer < size){
			//disassemble
			aline = iset.match(program, size, &seg->pointer);
			
			//check for invalid instruction
			if(aline.location == -1){
				aline.location = seg->pointer;
				afile.add(aline);
				break;		//stop disassembly of this segment 
			} 

			afile.add(aline);
			
			// if an address was found
			if(aline.address != -1) {
				seg = mmap.changeSegment(seg, (uint16_t) aline.address); 
			}
		}
	}
	afile.matchLabels(&mmap);
	afile.write(argv[2],&mmap);

}

int readfile(char *name, uint8_t *&rawdata)
{
	streamoff size;

	ifstream file(name, ios::in | ios::binary | ios::ate);
	if (file.is_open())
	{
		size = file.tellg();
		rawdata = new uint8_t[(size + 3) * sizeof(char) / sizeof(uint8_t)];

		file.seekg(0, ios::beg);
		file.read((char *)rawdata, size);
		file.close();

		rawdata[size] = 0xFF;
		rawdata[size + 1] = 0xFF;
		rawdata[size + 2] = 0xFF;
		return int(size);
	}
	else
		cout << "Unable to open file";
	return -1;
}
