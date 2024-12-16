#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "bf.h"
#include "bp_file.h"
#include "bp_datanode.h"
#include "bp_indexnode.h"
#include "record.h"

#define RECORDS_NUM 200 // you can change it if you want
#define FILE_NAME "data.db"

#define CALL_OR_DIE(call)     \
  {                           \
    BF_ErrorCode code = call; \
    if (code != BF_OK)        \
    {                         \
      BF_PrintError(code);    \
      exit(code);             \
    }                         \
  }

void insertEntries();
void findEntries();

int main()
{
  

  
  insertEntries();
  findEntries();

  ////////////////////////////////////////////////
  
}

void insertEntries(){
  BF_Init(LRU);
  BP_CreateFile(FILE_NAME);
  int file_desc;
  printf("\n--------------------------------------------------------------------------------------\n");
  printf("\n\nThis is insertEntries\n");
  BPLUS_INFO* info = BP_OpenFile(FILE_NAME, &file_desc);
  Record record;
  

  for (int i = 1; i < 200; i++)
  {
    record = randomRecord();
    int new_entry=record.id;
    new_entry = BP_InsertEntry(file_desc,info,record);
    printf("\nNew entry with id: %d , completed in block with id: %d\n", record.id , new_entry);
    
  }
  
  printf("\n--------------------------------------------------------------------------------------\n");
  printf("\n--------------------------------------------------------------------------------------\n");
  printf("BP_PRINT:\n\n");
  BP_print(file_desc, info);

  BP_CloseFile(file_desc,info);
  BF_Close();
}


void findEntries(){
  int file_desc;
  BPLUS_INFO* info;

  BF_Init(LRU);
  printf("\n\n\n--------------------------------------------------------------------------------------\n");
  printf("\n\nThis is findEntries..\n");
  info=BP_OpenFile(FILE_NAME, &file_desc);
 

  Record tmpRec;  //Αντί για malloc
  Record* result=&tmpRec;
  
  int id=159; 
  printf("Searching for: %d\n",id);
  BP_GetEntry( file_desc,info, id,&result);
  if(result!=NULL)
    printRecord(*result);

  BP_CloseFile(file_desc,info);
  BF_Close();
}
