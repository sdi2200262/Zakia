#ifndef BP_DATANODE_H
#define BP_DATANODE_H

#include "record.h"
#include "bf.h"
#include "bp_file.h"
#include "bp_indexnode.h"

#define recs_size ( 500 / sizeof(Record))



// ena DataNode ine ena BF_Block ara periexei 512bytes
// o pinakas pou periexei tis eggrafes tha ine:
//      {512 - ( 2 * sizeof(int) ) } / sizeof(Record)
typedef struct DataNode{
    int parent_id;
    int next_data_node;     // apothikevoume to block number tou epomenou datanode apo ta deksia - 4 bytes
    int recs_counter;   // apothikeoume to sunolo ton eggrafon pou einai mesa ston pinaka - 4 bytes
    Record recs[recs_size];      //pinakas pou periexei eggrafes

} DataNode;


int init_DataNode(BF_Block* block);
int insert_record_to_DataNode( BF_Block* block , Record* record);
int insert_pointer_to_DataNode( BF_Block* block, int new_block_id );
int set_parent_id_to_DataNode(BF_Block* block, int parent_block_id);
int debug_Data(BF_Block* block);
int split_DataNode(int file_desc, BF_Block* block, int split_block_id, BF_Block* new_block, int* new_index_key, int* new_block_id,  Record rec);
int search_record(BF_Block *block, int id, Record **result);


#endif 