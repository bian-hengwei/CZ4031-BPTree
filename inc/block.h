//
// Created by Yu Song on 15/2/23.
//

#ifndef BLOCK_H
#define BLOCK_H

#include <algorithm>

#include "config.h"
#include "dbtypes.h"

namespace block {

void Initialize(char *pBlock, block_type type);

void Free(char *pBlock);

bool IsEmpty(char *pBlock);

block_type GetBlockType(char *pBlock);

size_type GetUsedSize(char *pBlock);

namespace record {

int GetOccupiedCount(char *pBlock);

int GetEmptyCount(char *pBlock);

char *AllocateSlot(char *pBlock);

void FreeSlot(char *pBlock, char *pFree);

}  // record

namespace bptree {

// allocate new space with specified size and return the pointer
char *AppendSpace(char *pBlock, size_type size);

// allocate space at specified pointer with specified size
void AllocateSpace(char *pBlock, char *pDest, size_type size);

// free space at specified pointer with specified size
void FreeSpace(char *pBlock, char *pFree, size_type size);

}  // bptree

}  // block

#endif //BLOCK_H