#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bf.h"
#include "bp_file.h"
#include "record.h"
#include <bp_datanode.h>
#include <stdbool.h>

#define MAX_OPEN_FILES 20
#define CALL_BF(call)         \
  {                           \
    BF_ErrorCode code = call; \
    if (code != BF_OK)        \
    {                         \
      BF_PrintError(code);    \
      return bplus_ERROR;     \
    }                         \
  }


static BPLUS_INFO *open_files[MAX_OPEN_FILES] = {NULL};

int find_empty_slot(){
  for (int i = 0; i < MAX_OPEN_FILES; i++){
    if(open_files[i] == NULL){
      return i;
    }
  }
  return -1;
}


// ftaixnei to bplus file
int BP_CreateFile(char *fileName){

  int file_desc;    // apothikevoume se auto to var to adress
  CALL_BF(BF_OpenFile(fileName, &file_desc));   // anoigoume to file

  BF_Block* block;    // ftiaxnoume metabliti block 
  BF_Block_Init(&block);    // initialize to block
  CALL_BF(BF_AllocateBlock(file_desc, block));

  //stin ergasia1 - parousiasi to exei
  //void* data;
  //data = bf_block_getdata()
  //Record* rec = data;
  char* data = BF_Block_GetData(block);   //pare ta block data 
  BPLUS_INFO metadata = {.file_desc = file_desc, .root_block = -1, .tree_height = 0};    //pare ta bplus data 
  memcpy(data, &metadata, sizeof(BPLUS_INFO));

  BF_Block_SetDirty(block);
  CALL_BF(BF_UnpinBlock(block));
  //BF_Block_Destroy(&block);     de bgazei noima na to kaneis destroy kai oute to kanei sto paradeigma ergasias 1
  CALL_BF(BF_CloseFile(file_desc));
  CALL_BF(BF_Close());      // auti kleinei to block kai grafei sto disko to neo file - etsi to exei kai sto paradeigma 

  return 0;
}


// anoigei to arxeio
BPLUS_INFO* BP_OpenFile(char *fileName, int *file_desc)
{
  CALL_BF(BF_OpenFile(fileName, file_desc));    //anoigei to arxeio

  BF_Block* block;    //dimiourgoume adikeimeno blocked
  BF_Block_Init(&block);  // initialize to block

  CALL_BF(BF_GetBlock(&file_desc, 0, block));   //pairnoume to block me num 0 kai to apothikeuoume block 

  char* data = BF_Block_GetData(block);         // pairnoume ta data tou block me num 0     
  //den ktlbenw gt to kaneis etsi adi gia opos to ekanes panw apla BPLUS_INDFO info;
  BPLUS_INFO tmpInfo; //instead of malloc
  BPLUS_INFO* info = &tmpInfo;

  if(info == NULL){
    BF_UnpinBlock(block);
    BF_Block_Destroy(&block);
    CALL_BF(BF_CloseFile(&file_desc);)

    return NULL;
  }
  //kai edo kaneis allios to memcpy adi gia opos pano
  //memcpy(data, &info, sizeof(BPLUS_INFO));
  //epishs edo den exeis anathesei sto info tis times metadata tou dedrou apla to pernas adeio
  memcpy(info, data, sizeof(BPLUS_INFO));
  BF_UnpinBlock(block);
  //BF_Block_Destroy(&block);     //kai auto edo sigoura de xreiazetai

  //auto edo den exo katalabei gia ti ine opos kai h find_empty_slot()
  int slot = find_empty_slot();
  if (slot == -1){
    //no available slot found
    free(info);
    CALL_BF(BF_CloseFile(&file_desc));
    //CALL_BF(BF_Close());      edo den eimai sigouros an xreiazete auto
    return NULL;
  }


  else:
    open_files[slot] = info;
  
  //dld an uparxei free slot den kanoume BF_CloseFile()???
  //CALL_BF(BF_CloseFile(&file_desc))

  //edo to info ine adeio den uparxei anathesi
  return info;
}


int BP_CloseFile(int file_desc,BPLUS_INFO* info)
{
  //we find the wanted open file and set its tree info to NULL  
  for (int j = 0; j < MAX_OPEN_FILES; j++){
    if(open_files[j] == info){
      open_files[j] = NULL;
      break;
    }
  }

  free(info);   //free allocated space for info variable
  CALL_BF(BF_CloseFile(file_desc));
  CALL_BF(BF_Close());    //edo ime sxedon sgros oti thelei auto
  return 0;
}

int BP_InsertEntry(int file_desc,BPLUS_INFO *bplus_info, Record record)
{ 
  return 0;
}

int BP_GetEntry(int file_desc,BPLUS_INFO *bplus_info, int value,Record** record)
{  
  *record=NULL;
  return 0;
}