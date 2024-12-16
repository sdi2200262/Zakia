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


#define keys_size (((512 / sizeof(int)) / 2) - 2)
#define pointers_size (((512 / sizeof(int)) / 2) - 1)

typedef struct IndexNode{
    int parent_id;
    int keys_counter;
    int pointers_counter;
    int keys[keys_size];
    int pointers[pointers_size];

} IndexNode;


int init_IndexNode(BF_Block* block);
int insert_key_to_IndexNode(BF_Block* block, int key);
int insert_pointer_to_IndexNode(BF_Block* block, int new_block_id);
int set_parent_id_to_IndexNode(BF_Block* block, int parent_block_id);
int insert_split_pointer_to_IndexNode(BF_Block* block, int new_block_id, int split_block_id);
int find_next_Node(BF_Block* block, int key);
int find_leftest_Node(BF_Block* block);
//int split_indexNode(int file_desc, BPLUS_INFO* bplus_info, BF_Block* block, int split_block_id, BF_Block* new_block, int* new_index_key, int* new_block_id, int key);
int debug_Index(BF_Block* block);

#endif