//
// Created by hengwei on 2/19/2023.
//

#ifndef BPT_H
#define BPT_H

#include "bptnode.h"
#include "storage.h"

class BPT {
public:
    explicit BPT(char *pRoot, Storage *storage);

    ~BPT();

    void setRoot(char *pRoot);

    char *getRoot();

    void setInitialized(bool initialvalue);

    bool getInitialized() const;

    void initializeBPT(vector<int> keylist, vector<char *> addresslist);

    int getNoofNodes() const;

    int getNoofLevels() const;

    void PrintTree();

    void search(int lowerBoundKey, int upperBoundKey);

    int getIndexNodes() const;

    int getDataBlocks() const;

    int getAvgAvgRating() const;

    void Insert(int key, char *address);

    char *SearchLeafNode(int key);


private:
    char *root_;
    bool intialized_;
    Storage *storage_;
    int noofnodes;
    int nooflevels;
    int noofindexnodes;
    int noofdatablocks;
    int avgavgrating;

    static int SearchKeyIndex(BPTNode *node, int key);

    static void InsertToArray(BPTNode *node, int index, int key, char *address, int *keys, char **children);

    void SplitNonLeaf(char *leaf, int *keys, char **children);

    void SplitLeaf(char *leaf, int *keys, char **children);

    void InsertToParent(char *node, int middle_key, char *node_l_block, char *node_r_block);

    char *Find_Left_Leaf(char *leaf);
};

#endif //BPT_H