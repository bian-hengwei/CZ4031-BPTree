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

    int getNumOfNodes() const;

    int getNumOfLevels() const;

    void PrintTree();

    void search(unsigned int lowerBoundKey, unsigned int upperBoundKey);

    void Insert(unsigned int key, char *address);

    char *SearchLeafNode(unsigned int key);

    bool DeleteRecord(unsigned int keyToDelete);

    void FixTree(char *leafNodeAddr);


private:
    char *root_;
    bool initialized_;
    Storage *storage_;
    int num_of_nodes_;
    int num_of_levels_;

    static int SearchKeyIndex(BPTNode *node, unsigned int key);

    static void InsertToArray(BPTNode *node, int index, unsigned int key, char *address, unsigned int *keys, char **children);

    void SplitNonLeaf(char *leaf, unsigned int *keys, char **children);

    void SplitLeaf(char *leaf, unsigned int *keys, char **children);

    void InsertToParent(char *node, unsigned int middle_key, char *node_l_block, char *node_r_block);

    static void UpdateKeysToNextLB(BPTNode *nodeToUpdate);

    static char *Find_Left_Leaf(char *leaf);
};

#endif //BPT_H