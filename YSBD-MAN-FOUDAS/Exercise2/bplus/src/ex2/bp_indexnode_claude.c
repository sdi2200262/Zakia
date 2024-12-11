// File: bp_indexnode.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bf.h"
#include "bp_file.h"
#include "bp_indexnode.h"

// Define the maximum number of keys in an index node
#define MAX_INDEX_KEYS 10

// Structure for an index node in B+ tree
typedef struct {
    int is_leaf;           // Flag to indicate if this is a leaf node
    int num_keys;          // Number of keys in the node
    int parent_block_id;   // Block ID of the parent node
    int keys[MAX_INDEX_KEYS];   // Array of keys
    int child_block_ids[MAX_INDEX_KEYS + 1];  // Array of child block IDs
} IndexNode;


// Function to initialize an index node
int initializeIndexNode(BF_Block* block) {
    IndexNode* index_node = (IndexNode*)BF_Block_GetData(block);
    
    // Initialize node properties
    index_node->is_leaf = 0;  // This is an index node
    index_node->num_keys = 0;
    index_node->parent_block_id = -1;
    
    // Initialize keys and child block IDs to -1
    for (int i = 0; i < MAX_INDEX_KEYS; i++) {
        index_node->keys[i] = -1;
        index_node->child_block_ids[i] = -1;
    }
    index_node->child_block_ids[MAX_INDEX_KEYS] = -1;
    
    return 0;
}

// Function to insert a key and child block ID into an index node
int insertKeyToIndexNode(BF_Block* block, int key, int child_block_id) {
    IndexNode* index_node = (IndexNode*)BF_Block_GetData(block);
    
    if (index_node->num_keys >= MAX_INDEX_KEYS) {
        // Node is full, needs splitting
        return -1;
    }
    
    // Find the correct position to insert the key
    int i;
    for (i = index_node->num_keys; i > 0 && index_node->keys[i-1] > key; i--) {
        index_node->keys[i] = index_node->keys[i-1];
        index_node->child_block_ids[i+1] = index_node->child_block_ids[i];
    }
    
    // Insert the new key and child block ID
    index_node->keys[i] = key;
    index_node->child_block_ids[i+1] = child_block_id;
    index_node->num_keys++;
    
    return 0;
}

// Function to find the child block ID for a given key in an index node
int findChildBlockId(BF_Block* block, int key) {
    IndexNode* index_node = (IndexNode*)BF_Block_GetData(block);
    
    for (int i = 0; i < index_node->num_keys; i++) {
        if (key <= index_node->keys[i]) {
            return index_node->child_block_ids[i];
        }
    }
    
    // If key is larger than all keys, return the rightmost child
    return index_node->child_block_ids[index_node->num_keys];
}