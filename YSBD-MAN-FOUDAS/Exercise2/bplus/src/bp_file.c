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
/*
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
*/


// BP_CreateFile implementation
int BP_CreateFile(char *fileName) {

    //arxikopoiise to array me ta openfiles
    //initOpenFilesArray();


    // Try to create the file using BF level
    int file_desc;
    BF_Block *block;
    
    BF_Block_Init(&block);
    
    CALL_BF(BF_CreateFile(fileName)); 

    CALL_BF(BF_OpenFile(fileName,&file_desc));
    
    CALL_BF(BF_AllocateBlock(file_desc, block));

    // arxikopoiei ta metadedomena sto block 
    BPLUS_INFO* bplus_info = (BPLUS_INFO*)BF_Block_GetData(block);
    
    //arxikopoioume ta metadedomena tou bplus dedrou
    bplus_info->tree_height = -1;         // to dedro ine adeio - den uparxei riza
    bplus_info->file_desc = file_desc;  
    bplus_info->total_record_counter = 0;

    printf("Metadata block created\n");
    printf("tree_hight              initialized: %d\n", bplus_info->tree_height);
    printf("file_desc               initialized: %d\n", bplus_info->file_desc);
    printf("total_record_counter    initialized: %d\n\n", bplus_info->total_record_counter);

    // theto to block os dirty kai kano unpin
    BF_Block_SetDirty(block);
    CALL_BF(BF_UnpinBlock(block));
    
    // kleise to fakelo
    CALL_BF(BF_CloseFile(file_desc));
    BF_Close();
    //den imaste sigouroi an thelei destroy
    //BF_Block_Destroy(&block);
    printf("\nCreated file with filename: %s, (BP_CreateFile works)\n\n", fileName);
    
    return 0;
}

// BP_OpenFile implementation
BPLUS_INFO* BP_OpenFile(char *fileName, int *file_desc) {
    
    //initOpenFilesArray();
    
    // Open file at BF level
    BF_OpenFile(fileName, file_desc);

    /*
    // Find a free slot in open files array
    int slot = findFreeFileSlot();
    if (slot == -1) {
        BF_CloseFile(*file_desc);
        return NULL;
    }
    */
    
    // Read first block (metadata)
    BF_Block *block;
    BF_Block_Init(&block);
    BF_GetBlock(*file_desc,0,block);
    
    // Copy metadata
    BPLUS_INFO* bplus_info = malloc(sizeof(BPLUS_INFO));
    memcpy(bplus_info, BF_Block_GetData(block), sizeof(BPLUS_INFO));
    
    /*
    // Update open files array
    open_files[slot].is_open = 1;       //kleise auto to slot
    open_files[slot].file_desc = *file_desc;    // update file_desc
    strcpy(open_files[slot].filename, fileName);    // update fileName
    open_files[slot].info = bplus_info;     // update ta metadata
    */
    
    // Unpin block
    BF_UnpinBlock(block);
    //BF_Block_Destroy(&block);

    printf("\nOpened file with name: %s (BP_OpenFile works)\n\n", fileName);

    
    
    return bplus_info;
}

// BP_CloseFile implementation
int BP_CloseFile(int file_desc, BPLUS_INFO* info) {
    
    /*
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
    */
    
    // Close file at BF level
    CALL_BF(BF_CloseFile(file_desc));
    free(info);
    
    // Free metadata and reset slot
   
   /*
    free(open_files[slot].info);
    open_files[slot].is_open = 0;
    open_files[slot].file_desc = -1;
    open_files[slot].filename[0] = '\0';
    open_files[slot].info = NULL;
    
   */

    printf("\nClosed File (BP_CloseFile works\n");
    
    return 0;
}



