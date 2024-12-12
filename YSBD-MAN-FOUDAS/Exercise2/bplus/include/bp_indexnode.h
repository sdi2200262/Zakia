#ifndef BP_INDEX_NODE_H
#define BP_INDEX_NODE_H

#include "record.h"
#include "bf.h"
#include "bp_file.h"
#include "bp_datanode.h"

// ena IndexNode ine ena BF_Block ara periexei 512bytes
// o pinakas pou periexei ta kleidia tha ine:
//      {(512 / sizeof(int)) / 2} -1      giati theloume na einai ena ligotero apo ta pointers
//
// o pinakas pou periexei ta pointers ( block id numbers ) tha ine:
//       (512 / sizeof(int) / 2)   
//
// ta 4 bytes pou perisseoun antistoixoun sto int key_counter
// etsi desmeuoume olo to diathesimo xoro tou block !!!

int keys_size = ((512/ sizeof(int) ) /2) - 1;
int pointers_size = ((512/ sizeof(int) ) /2);

typedef struct IndexNode{

    int keys_counter;
    int keys[keys_size];
    int pointer[pointers_size];

} IndexNode;

#endif

int init_IndexNode(BF_Block* block);
int insert_key_to_IndexNode(BF_Block* block, int key);
int insert_pointer_to_IndexNode(BF_Block* block, int new_block_id);
int find_next_Node(BF_Block* block, int key);