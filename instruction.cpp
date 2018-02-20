#include <string>
#include <iostream>
#include "instruction.h"



using namespace std;


bool Instruction::hasMaskedArgs()
{
	if(	this->arg == BYTE_2_OFFSET_BIT_MASK ||
	   	this->arg == MASK_JDISP ||
	   	this->arg == MASK_FIXED_ADDR ||
	   	this->arg == MASK_TABLE_ADDR)
	   	
		return true;
	else
		return false;
}

bool Instruction::has2ndMatchByte()
{
	if(	this->length > 1 &&
        (this->arg == NO_ARGS ||
		this->arg == BYTE_3_DATA ||
		this->arg == BYTE_3_OFFSET ||
		this->arg == BYTE_3_DATA_OPT ||
		this->arg == WORD_3_4_ADDR ||
		this->arg == MASK_JDISP ||
		this->arg == MASK_FIXED_ADDR ||
		this->arg == MASK_TABLE_ADDR ))
		return true;
	else
		return false;	
}

void Instruction::print() {

   cout << dec << int(this->length) << " " << hex <<  int(this->byte1) <<" " << int(this->byte2) << " " << this->mnemonic << endl;
}