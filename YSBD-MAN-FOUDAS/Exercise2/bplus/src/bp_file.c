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

    //apodesmeuse to allocated block giati graftike sto disko
    BF_Block_Destroy(&block);
    printf("\nCreated file with filename: %s, (BP_CreateFile works)\n\n", fileName);
    
    return 0;
}

// BP_OpenFile implementation
BPLUS_INFO* BP_OpenFile(char *fileName, int *file_desc) {
    
    // Anoigma arxeiou se epipedo Block
    BF_OpenFile(fileName, file_desc);

    
    // Read first block (metadata)
    BF_Block *block;
    BF_Block_Init(&block);
    BF_GetBlock(*file_desc,0,block);
    
    // Copy metadata
    BPLUS_INFO* bplus_info = malloc(sizeof(BPLUS_INFO));
    char* data = BF_Block_GetData(block);
    memcpy(bplus_info, data, sizeof(BPLUS_INFO));
    

    BF_UnpinBlock(block);
    BF_Block_Destroy(&block);
    
    printf("\nOpened file with name: %s (BP_OpenFile works)\n\n", fileName);
    return bplus_info;
}

// BP_CloseFile implementation
int BP_CloseFile(int file_desc, BPLUS_INFO* info) {
    
    BF_Block* block;
    BF_Block_Init(&block);
    CALL_BF(BF_GetBlock(file_desc, 0, block));

    BF_Block_SetDirty(block);
    BF_UnpinBlock(block);
    BF_Block_Destroy(&block);
    free(info);
     
    // Close file at BF level
    CALL_BF(BF_CloseFile(file_desc));
    printf("\nClosed File with file_desc: %d (BP_CloseFile works)\n", file_desc);
    return 0;
}



