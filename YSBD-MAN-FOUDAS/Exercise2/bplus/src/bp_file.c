#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bf.h"
#include "bp_file.h"
#include "bp_indexnode.h"
#include "bp_datanode.h"

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


int BP_CreateFile(char *fileName) {

    //arxikopoiise to array me ta openfiles
    //initOpenFilesArray();


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

BPLUS_INFO* BP_OpenFile(char *fileName, int *file_desc) {
    
    // Anoigma arxeiou se epipedo Block
    BF_OpenFile(fileName, file_desc);

    
    BF_Block *block;
    BF_Block_Init(&block);
    BF_GetBlock(*file_desc,0,block);
    
    void* data = BF_Block_GetData(block);
    BPLUS_INFO* bplus_info = (BPLUS_INFO*) data;
    

    BF_UnpinBlock(block);
    BF_Block_Destroy(&block);
    
    printf("\nOpened file with name: %s (BP_OpenFile works)\n\n", fileName);
    return bplus_info;
}

int BP_CloseFile(int file_desc, BPLUS_INFO* info) {
    
    BF_Block* block;
    BF_Block_Init(&block);
    CALL_BF(BF_GetBlock(file_desc, 0, block));

    BF_Block_SetDirty(block);
    BF_UnpinBlock(block);
    BF_Block_Destroy(&block);
    
     
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
        //riza den exei parent node ara to parent id tha pairnei -1
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
        //riza den exei parent node ara to parent id tha pairnei -1
        if(insert_pointer_to_IndexNode(root_block, right_data_block_id) == 0){
            printf("Index Root Node me block ID %d pire pointer me id %d\n", root_block_id, right_data_block_id);
        }
        

        //tora exoume etoimo to index node tou root kai ta duo pointers
        //meta prepei na kanoume init ta duo data nodes

        // to left_data_node tha ine adeio alla o deiktis tou prepei na deixnei sto right_data_node
        if(init_DataNode(left_data_block)==0){
            //to parent node ine to root kai to node apo ta deksia einai to right_data_block
            insert_pointer_to_DataNode(left_data_block,right_data_block_id);
            set_parent_id_to_DataNode(left_data_block, root_block_id);           
            printf("\nData Node me block ID %d init.\n", left_data_block_id);
            printf("Data Node me block ID %d pire pointer %d\n", left_data_block_id, right_data_block_id);
        }   

        // to right_data_node tha periexei to record pou exei record.id to idio me to key tou root
        if(init_DataNode(right_data_block)==0){
            insert_record_to_DataNode(right_data_block, &record);
            //to parent node ine to root 
            set_parent_id_to_DataNode(right_data_block, root_block_id);
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
    printf("\nNew entry is: %d\n\n", record.id);

    //ksekinodas apo tin riza tha broume to sosto node sto opoio prepei na ginei
    //eisagogi eggrafis
    int curr_block = bplus_info->root_block_id;
    int curr_level=0;

    //perase apo olous tous index nodes sto sosto path
    while(curr_level < bplus_info->tree_height -1){
        
        //dimiourgoume ena temp block gia tin prospelasi tou dedrou
        BF_Block* tmpblock;
        BF_Block_Init(&tmpblock);
        //CALL_BF(BF_AllocateBlock(file_desc, tmpblock));

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
            printf("SPLIT\n");
            //to curr_block ine full ara dimiourgoume neo data_block gia na parei ta misa entries tou curr_block
            BF_Block* new_block;
            BF_Block_Init(&new_block);
            CALL_BF(BF_AllocateBlock(file_desc, new_block));
            
            int new_index_key;
            int new_block_id;
            int split = split_DataNode(file_desc, block, curr_block, new_block, &new_index_key, &new_block_id, record);
            if (split == 0){
                printf("\nto data node me id %d foulare\n", curr_block);
                printf("\ndimiourgithike neo block me id %d\n",new_block_id);
                printf("\no goneas tou neou block pire neo index key %d\n", new_index_key);

                //kane set dirty OLA ta blocks
                //kane unpin OLA ta blocks gia na graftoun ston disko
                BF_Block_SetDirty(block);
                BF_Block_SetDirty(new_block);
                BF_UnpinBlock(block);
                BF_UnpinBlock(new_block);
                BF_Block_Destroy(&block);
                BF_Block_Destroy(&new_block);

                return curr_block;
            }
            if ( split == 1){
                printf("\nto data node me id %d foulare\n", curr_block);
                printf("\ndimiourgithike neo block me id %d\n",new_block_id);
                printf("\no goneas tou neou block pire neo index key %d\n", new_index_key);

                //kane set dirty OLA ta blocks
                //kane unpin OLA ta blocks gia na graftoun ston disko
                BF_Block_SetDirty(block);
                BF_Block_SetDirty(new_block);
                BF_UnpinBlock(block);
                BF_UnpinBlock(new_block);
                BF_Block_Destroy(&block);
                BF_Block_Destroy(&new_block);
                
            
            return new_block_id;
            }
            else{
                 BF_Block* new_block;
                BF_Block_Init(&new_block);
                CALL_BF(BF_AllocateBlock(file_desc, new_block));
                
                int new_parent_index_key;
                int new_block_id;

                int result = split_indexNode(file_desc, bplus_info, block , split , new_block, &new_parent_index_key, &new_block_id,new_index_key);
                if(result == 0){
                    
                }

            }

        default:
            printf("\n");
            break;
    }


    bplus_info->total_record_counter++;
    
    debug_Data(block);

    BF_UnpinBlock(block);
    BF_Block_SetDirty(block);
    BF_Block_Destroy(&block);

    return curr_block;
}

int BP_GetEntry(int file_desc, BPLUS_INFO* bplus_info, int id, Record** result) {
    
    int curr_block = bplus_info->root_block_id;
    int curr_level=0;

    //perase apo olous tous index nodes sto sosto path
    while(curr_level < bplus_info->tree_height -1){
        
        //dimiourgoume ena temp block gia tin prospelasi tou dedrou
        BF_Block* tmpblock;
        BF_Block_Init(&tmpblock);
        //CALL_BF(BF_AllocateBlock(file_desc, tmpblock));

        //kaloume tin GetBlock gia na epistrepsei sto block to BF_Block me block_num = curr_block
        CALL_BF(BF_GetBlock(file_desc, curr_block, tmpblock));

        //kaloume tin find_next_Node gia na epistrepsei to block_num tou epomenou block gia tin prospelasi
        curr_block = find_next_Node(tmpblock,id);


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
    
    printf("Block: %d\n",curr_block);
    
    // h get block tha deiksei to neo block pointer sto sosto data node
    BF_GetBlock(file_desc, curr_block, block);
    //pleon to block ine ena leaf data node

    //debug_Index(block);
    search_record(block , id , result);

    BF_Block_SetDirty(block);
    BF_UnpinBlock(block);
    BF_Block_Destroy(&block);
}


/* BP Print sinarthsh gia ektypwsh periexomenwn twn data blocks meta to splittarisma */
int BP_print(int file_desc, BPLUS_INFO* bplus_info) {
    int curr_block = bplus_info->root_block_id;
    int curr_level = 0;

    // kanei traverse to dentro gia na ftasei sto pio aristera leaf tou pou einai
    // to prwto data node
    while (curr_level < bplus_info->tree_height - 1) {
        BF_Block* tmpblock;
        BF_Block_Init(&tmpblock);
        
        CALL_BF(BF_GetBlock(file_desc, curr_block, tmpblock));

        // klhsh sunartishs poy briskei to pio aristera leaf
        curr_block = find_leftest_Node(tmpblock);

        if (curr_block == -1) {
            BF_Block_SetDirty(tmpblock);
            BF_UnpinBlock(tmpblock);
            BF_Block_Destroy(&tmpblock);
            return -1;
        }

        curr_level++;
        
        // markarei to tmpblock ws dirty kai to kanei unpin k destroy gia na mh gemizei to buffer
        BF_Block_SetDirty(tmpblock);
        BF_UnpinBlock(tmpblock);
        BF_Block_Destroy(&tmpblock);
    }

    BF_Block* tmpblock;
    BF_Block_Init(&tmpblock);
    CALL_BF(BF_GetBlock(file_desc, curr_block, tmpblock));
    
    DataNode* tmpnode = (DataNode*)BF_Block_GetData(tmpblock);
    // Printarei ta dedomena kai ta block ids poy anoikei to kathe record(print mono to record.id)
    printf("Block %3d has: ",curr_block);

    while (tmpnode->next_data_node != -1) {
    
        for (int i = 0; i < tmpnode->recs_counter; i++) {
            printf("%d ", tmpnode->recs[i].id);
        }
        int next = tmpnode->next_data_node;
        printf("\n");
        printf("Block %3d has: ", next);
        CALL_BF(BF_GetBlock(file_desc, next, tmpblock));
        tmpnode = (DataNode*)BF_Block_GetData(tmpblock);
    }

    for (int i = 0; i < tmpnode->recs_counter; i++) {
        printf("%3d ", tmpnode->recs[i].id);
    }
    printf("\n");

    // katharismos block
    BF_Block_SetDirty(tmpblock);
    BF_UnpinBlock(tmpblock);
    BF_Block_Destroy(&tmpblock);

    return 0;
}