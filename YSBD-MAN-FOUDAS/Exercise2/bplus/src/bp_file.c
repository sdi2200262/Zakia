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

#define CALL_BF(call)         \
  {                           \
    BF_ErrorCode code = call; \
    if (code != BF_OK)        \
    {                         \
      BF_PrintError(code);    \
      return bplus_ERROR;     \
    }                         \
  }


typedef struct {
    int is_open;
    int file_desc;
    char filename[255];
    BPLUS_INFO* info;
} OpenFileEntry;

static OpenFileEntry open_files[MAXOPENFILES];

// Initialize open files array
void initOpenFilesArray() {
    
    static int initialized = 0;     //check an ine already initialized

    if (!initialized) {
        for (int i = 0; i < MAXOPENFILES; i++) {
            open_files[i].is_open = 0;      //set slot open
            open_files[i].file_desc = -1;       //
            open_files[i].filename[0] = '\0';
            open_files[i].info = NULL;
        }
        initialized = 1;        //mark as initialized
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

    //arxikopoiise to array me ta openfiles
    initOpenFilesArray();


    // Try to create the file using BF level
    int file_desc;
    BF_Block *block;
    
    CALL_BF(BF_CreateFile(fileName)); 
    
    CALL_BF(BF_OpenFile(fileName,&file_desc));

    BF_Block_Init(&block);
    
    CALL_BF(BF_AllocateBlock(file_desc, block));

    
    // arxikopoiei ta metadedomena sto block 
    BPLUS_INFO* bplus_info = (BPLUS_INFO*)BF_Block_GetData(block);
    
    //arxikopoioume ta metadedomena tou bplus dedrou
    bplus_info->tree_height = -1;         // to dedro ine adeio - den uparxei riza
    bplus_info->file_desc = 0;  
    bplus_info->record_counter = 0;

    // theto to block os dirty kai kano unpin
    BF_Block_SetDirty(block);
    CALL_BF(BF_UnpinBlock(block));
    
    // kleise to fakelo
    CALL_BF(BF_CloseFile(file_desc));
    BF_Close();
    //den imaste sigouroi an thelei destroy
    //BF_Block_Destroy(&block);
    
    return 0;
}

// BP_OpenFile implementation
BPLUS_INFO* BP_OpenFile(char *fileName, int *file_desc) {
    
    initOpenFilesArray();
    
    // Open file at BF level
    BF_OpenFile(fileName, file_desc);


    // Find a free slot in open files array
    int slot = findFreeFileSlot();
    if (slot == -1) {
        BF_CloseFile(*file_desc);
        return NULL;
    }
    
    // Read first block (metadata)
    BF_Block *block;
    BF_Block_Init(&block);
    BF_GetBlock(*file_desc,0,block);
    
    // Copy metadata
    BPLUS_INFO* bplus_info = malloc(sizeof(BPLUS_INFO));
    memcpy(bplus_info, BF_Block_GetData(block), sizeof(BPLUS_INFO));
    
    // Update open files array
    open_files[slot].is_open = 1;       //kleise auto to slot
    open_files[slot].file_desc = *file_desc;    // update file_desc
    strcpy(open_files[slot].filename, fileName);    // update fileName
    open_files[slot].info = bplus_info;     // update ta metadata
    
    // Unpin block
    BF_UnpinBlock(block);
    //BF_Block_Destroy(&block);

    printf("\nCalled OpenFile for %d here is the OpenFiles Array:\n", *file_desc);
    for (int i=0; i <MAXOPENFILES; i++ ){
        printf("keli  %d: %d , %d , %s , kai bplus info\n",i , open_files[i].is_open, open_files[i].file_desc, open_files[i].filename);
    }
    
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
    
    // to file den itan anoixto h de brethike
    if (slot == -1) {
        printf("\nClose File returned -1\nFile is already closed or doesnt exist\n");
        return -1;
    }
    
    // Close file at BF level
    CALL_BF(BF_CloseFile(file_desc));
    
    // Free metadata and reset slot
    free(open_files[slot].info);
    open_files[slot].is_open = 0;
    open_files[slot].file_desc = -1;
    open_files[slot].filename[0] = '\0';
    open_files[slot].info = NULL;
    

    printf("\nCalled CloseFile for %d here is the OpenFiles Array:\n", file_desc);
    for (int i=0; i <MAXOPENFILES; i++ ){
        printf("keli  %d: %d , %d , %s , kai bplus info\n",i , open_files[i].is_open, open_files[i].file_desc, open_files[i].filename);
    }
    return 0;
}



// BP_InsertEntry implementation
int BP_InsertEntry(int file_desc, BPLUS_INFO* bplus_info, Record record) {
    
    BF_Block *block;
    
    BF_Block_Init(&block);

    CALL_BF(BF_AllocateBlock(file_desc, block));

    if (init_DataNode(block)){
        printf("\n init data node workds!");
        printf("\n record is: %d\n", record.id);

    }

    return 0;
}
    /*
    // tsekaroume an ine to proto entry dld an exei riza
    if (bplus_info->tree_hight == -1) {
        
        // desmeuoume xoro gia tin riza
        CALL_BF(BF_AllocateBlock(file_desc, &block));
        
        // arxikopoiisi datanode
        initializeDataNode(block);
        
        // eisagoume to neo record
        if (insertRecordToDataNode(block, record) != 0) {
            BF_Block_Destroy(&block);
            return -1;
        }
        
        // Update B+ tree metadata
        bplus_info->root_block_data = CALL_BF(BF_Block_GetData(block));     //root_block = BF_getblocknum(block)
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
*/