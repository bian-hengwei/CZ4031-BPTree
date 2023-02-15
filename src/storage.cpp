//
// Created by Yu Song on 9/2/23.
//
#include <iostream>
#include "storage.h"

using namespace std;

size_t Storage::RECORD_SIZE = sizeof(RecordMovie);

Storage::Storage(size_t diskSize, size_t blockSize) {
//    Total capacity of the disk
    this->diskSize = diskSize;
    this->blockSize = blockSize;

    // allocate memory on the heap.
    // pointer to the start of the block of memory that has been allocated
    storagePtr = static_cast<char *>(operator new(diskSize));
    // Initialize the allocated memory
    memset(storagePtr, '\0', diskSize);
    // Initialize the first free block
    blockPtr = storagePtr;
}

void Storage::addBlock() {
    try {
        // Check if there is enough memory
        if (diskSize < numOfBlocks * blockSize + blockSize) {
            throw;
        }
        numOfBlocks += 1;
        curBlockUsedSize = 0;

        // address of a block in the storage.
        blockPtr = storagePtr + numOfBlocks * blockSize;
    } catch (...) {
        cout << "Unable to add a new block due to not insufficient storage. A null pointer is returned." << endl;
    }
}

char *Storage::allocateRecordSpace() {
    // no space in current block, create a new block
    if (blockSize < RECORD_SIZE + curBlockUsedSize) {
        addBlock();
    }
    // recordAddress of the record
    char *recordAddress = blockPtr + curBlockUsedSize;
    curBlockUsedSize += RECORD_SIZE;
    return recordAddress;
}

bool Storage::isMemoryBlockSetToZero(const char *startPtr) const {
    for (size_t i = 0; i < blockSize; i++) {
        if (startPtr[i] != '\0') {
            return false;
        }
    }
    return true;
}

void Storage::deallocRecord(char *recordAddress) {
    // free the allocated space for the record
    memset(recordAddress, '\0', RECORD_SIZE);
    numOfRecords--;
    if (isMemoryBlockSetToZero(getBlockPointerOfARecord(recordAddress))) {
        numOfBlocks--;
    }
}

void Storage::readRecord(char *recordAddress, char *targetBuffer) {
    memcpy(targetBuffer, recordAddress, RECORD_SIZE);
}

char *Storage::writeRecord(char *sourcePtr) {
    char *recordAddress = allocateRecordSpace();
    memcpy(recordAddress, sourcePtr, RECORD_SIZE);
    numOfRecords++;
    return recordAddress;
}

int Storage::getNumOfBlocks() const {
    return numOfBlocks;
}

int Storage::getNumOfRecords() const {
    return numOfRecords;
}

Storage::~Storage() {
    // Deallocate the memory
    operator delete(storagePtr);
}

int Storage::getBlockNumber(const char *recordAddress) {
    size_t diff = recordAddress - storagePtr;
    int blockNum = (int) (diff / blockSize);
    return blockNum;
}

char *Storage::getBlockPointerOfARecord(char *recordAddress) {
    int blockNum = getBlockNumber(recordAddress);
    char *blockPointer = storagePtr + blockNum * blockSize;
    return blockPointer;
}










