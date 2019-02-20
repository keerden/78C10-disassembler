#include <string>
#include <regex>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <list>
#include "instruction.h"
#include "instructionset.h"
#include "registerset.h"
#include "assemblyline.h"
#include "utils.h"

using namespace std;

void InstructionSet::add(Instruction in)
{
    std::list<Instruction>::iterator it;

    uint8_t masked1 = in.byte1 & ~(in.argmask1);
    uint8_t masked2 = in.byte2 & ~(in.argmask2);

    if (this->instrList.empty())
    {
        this->instrList.push_front(in);
    }
    else
    {
        for (it = this->instrList.begin(); it != this->instrList.end(); it++)
        {
            //check first opcode and mask
            if (masked1 < ((*it).byte1 & ~((*it).argmask1))) //just stop and insert here
            {
                break;
            }
            else if (masked1 == ((*it).byte1 & ~((*it).argmask1))) //byte 1 same, check byte2
            {
                if (in.length == 1) //only 1 byte, just insert
                    break;
                if (masked2 < ((*it).byte2 & ~((*it).argmask2)))
                    break;
            }
        }
        this->instrList.insert(it, in);
    }
}

bool InstructionSet::load(std::string filename)
{
    bool parse_error = false;
    uint8_t len, b1, b2, am1, am2;
    bool jps, sks, nret;
    enum argType arg;
    regMap *r;
    string temp;
    Instruction *i;

    string line;
    vector<string> *tokens;
    regex reg_hex("^0x([0-9a-fA-F]+)");
    smatch m;

    fstream fin(filename, fstream::in);

    if (!fin.is_open())
        return false;

    while (getline(fin, line) && !parse_error)
    {
        tokens = split_string(line, ",", 10);
        if (tokens->size() != 11)
        {
            parse_error = true;
            break;
        }

        //instr length
        len = stoi(trim_string((*tokens)[0]));

        //byte 1
        temp = trim_string((*tokens)[1]);
        if (regex_search(temp, m, reg_hex))
        {
            b1 = stoi(m[1], 0, 16);
        }
        else
        {
            parse_error = true;
            break;
        }

        //byte 2
        temp = trim_string((*tokens)[2]);
        if (regex_search(temp, m, reg_hex))
        {
            b2 = stoi(m[1], 0, 16);
        }
        else
        {
            parse_error = true;
            break;
        }

        //argtype
        arg = (enum argType)stoi((*tokens)[3]);

        //regname
        temp = trim_string((*tokens)[4]);
        if (temp == "")
            r = NULL;
        else
        {
            r = regs->get(temp);
            if (r == NULL)
            {
                parse_error = true;
                break;
            }
        }

        //am 1
        temp = trim_string((*tokens)[5]);
        if (regex_search(temp, m, reg_hex))
        {
            am1 = stoi(m[1], 0, 16);
        }
        else
        {
            parse_error = true;
            break;
        }

        //am 2
        temp = trim_string((*tokens)[6]);
        if (regex_search(temp, m, reg_hex))
        {
            am2 = stoi(m[1], 0, 16);
        }
        else
        {
            parse_error = true;
            break;
        }

        //jumps
        temp = trim_string((*tokens)[7]);
        jps = (temp != "0");

        //skips
        temp = trim_string((*tokens)[8]);
        sks = (temp != "0");

        //no return
        temp = trim_string((*tokens)[9]);
        nret = (temp != "0");

        temp = trim_string((*tokens)[10]);
        if (temp == "")
        {
            parse_error = true;
            break;
        }

        i = new Instruction(len, b1, b2, arg, r, am1, am2, temp, jps, sks, nret);
        this->add(*i);
    }
    return (!parse_error);
}

void InstructionSet::print()
{

    // Instruction i;
    for (auto i : this->instrList)
    {
        i.print();
    }
}

