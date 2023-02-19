#ifndef BPTNODE_H
#define BPTNODE_H

#include "config.h"
#include "dbtypes.h"

class BPTNode;

typedef struct BPTNodeHead {
    char *parent_;
    bool leaf_;
    int num_keys_;
} BPTNodeHead;

const int NODE_HEAD_SIZE = sizeof(BPTNodeHead);
const int MAX_KEYS = (BLOCK_SIZE - NODE_HEAD_SIZE - sizeof(void *)) / (sizeof(int) + sizeof(void *));

class BPTNode {
public:
    explicit BPTNode(char *pBlock);

    ~BPTNode();

    bool IsLeaf() const;

    void SetLeaf(bool leaf);

    int NumKeys() const;

    void SetNumKeys(int num_keys);

    char *Parent() const;

    void SetParent(char *parent);

    int GetMinKey() const;

    int GetMaxKey() const;

    void Insert(int index, int key, char *value);

    void Remove(int index);

private:
    char *pBlock_;
    BPTNodeHead *node_head_;
    int *keys_;
    char **children_;
};

#endif