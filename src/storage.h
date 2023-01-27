#ifndef STORAGE_H
#define STORAGE_H

#include "types.h"

using namespace std;

class DiskStorage
{
public:
    // constructor
    DiskStorage(size_t diskSize, size_t blockSize);

    // allocates a block from the memory pool
    bool allocateBlock();

    // allocates a chunk within a block
    Address allocate(size_t recordSize);

    // deletes existing record from its block, deallocates block if it becomes empty after deletion of record
    bool deallocate(Address recAddress, size_t sizeDeallocated);

    // returns data entry based on input parameters
    void *read(Address recAddress, size_t size);

    // write data entry into disk in a given memory address
    Address write(void *itemAddress, size_t size);

    // Method overloading
    // update data entry from disk from a given memory
    Address write(void *itemAddress, size_t size, Address diskAddress);

    size_t getPoolSize() const
    {
        return diskSize;
    }

    size_t getBlockSize() const
    {
        return blockSize;
    };

    size_t getUsedBlockSize() const
    {
        return usedBlockSize;
    };

    size_t getUsedSize() const
    {
        return usedSize;
    }

    size_t getRealUsedSize() const
    {
        return realUsedSize;
    }

    int getAllocatedBlocks() const
    {
        return allocatedBlocks;
    };

    int getVisitedBlocks() const
    {
        return visitedBlocks;
    }

    int resetVisitedBlocks()
    {
        int temp = visitedBlocks;
        visitedBlocks = 0;
        return temp;
    }

    // destructor
    ~DiskStorage();

private:
    size_t diskSize;       // max pool size
    size_t blockSize;      // fixed size of each block
    size_t usedSize;       // current storage size used based on number of blocks used
    size_t realUsedSize; // actual storage size used based on size of records
    size_t usedBlockSize;  // storage size used within current block pointed to
    int allocatedBlocks;
    int visitedBlocks;
    void *diskPtr;
    void *blockPtr;
};


#endif