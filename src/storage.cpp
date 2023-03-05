//
// Created by Yu Song on 9/2/23.
//

#include <cstring>
#include <iostream>

#include "config.h"
#include "storage.h"
#include "block.h"

Storage::Storage(size_t disk_size) : disk_size_(disk_size) {
    // allocate memory on the heap
    // pointer to the start of the memory that has been allocated
    pStorage_ = static_cast<char *>(operator new(disk_size));
    // Initialize the allocated memory
    memset(pStorage_, '\0', disk_size);
}

Storage::~Storage() = default;
//{
//    // Deallocate the memory
//    operator delete(pStorage_);
//}

char *Storage::FindSpaceForBlock() {
    // firstly consider following the latest block allocated
    char *latestBlock = GetLatestBlock();
    if (latestBlock - pStorage_ < disk_size_) {
        return latestBlock + BLOCK_SIZE;
    }
    // if no more space, come back to search for an available slot;
    char *pBlock = pStorage_;
    while (pBlock < pStorage_ + disk_size_) {
        if (!block::IsUsed_D(pBlock)) {
            return pBlock;
        }
        pBlock += BLOCK_SIZE;
    }
    // all block slots are used
    return nullptr;
}

char *Storage::AllocateBlock() {
    try {
        int num_of_blocks = GetNumOfBlocks();
        // Check if there is enough memory
        if (disk_size_ < num_of_blocks * BLOCK_SIZE + BLOCK_SIZE) {
            throw;
        }
        char *pBlock;
        // if no blocks, start from the storage ptr
        if (GetNumOfBlocks() == 0) {
            pBlock = pStorage_;
        }
        else {
            pBlock = FindSpaceForBlock();
        }
        // ensure that there is an available slot for the block
        if (!pBlock) {
            throw;
        }
        blocks_.emplace_back(pBlock);
        return pBlock;
    }
    catch (...) {
        std::cout << "Unable to add a new block due to not insufficient storage. A null pointer is returned." << std::endl;
        return nullptr;
    }
}

/**
 * Read a block to a given buffer address
 * @param pBuffer address of the buffer
 * @param pBlock address of the block in the storage
 */
void Storage::ReadBlock(char *pBuffer, char *pBlock) {
    memcpy(pBuffer, pBlock, BLOCK_SIZE);
}

int Storage::GetNumOfBlocks() const {
    return (int) blocks_.size();
}

void Storage::WriteBlock(char *pBlock, char *pBuffer) {
    memcpy(pBlock, pBuffer, BLOCK_SIZE);
}

void Storage::FreeBlock(char *pBlock) {
    block::Free_D(pBlock);
    auto it = find(blocks_.begin(), blocks_.end(), pBlock);
    if (it != blocks_.end()) {
        blocks_.erase(it);
    }
}

int Storage::GetNumOfRecords() const {
    int count = 0;
    for (const auto &block: blocks_) {
        if (block::GetBlockType_D(block) == BlockType::RECORD) {
            count += block::record::GetOccupiedCount_D(block);
        }
    }
    return count;
}

bool Storage::IsLatestBlockFull() {
    return block::record::IsFull_D(blocks_[blocks_.size() - 1]);
}

char *Storage::GetLatestBlock() {
    return blocks_[blocks_.size() - 1];
}

char *Storage::GetBlockByIndex(int index) {
    if (index >= 0 && index < blocks_.size()) {
        return blocks_[index];
    }
    return nullptr;
}

char *Storage::GetAddress() const {
    return pStorage_;
}