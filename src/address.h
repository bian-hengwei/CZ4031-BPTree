//
// Created by Yu Song on 9/2/23.
//

#ifndef ADDRESS_H
#define ADDRESS_H


#include <cstddef>

class Address {
public:
    Address(char *blockPtr, size_t offset);

    char *getAbsoluteAddress() const;

    char *getBlockPtr() const;

    size_t getOffset() const;

private:
    char *blockPtr;
    std::size_t offset;
};


#endif //ADDRESS_H
