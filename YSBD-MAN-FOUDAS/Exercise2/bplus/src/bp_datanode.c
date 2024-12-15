#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bf.h"
#include "bp_file.h"
#include "record.h"
#include "bp_datanode.h"

/* mesa sto file auto periexontai sinarthseis pou diaxeirizontai tous kombous fylla enos bplus tree, oi  opoioi komvoi einai 
    sthn ousia komvoi dedomenon(opos anaferetai k sth theoria). Oi sinartiseis autes aforoun basikes leitoyrgies pano se komvous
    opos arxikopoihsh, eisagwgh , anazhthsh alla kai diaxwrismos komvou ean se auton yparxoun parapanw apo MAX_RECORDS eggrafes.
    Kathe komvos mporei na anaparastathei ws mia domh DataNode h opoia mesa periexei simantikes plirofories opos plithos 
    eggrafwn(counter),enan deikth se epomeno block kai enan pinaka me tis eggrafes pou briskontai mesa sto current block(node), 
    o opoios exei statiko megethos kai iso me MAX_RECORDS */

// arxikopoiei to data node - block
int init_DataNode( BF_Block* block){
    DataNode* node = (DataNode*)BF_Block_GetData(block);
    
    node->parent_id = -1;
    node->next_data_node = -1;
    node->recs_counter = 0;
    for (int i=0; i< recs_size; i++) {
        node->recs[i].id = -1;
        strncpy(node->recs[i].name, "", sizeof(node->recs[i].name));
        strncpy(node->recs[i].surname, "", sizeof(node->recs[i].surname));
        strncpy(node->recs[i].city, "", sizeof(node->recs[i].city));
    }

    return 0;
}

