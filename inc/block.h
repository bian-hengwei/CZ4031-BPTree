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

void Initialize_D(char *pBlock, block_type type);

void Free(char *pBlockMem);

void Free_D(char *pBlock);

bool IsUsed(char *pBlockMem);

bool IsUsed_D(char *pBlock);

block_type GetBlockType(char *pBlockMem);

block_type GetBlockType_D(char *pBlock);

size_type GetUsedSize(char *pBlockMem);

size_type GetUsedSize_D(char *pBlock);

size_type GetEmptySize(char *pBlockMem);

size_type GetEmptySize_D(char *pBlock);

namespace record {

void Initialize(char *pBlockMem);

void Initialize_D(char *pBlock);

int GetOccupiedCount(char *pBlockMem);

int GetOccupiedCount_D(char *pBlock);

int GetEmptyCount(char *pBlockMem);

int GetEmptyCount_D(char *pBlock);

bool IsFull(char *pBlockMem);

bool IsFull_D(char *pBlock);

unsigned short AllocateSlot(char *pBlockMem);

unsigned short AllocateSlot_D(char *pBlock);

void FreeSlot(char *pBlockMem, unsigned short offset);

void FreeSlot_D(char *pBlock, unsigned short offset);

}  // record

namespace bpt {

void Initialize(char *pBlockMem);

void Initialize_D(char *pBlock);

}

}  // block

#endif //BLOCK_H