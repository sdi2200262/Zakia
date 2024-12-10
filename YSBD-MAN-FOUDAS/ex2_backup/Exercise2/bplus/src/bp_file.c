// File: bp_file.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bf.h"
#include "bp_file.h"
#include "bp_indexnode.h"
#include "bp_datanode.h"

// Global array to track open files
#define MAXOPENFILES 20
typedef struct {
    int is_open;
    int file_desc;
    char filename[255];
    BPLUS_INFO* info;
} OpenFileEntry;

static OpenFileEntry open_files[MAXOPENFILES];

// Initialize open files array
void initOpenFilesArray() {
    static int initialized = 0;
    if (!initialized) {
        for (int i = 0; i < MAXOPENFILES; i++) {
            open_files[i].is_open = 0;
            open_files[i].file_desc = -1;
            open_files[i].filename[0] = '\0';
            open_files[i].info = NULL;
        }
        initialized = 1;
    }
}

// Find a free slot in open files array
int findFreeFileSlot() {
    initOpenFilesArray();
    for (int i = 0; i < MAXOPENFILES; i++) {
        if (!open_files[i].is_open) {
            return i;
        }
    }
    return -1;
}

// BP_CreateFile implementation
int BP_CreateFile(char *fileName) {
    initOpenFilesArray();
    
    // Try to create the file using BF level
    int file_desc;
    BF_Block *block;
    BF_Block_Init(&block);
    
    if (BF_CreateFile(fileName) != BF_OK) {
        BF_Block_Destroy(&block);
        return -1;
    }
    
    // Open the file
    if (BF_OpenFile(fileName, &file_desc) != BF_OK) {
        BF_Block_Destroy(&block);
        return -1;
    }
    
    // Allocate first block for metadata
    if (BF_AllocateBlock(file_desc, block) != BF_OK) {
        BF_CloseFile(file_desc);
        BF_Block_Destroy(&block);
        return -1;
    }
    
    // Initialize metadata in first block
    BPLUS_INFO* bplus_info = (BPLUS_INFO*)BF_Block_GetData(block);
    strcpy(bplus_info->filename, fileName);
    bplus_info->root_block = -1;  // No root yet
    bplus_info->height = 0;
    bplus_info->total_records = 0;
    
    // Mark block as dirty and unpin
    BF_Block_SetDirty(block);
    BF_UnpinBlock(block);
    
    // Close the file
    BF_CloseFile(file_desc);
    BF_Block_Destroy(&block);
    
    return 0;
}

// BP_OpenFile implementation
BPLUS_INFO* BP_OpenFile(char *fileName, int *file_desc) {
    initOpenFilesArray();
    
    // Open file at BF level
    if (BF_OpenFile(fileName, file_desc) != BF_OK) {
        return NULL;
    }
    
    // Find a free slot in open files array
    int slot = findFreeFileSlot();
    if (slot == -1) {
        BF_CloseFile(*file_desc);
        return NULL;
    }
    
    // Read first block (metadata)
    BF_Block *block;
    BF_Block_Init(&block);
    if (BF_GetBlock(*file_desc, 0, block) != BF_OK) {
        BF_CloseFile(*file_desc);
        BF_Block_Destroy(&block);
        return NULL;
    }
    
    // Copy metadata
    BPLUS_INFO* bplus_info = malloc(sizeof(BPLUS_INFO));
    memcpy(bplus_info, BF_Block_GetData(block), sizeof(BPLUS_INFO));
    
    // Update open files array
    open_files[slot].is_open = 1;
    open_files[slot].file_desc = *file_desc;
    strcpy(open_files[slot].filename, fileName);
    open_files[slot].info = bplus_info;
    
    // Unpin block
    BF_UnpinBlock(block);
    BF_Block_Destroy(&block);
    
    return bplus_info;
}

// BP_CloseFile implementation
int BP_CloseFile(int file_desc, BPLUS_INFO* info) {
    // Find the file in open files array
    int slot = -1;
    for (int i = 0; i < MAXOPENFILES; i++) {
        if (open_files[i].file_desc == file_desc) {
            slot = i;
            break;
        }
    }
    
    if (slot == -1) {
        return -1;
    }
    
    // Close file at BF level
    if (BF_CloseFile(file_desc) != BF_OK) {
        return -1;
    }
    
    // Free metadata and reset slot
    free(open_files[slot].info);
    open_files[slot].is_open = 0;
    open_files[slot].file_desc = -1;
    open_files[slot].filename[0] = '\0';
    open_files[slot].info = NULL;
    
    return 0;
}

// BP_InsertEntry implementation
int BP_InsertEntry(int file_desc, BPLUS_INFO* bplus_info, Record record) {
    BF_Block *block;
    BF_Block_Init(&block);
    
    // If no root, create first data node as root
    if (bplus_info->root_block == -1) {
        if (BF_AllocateBlock(file_desc, block) != BF_OK) {
            BF_Block_Destroy(&block);
            return -1;
        }
        
        // Initialize as data node
        initializeDataNode(block);
        
        // Insert record
        if (insertRecordToDataNode(block, record) != 0) {
            BF_Block_Destroy(&block);
            return -1;
        }
        
        // Update B+ tree metadata
        bplus_info->root_block = BF_GetBlockNum(block);
        bplus_info->height = 1;
        bplus_info->total_records = 1;
        
        // Mark block dirty and unpin
        BF_Block_SetDirty(block);
        BF_UnpinBlock(block);
        
        return bplus_info->root_block;
    }
    
    // Traverse to find appropriate leaf node
    int current_block = bplus_info->root_block;
    int level = 0;
    
    while (level < bplus_info->height - 1) {
        if (BF_GetBlock(file_desc, current_block, block) != BF_OK) {
            BF_Block_Destroy(&block);
            return -1;
        }
        
        // Find child block for this key
        current_block = findChildBlockId(block, record.id);
        BF_UnpinBlock(block);
        level++;
    }
    
    // At leaf level, get block
    if (BF_GetBlock(file_desc, current_block, block) != BF_OK) {
        BF_Block_Destroy(&block);
        return -1;
    }
    
    // Insert record
    if (insertRecordToDataNode(block, record) != 0) {
        BF_UnpinBlock(block);
        BF_Block_Destroy(&block);
        return -1;  // Duplicate key or node full
    }
    
    // Update metadata
    bplus_info->total_records++;
    
    // Mark block dirty and unpin
    BF_Block_SetDirty(block);
    BF_UnpinBlock(block);
    
    return current_block;
}

// BP_GetEntry implementation
int BP_GetEntry(int file_desc, BPLUS_INFO* header_info, int id, Record** result) {
    // Start from root block
    int current_block = header_info->root_block;
    int level = 0;
    
    BF_Block *block;
    BF_Block_Init(&block);
    
    // Traverse to leaf level
    while (level < header_info->height - 1) {
        if (BF_GetBlock(file_desc, current_block, block) != BF_OK) {
            BF_Block_Destroy(&block);
            *result = NULL;
            return -1;
        }
        
        // Find child block for this id
        current_block = findChildBlockId(block, id);
        BF_UnpinBlock(block);
        level++;
    }
    
    // At leaf level, get block
    if (BF_GetBlock(file_desc, current_block, block) != BF_OK) {
        BF_Block_Destroy(&block);
        *result = NULL;
        return -1;
    }
    
    // Search for record in data node
    int ret = findRecordInDataNode(block, id, result);
    
    // Unpin block
    BF_UnpinBlock(block);
    BF_Block_Destroy(&block);
    
    return ret;
}