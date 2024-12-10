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



int initializeDataNode(DataNode* node){
    if(node !=  NULL){
        node->recCount = 0;
        node->nextBlock = -1;
        return 0;
    }else return -1;
}


int insertToDataNode(BF_Block *block, Record *rec, bool split){
    char* data = BF_Block_GetData(block);
    // dereference to data apo type Block se type DataNode
    DataNode* node = (DataNode *) data;

    for(int i =0 ; i< node->recCount; i++){
        if(node->recs[i].id == rec->id){
            printf("Record with id: %d already exists....Declined", rec->id);
            return -1; //yparxei hdh opote de ginete na ksanaginei insert k na yparxei diplotypo
        }
    }

    // den yparxei opote prepei na vrei thn swsth thesh gia na bei h eggrafh
    int j = node->recCount - 1 ;
    while (j>=0 && node->recs[j].id > rec->id){
        node->recs[j+1] = node->recs[j];    //metakinh oles tis eggrafes mia thesh pisw gia na yparjei xwros
                                            //gia thn eggrafh pou theloume na kanoume insert
        j--;
    }
    node->recs[j+1] = *rec; // efoson to epitrepomeno orio records ana node den paraviazete
                            // h eggrafh bainei sth thesh j+1 opou j h teleftea 'nomimh' thesh 
                            // opou yparxei egrafh
    node->recCount++;   // ananewnete to counter gia toys epomenous elegxous diathesimothtas xwrou me vash to max_number_recs

    if(node->recCount > MAX_RECS){
        split = true;       // an den yparxei xwros gia nea eggrafh tote h metavlhth split pernei timh true
    }
    return 0;
}

int split_data(BF_Block *block, BF_Block *newblock, Record *rec){
    char* data = BF_Block_GetData(block);
    DataNode* node = (DataNode*) data;

    char* new_data = BF_Block_GetData(newblock);
    DataNode* new_node = (DataNode*) new_data;

    if (node->recCount > MAX_RECS){
        // arxikopoioume to neo komvo
        initializeDataNode(new_node);
        int splitIndex = node->recCount / 2;
        // ta misa recs tou current node bainoun sto new node
        for(int i = 0; i < splitIndex; i++) {
            insertToDataNode(new_node, &node->recs[i], false);
        }

        node->recCount = splitIndex;
        // to pointer se epomeno block tou neo komvou einai auto tou current node

        new_node->nextBlock = node->nextBlock;
        node->nextBlock = -1;
        return 0;
    }else{
        printf("No splitting needed...\n");
        return -1;
    }
}

int search_record(BF_Block *block, int id, Record **result){
    char* data = BF_Block_GetData(block);
    DataNode* node = (DataNode*) data;  // pernaei ta data apo to block sth metablhth data k meta sto node 
                                        // pou einai type Datanode gia na mporei na exei prosvash sta antikeimena t struct
                                        // gia kathe eggrafh kai na diaxeirizetai tis plirofories ths gia to current node

    for(int i = 0; i < node->recCount; i++){
        if(node->recs[i].id == id){      // an brethei h eggrafh pou psaxnoume me record->id == id
            *result = &node->recs[i];   // pernate h diefthinsh ths ston deikth result
            return i; // epistrefete h thesh sthn opoia brethike h eggrafh
        }
    }

    *result = NULL; // an de vrethei h eggrafh tote to result pernei thn timh null
    return -1;
}