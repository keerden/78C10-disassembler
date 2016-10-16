#ifndef MATCH_TREE_H_INC
#define MATCH_TREE_H_INC

#include <stdint.h>
#include "instr-set.h"
#include "tree.h"
//  byte 1 ---- byte 2  gegroepeerd op zelfde opcode, gesorteerd op lengte -> opcode


struct structMatchTree {
	uint8_t opcode;
	struct structMatchTree* subnode;
	struct structMatchTree* nextnode;
	Instr* code;
};

typedef struct structMatchTree matchTree;


matchTree* matchTreeInit();
void matchTreeAdd(matchTree* tree, Instr code);
matchTree* matchTreeNext(matchTree* tree);
void matchTreeDelete(matchTree* tree);


#endif
