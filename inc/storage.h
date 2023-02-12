//
// Created by Yu Song on 9/2/23.
//

#ifndef STORAGE_H
#define STORAGE_H

#include "dataTypes.h"
#include "address.h"

class Storage {
public:
    Storage(size_t diskSize, size_t blockSize);

    virtual ~Storage();

    void addBlock();

    Address allocRecord(size_t recordSize);

    bool isMemoryBlockSetToZero(const char *blockPtr) const;

    void deallocRecord(Address recordAddress, size_t recordSize);

    static void loadRecord(Address address, size_t recordSize, char *recordBuffer);

    Address saveRecord(char *recordPtr, size_t recordSize);

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
