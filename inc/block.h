//
// Created by Yu Song on 15/2/23.
//

#ifndef BLOCK_H
#define BLOCK_H

#include <algorithm>

#include "config.h"
#include "dbtypes.h"

namespace block {

void Initialize(char *pBlockMem, block_type type);

void Free(char *pBlockMem);

bool IsUsed(char *pBlockMem);

block_type GetBlockType(char *pBlockMem);

size_type GetUsedSize(char *pBlockMem);

size_type GetEmptySize(char *pBlockMem);

namespace record {

void Initialize(char *pBlockMem);

int GetOccupiedCount(char *pBlockMem);

int GetEmptyCount(char *pBlockMem);

bool IsFull(char *pBlockMem);

unsigned short AllocateSlot(char *pBlockMem);

void FreeSlot(char *pBlockMem, unsigned short offset);

}  // record

namespace bpt {

void Initialize(char *pBlockMem);

}

}  // block

#endif //BLOCK_H