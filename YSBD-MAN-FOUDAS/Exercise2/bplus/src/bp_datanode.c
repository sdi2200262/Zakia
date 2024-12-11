#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bf.h"
#include "bp_file.h"
#include "record.h"
#include "bp_datanode.h"

#define MAX_RECORDS 2 

/* mesa sto file auto periexontai sinarthseis pou diaxeirizontai tous kombous fylla enos bplus tree, oi  opoioi komvoi einai 
    sthn ousia komvoi dedomenon(opos anaferetai k sth theoria). Oi sinartiseis autes aforoun basikes leitoyrgies pano se komvous
    opos arxikopoihsh, eisagwgh , anazhthsh alla kai diaxwrismos komvou ean se auton yparxoun parapanw apo MAX_RECORDS eggrafes.
    Kathe komvos mporei na anaparastathei ws mia domh DataNode h opoia mesa periexei simantikes plirofories opos plithos 
    eggrafwn(counter),enan deikth se epomeno block kai enan pinaka me tis eggrafes pou briskontai mesa sto current block(node), 
    o opoios exei statiko megethos kai iso me MAX_RECORDS */


//arxikopoiei to datanode pou tha kanoume eisagosi sto dedro
int init_DataNode(BF_Block* block){

    DataNode* node = (DataNode*)BF_Block_GetData(block);

    node->node_id = (int)BF_Block_GetData(block);    //pairnoume ena block id
    node->next_block_ptr = -1;    //arxikopoioume ton deikti gia to epomeno block
    node->parent_id = -1;
    node->record_counter=0;    
    for (int i = 0; i <MAX_RECORDS; i++){      //arxikopoioume ton pinaka me ta records
        node->recs[i].id = -1;
        /*node->recs[i].name = 'x';
        node->recs[i].surname = 'x';
        node->recs[i].surname = 'x';*/
    }

    printf("\nData node is initialized... Node ID:  %d\n", node->node_id);
    
    return 0;
}

void set_parent_id(BF_Block* block, int id){
    
    DataNode* node = (DataNode*)BF_Block_GetData(block);
    
    node->parent_id = id;
}

int insert_DataNode(BF_Block* block ,Record *rec){
    
    DataNode* node = (DataNode*)BF_Block_GetData(block);

    if ( node->record_counter>=MAX_RECORDS){
        //epistrefoume MAXRECORDS gia na kanei to splitting h BP_INSERT
        return MAX_RECORDS;
    }

    //tsekaroume gia diples eggrafes
    for (int i=0; i< node->record_counter;i++){
        //an to record id eisodou einai idio me kapoio apo ta keys tote ban
        if(node->recs[i].id == rec->id){
            printf("Duplicate key found %d", rec->id);
            return -1;
        }
    }

    // elegxoume se poia thesi tha baloume to neo record
    int i;
    // pame stin teleutaia thesi eggrafis kai tsekaroume an einai megaliteri apo to record
    // pou theloume na eisagoume
    // an einai tote ta kanoume swap kai auti i diadikasia ginetai mexri na min isxuei to condition
    // etsi diateiroume auskousa seira ton records sto datanode
    for (i = node->record_counter; i > 0 && node->recs[i-1].id > rec->id; i--) {
        node->recs[i] = node->recs[i-1];
    }
    
    // eisagoume to neo record
    node->recs[i] = *rec;
    node->record_counter++;

    return 0;
}
 