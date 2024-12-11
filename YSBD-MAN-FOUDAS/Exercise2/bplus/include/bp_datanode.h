#ifndef BP_DATANODE_H
#define BP_DATANODE_H

#include "record.h"
#include "bf.h"
#include "bp_file.h"
#include "bp_indexnode.h"

#define MAX_RECORDS 2 

typedef struct DataNode
{
    int next_block_ptr;  // pointer gia to epomeno data node block pros ta deksia
    Record recs[MAX_RECORDS];      // pinakas me ta records 
    int record_counter;     // counter me ta recs tou pinaka 
    int parent_id;      // index parent block id
}DataNode;


int init_DataNode(BF_Block* block);
int insert_DataNode( BF_BLOCK* block ,Record *rec);
//int split_data(BF_Block *block, BF_Block *newblock, Record *rec);
//int search_record(BF_Block *block, int id, Record **result);


#endif 