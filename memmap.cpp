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

/**********
 *  MemMap::~MemMap
 * 
 *  Destructor, destroys all list-items
 */

MemMap::~MemMap()
{
    while (!memlist.empty())
        delete memlist.front(), memlist.pop_front();
}

/**********
 *  MemMap::load
 * 
 *  Loads a memmap definition file.
 *  The file is parsed and segments are created and added to the ordered list.
 *  If loading fails, the state of this instance of MemMap is undefined.
 * 
 *  inputs:
 *      std::string filename, path of file to parse
 * 
 *  returns:
 *      True on succes, False on a parse error or I/O error
 */

bool MemMap::load(std::string filename)
{

    bool parse_error = false;
    string temp, name;
    uint16_t start;
    SegmentType type;
    MemSegment *m;

    string line;
    vector<string> *tokens;
    list<MemSegment *>::reverse_iterator it;

    fstream fin(filename, fstream::in);

    if (!fin.is_open())
        return false;

    while (getline(fin, line) && !parse_error)
    {
        line = trim_string(line);
        if (line.at(0) == '#')
            continue;
        tokens = split_string(line, " ");
        if (tokens->size() != 3)
        {
            parse_error = true;
            break;
        }

        name = trim_string((*tokens)[0]);

        //type
        temp = trim_string((*tokens)[1]);

        if (temp == "code")
        {
            type = CODE_SEGMENT;
        }
        else if (temp == "data")
        {
            type = DATA_SEGMENT;
        }
        else if (temp == "call_table")
        {
            type = CALL_TABLE_SEGMENT;
        }
        else
        {
            parse_error = true;
            break;
        }

        //address
        temp = trim_string((*tokens)[2]);
        start = stoi(temp, 0, 16);
        m = new MemSegment(name, type, start, start);

        this->insertSegment(m);
    }

    //push in reverse order to start disassembly with the first segment
    for (it = memlist.rbegin(); it != memlist.rend(); ++it)
        this->pushSegment(*it);

    return (!parse_error);
}

/**********
 *  MemMap::pushSegment
 * 
 *  Pushes the instruction-pointer of the given segment
 * 
 *  inputs:
 *      MemSegment *seg, segment to push
 * 
 */

void MemMap::pushSegment(MemSegment *seg)
{
    pointerStack.push(seg->pointer);
}

/**********
 *  MemMap::popSegment
 * 
 *  Pops an instruction-pointer and finds the matching segment
 * 
 *  returns:
 *      The resulting segment object
 * 
 */

MemSegment *MemMap::popSegment()
{
    uint16_t pntr = pointerStack.top();
    pointerStack.pop();
    return this->lookup(pntr);
}

/**********
 *  MemMap::stackEmpty
 * 
 *  returns:
 *      True when the internal stack is empty
 * 
 */
bool MemMap::stackEmpty()
{
    return pointerStack.empty();
}

/**********
 *  MemMap::changeSegment
 * 
 *  Checks if the given address is not an existing segment
 *  the current segment is pushed, a new label is generated and assigned to a new segment.
 * 
 *  inputs:
 *      MemSegment *current: The current segment to push
 *      uint16_t addr: the address to create a new segment on.    
 * 
 *  returns:
 *      If the given address was not an existing segment, the new segment is returned.
 *      Otherwise the current segment is returned
 */
MemSegment *MemMap::changeSegment(MemSegment *current, uint16_t addr)
{
    MemSegment *seg = new MemSegment("label" + to_string(labelCounter), CODE_SEGMENT, addr, addr);
    if(this->insertSegment(seg)){
        labelCounter++;
        this->pushSegment(current);
        return seg;
    }else{
        delete seg;
        return current;
    }
}

/**********
 *  MemMap::lookup
 * 
 *  Finds the segment in which the given address is located
 * 
 *  inputs:
 *      uint16_t addr: the address to look up    
 * 
 *  returns:
 *      - The segment in which the given address is located
 *      - NULL when the given address does not fall into a segment
 */

MemSegment* MemMap::lookup(uint16_t addr)
{
    list<MemSegment *>::iterator it;
    list<MemSegment *>::iterator it_next;
    for (it = memlist.begin(); it != memlist.end(); it++)
    {
        it_next = it;
        it_next++;
        if (it_next == memlist.end())
            return *it;
        if (addr >= (*it)->start && addr < (*it_next)->start)
            return *it;
    }

    return NULL;
}

/**********
 *  MemMap::getLabel
 * 
 *  Finds a label for a segment at the given address
 * 
 *  inputs:
 *      uint16_t addr: the address to look up    
 * 
 *  returns:
 *      - The label corresponding to the address
 *      - An empty string when the given address is not a segment
 */
string MemMap::getLabel(uint16_t addr) {
    list<MemSegment *>::iterator it;
    for (it = memlist.begin(); it != memlist.end(); it++)
    {
        if (addr == (*it)->start)
            return (*it)->name;
        if (addr < ((*it)->start))
            return "";  
    }
    return "";
}

/**********
 *  MemMap::print
 * 
 *  Prints the content of the memmap
 * 
 */
void MemMap::print()
{
    cout << "Memmap:" << endl;

    list<MemSegment *>::iterator it;
    for (it = memlist.begin(); it != memlist.end(); it++)
    {
        (*it)->print();
    }
}

/**********
 *  MemMap::insertSegment
 * 
 *  Inserts a new segment into the map. 
 *  If the new segment falls into an existing one, the existing one is split.
 *  If the pointer of the splitted segment falls into the new one, 
 *  the pointer of the new segment is set to pointer of the splitted one
 *  the pointer of the splitted segment is then set to the end of its segment
 * 
 *  If the segment already existed, nothing is done
 * 
 *  inputs:
 *      MemSegment *seg: the segment to insert
 * 
 *  returns:
 *      true on success, false when the segment already existed
 */

bool MemMap::insertSegment(MemSegment *seg)
{
    list<MemSegment *>::iterator it;
    list<MemSegment *>::iterator prev = memlist.end();
    if (memlist.empty())
    {
        seg->limit = 0xFFFF;
        memlist.push_front(seg);
    }
    else
    {
        for (it = memlist.begin(); it != memlist.end(); it++)
        {
            if ((*it)->start == seg->start)
                return false;

            if (seg->start < (*it)->start)
                break;

            prev = it;
        }

        if (prev != memlist.end())
        {
            seg->limit = (*prev)->limit;
            (*prev)->limit = seg->start - 1;
            if ((*prev)->pointer > seg->start)
            {
                seg->pointer = (*prev)->pointer;
                (*prev)->pointer = seg->start;
            }
            else
            {
                seg->pointer = seg->start;
            }
        }
        else
        {
            seg->pointer = seg->start;
        }
        memlist.insert(it, seg);
    }
    return true;
}

/**********
 *  MemMap::iterate
 * 
 *  Returns the next segment in the memmap
 * 
 * 
 *  inputs:
 *      bool first: restart with the first element
 * 
 *  returns:
 *      the next MemSegment in the map or
 *      NULL when the end is reached
 */

MemSegment *MemMap::iterate(bool first){
    MemSegment *output;

    if(first)
        this->iterator = memlist.begin();
    
    if(this->iterator == memlist.end())
        return NULL;
    
    output = *(this->iterator);
    this->iterator++;
    return output;
}

