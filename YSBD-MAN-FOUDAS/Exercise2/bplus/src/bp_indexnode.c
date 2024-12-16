#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bf.h"
#include "bp_file.h"
#include "bp_datanode.h"
#include "bp_indexnode.h"
#include "record.h"


/* afora tous komvous eyrethriou pou periexoun mesa kleidia. Kathe node me n kleidia exei n+1 deiktes se blocks epomenou
   epipedou h se fylla (komvous dedomenwn). Pali enas indexnode mporei na anaparastathei ws mia domh IndexNode kai exei 
   idies leitourgies me ena block dedomenwn*/


int init_IndexNode(BF_Block* block){
   IndexNode* node = (IndexNode*)BF_Block_GetData(block);
   
   node->keys_counter = 0;
   node->pointers_counter = 0;

   for (int i=0; i< keys_size; i++) {node->keys[i] = -1;}
   for (int i=0; i< pointers_size; i++) {node->pointers[i] = -1;}

   return 0;
}

int insert_key_to_IndexNode(BF_Block* block, int key){
   IndexNode* node = (IndexNode*)BF_Block_GetData(block);

   // elegxoume an xoraei to key sto block 
   // an den xoraei kanoume epistrefoume keys_size gia na klithei h split
   if(node->keys_counter >= keys_size) return keys_size;
   // elegxoume an to key uparxei hdh sto block
   for (int i = 0; i < node->keys_counter; i++) {
      
      if (node->keys[i] == key) {
         printf("Duplicate key found\n\n");
         // Duplicate key aporriptetai
         return -1;
      }
   }

   // elegxoume se poia thesi tha baloume to neo key
   int i = node->keys_counter -1;
   // pame stin teleutaia thesi kleidiou kai tsekaroume an einai megaliteri apo to key eisodou
   // an einai tote ta kanoume swap kai auti i diadikasia ginetai mexri na min isxuei to condition
   // etsi diateiroume auskousa seira ton keys sto IndexNode
   for (i = node->keys_counter; i > 0 && node->keys[i-1] > key; i--) {
      node->keys[i] = node->keys[i-1];
   }
   
   // eisagoume to neo record
   node->keys[i] = key;
   node->keys_counter++;


   return 0;
}


int insert_pointer_to_IndexNode(BF_Block* block, int new_block_id){
   IndexNode* node = (IndexNode*)BF_Block_GetData(block);

   // elegxoume an xoraei to key sto block 
   // an den xoraei kanoume epistrefoume keys_size gia na klithei h split
   if(node->pointers_counter >= pointers_size) return pointers_size; 
   // elegxoume an to key uparxei hdh sto block
   for (int i = 0; i < node->pointers_counter; i++) {
      
      if (node->pointers[i] == new_block_id) {
         printf("Duplicate pointer found!\n\n");
         return -1;
      }
   }

   // elegxoume se poia thesi tha baloume to neo pointer
   int i = node->pointers_counter;
   for (i = node->pointers_counter; i > 0 && node->pointers[i-1] > new_block_id; i--) {
      node->pointers[i] = node->pointers[i-1];
   }
   
   // eisagoume to neo record
   node->pointers[i] = new_block_id;
   node->pointers_counter++;

   return 0;
}

int insert_split_pointer_to_IndexNode(BF_Block* block, int new_block_id, int split_block_id) {
   IndexNode* node = (IndexNode*)BF_Block_GetData(block);
   
   int pos = 0;  //thesi tou split block 

   //psaxnoume ti the si tou split_block_id sto pointers[]
   while (pos < node->pointers_counter && node->pointers[pos] != split_block_id) {
      pos++;
   }
   
   //if (node->pointers_counter >= pointers_size) {return -1;}
   
   //kanoume shift mia thesi deksia ola ta pointers meta to split_block_id
   for (int i = node->pointers_counter; i > pos + 1; i--) {
      node->pointers[i] = node->pointers[i - 1];
   }
   //kai bazoume to new_block_id amesos deksia tou
   node->pointers[pos + 1] = new_block_id;
   node->pointers_counter++;

   
   return 0; // great Success!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
}



int set_parent_id_to_IndexNode(BF_Block* block, int parent_block_id){
   IndexNode* node = (IndexNode* )BF_Block_GetData(block);

   node->parent_id = parent_block_id;
   
   return 0;
}   

