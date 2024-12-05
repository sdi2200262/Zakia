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

int BP_CreateFile(char *fileName)
{
  int file_desc;
  CALL_BF(BF_OpenFile(fileName, &file_desc));
  BF_Block* block;
  BF_Block_Init(&block);
  CALL_BF(BF_AllocateBlock(file_desc, block));

  char* data = BF_Block_GetData(block);
  BPLUS_INFO metadata = {.file_desc = file_desc, .root_block = -1, .tree_height = 0};
  memcpy(data, &metadata, sizeof(BPLUS_INFO));

  BF_Block_SetDirty(block);
  CALL_BF(BF_UnpinBlock(block));
  BF_Block_Destroy(&block);
  CALL_BF(BF_CloseFile(file_desc));

  return 0;
}


BPLUS_INFO* BP_OpenFile(char *fileName, int *file_desc)
{
  CALL_BF(BF_OpenFile(fileName, file_desc));

  BF_Block* block;
  BF_Block_Init(&block);

  CALL_BF(BF_GetBlock(&file_desc, 0, block));

  char* data = BF_Block_GetData(block);
  BPLUS_INFO tmpInfo; //instead of malloc
  BPLUS_INFO* info = &tmpInfo;

  if(info == NULL){
    BF_UnpinBlock(block);
    BF_Block_Destroy(&block);
    CALL_BF(BF_CloseFile(&file_desc);)

    return NULL;
  }

  memcpy(info, data, sizeof(BPLUS_INFO));
  BF_UnpinBlock(block);
  BF_Block_Destroy(&block);

  int slot = find_empty_slot();
  if (slot == -1){
    //no available slot found
    free(info);
    CALL_BF(BF_CloseFile(&file_desc));
    return NULL;
  }

  open_files[slot] = info;


  return info;
}

int BP_CloseFile(int file_desc,BPLUS_INFO* info)
{  
  for (int j = 0; j < MAX_OPEN_FILES; j++){
    if(open_files[j] == info){
      open_files[j] = NULL;
      break;
    }
  }
  free(info);
  CALL_BF(BF_CloseFile(file_desc));
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