#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <list>
#include "memmap.h"
#include "memsegment.h"
#include "utils.h"


using namespace std;

MemMap::~MemMap(){
    delete pointerStack;
    while(!memlist.empty()) delete memlist.front(), memlist.pop_front();
}


bool MemMap::load(std::string filename) {

    bool parse_error = false;
    string temp, name;
    uint16_t start;
    SegmentType type;
    MemSegment* m;

    string line;
    vector <string>* tokens;
    std::list<MemSegment*>::reverse_iterator it;


	fstream fin(filename, fstream::in);

    if(!fin.is_open())
        return false;

	while (getline(fin, line) && !parse_error) {
        line= trim_string(line);
        if (line.at(0) == '#')
            continue;
		tokens = split_string(line, " ");
        if (tokens->size() != 3) {
            parse_error = true;
            break;
        }

        name = trim_string((*tokens)[0]);

        //type
        temp = trim_string((*tokens)[1]);

        if(temp ==  "code"){
            type = CODE_SEGMENT;
        }else if (temp == "data") {
            type = DATA_SEGMENT;
        }else if(temp == "call_table"){
            type = CALL_TABLE_SEGMENT;
        }else{
            parse_error = true;
            break;
        }


        //address
        temp = trim_string((*tokens)[2]);
        start = stoi(temp, 0, 16);
        m = new MemSegment(name,type, start, start);

        this->insertSegment(m);
	}

    for(it = memlist.rbegin(); it != memlist.rend(); ++it)
        this->pushPointer((*it)->pointer);

	return (!parse_error);
}

void MemMap::void pushPointer(uint16_t pntr){
    pointerStack.push(pntr);
}

uint16_t MemMap::popPointer() {
    uint16_t pntr =  pointerStack.top();
    pointerStack.pop();
    return pntr;
}

int MemMap::stackSize(){
    return pointerStack.size();
}

MemSegment* MemMap::lookupSegment(uint16_t address){
    list<MemSegment*>::iterator it;
    list<MemSegment*>::iterator it_n;
    for(it = memlist.begin(); it != memlist.end(); it++)
    {
        it_n = it + 1;
        if(it_n == memlist.end())
            return *it;
        if(address >= (*it)->start && address < (*it_n)->start)
            return *it;
    }

     return NULL;
}



bool MemMap::isSegment(uint16_t address) {
    list<MemSegment*>::iterator it;
    for(it = memlist.begin(); it != memlist.end(); it++)
    {
        if(address == (*it)->start)
            return true;
        if(address < (*it)->start)
            return false;
    }
    return false;
}


int MemMap::getLenght(uint16_t address) {
    list<MemSegment*>::iterator it;
    for(it = memlist.begin(); it != memlist.end(); it++)
    {
        if(address < (*it)->start)
            return (*it)->start - address;
 
    }
    return -1;

}



bool MemMap::addNewLabel(uint16_t address){
    MemSegment* seg
    seg = new MemSegment("label" + to_string(labelCounter), CODE_SEGMENT, address, address);
    if(insertSegment(seg)){
        labelCounter++;
        return true;
    }else{
        delete seg;
        return false;
    }

}
bool MemMap::insertSegment(MemSegment* seg) {
    list<MemSegment*>::iterator it;
    list<MemSegment*>::iterator prev = memlist.end();
    if(memlist.empty())
	{
		memlist.push_front(seg);
	}else{
		for(it = memlist.begin(); it != memlist.end(); it++)
		{
            if((*it)->start == seg->start)
                return false;
            
            if(seg->start < (*it)->start)
                break;

            prev = it;
		}

        if(prev != memlist.end()) {
            if((*prev)->pointer > seg->start) {
                seg->pointer = (*prev)->pointer;
                (*prev)->pointer = seg->start;
            } else {
                seg->pointer = seg->start;
            }
        } else {
            seg->pointer = seg->start;
        }   
		memlist.insert(it,seg);
    }
    return true;
}






void MemMap::print() {
    cout << "Memmap:" <<endl;

    list<MemSegment*>::iterator it;
    for(it = memlist.begin(); it != memlist.end(); it++)
    {
        (*it)->print();
 
    }
 

}