int find_next_Node(BF_Block* block, int key) {
   // briskei to swsto path gia ton epomeno index node
   IndexNode* node = (IndexNode*)BF_Block_GetData(block);

   for (int i = 0; i < node->pointers_counter; i++) {
      if (key < node->keys[i]) {
         return node->pointers[i];
      }
   }
   return node->pointers[node->pointers_counter - 1];
}

int find_leftest_Node(BF_Block* block) {
    IndexNode* node = (IndexNode*)BF_Block_GetData(block);

   //epistrefei ton pio aristera komvo
   return node->pointers[0];
}


int split_indexNode(int file_desc,BPLUS_INFO* bplus_info, BF_Block* block, int split_block_id, BF_Block* new_block, int* new_index_key, int* new_block_id, int key) 
{
    IndexNode* old_node = (IndexNode*)BF_Block_GetData(block);
    IndexNode* new_node = (IndexNode*)BF_Block_GetData(new_block);
    int midpoint = (old_node->keys_counter + 1) / 2;

    // Move half the keys and pointers to the new node
    for (int i = midpoint; i < old_node->keys_counter; i++) {
        new_node->keys[i - midpoint] = old_node->keys[i];
        new_node->pointers[i - midpoint] = old_node->pointers[i];
    }
    new_node->pointers[old_node->keys_counter - midpoint] = old_node->pointers[old_node->keys_counter];

    // Update counters for both nodes
    new_node->keys_counter = old_node->keys_counter - midpoint;
    old_node->keys_counter = midpoint - 1;

    // Set parent ID for new node
    new_node->parent_id = old_node->parent_id;

    // New index key is the middle key
    *new_index_key = old_node->keys[midpoint - 1];

    // Update new block ID
    int new_block_id_temp;
    BF_GetBlockCounter(file_desc, &new_block_id_temp);
    new_block_id_temp--;
    *new_block_id = new_block_id_temp;

    // Special case: Splitting the root
    if (old_node->parent_id == -1) {
        // Create a new root
        BF_Block* new_root_block;
        BF_Block_Init(&new_root_block);
        BF_AllocateBlock(file_desc, new_root_block);
        IndexNode* new_root = (IndexNode*)BF_Block_GetData(new_root_block);
        init_IndexNode(new_root_block);

        new_root->keys[0] = *new_index_key;
        new_root->pointers[0] = split_block_id;
        new_root->pointers[1] = new_block_id_temp;
        new_root->keys_counter = 1;

        // Update the parent ID of the split nodes
        old_node->parent_id = new_block_id_temp;
        new_node->parent_id = new_block_id_temp;

        // Update the root block
        BF_Block_SetDirty(new_root_block);
        BF_UnpinBlock(new_root_block);
        BF_Block_Destroy(&new_root_block);
        
        return 1;  // New root was created
    }

    // Insert the new index key into the parent node
    BF_Block* parent_block;
    BF_Block_Init(&parent_block);
    BF_GetBlock(file_desc, old_node->parent_id, parent_block);

    int result = insert_key_to_IndexNode(parent_block, *new_index_key);
    switch (result) {
        case 0:  // Key was inserted successfully
            insert_split_pointer_to_IndexNode(parent_block, *new_block_id, split_block_id);
            BF_Block_SetDirty(parent_block);
            BF_UnpinBlock(parent_block);
            BF_Block_Destroy(&parent_block);
            return 0;

        case keys_size:  // Parent block needs splitting
            BF_Block_SetDirty(parent_block);
            BF_UnpinBlock(parent_block);
            BF_Block_Destroy(&parent_block);
            return keys_size;

        default:
            // Unexpected result
            BF_Block_SetDirty(parent_block);
            BF_UnpinBlock(parent_block);
            BF_Block_Destroy(&parent_block);
            return -1;
    }
}

 
int debug_Index(BF_Block* block){
    IndexNode* node = (IndexNode*)BF_Block_GetData(block);
    printf("%d\n", node->keys_counter);
    for(int i =0; i < node->keys_counter; i++){
        printf("%d ",node->keys[i]);
    }
    printf("\n");
    printf("%d\n", node->pointers_counter);
    for (int i =0; node->pointers_counter; i++){
       printf("%d ",node->pointers[i]);
    }
    printf("\n");
    return 0;
}