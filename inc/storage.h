//
// Created by Yu Song on 9/2/23.
//

#ifndef STORAGE_H
#define STORAGE_H

#include "dataTypes.h"
#include "address.h"

class Storage {
public:
    static size_t RECORD_SIZE;

    Storage(size_t diskSize, size_t blockSize);

    virtual ~Storage();

    void addBlock();

    Address allocRecord();

    bool isMemoryBlockSetToZero(const char *blockPtr) const;

    void deallocRecord(Address recordAddress);

    static void loadRecord(Address address, char *recordBuffer);

    Address saveRecord(char *recordPtr);

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
