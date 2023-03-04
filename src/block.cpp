//
// Created by Yu Song on 15/2/23.
//

#include <cassert>
#include <cstring>

#include "block.h"
#include "bptnode.h"
#include "storage.h"

namespace block {

void Initialize(char *pBlockMem, block_type type) {
    BlockHeader *block_header = dbtypes::ReadBlockHeader(pBlockMem);
    block_header->used = true;
    block_header->type = type;
    block_header->used_size = (size_type) sizeof(BlockHeader);
}

void Initialize_D(char *pBlock, block_type type) {
    char *pBlockMem = static_cast<char *>(operator new(BLOCK_SIZE));
    Storage::ReadBlock(pBlockMem, pBlock);
    Initialize(pBlockMem, type);
    Storage::WriteBlock(pBlock, pBlockMem);
    operator delete(pBlockMem);
}

void Free(char *pBlockMem) {
    BlockHeader *block_header = dbtypes::ReadBlockHeader(pBlockMem);
    block_header->used = false;
    block_header->type = BlockType::UNUSED;
    block_header->used_size = (size_type) 0;
}

void Free_D(char *pBlock) {
    char *pBlockMem = static_cast<char *>(operator new(BLOCK_SIZE));
    Storage::ReadBlock(pBlockMem, pBlock);
    Free(pBlockMem);
    Storage::WriteBlock(pBlock, pBlockMem);
    operator delete(pBlockMem);
}

bool IsUsed(char *pBlockMem) {
    return dbtypes::ReadBlockHeader(pBlockMem)->used;
}

bool IsUsed_D(char *pBlock) {
    char *pBlockMem = static_cast<char *>(operator new(BLOCK_SIZE));
    Storage::ReadBlock(pBlockMem, pBlock);
    bool is_used = IsUsed(pBlockMem);
    operator delete(pBlockMem);
    return is_used;
}

block_type GetBlockType(char *pBlockMem) {
    return dbtypes::ReadBlockHeader(pBlockMem)->type;
}

block_type GetBlockType_D(char *pBlock) {
    char *pBlockMem = static_cast<char *>(operator new(BLOCK_SIZE));
    Storage::ReadBlock(pBlockMem, pBlock);
    block_type type = GetBlockType(pBlockMem);
    operator delete(pBlockMem);
    return type;
}

size_type GetUsedSize(char *pBlockMem) {
    return dbtypes::ReadBlockHeader(pBlockMem)->used_size;
}

size_type GetUsedSize_D(char *pBlock) {
    char *pBlockMem = static_cast<char *>(operator new(BLOCK_SIZE));
    Storage::ReadBlock(pBlockMem, pBlock);
    size_type size = GetUsedSize(pBlockMem);
    operator delete(pBlockMem);
    return size;
}

size_type GetEmptySize(char *pBlockMem) {
    return BLOCK_SIZE - GetUsedSize(pBlockMem);
}

size_type GetEmptySize_D(char *pBlock) {
    char *pBlockMem = static_cast<char *>(operator new(BLOCK_SIZE));
    Storage::ReadBlock(pBlockMem, pBlock);
    size_type size = GetEmptySize(pBlockMem);
    operator delete(pBlockMem);
    return size;
}

namespace record {

void Initialize(char *pBlockMem) {
    block::Initialize(pBlockMem, BlockType::RECORD);
    BlockHeader *block_header = dbtypes::ReadBlockHeader(pBlockMem);
    block_header->used_size += RECORD_BLOCK_HEADER_SIZE;
    std::memset(pBlockMem + BLOCK_HEADER_SIZE, 0, RECORD_BLOCK_HEADER_SIZE);
}

void Initialize_D(char *pBlock) {
    char *pBlockMem = static_cast<char *>(operator new(BLOCK_SIZE));
    Storage::ReadBlock(pBlockMem, pBlock);
    Initialize(pBlockMem);
    Storage::WriteBlock(pBlock, pBlockMem);
    operator delete(pBlockMem);
}

int GetOccupiedCount(char *pBlockMem) {
    RecordBlockHeader *record_block_header = dbtypes::ReadRecordBlockHeader(pBlockMem);
    return record_block_header->num_occupied;
}

int GetOccupiedCount_D(char *pBlock) {
    char *pBlockMem = static_cast<char *>(operator new(BLOCK_SIZE));
    Storage::ReadBlock(pBlockMem, pBlock);
    int occupied_count = GetOccupiedCount(pBlockMem);
    operator delete(pBlockMem);
    return occupied_count;
}

void GetOccupied(char *pBlockMem, bool *occupied) {
    RecordBlockHeader *record_block_header = dbtypes::ReadRecordBlockHeader(pBlockMem);
    std::memcpy(occupied, record_block_header->occupied, RECORD_PER_BLOCK);
}

void GetOccupied_D(char *pBlock, bool *occupied) {
    char *pBlockMem = static_cast<char *>(operator new(BLOCK_SIZE));
    Storage::ReadBlock(pBlockMem, pBlock);
    GetOccupied(pBlockMem, occupied);
    operator delete(pBlockMem);
}

int GetEmptyCount(char *pBlockMem) {
    return RECORD_PER_BLOCK - GetOccupiedCount(pBlockMem);
}

int GetEmptyCount_D(char *pBlock) {
    char *pBlockMem = static_cast<char *>(operator new(BLOCK_SIZE));
    Storage::ReadBlock(pBlockMem, pBlock);
    int empty_count = GetEmptyCount(pBlockMem);
    operator delete(pBlockMem);
    return empty_count;
}

bool IsFull(char *pBlockMem) {
    return GetEmptyCount(pBlockMem) == 0;
}

bool IsFull_D(char *pBlock) {
    char *pBlockMem = static_cast<char *>(operator new(BLOCK_SIZE));
    Storage::ReadBlock(pBlockMem, pBlock);
    bool is_full = IsFull(pBlockMem);
    operator delete(pBlockMem);
    return is_full;
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

unsigned short AllocateSlot_D(char *pBlock) {
    char *pBlockMem = static_cast<char *>(operator new(BLOCK_SIZE));
    Storage::ReadBlock(pBlockMem, pBlock);
    unsigned short offset = AllocateSlot(pBlockMem);
    Storage::WriteBlock(pBlock, pBlockMem);
    operator delete(pBlockMem);
    return offset;
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

void FreeSlot_D(char *pBlock, unsigned short offset) {
    char *pBlockMem = static_cast<char *>(operator new(BLOCK_SIZE));
    Storage::ReadBlock(pBlockMem, pBlock);
    FreeSlot(pBlockMem, offset);
    Storage::WriteBlock(pBlock, pBlockMem);
    operator delete(pBlockMem);
}

}  // record

namespace bpt {

void Initialize(char *pBlockMem) {
    block::Initialize(pBlockMem, BlockType::BPTREE);
    BlockHeader *block_header = dbtypes::ReadBlockHeader(pBlockMem);
    block_header->used_size += RECORD_BLOCK_HEADER_SIZE;
    std::memset(pBlockMem + BLOCK_HEADER_SIZE, 0, NODE_HEAD_SIZE);
}

void Initialize_D(char *pBlock) {
    char *pBlockMem = static_cast<char *>(operator new(BLOCK_SIZE));
    Storage::ReadBlock(pBlockMem, pBlock);
    Initialize(pBlockMem);
    Storage::WriteBlock(pBlock, pBlockMem);
    operator delete(pBlockMem);
}

}// bpt

}  // block