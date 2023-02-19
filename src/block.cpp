//
// Created by Yu Song on 15/2/23.
//

#include "block.h"
#include <algorithm>

using namespace std;

Block::~Block() = default;

Block::Block(char *blockPtr) : currentBlockPtr(blockPtr) {
    int numOfSlots = BLOCK_SIZE / RECORD_SIZE;
    // initialize the elements in slot vector to false, i.e. not associated with a record pointer;
    for (int i = 0; i < numOfSlots; i++) {
        slots.push_back(false);
    }
}

int Block::GetNumOfUsedSlots() const {
    return (int) count(slots.begin(), slots.end(), true);
}

size_t Block::GetUsedSize() const {
    // induce based on the number of slots used.
    return GetNumOfUsedSlots() * RECORD_SIZE;
}


int Block::FindNextSlot() {
    // find a slot that has not been allocated to a record.
    for (int i = 0; i < slots.size(); i++) {
        if (!slots[i]) {
            return i;
        }
    }
    return -1; // All slots are true
}

char *Block::AllocateRecord() {
    int slotIndex = FindNextSlot();
    if (slotIndex != -1) {
        slots[slotIndex] = true;
        char *recordAddress = GetAddressBySlotIndex(slotIndex);
        return recordAddress;
    }
    return nullptr;
}

void Block::FreeRecord(char *recordAddress) {
    int slotIndex = GetSlotIndexByAddress(recordAddress);
    slots[slotIndex] = false;
    // free the space allocated for the record
    memset(recordAddress, '\0', RECORD_SIZE);
}

char *Block::GetAddressBySlotIndex(int slotIndex) {
    return currentBlockPtr + slotIndex * RECORD_SIZE;
}

int Block::GetSlotIndexByAddress(const char *address) {
    size_t diff = address - currentBlockPtr;
    return (int) (diff / RECORD_SIZE);
}

bool Block::isEmpty() const {
    return GetNumOfUsedSlots() == 0;
}