int insert_record_to_DataNode(BF_Block* block, Record* record){
    DataNode* node = (DataNode*)BF_Block_GetData(block);

    // elegxoume an xoraei to record sto block 
    // an den xoraei kanoume epistrefoume recs_size gia na klithei h split
    if(node->recs_counter >= recs_size) return recs_size; 

    // elegxoume an to id apo to record eisodou uparxei hdh sto block
    for (int i = 0; i < node->recs_counter; i++) {
        

        if (node->recs[i].id == record->id) {
            // debug
            printf("recs[%d].id = %d ",i,node->recs[i].id);
            printf("\nDuplicate found!\n\n");
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
    for (i = node->recs_counter; i > 0 && node->recs[i-1].id > record->id; i--) {
        node->recs[i] = node->recs[i-1];
    }
    
    // eisagoume to neo record
    node->recs[i] = *record;
    node->recs_counter++;

    return 0;
}


int insert_pointer_to_DataNode( BF_Block* block, int new_block_id, int parent_block_id){
    DataNode* node = (DataNode*)BF_Block_GetData(block);

    node->next_data_node = new_block_id;
    node->parent_id = parent_block_id;
    
    return 0;
}

int debug(BF_Block* block){
    DataNode* node = (DataNode*)BF_Block_GetData(block);
    for(int i =0; i < node->recs_counter; i++){
        printf("%d ",node->recs[i].id);
    }
    
    return 0;
}

int split_DataNode(int file_desc, BPLUS_INFO* bplus_info, BF_Block* block, BF_Block* new_block, int* new_index_key, int* new_block_id, Record rec){
    DataNode* old_node = (DataNode*)BF_Block_GetData(block);
    
    DataNode* new_node = (DataNode*)BF_Block_GetData(new_block);
    init_DataNode(new_block);

    int midpoint = old_node->recs_counter / 2;

    //metakinise ta misa entries sto new_block
    for(int i = midpoint; i < old_node->recs_counter; i++) {
        new_node->recs[i - midpoint] = old_node->recs[i];
        new_node->recs_counter++;
    }

    //arxikopoiise ta entries tou old_node pou metaferthikan sto new_node
    for (int i=midpoint; i< old_node->recs_counter; i++) {
        old_node->recs[i].id = -1;
        strncpy(old_node->recs[i].name, "", sizeof(old_node->recs[i].name));
        strncpy(old_node->recs[i].surname, "", sizeof(old_node->recs[i].surname));
        strncpy(old_node->recs[i].city, "", sizeof(old_node->recs[i].city));
    }

    //update to recs_counter tou old node sta misa
    old_node->recs_counter = midpoint;

    // update to next_data_node pointer twn duo nodes
    int old_node_pointer;
    BF_GetBlockCounter(file_desc, &old_node_pointer);
    old_node_pointer--;
    new_node->next_data_node = old_node->next_data_node;
    old_node->next_data_node = old_node_pointer;

    //kratame new_block_id gia na to epistrepsei h BP_InserEntry;
    *new_block_id = old_node->next_data_node;

    //update to parent_id tou new_data_node
    new_node->parent_id = old_node->parent_id;

    //vale to rec sto swsto node opos akribos stin insert 

    //ksekinodas apo tin riza tha broume to sosto node sto opoio prepei na ginei
    //eisagogi eggrafis
    int curr_block = bplus_info->root_block_id;
    int curr_level=0;

    //perase apo olous tous index nodes sto sosto path
    while(curr_level < bplus_info->tree_height -1){
        
        //dimiourgoume ena temp block gia tin prospelasi tou dedrou
        BF_Block* tmpblock;
        BF_Block_Init(&tmpblock);

        //kaloume tin GetBlock gia na epistrepsei sto block to BF_Block me block_num = curr_block
        BF_GetBlock(file_desc, curr_block, tmpblock);

        //kaloume tin find_next_Node gia na epistrepsei to block_num tou epomenou block gia tin prospelasi
        curr_block = find_next_Node(tmpblock,rec.id);

        curr_level++;

        //diagrafoume to tmp block se kathe iteration
        BF_Block_SetDirty(tmpblock);
        BF_UnpinBlock(tmpblock);
        BF_Block_Destroy(&tmpblock);
    }

    // dimiourgoume neo block pointer gia na deixnei sto data node sto opoio
    // eftase telika h prospelasi 
    BF_Block* tmpblock;
    BF_Block_Init(&tmpblock);
    
    // h get block tha deiksei to neo block pointer sto sosto data node
    BF_GetBlock(file_desc, curr_block, tmpblock);
    
    insert_record_to_DataNode(tmpblock, &rec);
    
    //----------------------------------------------------------------------------------------------------------------------------------
    //vale to proto record.id tou new_data_node na ine neo key ston gonea index_node
    //kalese tin insert_key_to_IndexNode
    *new_index_key = new_node->recs[0].id;
    BF_Block* parent_block;
    BF_Block_Init(&parent_block);
    //BF_AllocateBlock(file_desc, parent_block);

    BF_GetBlock(file_desc, new_node->parent_id, parent_block);
    //an einai gemato KAI to parent_block epistrefoume tin keys_size gia na kalesoume tin split_IndexNode
    int result = insert_key_to_IndexNode(parent_block, *new_index_key);
    
    switch(result){

        case 0: // stin periprosi pou to key bike kanonika sto parent block prepei na ftiaksoume ta pointers
        insert_pointer_to_IndexNode(parent_block, *new_block_id , -1);

        //kane set_dirty, unpin kai destroy to temp block tou parent_block
        BF_Block_SetDirty(parent_block);
        BF_UnpinBlock(parent_block);
        BF_Block_Destroy(&parent_block);
        return 0;

        case keys_size:
        //kane set_dirty, unpin kai destroy to temp block tou parent_block
        BF_Block_SetDirty(parent_block);
        BF_UnpinBlock(parent_block);
        BF_Block_Destroy(&parent_block);
        //an h inser_key_to_IndexNode epistrepsei keys_size tote xreaizomaste splitarisma tou parent IndexNode
        return keys_size;

        default: 
        // Handle other cases if needed
        printf("Unhandled result: %d\n", result);
        break;
    }
    
    
    //kane set_dirty, unpin kai destroy to temp block tou parent_block
    BF_Block_SetDirty(parent_block);
    BF_UnpinBlock(parent_block);
    BF_Block_Destroy(&parent_block);

    return 0;
}