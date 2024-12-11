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

    node->next_block_ptr = -1;    //arxikopoioume ton deikti gia to epomeno block
    for (int i = 0; i <MAX_RECORDS; i++){      //arxikopoioume ton pinaka me ta records
        node->recs[i].id = -1;
        /*node->recs[i].name = 'x';
        node->recs[i].surname = 'x';
        node->recs[i].surname = 'x';*/
    }

    printf("\nData node is initialized..\n");

    return 0;
}

 