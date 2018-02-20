#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include "instr-set.h"

/* run this program using the console pauser or add your own getch, system("pause") or input loop */
using namespace std;


void printList(InstrList* lst);
int readfile(char* name, uint8_t*& rawdata);
uint8_t shiftMask(uint8_t mask, uint8_t byte);
uint16_t shiftMask(uint16_t mask, uint16_t byte);
int8_t shiftMaskSigned(uint8_t mask, uint8_t byte);
int16_t shiftMaskSigned(uint16_t mask, uint16_t byte);


typedef map<int, string> stringMap;
typedef pair<int, string> stringMapItem;
typedef pair<stringMap::iterator,bool> StringMapCheck;

int main(int argc, char** argv) {
	
	InstrList* lst;
	uint8_t* rawdata;
	uint8_t* ptr;
	int size;
	int pc;
	ofstream file;

    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " <binary file>  <output file>" << endl;
        return 1;
    }
	
	file.open(argv[2]);

	lst = initInstrSet(instructionSet);
	size = readfile(argv[1], rawdata);

	printList(lst);

	InstrList::iterator it;
	regMap::iterator regit;
	uint8_t masked;
	string line;
	int pc_skip;
	stringstream argvalue;

	stringstream labelname;	
	stringMap disassembly;
	stringMap labelmap;
	stringMapItem label;
	StringMapCheck labelcheck;
	int labelcounter = 0;
	int temp;



	pc = 0;
	while(pc < size)
	{
		ptr = &rawdata[pc];
		
		//start matching instructions by going through the list
		for(it = lst->begin(); it != lst->end(); it++)
		{
			//match masked first byte with list
		    if(it->byte1 != (ptr[0] & ~it->argmask1))
				continue; 
			
			//check if masked bytes represent register
			if(!hasMaskedArgs(it->arg) && it->argmask1 != 0x00)
			{
				//match register
				masked = shiftMask(it->argmask1, ptr[0]);
				regit = it->reg->find(masked);
			    if(regit == it->reg->end()) // no match, go to next 
					continue;
	
			}
			
			if(has2ndMatchByte(it->arg) && it->length > 1) //match second byte
			{
		    	if(it->byte2 != (ptr[1] & ~it->argmask2))
					continue;			
				//check if masked bytes represent register
				if(!hasMaskedArgs(it->arg) && it->argmask2 != 0x00)
				{
					//match register
					masked = shiftMask(it->argmask2, ptr[1]);
					regit = it->reg->find(masked);
				    if(regit == it->reg->end()) // no match, go to next 
						continue;
		
				}						
			}
			

			//we have a match here		    	
			line = it->mnemonic;
			
			//replace regname
			if(!hasMaskedArgs(it->arg) && (it->argmask1 || it->argmask2))
				line.replace(line.find("$reg"), 4, regit->second);
			
			pc_skip = it->length;
			argvalue.str("");	
			labelname.str("");
			switch(it->arg)
			{
				case BYTE_2_DATA:	
					argvalue << "0x" << hex << int(ptr[1]);
					line.replace(line.find("$data"), 5, argvalue.str());
					break;
				case BYTE_3_DATA:	
					argvalue << "0x" << hex << int(ptr[2]);
					line.replace(line.find("$data"), 5, argvalue.str());
					break;		
				case BYTE_2_OFFSET:	
					argvalue << "0x" << hex << int(ptr[1]);
					line.replace(line.find("$offset"), 7, argvalue.str());
					break;						
				case BYTE_2_OFFSET_BIT_MASK:	
					argvalue << "0x" << hex <<int(ptr[1]);
					line.replace(line.find("$offset"), 7, argvalue.str());
					argvalue.str("");
					argvalue << dec <<int(shiftMask(it->argmask1, ptr[0]));
					line.replace(line.find("$bit"), 4, argvalue.str());
					break;						
				case BYTE_3_OFFSET:	
					argvalue << "0x" << hex << int(ptr[2]);
					line.replace(line.find("$offset"), 7, argvalue.str());
					break;						
				case BYTE_2_OFFSET_3_DATA:	
					argvalue << "0x" << hex << int(ptr[1]);
					line.replace(line.find("$offset"), 7, argvalue.str());
					argvalue.str("");
					argvalue << "0x" << hex << int(ptr[2]);
					line.replace(line.find("$data"), 5, argvalue.str());
					break;	
				case BYTE_2_DATA_OPT:	
					if(line.find("$data") != string::npos)
					{
						argvalue << "0x" << hex << int(ptr[1]);
						line.replace(line.find("$data"), 5, argvalue.str());	
						pc_skip++;
					}
					break;
				case BYTE_3_DATA_OPT:	
					if(line.find("$data") != string::npos)
					{
						argvalue << "0x" << hex << int(ptr[2]);
						line.replace(line.find("$data"), 5, argvalue.str());	
						pc_skip++;
					}
					break;	
				case WORD_2_3:	
					argvalue << "0x" << hex << int((ptr[2] << 8) | ptr[1]);
					line.replace(line.find("$word"), 5, argvalue.str());
					break;
				case WORD_2_3_ADDR:	
					temp = (ptr[2] << 8) | ptr[1];
					labelname << "Label" << labelcounter;
					
					labelcheck = labelmap.insert(stringMapItem (temp, labelname.str()) );
					if (labelcheck.second==true)
						labelcounter++;
					else
						labelname.str(labelcheck.first->second);
																
					argvalue << labelname.str() << "  @0x" << hex << temp;
					line.replace(line.find("$addr"), 5, argvalue.str());
					break;	
				case WORD_3_4_ADDR:	
					temp =  int((ptr[3] << 8) | ptr[2]);
					labelname << "Label" << labelcounter;
					
					labelcheck = labelmap.insert(stringMapItem (temp, labelname.str()) );
					if (labelcheck.second==true)
						labelcounter++;
					else
						labelname.str(labelcheck.first->second);	
					argvalue << labelname.str() << "  @0x" << hex << temp;
					line.replace(line.find("$addr"), 5, argvalue.str());
					break;	
				case MASK_JDISP:	
					if(it->argmask1 == 0)
						temp = int(shiftMaskSigned(it->argmask2, ptr[1]));
					else if(it->argmask2 == 0)
						temp =  int(shiftMaskSigned(it->argmask1, ptr[0]));
					else
						temp =  int( shiftMaskSigned(uint16_t ((it->argmask1 << 8) | it->argmask2), uint16_t ((ptr[0] << 8) | ptr[1]))) ;				
					temp += it->length;

					labelname << "Label" << labelcounter;
					labelcheck = labelmap.insert(stringMapItem (pc + temp, labelname.str()) );
					if (labelcheck.second==true)
						labelcounter++;
					else
					labelname.str(labelcheck.first->second);

					argvalue << labelname.str() << " " << dec << temp;	
					line.replace(line.find("$disp"), 5, argvalue.str());
					break;	
				case MASK_FIXED_ADDR:	
					temp = unsigned( 0x0800 | shiftMask(uint16_t ((it->argmask1 << 8) | it->argmask2), uint16_t ((ptr[0] << 8) | ptr[1])));
						
					labelname << "Label" << labelcounter;
					labelcheck = labelmap.insert(stringMapItem (temp, labelname.str()) );
					if (labelcheck.second==true)
						labelcounter++;
					else
						labelname.str(labelcheck.first->second);
												
					argvalue << labelname.str() << " 0x"<< hex  << temp;
					line.replace(line.find("$fa"), 3, argvalue.str());
					break;
				case MASK_TABLE_ADDR:	
					argvalue << dec << unsigned( 0x80 | (shiftMask(it->argmask1, ptr[0]) << 1)) ;
					line.replace(line.find("$ta"), 3, argvalue.str());
					break;										

			}
			disassembly[pc] = line;
			pc += pc_skip;	
	  		break;
		}	
		if(it == lst->end())
		{
			line = "?????";
			disassembly[pc] = line;
			pc++;
		}		
	}

	stringMap::iterator lineIt, labelIt;
	for(lineIt = disassembly.begin();lineIt != disassembly.end(); ++lineIt)
	{
		labelIt = labelmap.find(lineIt->first);
		if(labelIt != labelmap.end())
			file << endl << labelIt->second << ":" << endl;
		file << lineIt->second << endl;
	}

	file.close();
	delete[] rawdata;
	delete lst;
		
	return 0;
}

