#ifndef BP_INDEXNODE_H
#define BP_INDEXNODE_H

#include "bf.h"

// Function to initialize an index node in a block
// Returns 0 on success, -1 on failure
int initializeIndexNode(BF_Block* block);

// Function to insert a key and child block ID into an index node
// Returns 0 on success, -1 on failure (if node is full)
int insertKeyToIndexNode(BF_Block* block, int key, int child_block_id);

// Function to find the appropriate child block ID for a given key in an index node
// Returns the block ID of the child node
int findChildBlockId(BF_Block* block, int key);

#endif // BP_INDEXNODE_H