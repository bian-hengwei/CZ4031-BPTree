#ifndef BPTNODE_H
#define BPTNODE_H

#include "config.h"
#include "dbtypes.h"

class BPTNode;

typedef struct BPTNodeHead {
    char *parent_;
    bool leaf_;
    int num_keys_;
} BPTNodeHead;  // 16 Bytes

const int NODE_HEAD_SIZE = sizeof(BPTNodeHead);
const int MAX_KEYS = (BLOCK_SIZE - BLOCK_HEADER_SIZE - NODE_HEAD_SIZE - sizeof(void *)) /
                     (sizeof(int) + sizeof(void *));

class BPTNode {
public:
    explicit BPTNode(char *pBlock);

    ~BPTNode();

    void SaveNode() const;

    char *GetAddress() const;

    bool IsLeaf() const;

    void SetLeaf(bool leaf);

    int GetNumKeys() const;

    void SetNumKeys(int num_keys);

    char *GetParent() const;

    void SetParent(char *parent);

    unsigned int GetMinKey() const;

    unsigned int GetMaxKey() const;

    unsigned int GetKey(unsigned short index) const;

    char *GetChild(unsigned short index) const;

    void SetKey(unsigned short index, unsigned int key);

    void SetChild(unsigned short index, char *child);

    void InsertKey(unsigned short index, unsigned int key);

    void InsertChild(unsigned short index, char *child);

    void RemoveKey(unsigned short index);

    void RemoveChild(unsigned short index);

    int GetChildIndex(const char *childNodeAddress) const;

    bool IsNumOfKeysEnough() const;

private:
    char *pBlock_;
    char *pBlockMem_;
    BPTNodeHead *bpt_node_head_;
    unsigned int *keys_;
    char **children_;
};

#endif