// BP_InsertEntry implementation
int BP_InsertEntry(int file_desc, BPLUS_INFO* bplus_info, Record record) {
    
    // periptosi pou to dedro ine adeio kai den uparxei riza
    if (bplus_info->tree_height==-1){

        // dimiourgoume to root block me ID = 2 giati to metadata block exei ID=1
        BF_Block* root_block;
        BF_Block_Init(&root_block);      
        CALL_BF(BF_AllocateBlock(file_desc, root_block));  

        // kratame to root_block_id gia na to baloume sta metadata
        int root_block_id;
        CALL_BF(BF_GetBlockCounter(file_desc , &root_block_id));
        root_block_id--;

        printf("\n\nAdeio tree - bazoume riza me ID: %d kai key: %d \n", root_block_id, record.id);

        // ine to proto entry tou dedrou ara tha baloume ena IndexNode me to record.id 
        // kai 2 DataNodes to ena adeio kai to allo me to record
        if (init_IndexNode(root_block)==0){               
            printf("Index Root Node me block ID %d init.\n", root_block_id);
        }

        //bazume to record.id sto root IndexNode
        if(insert_key_to_IndexNode(root_block, record.id) == 0){
            printf("Index Root Node me block ID %d pire key me value %d\n", root_block_id, record.id);
        }
        
        // tora ftiaxnoume ta duo DataNodes pou tha deixnoun ta pointers ths root
        //left data block
        BF_Block* left_data_block;
        BF_Block_Init(&left_data_block);
        CALL_BF(BF_AllocateBlock(file_desc, left_data_block));

        
        int left_data_block_id;
        CALL_BF(BF_GetBlockCounter(file_desc , &left_data_block_id));
        left_data_block_id--;
        if(insert_pointer_to_IndexNode(root_block, left_data_block_id) == 0){
            printf("Index Root Node me block ID %d pire pointer me id %d\n", root_block_id, left_data_block_id);
        }

        //right data block
        BF_Block* right_data_block;
        BF_Block_Init(&right_data_block);
        CALL_BF(BF_AllocateBlock(file_desc, right_data_block));


        int right_data_block_id;
        CALL_BF(BF_GetBlockCounter(file_desc , &right_data_block_id));
        right_data_block_id--;
        if(insert_pointer_to_IndexNode(root_block, right_data_block_id) == 0){
            printf("Index Root Node me block ID %d pire pointer me id %d\n", root_block_id, right_data_block_id);
        }
        

        //tora exoume etoimo to index node tou root kai ta duo pointers
        //meta prepei na kanoume init ta duo data nodes

        // to left_data_node tha ine adeio alla o deiktis tou prepei na deixnei sto right_data_node
        if(init_DataNode(left_data_block)==0){
            insert_pointer_to_DataNode(left_data_block,right_data_block_id);           
            printf("\nData Node me block ID %d init.\n", left_data_block_id);
            printf("Data Node me block ID %d pire pointer %d\n", left_data_block_id, right_data_block_id);
        }   

        // to right_data_node tha periexei to record pou exei record.id to idio me to key tou root
        if(init_DataNode(right_data_block)==0){
            insert_record_to_DataNode(right_data_block, &record);
            printf("\nData Node me block ID %d init.\n", right_data_block_id);
            printf("Data Node me block ID %d pire record %d\n", right_data_block_id, record.id);
        }    

        //update bplus info
        bplus_info->root_block_id = root_block_id;          //apothikevoume to block id sto opoio ine to root
        bplus_info->tree_height=2;                          //kanoume update to tree hight
        bplus_info->total_record_counter++;                 //auksanoume to total record  counter

        printf("\nbplus info updated.......\n");
        printf("updated height to: %d\n\n" , bplus_info->tree_height);

        //kaname allages sta blocks ara set dirty kai unpin gia na graftei sto disko
        BF_Block_SetDirty(root_block);
        BF_UnpinBlock(root_block);

        BF_Block_SetDirty(right_data_block);
        BF_UnpinBlock(right_data_block);

        BF_Block_SetDirty(left_data_block);
        BF_UnpinBlock(left_data_block);

        BF_Block_Destroy(&root_block);
        BF_Block_Destroy(&right_data_block);
        BF_Block_Destroy(&left_data_block);

        return right_data_block_id;  
    }


    //upoloipes periptoseis
    printf("\n--------------------------------------------------------\n");
    
    //ksekinodas apo tin riza tha broume to sosto node sto opoio prepei na ginei
    //eisagogi eggrafis
    int curr_block = bplus_info->root_block_id;
    int curr_level=0;

    //gia debugging
    
    printf("\n\n");
    //perase apo olous tous index nodes sto sosto path
    while(curr_level < bplus_info->tree_height -1){
        
        //dimiourgoume ena temp block gia tin prospelasi tou dedrou
        BF_Block* tmpblock;
        BF_Block_Init(&tmpblock);
        CALL_BF(BF_AllocateBlock(file_desc, tmpblock));

        //kaloume tin GetBlock gia na epistrepsei sto block to BF_Block me block_num = curr_block
        CALL_BF(BF_GetBlock(file_desc, curr_block, tmpblock));

        //kaloume tin find_next_Node gia na epistrepsei to block_num tou epomenou block gia tin prospelasi
        curr_block = find_next_Node(tmpblock,record.id);

        curr_level++;

        //diagrafoume to tmp block se kathe iteration
        BF_Block_SetDirty(tmpblock);
        BF_UnpinBlock(tmpblock);
        BF_Block_Destroy(&tmpblock);
    }

    // dimiourgoume neo block pointer gia na deixnei sto data node sto opoio
    // eftase telika h prospelasi 
    BF_Block* block;
    BF_Block_Init(&block);
    CALL_BF(BF_AllocateBlock(file_desc, block));
    
    // h get block tha deiksei to neo block pointer sto sosto data node
    BF_GetBlock(file_desc, curr_block, block);

    //pleon to block ine ena leaf data node

    //kai kane insert sto leaf node to key
    

    int result = insert_record_to_DataNode(block, &record);

    switch (result) {
        case 0:
            printf("Data Node me block ID %d pire key me value %d \n", curr_block, record.id);
            break;

        case recs_size:
            printf("\nto data node me id %d foulare\n", curr_block);
            // splitarisma
            return 0;

        default:
            // Handle other cases if needed
            printf("Unhandled result: %d\n", result);
            break;
    }


    bplus_info->total_record_counter++;

    BF_UnpinBlock(block);
    BF_Block_SetDirty(block);
    BF_Block_Destroy(&block);

    return curr_block;
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