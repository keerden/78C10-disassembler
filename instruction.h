#ifndef INSTRUCTION_H
#define INSTRUCTION_H
 
#include <cstdint>
#include <string>
#include "registerset.h"


enum argType {
	NO_ARGS,
	BYTE_2_DATA,
	BYTE_3_DATA,
	BYTE_2_OFFSET,
	BYTE_2_OFFSET_BIT_MASK,
	BYTE_3_OFFSET,
	BYTE_2_OFFSET_3_DATA,
	BYTE_2_DATA_OPT, 
	BYTE_3_DATA_OPT,
	WORD_2_3,
	WORD_2_3_ADDR,
	WORD_3_4_ADDR,
	MASK_JDISP,
	MASK_FIXED_ADDR,
	MASK_TABLE_ADDR	
};

class Instruction
{
    public:
	uint8_t length;
	uint8_t byte1;
	uint8_t byte2;
	enum argType arg;
	regMap* reg;
	uint8_t argmask1;
	uint8_t argmask2;	
	std::string mnemonic;
    bool jumps;
    bool skips;
    bool noreturn;	

    Instruction(    uint8_t len, 
                    uint8_t b1, 
                    uint8_t b2, 
                    enum argType a, 
                    regMap * r, 
                    uint8_t am1, 
                    uint8_t am2, 
                    std::string mnem,
                    bool jps,
                    bool sks,
                    bool nret
                ) :
                    length(len),
                    byte1(b1),
                    byte2(b2),
                    arg(a),
                    reg(r),
                    argmask1(am1),
                    argmask2(am2),
                    mnemonic(mnem),
                    jumps(jps),
                    skips(sks),
                    noreturn(nret) 
               {};
    Instruction() : Instruction(0,0,0,NO_ARGS,NULL,0,0,"", false, false, false){};
    bool has2ndMatchByte();
    bool hasMaskedArgs();
    void print();

};

#endif