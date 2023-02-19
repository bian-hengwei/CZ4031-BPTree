//
// Created by Yu Song on 15/2/23.
//

#ifndef BLOCK_H
#define BLOCK_H


#include <cstddef>
#include <vector>
#include "config.h"

using namespace std;

class Block {
public:
    explicit Block(char *blockPtr);

    virtual ~Block();

    int GetNumOfUsedSlots() const;

    size_t GetUsedSize() const;

    bool isEmpty() const;

    char *AllocateRecord();

    int FindNextSlot();

    char *GetAddressBySlotIndex(int slotIndex);

    int GetSlotIndexByAddress(const char*address);

    void FreeRecord(char * recordAddress);

private:
    // the pointer to the memory address where the block starts
    char *currentBlockPtr;
    // since we use records and blocks that have fixed size, we divide the block into multiple slots.
    // each slot corresponds to a pointer of a record. if a record is allocated to a slot, the slot becomes 'true'.
    vector<bool> slots;
};


#endif //BLOCK_H
