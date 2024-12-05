#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bf.h"
#include "bp_file.h"
#include "record.h"
#include <bp_datanode.h>
#include <stdbool.h>

#define CALL_BF(call)         \
  {                           \
    BF_ErrorCode code = call; \
    if (code != BF_OK)        \
    {                         \
      BF_PrintError(code);    \
      return bplus_ERROR;     \
    }                         \
  }




int BP_CreateFile(char *fileName)
{
  return 0;
}


BPLUS_INFO* BP_OpenFile(char *fileName, int *file_desc)
{
  return 0;
}

int BP_CloseFile(int file_desc,BPLUS_INFO* info)
{  
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