AssemblyLine InstructionSet::match(uint8_t *program, int size, uint16_t *ipointer)
{
    AssemblyLine wrong;
    AssemblyLine aline;
    list<Instruction>::iterator it;
    uint8_t masked;
    regMap::iterator regit;
    stringstream argvalue;
    stringstream labelname;

    uint8_t *ptr = &program[*ipointer];
    int instr_size = 0;
    int len = size - *ipointer;

    if (len < 1)
        return wrong;

    for (it = instrList.begin(); it != instrList.end(); it++)
    {
        //check if first byte matches
        if (it->byte1 != (ptr[0] & ~it->argmask1))
            continue;
        //check if the size of the byte fits in the length of the code
        if (it->length > len)
            continue;

        //check if masked bits represent register
        if (!it->hasMaskedArgs() && it->argmask1 != 0x00)
        {
            //match register
            masked = shiftMask(it->argmask1, ptr[0]);
            regit = it->reg->find(masked);
            if (regit == it->reg->end()) // no match, go to next
                continue;
        }
        if (it->has2ndMatchByte()) //match second byte
        {
            if (it->byte2 != (ptr[1] & ~it->argmask2))
                continue;
            //check if masked bytes represent register
            if (!it->hasMaskedArgs() && it->argmask2 != 0x00)
            {
                //match register
                masked = shiftMask(it->argmask2, ptr[1]);
                regit = it->reg->find(masked);
                if (regit == it->reg->end()) // no match, go to next
                    continue;
            }
        }

        // from this point the instruction is matched, now we can extract the values
        aline.instruct = *it;
        instr_size = it->length;
        aline.location = *ipointer;
        aline.line = it->mnemonic;

        //replace regname
        if (!it->hasMaskedArgs() && (it->argmask1 || it->argmask2))
            aline.line.replace(aline.line.find("$reg"), 4, regit->second);

        switch (it->arg)
        {
        case BYTE_2_DATA:
            argvalue << "0x" << hex << int(ptr[1]);
            aline.line.replace(aline.line.find("$data"), 5, argvalue.str());
            break;
        case BYTE_3_DATA:
            argvalue << "0x" << hex << int(ptr[2]);
            aline.line.replace(aline.line.find("$data"), 5, argvalue.str());
            break;
        case BYTE_2_OFFSET:
            argvalue << "0x" << hex << int(ptr[1]);
            aline.line.replace(aline.line.find("$offset"), 7, argvalue.str());
            break;
        case BYTE_2_OFFSET_BIT_MASK:
            argvalue << "0x" << hex << int(ptr[1]);
            aline.line.replace(aline.line.find("$offset"), 7, argvalue.str());
            argvalue.str("");
            argvalue << dec << int(shiftMask(it->argmask1, ptr[0]));
            aline.line.replace(aline.line.find("$bit"), 4, argvalue.str());
            break;
        case BYTE_3_OFFSET:
            argvalue << "0x" << hex << int(ptr[2]);
            aline.line.replace(aline.line.find("$offset"), 7, argvalue.str());
            break;
        case BYTE_2_OFFSET_3_DATA:
            argvalue << "0x" << hex << int(ptr[1]);
            aline.line.replace(aline.line.find("$offset"), 7, argvalue.str());
            argvalue.str("");
            argvalue << "0x" << hex << int(ptr[2]);
            aline.line.replace(aline.line.find("$data"), 5, argvalue.str());
            break;
        case BYTE_2_DATA_OPT:
            if (aline.line.find("$data") != string::npos)
            {
                instr_size++;
                if (instr_size > len)
                    return wrong;
                argvalue << "0x" << hex << int(ptr[1]);
                aline.line.replace(aline.line.find("$data"), 5, argvalue.str());
            }
            break;
        case BYTE_3_DATA_OPT:
            if (aline.line.find("$data") != string::npos)
            {
                instr_size++;
                if (instr_size > len)
                    return wrong;
                argvalue << "0x" << hex << int(ptr[2]);
                aline.line.replace(aline.line.find("$data"), 5, argvalue.str());
            }
            break;
        case WORD_2_3:
            argvalue << "0x" << hex << int((ptr[2] << 8) | ptr[1]);
            aline.line.replace(aline.line.find("$word"), 5, argvalue.str());
            break;
        case WORD_2_3_ADDR:
            aline.address = int((ptr[2] << 8) | ptr[1]);
            break;
        case WORD_3_4_ADDR:
            aline.address = int((ptr[3] << 8) | ptr[2]);
            break;
        case MASK_JDISP:
            if (it->argmask1 == 0)
                aline.address = int(shiftMaskSigned(it->argmask2, ptr[1]));
            else if (it->argmask2 == 0)
                aline.address = int(shiftMaskSigned(it->argmask1, ptr[0]));
            else
                aline.address = int(shiftMaskSigned(uint16_t((it->argmask1 << 8) | it->argmask2), uint16_t((ptr[0] << 8) | ptr[1])));

            argvalue << "$label " << dec << aline.address;
            aline.address += it->length + *ipointer;
            aline.line.replace(aline.line.find("$disp"), 5, argvalue.str());
            break;
        case MASK_FIXED_ADDR:
            aline.address = unsigned(0x0800 | shiftMask(uint16_t((it->argmask1 << 8) | it->argmask2), uint16_t((ptr[0] << 8) | ptr[1])));

            argvalue << "$label  0x" << hex << aline.address;
            aline.line.replace(aline.line.find("$fa"), 3, argvalue.str());
            break;
        case MASK_TABLE_ADDR:
            argvalue << "table entry: 0x"<< hex << unsigned(0x80 | (shiftMask(it->argmask1, ptr[0]) << 1));
            aline.line.replace(aline.line.find("$ta"), 3, argvalue.str());
            break;
        }
        break;
    }
    if (it == instrList.end())
        return wrong;

    *ipointer += instr_size;
    return aline;
}

