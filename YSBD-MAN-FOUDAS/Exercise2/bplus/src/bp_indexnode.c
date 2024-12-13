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
         printf("Duplicate key found!\n\n");
         // Duplicate key not allowed
         return -1;
      }
   }

   // elegxoume se poia thesi tha baloume to neo key
   int i;
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

   // elegxoume an to pointer uparxei hdh sto block
   for (int i = 0; i < node->keys_counter+1; i++) {
      
      if (node->pointers[i] == new_block_id) {
         printf("Duplicate pointer found!\n\n");
         // Duplicate key not allowed
         return -1;
      }
   }

   // elegxoume se poia thesi tha baloume to neo pointer
   int i;
   // pame stin teleutaia thesi kleidiou kai tsekaroume an einai megaliteri apo to key eisodou
   // an einai tote ta kanoume swap kai auti i diadikasia ginetai mexri na min isxuei to condition
   // etsi diateiroume auskousa seira ton keys sto IndexNode
   for (i = node->keys_counter+1; i > 0 && node->pointers[i-1] > new_block_id; i--) {
      node->pointers[i] = node->pointers[i-1];
   }
   
   // eisagoume to neo pointer
   node->pointers[i] = new_block_id;
   
    return 0;
}
   

int find_next_Node(BF_Block* block, int key){
   IndexNode* node = (IndexNode*)BF_Block_GetData(block);

   for ( int i =0; i < node->keys_counter; i++){
      
      // an to input key ine mikrotero apo to key tou slot i tote epistrefoume ton adistoixo pointer
      if (key <= node->keys[i]){
      
         return node->pointers[i];
      }
   }

   // an einai megalutero apo ola ta kleidia tote balto sto teleutaio slot oso pio deksia paei
   return node->pointers[node->keys_counter];
}