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

    int GetNumOfBlocks() const;

    int GetNumOfRecords() const;

    bool IsLatestBlockFull();

    char *GetBlockByIndex(int index);

    char *GetLatestBlock();

    char *AllocateBlock();  // initialize some block in storage and return the pointer to storage

    static void ReadBlock(char *pBuffer, char *pBlock);  // copies block from storage to memory

    static void WriteBlock(char *pBlock, char *pBuffer);

    void FreeBlock(char *pBlock);

    char *FindSpaceForBlock();

private:
    size_t disk_size_;
    vector<char *> blocks_;
    char *pStorage_;
};


#endif //STORAGE_H
