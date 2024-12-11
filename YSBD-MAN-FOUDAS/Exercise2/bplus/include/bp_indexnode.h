#ifndef BP_INDEX_NODE_H
#define BP_INDEX_NODE_H

#include "record.h"
#include "bf.h"
#include "bp_file.h"
#include "bp_datanode.h"

#define MAX_INDEX_KEYS 2

typedef struct
{
    int node_id;
    int keys[MAX_INDEX_KEYS];   //pinakas me ta index keys 
    int key_counter;    //counter gia ta index keys tou pinaka
    int child_id_array[MAX_INDEX_KEYS+1];  //pinakas me tous deiktes sta ids ton paidion
    int child_id_counter;       //counter gia ta pointer se paidia kombous
} IndexNode;

#endif
