//
// Created by Yu Song on 9/2/23.
//

#include <cstring>
#include <iostream>

#include "config.h"
#include "storage.h"

Storage::Storage(size_t disk_size) : disk_size_(disk_size) {
    // allocate memory on the heap
    // pointer to the start of the memory that has been allocated
    pStorage_ = static_cast<char *>(operator new(disk_size));
    // Initialize the allocated memory
    memset(pStorage_, '\0', disk_size);
}

Storage::~Storage() {
    // Deallocate the memory
    operator delete(pStorage_);
}

char *Storage::AddBlock() {
    try {
        int num_of_blocks = GetNumOfBlocks();
        // Check if there is enough memory
        if (disk_size_ < num_of_blocks * BLOCK_SIZE + BLOCK_SIZE) {
            throw;
        }
        char *pBlock = pStorage_ + num_of_blocks * BLOCK_SIZE;
        blocks_.emplace_back(pBlock);
        return pBlock;
    } catch (...) {
        cout << "Unable to add a new block due to not insufficient storage. A null pointer is returned." << endl;
        return nullptr;
    }
}

int Storage::GetBlockIndexByAddress(const char *address) {
    size_t diff = address - pStorage_;
    // Round down the byte difference to the nearest block boundary
    size_t blockBoundaryDiff = diff % BLOCK_SIZE;
    diff -= blockBoundaryDiff;

    // Calculate the block index by dividing the rounded-down byte difference by the block size
    return (int) (diff / BLOCK_SIZE);
}

int Storage::GetNumOfBlocks() const {
    return (int) blocks_.size();
}