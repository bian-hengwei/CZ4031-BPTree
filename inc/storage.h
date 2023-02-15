//
// Created by Yu Song on 9/2/23.
//

#ifndef STORAGE_H
#define STORAGE_H

#include "dataTypes.h"

class Storage {
public:
    static size_t RECORD_SIZE;

    Storage(size_t diskSize, size_t blockSize);

    virtual ~Storage();

    void addBlock();

    char *allocateRecordSpace();

    bool isMemoryBlockSetToZero(const char *blockPtr) const;

    int getBlockNumber(const char *recordAddress);

    char *getBlockPointerOfARecord(char *recordAddress);

    void deallocRecord(char *recordAddress);

    static void readRecord(char *recordAddress, char *targetBuffer);

    char *writeRecord(char *sourcePtr);

    int getNumOfBlocks() const;

    int getNumOfRecords() const;

private:
    size_t diskSize;
    size_t blockSize;
    size_t curBlockUsedSize = 0;
    int numOfBlocks = 0;
    int numOfRecords = 0;
    char *storagePtr;
    char *blockPtr;
};


#endif //STORAGE_H
