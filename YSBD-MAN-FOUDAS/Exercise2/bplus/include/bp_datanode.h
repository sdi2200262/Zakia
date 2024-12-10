#ifndef BP_DATANODE_H
#define BP_DATANODE_H
#define MAX_RECS 2
#include <record.h>
#include <record.h>
#include <bf.h>
#include <bp_file.h>
#include <bp_indexnode.h>



typedef struct DataNode
{
    int nextBlock;  // pointer
    int recCount;
    Record recs[MAX_RECS];
}DataNode;


int initializeDataNode(DataNode* node);
int insertToDataNode(BF_Block *block, Record *rec, bool split);
int split_data(BF_Block *block, BF_Block *newblock, Record *rec);
int search_record(BF_Block *block, int id, Record **result);


#endif 