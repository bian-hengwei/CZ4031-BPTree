//
// Created by Yu Song on 9/2/23.
//

#include "address.h"

Address::Address(char *blockPtr, size_t offset) {
    this->blockPtr = blockPtr;
    this->offset = offset;
}

char *Address::getBlockPtr() const {
    return blockPtr;
}

size_t Address::getOffset() const {
    return offset;
}

char *Address::getAbsoluteAddress() const {
    return getBlockPtr() + getOffset();
}
