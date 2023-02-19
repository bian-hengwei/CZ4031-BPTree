//
// Created by Yu Song on 9/2/23.
//
#include <iostream>
#include <cmath>
#include "storage.h"
#include "config.h"
#include "block.h"

using namespace std;

Storage::Storage(size_t diskSize) {
    // Total capacity of the disk
    this->diskSize = diskSize;

    // allocate memory on the heap
    // pointer to the start of the memory that has been allocated
    storagePtr = static_cast<char *>(operator new(diskSize));
    // Initialize the allocated memory
    memset(storagePtr, '\0', diskSize);
}

bool Storage::AddBlock() {
    try {
        int numOfBlocks = getNumOfBlocks();
        // Check if there is enough memory
        if (diskSize < numOfBlocks * BLOCK_SIZE + BLOCK_SIZE) {
            throw;
        }
        char *blockPtr = storagePtr + numOfBlocks * BLOCK_SIZE;
        blocks.emplace_back(blockPtr);
        return true;
    } catch (...) {
        cout << "Unable to add a new block due to not insufficient storage. A null pointer is returned." << endl;
        return false;
    }
}

char *Storage::AllocateRecordSpace() {
    if (blocks.empty()) {
        AddBlock();
        // no space in current block, create a new block
    } else if (BLOCK_SIZE < RECORD_SIZE + blocks[blocks.size() - 1].GetUsedSize()) {
        if (!AddBlock()) {
            return nullptr;
        }
    }
    // recordAddress of the record
    return blocks[blocks.size() - 1].AllocateRecord();
}

int Storage::GetBlockIndexByAddress(const char *address) {
    size_t diff = address - storagePtr;
    // Round down the byte difference to the nearest block boundary
    size_t blockBoundaryDiff = diff % BLOCK_SIZE;
    diff -= blockBoundaryDiff;

    // Calculate the block index by dividing the rounded-down byte difference by the block size
    return (int) (diff / BLOCK_SIZE);
}

void Storage::ReadRecord(char *recordAddress, char *targetBuffer) {
    memcpy(targetBuffer, recordAddress, RECORD_SIZE);
}

char *Storage::WriteRecord(void *sourcePtr) {
    char *recordAddress = AllocateRecordSpace();
    memcpy(recordAddress, sourcePtr, RECORD_SIZE);
    return recordAddress;
}

void Storage::DeleteRecord(char *recordAddress) {
    int blockIndex = GetBlockIndexByAddress(recordAddress);
    Block targetBlock = blocks[blockIndex];
    targetBlock.FreeRecord(recordAddress);
    // if block becomes empty, reduce the number of blocks;
    if (targetBlock.isEmpty()) {
        blocks.erase(blocks.begin() + blockIndex);
    }
}

int Storage::getNumOfBlocks() const {
    return (int) blocks.size();
}

int Storage::getNumOfRecords() const {
    int count = 0;

    for (const auto &block: blocks) {
        count += block.GetNumOfUsedSlots();
    }

    return count;
}

Storage::~Storage() {
    // Deallocate the memory
    operator delete(storagePtr);
}














