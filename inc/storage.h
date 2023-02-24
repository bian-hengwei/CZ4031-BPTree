//
// Created by Yu Song on 9/2/23.
//

#ifndef STORAGE_H
#define STORAGE_H

#include<vector>

using namespace std;

class Storage {
public:
    explicit Storage(size_t disk_size);

    ~Storage();

    char *AddBlock();

    int GetBlockIndexByAddress(const char *address);

    int GetNumOfBlocks() const;

    char *AllocateBlock();  // initialize some block in storage and return the pointer to storage

    char *ReadBlock(char *pBlock);  // copies block from storage to memory

    void WriteBlock(char *pBlock, char *pBlockMem);

    void FreeBlock(char *pBlock);



private:
    size_t disk_size_;
    vector<char *> blocks_;
    char *pStorage_;
};


#endif //STORAGE_H
