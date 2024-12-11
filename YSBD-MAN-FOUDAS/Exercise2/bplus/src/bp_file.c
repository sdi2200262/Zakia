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
    bplus_info->total_record_counter = 0;

    // theto to block os dirty kai kano unpin
    BF_Block_SetDirty(block);
    CALL_BF(BF_UnpinBlock(block));
    
    // kleise to fakelo
    CALL_BF(BF_CloseFile(file_desc));
    BF_Close();
    //den imaste sigouroi an thelei destroy
    //BF_Block_Destroy(&block);
    printf("\nCreated file (BP_CreateFile works)\n");
    
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

    printf("\nOpened file (BP_OpenFile works)\n");
    
    
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
    

    printf("\nClosed File (BP_CloseFile works\n");
    
    return 0;
}



// BP_InsertEntry implementation
int BP_InsertEntry(int file_desc, BPLUS_INFO* bplus_info, Record record) {
    
    // periptosi pou to dedro ine adeio kai den uparxei riza
    if (bplus_info->tree_height==-1){

        BF_Block* block;
        
        BF_Block_Init(&block);

        CALL_BF(BF_AllocateBlock(file_desc, block));

        printf("\nAdeio tree - bazoume riza\n");

        if (init_DataNode(block)==0){
            printf("\n init data node workds!");
        }

        if(insert_DataNode(block, &record) == 0){
            printf("\nInsert doulepse\n");

        }

        //update bplus info
        bplus_info->root = BF_Block_GetData(block);  //apothikevoume to block id sto opoio ine to root
        bplus_info->tree_height=1;
        bplus_info->total_record_counter++;      

        BF_Block_SetDirty(block);
        BF_UnpinBlock(block);

        return block;  
    }

    //upoloipes periptoseis
    BF_Block* block;
    BF_Block_Init(&block);

   
    //ksekinodas apo tin riza tha broume to sosto node sto opoio prepei na ginei
    //eisagogi eggrafis
    int curr_node = bplus_info->root;
    int curr_level=0;

    //perase apo olous tous index nodes sto sosto path
    while(level< bplus_info->tree_height -1){
        
        //bres pointer gia curr node
        BF_GetBlock(file_desc, curr_node, block);

        //bres to sosto path gia to epomeno node kai kane update to curr node
        curr_node = find_path(block,record);
        
        //unpin unused block
        BF_UnpinBlock(block);
        curr_level++;
    }


    //otan ftasoume se node fullo pare pointer gia to block
    BF_GetBlock(file_desc, curr_node, block);

    //kai kane insert sto leaf node to key
    if(insert_DataNode(block, &record) == 0){
        printf("\nInsert doulepse\n");
    }

    bplus_info->total_record_counter++;

    BF_Block_SetDirty(block);
    BF_UnpinBlock(block);

    return curr_node;
    
}
    

/*
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