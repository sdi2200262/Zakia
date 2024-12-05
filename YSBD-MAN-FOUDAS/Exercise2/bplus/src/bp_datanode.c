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
            return -1; //yparxei hdh opote de ginete na 
        }
    }

    // den yparxei opote prepei na vrei thn swsth thesh gia na bei h eggrafh
    int j = node->recCount -1;
    while (j>=0 && node->recs[j].id > rec->id){
        node->recs[j+1] = node->recs[j];
        j--;
    }
    node->recs[j+1] = *rec;
    node->recCount++;

    if(node->recCount > MAX_RECS){
        split = true;
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
    // to be doneeeeeeeeeeee
}