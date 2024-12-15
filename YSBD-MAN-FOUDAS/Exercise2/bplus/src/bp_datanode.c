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

    for(int i=0; i < node->recs_counter; i++){
        printf("%d ", node->recs[i].id);
    }
    return 0;
}


int insert_pointer_to_DataNode( BF_Block* block, int new_block_id){
    DataNode* node = (DataNode*)BF_Block_GetData(block);

    node->next_data_node = new_block_id;
    
    return 0;
}