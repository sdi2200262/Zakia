#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bf.h"
#include "bp_file.h"
#include "record.h"

#define MAX_RECS 2  // opos sto paradeigma sto power point

typedef struct DataNode
{
    int nextBlock;  // pointer
    int recCount;
    Record recs[MAX_RECS];
}DataNode;


int insert(BF_Block *block, Record *rec, bool split){
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
    int j = node->recCount -1;
    while (j>=0 && node->recs[j].id > rec->id){
        node->recs[j+1] = node->recs[j];
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

    int mid = node->recCount / 2;
    new_node->recCount = node->recCount - mid;  // to neo node exei arxika xwro gia tis mises eggrafes tou paliou node
    memcpy(new_node->recs , &node->recs[mid], new_node->recCount * sizeof(Record));

    new_node->nextBlock = node->nextBlock;  // to neo node tha deixnei sto block pou edeixne to prohgoumeno node 

    node->recCount = mid;
    *rec = new_node->recs[0]; //h nea eggrafh pou dinete san orisma tha bei sth prwth thesh tou neou node

    return 0;
}

int search_record(BF_Block *block, int id, Record **result){
    char* data = BF_Block_GetData(block);
    DataNode* node = (DataNode*) data;  // pernaei ta data apo to block sth metablhth data k meta sto node 
                                        // pou einai type Datanode gia na mporei na exei prosvash sta antikeimena t struct
                                        // gia kathe eggrafh kai na diaxeirizetai tis plirofories ths gia to current node

    for(int i = 0; i < node->recCount; i++){
        if(node->recs[i].id == id){ // an brethei h eggrafh pou psaxnoume me record->id == id
            *result = &node->recs[i];   // pernate h diefthinsh ths ston deikth result kai aytos me th seira tou
                                        // tha xrhshmopoieithei sthn sunarthsh BPLUS_GET_ENTRY opou ths epistrafei
                                        // to periexomeno ths egrafhs auths 
            return 0;
        }
    }

    *result = NULL; // an de vrethei h eggrafh tote to result pernei thn timh null kai
                    // h sunarthsh GET_ENTRY epistrefei null
    return -1;
}