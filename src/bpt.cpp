//
// Created by hengwei on 2/19/2023.
//

#include "bpt.h"
#include "bptnode.h"

#include <vector>

BPT::BPT(char *pRoot, Storage storage) : root_(pRoot), initialized_(false), storage_(storage) {}

BPT::~BPT() = default;

void BPT::setRoot(char *pRoot){
    this->root_ = pRoot;
}

// Keylist and addresslist are sorted
void BPT::initializeBPT(int[] keylist, char*[] addresslist){
    if (intialized_){
        std::cout << "Already initialized!" << endl;
        return;
    }
    int maxKeyCount = MAX_KEYS;
    vector<BPTNode *> nodes;
    vector<BPTNode *> tempnodes;
    vector<char *> addresses;
    vector<char *> tempaddresses;
    vector<int> lowerbounds;
    vector<int> templowerbounds;
    size_t count = 0;
    BPTNode leafnode = new BPTNode(root_);
    BPTNode prevnode;
    char *firstnodeaddress = root_;
    int curKeyCount = 0;
    bool setmax = false;
    //Initialize leaf nodes
    while (count < keylist.size){
        if (leafnode.NumKeys() == maxKeyCount){
            
            
            prevnode = leafnode;
            char *nodeAddress = storage_.AddBlock();
            leafnode -> children_ + MAX_KEYS = nodeAddress;
            leafnode = new BPTNode(nodeAddress);
            nodes.push_back(leafnode);
            addresses.push_back(nodeAddress);
            lowerbounds.push_back(keylist[count]);
            curKeyCount = 0;

            //ensure minimum no of keys
            if((keylist.size - count) <= (2*maxKeyCount)){
                if (!setmax){
                    setmax = true;
                    if ((keylist.size - count - maxKeyCount) < floor((maxKeyCount + 1)/2)){
                        maxKeyCount = keylist.size - count - floor((maxKeyCount + 1)/2);
                    }
                }
            }
        }
        leafnode.Insert(curKeyCount, keylist[count], addresslist[count]);
        curKeyCount++;
        count++;
    }
    
    while (nodes.size > 0){
        maxKeyCount = MAX_KEYS;
        setmax = false;
        count = 0;
        for (int i = 0; i<nodes.size; i++){}
    }

    
    
    

}