// BP_InsertEntry implementation
int BP_InsertEntry(int file_desc, BPLUS_INFO* bplus_info, Record record) {
    
    // periptosi pou to dedro ine adeio kai den uparxei riza
    if (bplus_info->tree_height==-1){

        BF_Block* root_block;
        
        BF_Block_Init(&root_block);      

        CALL_BF(BF_AllocateBlock(file_desc, root_block));    

        int root_block_id;
        CALL_BF(BF_GetBlockCounter(file_desc , &root_block_id));

        printf("\n\nAdeio tree - bazoume riza me ID: %d kai key: %d \n", root_block_id, record.id);

        // ine to proto entry tou dedrou ara tha baloume ena IndexNode me to record.id 
        // kai 2 DataNodes to ena adeio kai to allo me to record
        if (init_IndexNode(root_block)==0){               
            printf("Index Root Node with block ID %d is initialized.\n", root_block_id);
        }

        //bazume to record.id sto root IndexNode
        if(insert_key_to_IndexNode(root_block, record.id) == 0){
            printf("\nKey: %d is inserted in Index Root Noode with block with ID: %d\n", record.id, root_block_id);
        }

        
        // tora ftiaxnoume ta duo DataNodes pou tha deixnoun ta pointers ths root
        BF_Block* left_data_block;
        BF_Block_Init(&left_data_block);
        CALL_BF(BF_AllocateBlock(file_desc, left_data_block));
        
        int left_data_block_id;
        CALL_BF(BF_GetBlockCounter(file_desc , &left_data_block_id));
        insert_pointer_to_IndexNode(root_block, left_data_block_id);

        BF_Block* right_data_block;
        BF_Block_Init(&right_data_block);
        CALL_BF(BF_AllocateBlock(file_desc, right_data_block));

        int right_data_block_id;
        CALL_BF(BF_GetBlockCounter(file_desc , &right_data_block_id));
        insert_pointer_to_IndexNode(root_block, right_data_block_id);

        //tora exoume etoimo to index node tou root kai ta duo pointers
        //meta prepei na kanoume init ta duo data nodes

        // to left_data_node tha ine adeio alla o deiktis tou prepei na deixnei sto right_data_node
        if(init_DataNode(left_data_block)==0){
            insert_pointer_to_DataNode(left_data_block,right_data_block_id);           
            printf("\nInited DataNode with id: %d\n", left_data_block_id);
            printf("Inserted pointer with value: %d to DataNode with id: %d\n", right_data_block_id, left_data_block_id);
        }   

        // to right_data_node tha periexei to record pou exei record.id to idio me to key tou root
        if(init_DataNode(right_data_block)==0){
            insert_record_to_DataNode(right_data_block, &record);
            printf("\nInted DataNode with id: %d\n", right_data_block_id);
            printf("Inserted record with record.id: %d to DataNode with id: %d\n", record.id, right_data_block_id);
        }    

        //update bplus info
        bplus_info->root_block_id = root_block_id;          //apothikevoume to block id sto opoio ine to root
        bplus_info->tree_height=2;                          //kanoume update to tree hight
        bplus_info->total_record_counter++;                 //auksanoume to total record  counter

        printf("\nbplus info updated.......\n");
        printf("height = %d\n\n" , bplus_info->tree_height);
        //kaname allages sta blocks ara set dirty kai unpin gia na graftei sto disko
        BF_Block_SetDirty(root_block);
        //BF_UnpinBlock(root_block);

        BF_Block_SetDirty(right_data_block);
        //BF_UnpinBlock(right_data_block);

        BF_Block_SetDirty(left_data_block);
        //BF_UnpinBlock(left_data_block);

        return right_data_block_id;  
    }


    //upoloipes periptoseis
    printf("\n--------------------------------------------------------\n");
    printf("\neimaste stis upoloipes periptoseis ektos tis rizas META TO if()\n\n");
    //dimiourgoume neo block
    BF_Block* block;
    BF_Block_Init(&block);
    CALL_BF(BF_AllocateBlock(file_desc, block));

   
    //ksekinodas apo tin riza tha broume to sosto node sto opoio prepei na ginei
    //eisagogi eggrafisn
    int curr_node = bplus_info->root_block_id;
    int curr_level=0;

    printf("bainoume sto while curr_node = %d and curr_level = 0\n\n", curr_node);
    
    //perase apo olous tous index nodes sto sosto path
    while(curr_level <= bplus_info->tree_height -1){
        printf("mesa stin while ( prin to GetBlock ):\n");

        //bres pointer gia curr node
        CALL_BF(BF_GetBlock(file_desc, 0, block));
        

        printf("mesa stin while ( META to GetBlock ):\n");
        printf("curr_node = %d and curr_level = %d\n", curr_node, curr_level);

        //bres to sosto path gia to epomeno node kai kane update to curr node
        curr_node = find_next_Node(block,record.id);
        
        //unpin unused block
        BF_UnpinBlock(block);
        curr_level++;
    }

    printf("\n--------------------------------------------------------\n");
    printf("Bghkame apo tin while kai exoume curr_block = %d and curr_level = %d", curr_node, curr_level);
    
    //otan ftasoume se node fullo pare pointer gia to block
    BF_GetBlock(file_desc, curr_node, block);

    //pleon to block ine ena leaf data node

    //kai kane insert sto leaf node to key
    if(insert_record_to_DataNode(block, &record) == 0){
        printf("\nDeftero Insert doulepse\n");
    }
    
    if(insert_record_to_DataNode(block, &record) == recs_size){

        printf("\nto root foulare!\n");
        //splitarisma
        return 0;
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