#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bf.h"
#include "hp_file.h"
#include "record.h"

#define CALL_BF(call)       \
{                           \
  BF_ErrorCode code = call; \
  if (code != BF_OK) {         \
    BF_PrintError(code);    \
    return HP_ERROR;        \
  }                         \
}

int HP_CreateFile(char *fileName){
    CALL_BF(BF_CreateFile(fileName));
    int file_id;
    CALL_BF(BF_OpenFile(fileName, &file_id));

    BF_Block* block;
    BF_Block_Init(&block);
    CALL_BF(BF_AllocateBlock(file_id, block));

    HP_info* hpInfo;
    hpInfo->file_id = file_id;
    hpInfo->numOfBlocks = 1;

    void* data = BF_Block_GetData(block);
    int* info = data;
    info[0] = hpInfo->file_id;
    info[1] = hpInfo->numOfBlocks;

    BF_Block_SetDirty(block);
    CALL_BF(BF_UnpinBlock(block));
    CALL_BF(BF_CloseFile(hpInfo->file_id));
    CALL_BF(BF_Close());

    return 0;
  }

HP_info* HP_OpenFile(char *fileName, int *file_desc){
      HP_info* hpInfo;
      hpInfo = malloc(sizeof(hpInfo));
      BF_OpenFile(fileName, file_desc);
      hpInfo->file_id = *file_desc;
      int counter;
      BF_GetBlockCounter(*file_desc, &counter);
      hpInfo->numOfBlocks = counter;

      return hpInfo;
}


int HP_CloseFile(int file_desc,HP_info* hp_info ){
   if(hp_info ==  NULL){
    return -1;
   }else{
        CALL_BF(BF_CloseFile(file_desc));
        free(hp_info);
        return 0;
   }
}

int HP_InsertEntry(int file_desc,HP_info* hp_info, Record record){

    BF_Block* block;
    BF_Block_Init(&block);

    CALL_BF(BF_GetBlockCounter(file_desc,&hp_info->numOfBlocks));
    for(int i=0; i<hp_info->numOfBlocks; i++){
      CALL_BF(BF_GetBlock(file_desc,i, block));
      void* data = BF_Block_GetData(block);

      int counterOfRecs;
      memcpy(data, &counterOfRecs, sizeof(int));

      if(counterOfRecs * sizeof(Record) < BF_BLOCK_SIZE){
        //to current block tou arxeiou  exei
        //xwro gia na graftei akoma mia eggrafh
        memcpy(data, &record, sizeof(Record));
        counterOfRecs++;
        memcpy(data, &counterOfRecs, sizeof(int));
        return i;      }
      BF_Block_SetDirty(block);
      CALL_BF(BF_UnpinBlock(block));
    }
    //an de vrethei xwros se ena block 
    //dhmiourgeitai neo gia an apothikeftei se auto h eggrafh
    CALL_BF(BF_AllocateBlock(file_desc, block));
    void* data = BF_Block_GetData(block);
    memcpy(data, &record, sizeof(Record));
    int counterOfRecs = 1;
    memcpy(data, &counterOfRecs, sizeof(int));

    BF_Block_SetDirty(block);
    CALL_BF(BF_UnpinBlock(block));
    BF_Block_Destroy(&block);

    hp_info->numOfBlocks ++;

    return --hp_info->numOfBlocks ;

}

int HP_GetAllEntries(int file_desc,HP_info* hp_info, int value){    
    int num_blocks_read = 0;
    int num_records = 0;
    BF_Block *block;
    BF_Block_Init(&block);

    for (int i = 0; BF_GetBlock(file_desc, i, block) == BF_OK; i++){ //oso mporeis na diabazeis block tou arxeiou
      
      //auksisi counter
      num_blocks_read++;

      //perna stin metabliti data ta stoixeia tou block
      char* data = BF_Block_GetData(block);
      //bres to record size gia na ektiposei sosta name , surname ktlp
      int record_size = sizeof(Record);
      //bres posa records xorane sto block 
      int num_records_in_block = BF_BLOCK_SIZE / record_size;

      //gia kathe egrafe
      for (int i = 0; i < num_records_in_block; ++i) {
        //kane point stin sosti egrafi poy ine data + i eggrafes meta
        Record *record = (Record*)(data + i * record_size);
        //an exei to sosto value tote print
        if (record->id == value) {
            printRecord(*record);
        }
      }
    }    
    return -1;
}

