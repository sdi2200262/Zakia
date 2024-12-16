#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "bf.h"
#include "bp_file.h"
#include "bp_datanode.h"
#include "bp_indexnode.h"

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

// int main()
// {
//   srand(42);
//   BF_Init(LRU);
//   BP_CreateFile(FILE_NAME);
//   int file_desc;
//   BPLUS_INFO* info = BP_OpenFile(FILE_NAME, &file_desc);
//   Record record;
//   srand(42);
//   for (int i = 0; i < RECORDS_NUM; i++)
//   {
    
//     record = randomRecord();
//     printf("Insert value: %d\n",record.id);
//     BP_InsertEntry(file_desc,info, record);
//   }
//   BP_CloseFile(file_desc,info);
//   BF_Close();

  
  
 

//   ////////////////////////////////////////////////
//   srand(42);
//   BF_Init(LRU);
//   info=BP_OpenFile(FILE_NAME, &file_desc);

//   int blockCount;
//   BF_GetBlockCounter(file_desc, &blockCount);
//   for (int i = 1; i < blockCount; i++)
//   {
//     BF_Block *block;
//     if (BP_isIndexNode(file_desc, i))
//     {
//       BPLUS_INDEX_NODE *indexNode;
//       load_Index_NODE(file_desc, i, &block, &indexNode);
//       printIndexNode(indexNode);
//       unload_Node(block, false);
//     }
//     else
//     {
//       BPLUS_DATA_NODE *dataNode;
//       load_DATA_NODE(file_desc, i, &block, &dataNode);
//       printDataNode(dataNode);
//       unload_Node(block, false);
//       printf("\n");
//     }    
//   }

//   Record tmpRec;  //Αντί για malloc
//   Record* result=&tmpRec;
//   for (int i = 0; i < RECORDS_NUM; i++)
//   {
//     record = randomRecord();    
//     // printRecord(record);
//     printf("Searching: %d\n",record.id);
//     BP_GetEntry( file_desc,info, record.id,&result);
//     if(result!=NULL)
//       printRecord(*result);
//   }

//   BP_CloseFile(file_desc,info);
//   BF_Close();
// }











/*
void insertEntries(int file_desc, BPLUS_INFO *bp_info, int entryNumber,Record sampledRecords[10]);
void printEntries(int file_desc, BPLUS_INFO *bp_info, int entryNumber,Record sampledRecords[10]);
void test1(int entryNumber);
void test2(int entryNumber);

int main(int argc, char *argv[])
{
  int entryNumber = atoi(argv[1]);
  test1(entryNumber);
}

void test1(int entryNumber)
{
  BF_Init(LRU);
  BP_CreateFile(FILE_NAME);
  int file_desc;
  // Insert Records
  BPLUS_INFO *bp_info = BP_OpenFile(FILE_NAME, &file_desc);
  Record sampledRecords[10];
  insertEntries(file_desc, bp_info, entryNumber,sampledRecords);
  
  
  // Print Records
  printEntries(file_desc, bp_info, entryNumber,sampledRecords);
  BP_CloseFile(file_desc, bp_info);
  BF_Close();
}


void insertEntries(int file_desc, BPLUS_INFO *bp_info, int entryNumber,Record sampledRecords[10])
{
  Record record;
  int counter=0;
  while(counter<10)
  {    
    double random_value = (double)rand() / RAND_MAX;
    record = randomRecord();
    record.id=rand()%(entryNumber*10);
    BP_InsertEntry(file_desc, bp_info, record);
    if (random_value <= (double)10 / entryNumber){
      sampledRecords[counter++]=record;
    }    
  }
}

void printEntries(int file_desc, BPLUS_INFO *bp_info, int entryNumber,Record sampledRecords[10])
{
  for (int i = 0; i < 10; i++){
    Record searchedRec= sampledRecords[i];
    printf("%d ", searchedRec.id);
  }
  printf("\n");
  for (int i = 0; i < 10; i++)
  {
    Record searchedRec= sampledRecords[i];
    


    Record result;
    Record* resultPointer=&result;
    BP_GetEntry(file_desc,bp_info, searchedRec.id,&resultPointer);
    printRecord(*resultPointer);
  }
}*/