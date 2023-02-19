//
// Created by Yu Song on 15/2/23.
//

#include <cassert>

#include "block.h"

namespace block {

void Initialize(char *pBlock, block_type type) {
    BlockHeader *block_header = dbtypes::ReadBlockHeader(pBlock);
    block_header->used = true;
    block_header->type = type;
    block_header->used_size = (size_type) sizeof(BlockHeader);
}

void Free(char *pBlock) {
    BlockHeader *block_header = dbtypes::ReadBlockHeader(pBlock);
    block_header->used = false;
    block_header->type = BlockType::UNUSED;
    block_header->used_size = (size_type) 0;
}

bool IsEmpty(char *pBlock) {
    return dbtypes::ReadBlockHeader(pBlock)->used;
}

block_type GetBlockType(char *pBlock) {
    return dbtypes::ReadBlockHeader(pBlock)->type;
}

size_type GetUsedSize(char *pBlock) {
    return dbtypes::ReadBlockHeader(pBlock)->used_size;
}

namespace record {

int GetOccupiedCount(char *pBlock) {
    RecordBlockHeader *record_block_header = dbtypes::ReadRecordBlockHeader(pBlock);
    return (int) std::count(record_block_header->occupied,
                            record_block_header->occupied + RECORD_PER_BLOCK, true);
}

int GetEmptyCount(char *pBlock) {
    RecordBlockHeader *record_block_header = dbtypes::ReadRecordBlockHeader(pBlock);
    return (int) std::count(record_block_header->occupied,
                            record_block_header->occupied + RECORD_PER_BLOCK, false);
}

char *AllocateSlot(char *pBlock) {
    RecordBlockHeader *record_block_header = dbtypes::ReadRecordBlockHeader(pBlock);
    for (int i = 0; i < RECORD_PER_BLOCK; i++) {
        if (record_block_header->occupied[i]) {}
        else {
            record_block_header->occupied[i] = true;
            return pBlock + RECORD_BLOCK_OFFSET + PACKED_RECORD_SIZE * i;
        }
    }
    assert(false);
    return nullptr;
}

void FreeSlot(char *pBlock, char *pFree) {
    RecordBlockHeader *record_block_header = dbtypes::ReadRecordBlockHeader(pBlock);
    int pFree_offset = pFree - pBlock;
    assert(RECORD_BLOCK_OFFSET < pFree_offset);
    assert(pFree_offset < BLOCK_SIZE);
    int pFree_index = (pFree_offset - RECORD_BLOCK_OFFSET) / PACKED_RECORD_SIZE;
    assert(0 < pFree_index and pFree_index < RECORD_PER_BLOCK);
    assert(pFree_index * PACKED_RECORD_SIZE + RECORD_BLOCK_OFFSET == pFree_offset);
    record_block_header->occupied[pFree_index] = false;
}

}  // record

namespace bptree {

char *AppendSpace(char *pBlock, size_type size) {
    return nullptr;
}

void AllocateSpace(char *pBlock, char *pDest, size_type size) {}

void FreeSpace(char *pBlock, char *pFree, size_type size) {}

}  // bptree

}  // block