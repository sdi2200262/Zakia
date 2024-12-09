// File: bp_datanode.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bf.h"
#include "bp_file.h"
#include "bp_datanode.h"

// Define the maximum number of records in a data node
#define MAX_RECORDS 2

// Structure for a data node in B+ tree
typedef struct {
    int is_leaf;           // Flag to indicate if this is a leaf node
    int num_records;       // Number of records in the node
    int index_block_id;   // Block ID of the parent node
    Record records[MAX_RECORDS];  // Array of records
    int next_block_id;     // deiktis sto epomeno pros ta deksia apo ta fulla
} DataNode;

// Function to initialize a data node
int initializeDataNode(BF_Block* block) {
    DataNode* data_node = (DataNode*)BF_Block_GetData(block);
    
    // Initialize node properties
    data_node->is_leaf = 1;  // This is a data node
    data_node->num_records = 0;
    data_node->index_block_id = -1;
    data_node->next_block_id = -1;
    
    // Initialize records to empty/zero
    for (int i = 0; i < MAX_RECORDS; i++) {
        data_node->records[i].id = -1;
        // You might want to initialize other record fields as well
    }
    
    return 0;
}

// Function to insert a record into a data node
int insertRecordToDataNode(BF_Block* block, Record record) {
    DataNode* data_node = (DataNode*)BF_Block_GetData(block);
    
    if (data_node->num_records >= MAX_RECORDS) {
        // Node is full, needs splitting
        return MAX_RECORDS;
    }
    
    // Check for duplicate keys
    for (int i = 0; i < data_node->num_records; i++) {
        if (data_node->records[i].id == record.id) {
            printf("Duplicate found at datanode: %d \n", data_node->records[i].id);
            // Duplicate key not allowed
            return -1;
        }
    }
    
    
    // elegxoume se poia thesi tha baloume to neo record
    int i;
    // pame stin teleutaia thesi eggrafis kai tsekaroume an einai megaliteri apo to record
    // pou theloume na eisagoume
    // an einai tote ta kanoume swap kai auti i diadikasia ginetai mexri na min isxuei to condition
    // etsi diateiroume auskousa seira ton records sto datanode
    for (i = data_node->num_records; i > 0 && data_node->records[i-1].id > record.id; i--) {
        data_node->records[i] = data_node->records[i-1];
    }
    
    // eisagoume to neo record
    data_node->records[i] = record;
    data_node->num_records++;
    
    // petuxe epistrefoume kouloura
    return 0;
}

// Function to find a record by its ID in a data node
int findRecordInDataNode(BF_Block* block, int id, Record** result) {
    DataNode* data_node = (DataNode*)BF_Block_GetData(block);
    
    for (int i = 0; i < data_node->num_records; i++) {
        if (data_node->records[i].id == id) {
            *result = &(data_node->records[i]);
            return 0;
        }
    }
    
    // Record not found
    *result = NULL;
    return -1;
}

// Function to link data nodes (for supporting range queries)
int linkDataNodes(BF_Block* current_block, int next_block_id) {
    DataNode* data_node = (DataNode*)BF_Block_GetData(current_block);
    data_node->next_block_id = next_block_id;
    return 0;
}