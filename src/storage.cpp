#include "storage.h"
#include "types.h"

#include <iostream>
#include <vector>
#include <tuple>
#include <cstring>

using namespace std;

// constructor
DiskStorage::DiskStorage(size_t diskSize, size_t blockSize)
{
    // blockSize is the capacity of each block - set to 200B in main.cpp
    this->blockSize = blockSize; 

    // diskSize is the capacity of the disk
    this->diskSize = diskSize;   
    
    // Initialise the variables keeping track of usage to 0.
    this->realUsedSize = 0;
    this->allocatedBlocks = 0;
    this->usedSize = 0;

    this->diskPtr = operator new(diskSize);
    memset(diskPtr, '\0', diskSize); 
    this->blockPtr = nullptr;
    this->usedBlockSize = 0;
    this->visitedBlocks = 0;
}

bool DiskStorage::allocateBlock()
{
    // Allocate block as long as disk capacity still allows
    if (usedSize + blockSize <= diskSize)
    {
        usedSize = blockSize + usedSize;
        usedBlockSize = 0; // reset offset to 0
        blockPtr = (char *)diskPtr + allocatedBlocks * blockSize;
        allocatedBlocks += 1;
        return true;
    }
    else
    {
        cout << "Disk capacity is full! " << usedSize << "/" << diskSize << " B is used right now." << '\n';
        return false;
    }
}

Address DiskStorage::allocate(size_t recordSize)
{
    if (recordSize > blockSize)
    {
        cout << "Size of record to be allocated is larger than block size! (Size required:" << recordSize << "> Block size:" << blockSize << ")" << '\n';
        throw invalid_argument("Size requested for new record is bigger than block size!");
    }

    if (allocatedBlocks == 0 || (usedBlockSize + recordSize > blockSize))
    {
        if (allocateBlock() == false)
        {
            throw logic_error("Block allocation failed!");
        }
    }

    short int blockOffset = usedBlockSize;

    usedBlockSize += recordSize;
    realUsedSize += recordSize;

    Address recAddress = {blockPtr, blockOffset};

    return recAddress;
}

bool DiskStorage::deallocate(Address recAddress, size_t sizeDeallocated)
{
    try
    {
        // deleting the record from the block
        void *addressToDelete = (char *)recAddress.blockAddress + recAddress.offset;
        memset(addressToDelete, '\0', sizeDeallocated);

        realUsedSize -= sizeDeallocated;

        // case where block becomes empty after deallocation of record
        // creating a test block full of NULL values to test whether actual block is empty
        unsigned char testBlock[blockSize];
        memset(testBlock, '\0', blockSize);

        if (memcmp(testBlock, recAddress.blockAddress, blockSize) == 0) // if the first blockSize bytes of memory are equal in testBlock and in blockAddress
        {
            usedSize -= blockSize;
            allocatedBlocks--;
        }

        return true;
    }
    catch (...) // for all kinds of exceptions caught
    {
        cout << "Could not remove record/block at given address (" << recAddress.blockAddress << ") and offset (" << recAddress.offset << "). Please try again" << '\n';
        return false;
    };
}

void *DiskStorage::read(Address recAddress, size_t size)
{
    void *mainMemAddress = operator new(size);
    memcpy(mainMemAddress, (char *)recAddress.blockAddress + recAddress.offset, size);

    visitedBlocks++;

    return mainMemAddress;
}

Address DiskStorage::write(void *itemAddress, size_t size)
{
    Address diskAddress = allocate(size);
    memcpy((char *)diskAddress.blockAddress + diskAddress.offset, itemAddress, size);

    visitedBlocks++;

    return diskAddress;
}

// method overloading
Address DiskStorage::write(void *itemAddress, size_t size, Address diskAddress)
{
    memcpy((char *)diskAddress.blockAddress + diskAddress.offset, itemAddress, size);

    visitedBlocks++;

    return diskAddress;
}

// deconstructor
DiskStorage::~DiskStorage(){};