#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "bf.h"
#include "bp_file.h"
#include "bp_datanode.h"
#include "bp_indexnode.h"
#include "record.h"

#define RECORDS_NUM 100 // you can change it if you want
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
  //findEntries();

  ////////////////////////////////////////////////
  
}

void insertEntries(){
  BF_Init(LRU);
  BP_CreateFile(FILE_NAME);
  int file_desc;
  BPLUS_INFO* info = BP_OpenFile(FILE_NAME, &file_desc);
  Record record;
  

  //bazume riza einai 383 tha bei deksia
  record = randomRecord(0);
  int first_entry=record.id;
  printf("First entry is: %d\n", first_entry);
  first_entry = BP_InsertEntry(file_desc,info,record);
  printf("\nFirst entry with id: %d, completed in block  with id :%d\n", record.id, first_entry);

  //bazoume deftero record ( einai 783 tha bei deksia meta to proto)
  record = randomRecord(0);
  int second_entry=record.id;
  printf("Second entry is: %d\n", second_entry);
  second_entry = BP_InsertEntry(file_desc,info,record);
  printf("\nSecond entry with id: %d , completed in block with id: %d\n", record.id , second_entry);

  //bazoume trito gia na bei aristera
  record = randomRecord(100);
  int third_entry=record.id;
  printf("Third entry is: %d\n", third_entry);
  third_entry = BP_InsertEntry(file_desc,info,record);
  printf("\nThird entry with id: %d , completed in block with id: %d\n", record.id , third_entry);

  //bazoume tetarto gia na bei aristera meta to trito
  record = randomRecord(150);
  int fourth_entry=record.id;
  printf("fourth entry is: %d\n", fourth_entry);
  fourth_entry = BP_InsertEntry(file_desc,info,record);
  printf("\nFourth entry with id: %d , completed in block with id: %d\n", record.id , fourth_entry);


  for (int i = 5; i < RECORDS_NUM; i++)
  {
    record = randomRecord(0);
    int result;
    result = BP_InsertEntry(file_desc,info, record);
    if( result == recs_size){
      printf("ena node foulare!\n\n");
      break; 
    }
  }
  

  BP_CloseFile(file_desc,info);
  BF_Close();
}
/*
void findEntries(){
  int file_desc;
  BPLUS_INFO* info;

  BF_Init(LRU);
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
*/
