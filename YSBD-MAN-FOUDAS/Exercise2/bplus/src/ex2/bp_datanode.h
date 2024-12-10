#ifndef BP_DATANODE_H
#define BP_DATANODE_H

#include "bf.h"
#include "bp_file.h"

// Function to initialize a data node in a block
// Returns 0 on success, -1 on failure
int initializeDataNode(BF_Block* block);

// Function to insert a record into a data node
// Returns 0 on success, -1 on failure (if node is full or duplicate key)
int insertRecordToDataNode(BF_Block* block, Record record);

// Function to find a record by its ID in a data node
// Returns 0 on success, -1 on failure
// If found, sets *result to point to the record
int findRecordInDataNode(BF_Block* block, int id, Record** result);

// Function to link data nodes (for supporting potential range queries)
// Returns 0 on success, -1 on failure
int linkDataNodes(BF_Block* current_block, int next_block_id);

#endif // BP_DATANODE_H