void printList(InstrList* lst)
{
	InstrList::iterator it;
	

	for(it = lst->begin(); it != lst->end(); it++)
	{
	    cout << dec << int((*it).length) << " " << hex << int((*it).byte1)  <<" " << int((*it).byte2)<< " " << (*it).mnemonic << endl;
	}		
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

uint8_t shiftMask(uint8_t mask, uint8_t byte)
{
	int i, shifts;
	uint8_t retval = 0;
	shifts = 0;
	for(i = 0; i < 8; i++)
	{
		if(mask & (1 << i))
		{
			retval |= (byte & (1 << i)) >> shifts;
		} else
			shifts++;
	}
	return retval;
	
}

uint16_t shiftMask(uint16_t mask, uint16_t byte)
{
	int i, shifts;
	uint16_t retval = 0;
	shifts = 0;
	for(i = 0; i < 16; i++)
	{
		if(mask & (1 << i))
		{
			retval |= (byte & (1 << i)) >> shifts;
		} else
			shifts++;
	}
	return retval;
	
}


int8_t shiftMaskSigned(uint8_t mask, uint8_t byte)
{
	int i, shifts;
	int8_t retval = 0;
	shifts = 0;
	for(i = 0; i < 8; i++)
	{
		if(mask & (1 << i))
		{
			retval |= (byte & (1 << i)) >> shifts;
		} else
			shifts++;  
	}
	
	if(retval & (1 << (7-shifts)))
		for(i = 7-shifts; i < 8; i++)
			retval |=  (1 << i);	
	
	return retval;	
}
int16_t shiftMaskSigned(uint16_t mask, uint16_t byte)
{
	int i, shifts;
	int16_t retval = 0;
	shifts = 0;
	for(i = 0; i < 16; i++)
	{
		if(mask & (1 << i))
		{
			retval |= (byte & (1 << i)) >> shifts;
		} else
			shifts++;
	}
	
	if(retval & (1 << (15-shifts)))
		for(i = 15-shifts; i < 16; i++)
			retval |=  (1 << i);	
	
	return retval;	
}


















