#ifndef INSTR_SET_H_INC
#define INSTR_SET_H_INC

#include <stdint.h>
#include <string>
#include <map>
#include <list>
#include <vector>


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

typedef std::map<uint8_t,std::string> regMap;

struct structInstruction {
	uint8_t length;
	uint8_t byte1;
	uint8_t byte2;
	enum argType arg;
	regMap* reg;
	uint8_t argmask1;
	uint8_t argmask2;	
	std::string mnemonic;	
};


typedef struct structInstruction Instr;

typedef std::vector<Instr> InstrGroup;
typedef std::list<Instr> InstrList;

extern regMap reg_no;
extern regMap reg_r;
extern regMap reg_r1;
extern regMap reg_r2;
extern regMap reg_rpa;
extern regMap reg_rpa1;
extern regMap reg_rpa2;
extern regMap reg_rpa3;
extern regMap reg_sr;
extern regMap reg_sr1;
extern regMap reg_sr2;
extern regMap reg_sr3;
extern regMap reg_sr4;
extern regMap reg_rp;
extern regMap reg_rp1;
extern regMap reg_rp2;
extern regMap reg_rp3;
extern regMap reg_f;
extern regMap reg_irf;	
extern InstrGroup instructionSet; 

InstrList* initInstrSet(InstrGroup iset);
void InstrListInsert (InstrList& lst, Instr in);
bool has2ndMatchByte(enum argType arg);
bool hasMaskedArgs(enum argType arg);


#endif
