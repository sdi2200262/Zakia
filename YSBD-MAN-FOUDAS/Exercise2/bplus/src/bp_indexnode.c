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
   init_IndexNode(new_block);

   int midpoint = (old_node->keys_counter + 1) / 2;

   // metakinise ta misa keys sto neo block
   for (int i = midpoint; i < old_node->keys_counter; i++) {
      new_node->keys[i - midpoint] = old_node->keys[i];
      new_node->keys_counter++;
      new_node->pointers[i - midpoint] = old_node->pointers[i];
      new_node->pointers_counter++;
   }
   //new_node->pointers[old_node->keys_counter - midpoint] = old_node->pointers[old_node->keys_counter];
   // arxikopoihsh twn keys - pointers tou old node pou metaferthikan sto new node
   for(int i = midpoint; i < old_node->keys_counter; i++) {
   old_node->keys[i] = -1;
   }

   for(int i = midpoint; i < old_node->pointers_counter; i++){
   old_node->pointers[i] = -1;
   }

   old_node->keys_counter = midpoint - 1;
   old_node->pointers_counter = midpoint;

   // update to neo block id
   int id;
   BF_GetBlockCounter(file_desc, &id);
   id--;
   *new_block_id = id;

   // update to parent id tou neou imdex node
   new_node->parent_id = old_node->parent_id;

   int res;
   // an to key ine mikrotero apo to prwto key tou neou node balto sto palio node
   if(key < new_node->keys[0]){
      insert_key_to_IndexNode(block, key);
      res =0;
   }else{
      insert_key_to_IndexNode(new_block, key);
      res = 1;
   }

   // to neo index key ine to prwtp key tou neo block
   *new_index_key = new_node->keys[0];

   // special case na thelei splittarisma to root
   if (old_node->parent_id == -1) {
      BF_Block* new_root;
      BF_Block_Init(&new_root);
      BF_AllocateBlock(file_desc, new_root);

      IndexNode* node = (IndexNode*)BF_Block_GetData(new_root);
      init_IndexNode(new_root);

      node->keys[0] = *new_index_key;
      node->pointers[0] = split_block_id;
      node->pointers[1] = *new_block_id;
      node->keys_counter = 1;

      // update ta parent ids ton apo katw komvwn
      int parent_id;
      BF_GetBlockCounter(file_desc, &parent_id);
      parent_id--;

      bplus_info->root_block_id = parent_id;
      bplus_info->tree_height++;

      old_node->parent_id = parent_id;
      new_node->parent_id = parent_id;

      BF_Block_SetDirty(new_root);
      BF_UnpinBlock(new_root);
      BF_Block_Destroy(&new_root);
      
      return 1;
   }

   // kane insert to index key ston komvo gonea
   BF_Block* parent_block;
   BF_Block_Init(&parent_block);
   BF_GetBlock(file_desc, old_node->parent_id, parent_block);

   int result = insert_key_to_IndexNode(parent_block, *new_index_key);

   switch (result) {
      case 0:
         insert_split_pointer_to_IndexNode(parent_block, *new_block_id, split_block_id);
         BF_Block_SetDirty(parent_block);
         BF_UnpinBlock(parent_block);
         BF_Block_Destroy(&parent_block);
         return 0;

      case keys_size:
         printf("Periptosh pou to parent index node thelei split\n\n");
         BF_Block_SetDirty(parent_block);
         BF_UnpinBlock(parent_block);
         BF_Block_Destroy(&parent_block);
         return keys_size;

      default:
         break;
   }
   BF_Block_SetDirty(parent_block);
   BF_UnpinBlock(parent_block);
   BF_Block_Destroy(&parent_block);
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