//
// Created by hengwei on 2/19/2023.
//

#include "bpt.h"
#include "bptnode.h"

#include <vector>
#include <math.h>
#include <cassert>

BPT::BPT(char *pRoot, Storage storage) : root_(pRoot), intialized_(false), storage_(storage) {}

BPT::~BPT() = default;

void BPT::setRoot(char *pRoot) {
    this->root_ = pRoot;
}

char *BPT::getRoot() {
    return this->root_;
}

void BPT::setInitialized(bool initialvalue) {
    this->intialized_ = initialvalue;
}

bool BPT::getInitialized() {
    return this->intialized_;
}

// Keylist and addresslist are sorted
void BPT::initializeBPT(vector<int> keylist, vector<char *> addresslist) {
    if (getInitialized()) {
        return;
    }
    int maxKeyCount = MAX_KEYS;
    vector<BPTNode *> nodes;
    vector<BPTNode *> tempnodes;
    vector<char *> addresses;
    vector<char *> tempaddresses;
    // lowerbounds will be used later for inserting keys of 2nd and above levels.
    vector<int> lowerbounds;
    vector<int> templowerbounds;
    size_t count = 0;
    BPTNode *leafnode = new BPTNode(getRoot());
    BPTNode *nonleafNode;
    int curKeyCount = 0;
    bool setmax = false;
    //Initialize leaf nodes
    addresses.push_back(getRoot());
    // note that we do not push the first node of each level into the nodes vector.
    // This is because while we need the first node address, we do not need the actual node
    while (count < keylist.size()) {
        if (curKeyCount == maxKeyCount) {
            char *nodeAddress = storage_.AllocateBlock();
            leafnode->SetChild(MAX_KEYS, nodeAddress);
            leafnode = new BPTNode(nodeAddress);
            nodes.push_back(leafnode);
            addresses.push_back(nodeAddress);
            lowerbounds.push_back(keylist[count]);
            curKeyCount = 0;

            //ensure minimum no of keys
            if ((keylist.size() - count) <= (2 * maxKeyCount)) {
                if (!setmax) {
                    setmax = true;
                    if ((keylist.size() - count - maxKeyCount) < floor((maxKeyCount + 1) / 2)) {
                        maxKeyCount = keylist.size() - count - floor((maxKeyCount + 1) / 2);
                    }
                }
            }
        }
        leafnode->InsertKey(curKeyCount, keylist[count]);
        leafnode->InsertChild(curKeyCount, addresslist[count]);
        curKeyCount++;
        count++;
    }
    if (nodes.size()>0){
        assert(curKeyCount>=floor((maxKeyCount + 1) / 2));
    }

    // Recursively create levels. If nodes.size == 0, means that only one node in the level.
    while (nodes.size() > 0) {
        maxKeyCount = MAX_KEYS;
        setmax = false;
        count = 0;
        char *nodeAddress = storage_.AllocateBlock();
        nonleafNode = new BPTNode(nodeAddress);

        // Here we
        nonleafNode->SetChild(0, addresses[0]);
        tempaddresses.push_back(nodeAddress);
        curKeyCount = 0;
        for (int i = 0; i < nodes.size(); i++) {
            if (curKeyCount == maxKeyCount) {

                char *nodeAddress = storage_.AllocateBlock();
                nonleafNode = new BPTNode(nodeAddress);
                tempaddresses.push_back(nodeAddress);


                // Insert the first pointer everytime we create a node and continue.
                // Note that since we also added an extra address in the addresses vector every loop, (the first one)
                // the corresponding index of the address to the node is i + 1.
                nonleafNode->InsertChild(0, addresses[i + 1]);

                // Pay attention to this section. The relevant index of the node that
                // the first pointer of the node points to is inserted in lowerbounds instead of the first key.
                templowerbounds.push_back(lowerbounds[i]);

                tempnodes.push_back(nonleafNode);
                curKeyCount = 0;

                // this ensures the minimum amount of keys.
                if ((nodes.size() - i) <= (2 * (maxKeyCount) + 1)) {
                    if (!setmax) {
                        setmax = true;
                        if ((nodes.size() - i - maxKeyCount) < (floor((maxKeyCount) / 2) + 1)) {
                            maxKeyCount = nodes.size() - i - floor((maxKeyCount) / 2) - 1;
                        }
                    }
                }
            } else {
                nonleafNode->InsertKey(curKeyCount, lowerbounds[i]);
                nonleafNode->InsertChild(curKeyCount + 1, addresses[i + 1]);
                curKeyCount++;
            }
        }
        vector<BPTNode *> nodes(tempnodes);
        vector<char *> addresses(tempaddresses);
        vector<int> lowerbounds(templowerbounds);
        tempnodes.clear();
        tempaddresses.clear();
        lowerbounds.clear();

        if (nodes.size()>0){
            assert(curKeyCount>=floor((maxKeyCount) / 2));
        }
    }
    setRoot(addresses[0]);
    setInitialized(true);

}