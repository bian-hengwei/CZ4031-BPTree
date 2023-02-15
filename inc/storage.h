//
// Created by Yu Song on 9/2/23.
//

#ifndef STORAGE_H
#define STORAGE_H

#include "block.h"
#include <vector>

using namespace std;

class Storage {
public:
    explicit Storage(size_t diskSize);

    virtual ~Storage();

    bool AddBlock();

    char *AllocateRecordSpace();

    int GetBlockIndexByAddress(const char *address);

    void DeleteRecord(char *recordAddress);

    static void ReadRecord(char *recordAddress, char *targetBuffer);

    char *WriteRecord(char *sourcePtr);

    int getNumOfBlocks() const;

    int getNumOfRecords() const;

private:
    size_t diskSize;
    vector<Block> blocks;
    char *storagePtr;
};


#endif //STORAGE_H
