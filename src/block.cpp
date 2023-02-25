//
// Created by Yu Song on 15/2/23.
//

#include <cassert>
#include <cstring>

#include "block.h"
#include "bptnode.h"

namespace block {

void Initialize(char *pBlockMem, block_type type) {
    BlockHeader *block_header = dbtypes::ReadBlockHeader(pBlockMem);
    block_header->used = true;
    block_header->type = type;
    block_header->used_size = (size_type) sizeof(BlockHeader);
}

void Free(char *pBlockMem) {
    BlockHeader *block_header = dbtypes::ReadBlockHeader(pBlockMem);
    block_header->used = false;
    block_header->type = BlockType::UNUSED;
    block_header->used_size = (size_type) 0;
}

bool IsUsed(char *pBlockMem) {
    return dbtypes::ReadBlockHeader(pBlockMem)->used;
}

block_type GetBlockType(char *pBlockMem) {
    return dbtypes::ReadBlockHeader(pBlockMem)->type;
}

size_type GetUsedSize(char *pBlockMem) {
    return dbtypes::ReadBlockHeader(pBlockMem)->used_size;
}

size_type GetEmptySize(char *pBlockMem) {
    return BLOCK_SIZE - GetUsedSize(pBlockMem);
}

namespace record {
    /**
     * Initialize RecordBlockHeader, increment used_size
     * @param pBlockMem
     */
void Initialize(char *pBlockMem) {
    BlockHeader *block_header = dbtypes::ReadBlockHeader(pBlockMem);
    block_header->used_size += RECORD_BLOCK_HEADER_SIZE;
    std::memset(pBlockMem + BLOCK_HEADER_SIZE, 0, RECORD_BLOCK_HEADER_SIZE);
}

int GetOccupiedCount(char *pBlockMem) {
    RecordBlockHeader *record_block_header = dbtypes::ReadRecordBlockHeader(pBlockMem);
    return record_block_header->num_occupied;
}

int GetEmptyCount(char *pBlockMem) {
    return RECORD_PER_BLOCK - GetOccupiedCount(pBlockMem);
}

bool IsFull(char *pBlockMem) {
    return GetEmptyCount(pBlockMem) == 0;
}

unsigned short AllocateSlot(char *pBlockMem) {
    BlockHeader *block_header = dbtypes::ReadBlockHeader(pBlockMem);
    RecordBlockHeader *record_block_header = dbtypes::ReadRecordBlockHeader(pBlockMem);
    assert(record_block_header->num_occupied < RECORD_PER_BLOCK);
    for (int i = 0; i < RECORD_PER_BLOCK; i++) {
        if (record_block_header->occupied[i]) {}
        else {
            block_header->used_size += PACKED_RECORD_SIZE;
            record_block_header->occupied[i] = true;
            record_block_header->num_occupied++;
            return RECORD_BLOCK_OFFSET_TOTAL + PACKED_RECORD_SIZE * i;
        }
    }
    return -1;
}

void FreeSlot(char *pBlockMem, unsigned short offset) {
    BlockHeader *block_header = dbtypes::ReadBlockHeader(pBlockMem);
    RecordBlockHeader *record_block_header = dbtypes::ReadRecordBlockHeader(pBlockMem);
    assert(RECORD_BLOCK_OFFSET_TOTAL <= offset);  // does not try to free the headers
    assert(offset <= BLOCK_SIZE - RECORD_SIZE);  // does not try to free outside the block
    assert((offset - RECORD_BLOCK_OFFSET_TOTAL) % PACKED_RECORD_SIZE == 0);  // not in the middle of a record
    int index = ((offset - RECORD_BLOCK_OFFSET_TOTAL) / PACKED_RECORD_SIZE);
    assert(record_block_header->occupied[index]);
    block_header->used_size -= PACKED_RECORD_SIZE;
    record_block_header->occupied[index] = false;
    record_block_header->num_occupied--;
}

}  // record

namespace bpt {

void Initialize(char *pBlockMem) {
    BlockHeader *block_header = dbtypes::ReadBlockHeader(pBlockMem);
    block_header->used_size += RECORD_BLOCK_HEADER_SIZE;
    std::memset(pBlockMem + BLOCK_HEADER_SIZE, 0, NODE_HEAD_SIZE);
}

}// bpt

}  // block