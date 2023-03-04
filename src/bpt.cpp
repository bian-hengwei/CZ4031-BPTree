//
// Created by hengwei on 2/19/2023.
//

#include "bpt.h"
#include "bptnode.h"
#include "block.h"
#include "dbtypes.h"

#include <cmath>
#include <cassert>
#include <iostream>
#include <queue>
#include <vector>
#include <chrono>

BPT::BPT(char *pRoot, Storage *storage) : root_(pRoot), intialized_(false), storage_(storage), noofnodes(1),
                                          nooflevels(1), noofindexnodes(1), noofdatablocks(0), avgavgrating(0) {}

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

bool BPT::getInitialized() const {
    return this->intialized_;
}

int BPT::getNoofNodes() const {
    return noofnodes;
}

int BPT::getNoofLevels() const {
    return nooflevels;
}

int BPT::getIndexNodes() const {
    return noofindexnodes;
}

int BPT::getDataBlocks() const {
    return noofdatablocks;
}

int BPT::getAvgAvgRating() const {
    return avgavgrating;
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
    int count = 0;
    auto *leafnode = new BPTNode(getRoot());
    BPTNode *nonleafNode;
    char *nodeAddress;
    int curKeyCount = 0;
    bool setmax = false;

    //Setup first note
    addresses.push_back(getRoot());
    leafnode->SetLeaf(true);

    // note that we do not push the first node of each level into the nodes vector.
    // This is because while we need the first node address, we do not need the actual node
    if ((keylist.size() <= (2 * maxKeyCount)) & (keylist.size() > maxKeyCount)) {
        setmax = true;
        if ((keylist.size() - maxKeyCount) < floor((maxKeyCount + 1) / 2)) {
            maxKeyCount = keylist.size() - floor((maxKeyCount + 1) / 2);
        }
    }

    // This loop initializes all leaf nodes.
    while (count < keylist.size()) {
        if (curKeyCount == maxKeyCount) {
            nodeAddress = storage_->AllocateBlock();
            leafnode->SetChild(MAX_KEYS, nodeAddress);

            block::bpt::Initialize_D(nodeAddress);
            leafnode = new BPTNode(nodeAddress);
            leafnode->SetLeaf(true);
            nodes.push_back(leafnode);
            addresses.push_back(nodeAddress);
            lowerbounds.push_back(keylist[count]);
            curKeyCount = 0;
            noofnodes++;

            //ensure minimum no of keys. This checks that there are an amount of keys
            // that can fit in at most 2 leaf nodes.  
            if ((keylist.size() - count) <= (2 * maxKeyCount)) {
                if (!setmax) {
                    setmax = true;
                    if ((keylist.size() - count - maxKeyCount) < floor((maxKeyCount + 1) / 2)) {
                        // Set maxKeyCount for the next node so that the final node will have the mininum amount of keys.
                        // reset the maxKeyCount after initialization is finished.
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
    if (!nodes.empty()) {
        assert(curKeyCount >= floor((MAX_KEYS + 1) / 2));
    }

    // Recursively create levels. If nodes.size == 0, means that only one node in the level.
    while (!nodes.empty()) {
        nooflevels++;
        noofnodes++;

        maxKeyCount = MAX_KEYS;
        setmax = false;

        nodeAddress = storage_->AllocateBlock();

        block::bpt::Initialize_D(nodeAddress);
        nonleafNode = new BPTNode(nodeAddress);

        // Here we set initial parameters of the first node in the level.
        nonleafNode->SetChild(0, addresses[0]);
        nonleafNode->SetLeaf(false);
        tempaddresses.push_back(nodeAddress);
        curKeyCount = 0;
        if (((nodes.size()) <= (2 * (maxKeyCount) + 1)) & (nodes.size() > maxKeyCount)) {
            setmax = true;
            if ((nodes.size() - maxKeyCount) < (floor((maxKeyCount) / 2) + 1)) {
                maxKeyCount = nodes.size() - floor((maxKeyCount) / 2) - 1;
            }
        }
        for (int i = 0; i < nodes.size(); i++) {
            if (curKeyCount == maxKeyCount) {

                nodeAddress = storage_->AllocateBlock();
                block::bpt::Initialize_D(nodeAddress);
                nonleafNode = new BPTNode(nodeAddress);
                nonleafNode->SetLeaf(false);
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
                noofnodes++;
                // this ensures the minimum amount of keys.
                if ((nodes.size() - i) <= (2 * (MAX_KEYS) + 1)) {
                    if (!setmax) {
                        setmax = true;
                        if ((nodes.size() - i - MAX_KEYS) < (floor((MAX_KEYS) / 2) + 1)) {
                            // Set maxKeyCount for the next node so that the final node will have the mininum amount of keys.
                            // reset the maxKeyCount after level initialization is finished.
                            maxKeyCount = nodes.size() - i - floor((MAX_KEYS) / 2) - 1;
                        }
                    }
                }
            } else {
                nonleafNode->InsertKey(curKeyCount, lowerbounds[i]);
                nonleafNode->InsertChild(curKeyCount + 1, addresses[i + 1]);
                curKeyCount++;
            }
        }

        nodes = tempnodes;
        addresses = tempaddresses;
        lowerbounds = templowerbounds;
        tempnodes.clear();
        tempaddresses.clear();
        templowerbounds.clear();
        assert(tempnodes.empty());

        if (!nodes.empty()) {
            assert(curKeyCount >= floor((maxKeyCount / 2)));
        }
    }
    setRoot(addresses[0]);
    setInitialized(true);
}

void BPT::PrintTree() {
    queue<char *> node_q;
    node_q.push(root_);
    unsigned int level_nodes_next = 0;
    unsigned int level_nodes = 1;  // No. of nodes left in this level
    while (!node_q.empty()) {
        char *pNode = node_q.front();
        node_q.pop();

        auto *node = new BPTNode(pNode);

        for (int i = 0; i < node->GetNumKeys(); i++) {
            std::cout << node->GetKey(i) << " ";
        }

        std::cout << " | ";

        if (!node->IsLeaf()) {
            for (int i = 0; i < node->GetNumKeys() + 1; i++) {
                node_q.push(node->GetChild(i));
                level_nodes_next++;
            }
        }

        level_nodes--;
        if (level_nodes == 0) {  // end of level is reached
            level_nodes = level_nodes_next;
            std::cout << std::endl << std::endl;
            level_nodes_next = 0;
        }
    }
}

void BPT::search(int lowerBoundKey, int upperBoundKey) //take in lower and upper range
{
    // Traverse the tree, start with the root and move left and right accordingly
    auto *node = new BPTNode(getRoot());
    bool stop = false; // default is false because not found any keys
    int totalavgrating = 0;
    int numavgrating = 0;

    while (!node->IsLeaf()) {
        noofindexnodes++;
        node = new BPTNode(node->GetChild(SearchKeyIndex(node, lowerBoundKey)));
    }
    while (!stop) // continue search over the entire range (reach leaf node)
    {
        int i;
        for (i = 0; i < node->GetNumKeys(); i++) {
            // print the movie record, found
            char *pRecord = node->GetChild(i);
            int offset = (pRecord - storage_->GetAddress()) % BLOCK_SIZE;
            char *pBlockMem = pRecord - offset;
            RecordMovie *recordMovie = dbtypes::ReadRecordMovie(pBlockMem, offset);
            if (recordMovie->num_votes > upperBoundKey) // continue till you reach upperBoundKey
            {
                stop = true;
                break; // reach the upper bound key
            }
            if (recordMovie->num_votes >= lowerBoundKey && recordMovie->num_votes <= upperBoundKey) {
                totalavgrating = recordMovie->avg_rating + totalavgrating;
                numavgrating++;
//                cout << "Movie Record -- tconst: " << recordMovie->tconst << " avgRating: "
//                     << recordMovie->avg_rating
//                     << " numVotes: " << recordMovie->num_votes << endl;
            }
        }

        // Follow the right sibling pointer to the next leaf node
        if (!stop && node->GetChild(node->GetNumKeys()) != nullptr) {
            node = new BPTNode(node->GetChild(node->GetNumKeys()));
        } else {
            stop = true; // no right sibling or end of search range
        }
    }
    cout << "found " << numavgrating << " records" << endl;
    if (numavgrating == 0) {
        avgavgrating = 0;
    } else {
        avgavgrating = totalavgrating / numavgrating;
    }
}

char *BPT::Find_Left_Leaf(char *leaf) {
    // try to find node before this leaf
    auto *node_fix_l = new BPTNode(leaf);
    char *child_address = leaf;
    int node_fix_l_index = 0;
    while (node_fix_l->GetParent() && node_fix_l_index <= 0) {
        node_fix_l = new BPTNode(node_fix_l->GetParent());
        node_fix_l_index = node_fix_l->GetChildIndex(child_address);
        child_address = node_fix_l->GetAddress();
    }
    if (node_fix_l_index > 0) {
        auto *node_l_l = new BPTNode(node_fix_l->GetChild(node_fix_l_index - 1));
        while (!node_l_l->IsLeaf()) {
            node_l_l = new BPTNode(node_l_l->GetChild(node_l_l->GetNumKeys()));
        }
        return node_l_l->GetAddress();
    }
    return nullptr;
}

char *BPT::SearchLeafNode(int key) {
    auto *node = new BPTNode(root_);
    while (!node->IsLeaf()) {
        int keyIndex = SearchKeyIndex(node, key);
//        cout << "Key index: " << keyIndex << endl;
        char *childAddr = node->GetChild(keyIndex);
//        cout << "Child Addr: " << childAddr << endl;
        node = new BPTNode(childAddr);
    }
    return node->GetAddress();
}

int BPT::SearchKeyIndex(BPTNode *node, int key) {
    int i = 0;
    while (i < node->GetNumKeys() && key > node->GetKey(i)) {
        i += 1;
    }
    return i;
}

void BPT::InsertToArray(BPTNode *node, int index, int key, char *address, int *keys, char **children) {
    for (int j = 0; j < node->GetNumKeys() + 1; j++) {
        if (j < index) {
            keys[j] = node->GetKey(j);
            children[j] = node->GetChild(j);
        } else if (index == j) {
            keys[j] = key;
            children[j] = address;
        } else {
            keys[j] = node->GetKey(j - 1);
            children[j] = node->GetChild(j - 1);
        }
    }
    children[MAX_KEYS + 1] = node->GetChild(MAX_KEYS);
}

/**
 * Assume node_l and node_r already correctly constructed,
 * this function tries to either insert them into their parents.
 */
void BPT::InsertToParent(char *node, int middle_key, char *node_l_block, char *node_r_block) {
    auto *current_node = new BPTNode(node);
    auto *node_l = new BPTNode(node_l_block);
    auto *node_r = new BPTNode(node_r_block);
    if (node == root_) {
        // allocate a new block for root
        char *new_root_block = storage_->AllocateBlock();
        block::bpt::Initialize_D(new_root_block);
        auto *new_root = new BPTNode(new_root_block);
        node_l->SetParent(new_root_block);
        node_r->SetParent(new_root_block);
        // initialize root and insert values
        new_root->SetLeaf(false);
        new_root->InsertKey(0, middle_key);
        new_root->InsertChild(0, node_l_block);
        new_root->InsertChild(1, node_r_block);
        // update root address
        root_ = new_root_block;
        nooflevels++;
    } else {
        // if node is not root, we find its parent
        char *parent = current_node->GetParent();
        auto *parent_node = new BPTNode(parent);
        node_l->SetParent(parent);
        node_r->SetParent(parent);
        int i = 0;  // address of node
        while (parent_node->GetChild(i) != node) {
            i++;
        }
        // modify node value
        parent_node->SetChild(i, node_r_block);
        if (parent_node->GetNumKeys() < MAX_KEYS) {
            parent_node->InsertKey(i, middle_key);
            parent_node->InsertChild(i, node_l_block);
            return;
        }
        int parent_keys[MAX_KEYS + 1];
        char *parent_children[MAX_KEYS + 2];
        InsertToArray(parent_node, i, middle_key,
                      node_l_block, parent_keys, parent_children);
        SplitNonLeaf(parent_node->GetAddress(), parent_keys, parent_children);
    }
}

/**
 * Splits a non-leaf node
 */
void BPT::SplitNonLeaf(char *node, int keys[], char *children[]) {
    noofnodes++;
    int middle_key;
    int size_l = (MAX_KEYS + 1) / 2;  // 7 keys & 8 children; size_r = MAX_KEYS - size_l;  // 7 keys & 8 children

    // initialize new blocks
    char *node_l_block = storage_->AllocateBlock();
    block::bpt::Initialize_D(node_l_block);
    char *node_r_block = storage_->AllocateBlock();
    block::bpt::Initialize_D(node_r_block);
    auto *node_l = new BPTNode(node_l_block);
    auto *node_r = new BPTNode(node_r_block);
    node_l->SetLeaf(false);
    node_r->SetLeaf(false);

    // copy keys
    for (int i = 0; i < MAX_KEYS + 1; i++) {
        if (i < size_l) {
            node_l->InsertKey(i, keys[i]);
        } else if (i == size_l) {
            middle_key = keys[i];
        } else {
            node_r->InsertKey(i - size_l - 1, keys[i]);
        }
    }

    // copy children
    for (int i = 0; i < MAX_KEYS + 2; i++) {
        auto *temp = new BPTNode(children[i]);
        if (i <= size_l) {
            node_l->InsertChild(i, children[i]);
            temp->SetParent(node_l->GetAddress());
        } else {
            node_r->InsertChild(i - size_l - 1, children[i]);
            temp->SetParent(node_r->GetAddress());
        }
    }

    InsertToParent(node, middle_key, node_l_block, node_r_block);

    storage_->FreeBlock(node);
}

void BPT::SplitLeaf(char *leaf, int keys[], char *children[]) {
    noofnodes++;
    int size_l = (MAX_KEYS + 1) / 2;  // 7 keys; size_r = MAX_KEYS + 1 - size_l;  // 8 keys
    int middle_key = keys[size_l];

    char *node_l_block = storage_->AllocateBlock();
    block::bpt::Initialize_D(node_l_block);
    char *node_r_block = storage_->AllocateBlock();
    block::bpt::Initialize_D(node_r_block);

    auto *node_l = new BPTNode(node_l_block);
    auto *node_r = new BPTNode(node_r_block);
    node_l->SetLeaf(true);
    node_r->SetLeaf(true);

    // copy keys
    for (int i = 0; i < MAX_KEYS + 1; i++) {
        if (i < size_l) {
            node_l->InsertKey(i, keys[i]);
        } else {
            node_r->InsertKey(i - size_l, keys[i]);
        }
    }

    // copy children
    for (int i = 0; i < MAX_KEYS + 2; i++) {
        if (i < size_l) {
            node_l->InsertChild(i, children[i]);
        } else {
            node_r->InsertChild(i - size_l, children[i]);
        }
    }
    node_l->SetChild(size_l, node_r->GetAddress());

    char *left_leaf = Find_Left_Leaf(leaf);
    if (left_leaf) {
        BPTNode *left_leaf_node = new BPTNode(left_leaf);
        left_leaf_node->SetChild(left_leaf_node->GetNumKeys(), node_l_block);
    }

    InsertToParent(leaf, middle_key, node_l_block, node_r_block);

    storage_->FreeBlock(leaf);
}

void BPT::Insert(int key, char *address) {
    auto *leaf_node = new BPTNode(SearchLeafNode(key));
    int i = SearchKeyIndex(leaf_node, key);

    if (leaf_node->GetNumKeys() < MAX_KEYS) {
        leaf_node->InsertKey(i, key);
        leaf_node->InsertChild(i, address);
        return;
    }
    int keys[MAX_KEYS + 1];
    char *children[MAX_KEYS + 2];
    InsertToArray(leaf_node, i, key, address, keys, children);
    SplitLeaf(leaf_node->GetAddress(), keys, children);
}


/**
 * Delete a record given the key of the record in the B+ tree index
 * @param keyToDelete the key that corresponds to the record
 * @return true if deletion is successful. If false is returned, it means the key may not exist in the index
 * @author Song Yu
 */
bool BPT::DeleteRecord(int keyToDelete) {
    cout << "deleting key " << keyToDelete << endl;
    // start timer
    auto start = std::chrono::high_resolution_clock::now();
    // find nodes of the target
    auto *leafNode = new BPTNode(SearchLeafNode(keyToDelete));

    // get address of the record
    char *recordAddr = nullptr;
    int targetIndex = -1;
    for (int i = 0; i < leafNode->GetNumKeys(); i++) {
        if (leafNode->GetKey(i) == keyToDelete) {
            recordAddr = leafNode->GetChild(i);
            targetIndex = i;
            break;
        }
    }

    if (!recordAddr || targetIndex == -1) {
        // TODO: update the way to get the proper next node
        char *nextNodeAddr = leafNode->GetChild(leafNode->GetNumKeys());

        if (nextNodeAddr) {
            auto *nextNode = new BPTNode(nextNodeAddr);

            if (nextNode->GetMinKey() == keyToDelete) {
                leafNode = nextNode;
                cout << "updated leaf node to next node" << endl;
                recordAddr = leafNode->GetChild(0);
                targetIndex = 0;
            } else {
                cout << "The key " << keyToDelete << " is not in tree. Unable to delete" << endl;
                // end timer
                auto finish = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> elapsed = finish - start;
                std::cout << "Elapsed time: " << elapsed.count() << " s\n";
                return false;
            }
        } else {
            cout << "The key " << keyToDelete << " is not in tree. Unable to delete" << endl;
            // end timer
            auto finish = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed = finish - start;
            std::cout << "Elapsed time: " << elapsed.count() << " s\n";
            return false;
        }
    }

    // free record
    int offset = (recordAddr - storage_->GetAddress()) % BLOCK_SIZE;
    // the block where the record resides
    char *pBlock = recordAddr - offset;
    cout << "Freeing block " << (void *) pBlock << " with offset " << offset << endl;
    block::record::FreeSlot_D(pBlock, offset);

    // delete the corresponding key & child;
    cout << "removing child & key for target Index " << targetIndex << endl;
    leafNode->RemoveChild(targetIndex);
    leafNode->RemoveKey(targetIndex);

    // fix the remaining B+ tree
    FixTree(leafNode->GetAddress());

    // end timer
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    std::cout << "Key " << keyToDelete << " deleted! Elapsed time: " << elapsed.count() << " s\n";
    return true;
}

/**
 * Fix the B+ tree after the deletion, which may cause changes that could invalidate the B+ tree structure
 * @param leafNodeAddr the address of the leaf node where a key is deleted
 * @author Song Yu
 */
void BPT::FixTree(char *leafNodeAddr) {
    auto *curNode = new BPTNode(leafNodeAddr);
    while (curNode->GetAddress() != getRoot()) {
        // If the current node has too few keys, borrow or merge with siblings
        if (!curNode->IsNumOfKeysEnough()) {
            auto *parentNode = new BPTNode(curNode->GetParent());
            // TODO: update the way to get the proper left sibling
            int curIndex = parentNode->GetChildIndex(leafNodeAddr);

            // Try to borrow a key from the left sibling
            if (curIndex > 0) {
                int leftSiblingIndex = curIndex - 1;
                auto *leftSibling = new BPTNode(parentNode->GetChild(leftSiblingIndex));
                if (leftSibling->IsNumOfKeysEnough()) {
                    int indexToBorrow = leftSibling->GetNumKeys() - 1;
                    // get the key to borrow from the left sibling
                    int borrowedKey = leftSibling->GetKey(indexToBorrow);
                    // get the rightmost child of the left sibling
                    auto *borrowedChildAddr = leftSibling->GetChild(indexToBorrow);
                    // insert the borrowed key and child into the current node
                    curNode->InsertKey(0, borrowedKey);
                    curNode->InsertChild(0, borrowedChildAddr);
                    // remove the borrowed key and child from the left sibling
                    leftSibling->RemoveChild(indexToBorrow);
                    leftSibling->RemoveKey(indexToBorrow);
                    // update the parent's key that separates the current node and the left sibling
                    parentNode->SetKey(leftSiblingIndex, curNode->GetKey(0));

                    break;
                }
            }

            // Try to borrow a key from the right sibling
            if (curIndex < parentNode->GetNumKeys()) {
                int rightSiblingIndex = curIndex + 1;
                auto *rightSibling = new BPTNode(parentNode->GetChild(rightSiblingIndex));
                if (rightSibling->IsNumOfKeysEnough()) {
                    int indexToBorrow = 0;
                    // get the key to borrow from the right sibling
                    int borrowedKey = rightSibling->GetKey(indexToBorrow);
                    // get the leftmost child of the right sibling
                    auto *borrowedChildAddr = rightSibling->GetChild(indexToBorrow);
                    // insert the borrowed key and child into the current node
                    curNode->InsertKey(curNode->GetNumKeys(), borrowedKey);
                    curNode->InsertChild(curNode->GetNumKeys(), borrowedChildAddr);
                    // remove the borrowed key and child from the right sibling
                    rightSibling->RemoveChild(indexToBorrow);
                    rightSibling->RemoveKey(indexToBorrow);
                    // update the parent's key that separates the current node and the right sibling
                    parentNode->SetKey(curIndex, curNode->GetKey(0));

                    break;
                }
            }

            // Merge with the left sibling
            if (curIndex > 0) {
                int leftSiblingIndex = curIndex - 1;
                auto *leftSibling = new BPTNode(parentNode->GetChild(leftSiblingIndex));

                for (int i = 0; i < curNode->GetNumKeys(); i++) {
                    leftSibling->InsertKey(leftSibling->GetNumKeys(), curNode->GetKey(i));
                    leftSibling->InsertChild(leftSibling->GetNumKeys(), curNode->GetChild(i));
                }

                // remove curNode
                parentNode->RemoveChild(curIndex);
                parentNode->RemoveKey(curIndex);


                // update curNode to left sibling
                curNode = leftSibling;
            } else {
                // Merge with the right sibling
                int rightSiblingIndex = curIndex + 1;
                auto *rightSibling = new BPTNode(parentNode->GetChild(rightSiblingIndex));

                for (int i = 0; i < rightSibling->GetNumKeys(); i++) {
                    curNode->InsertKey(curNode->GetNumKeys(), rightSibling->GetKey(i));
                    curNode->InsertChild(curNode->GetNumKeys(), rightSibling->GetChild(i));
                }

                // remove right sibling
                parentNode->RemoveChild(rightSiblingIndex);
                parentNode->RemoveKey(rightSiblingIndex);
            }

        }

        // fixing the number of keys in each node, then we update values for non-leaf nodes at current level
        if (!curNode->IsLeaf()) {
            auto *parentNode = new BPTNode(curNode->GetParent());
            int numOfNodesInCurrentLevel = parentNode->GetNumKeys() + 1;
            // iterate all nodes at current level
            for (int i = 0; i < numOfNodesInCurrentLevel; i++) {
                auto *currentLevelNode = new BPTNode(parentNode->GetChild(i));
                // ensure all keys in this node is the lower bound of the next child
                // see B+ tree lecture slide page 15, get LB recursively
                UpdateKeysToNextLB(currentLevelNode);
            }
        }

        // Update the current node to its parent
        curNode = new BPTNode(curNode->GetParent());
    }

    auto *rootNode = new BPTNode(getRoot());

    // If the root has no keys, make the first child the new root
    if (rootNode->GetNumKeys() == 0) {
        char *newRootAddr = rootNode->GetChild(0);
        auto *newRootNode = new BPTNode(newRootAddr);
        newRootNode->SetParent(nullptr);
        setRoot(newRootAddr);
    } else {
        // update the keys in root
        UpdateKeysToNextLB(rootNode);
    }
}

/**
 * Update the keys of non-leaf nodes so that the ith key equals the lower bound of the range of the i+1th pointer
 * See B+ tree lecture slide page 15 for more information
 * @param nodeToUpdate the non-leaf node whose keys that need to be updated
 * @author Song Yu
 */
void BPT::UpdateKeysToNextLB(BPTNode *nodeToUpdate) {
    for (int keyIndex = 0; keyIndex < nodeToUpdate->GetNumKeys(); keyIndex++) {
        auto *nextChild = new BPTNode(nodeToUpdate->GetChild(keyIndex + 1));
        while (!nextChild->IsLeaf()) {
            nextChild = new BPTNode(nextChild->GetChild(0));
        }
        // update the value of each key to the LB of the next node
        nodeToUpdate->SetKey(keyIndex, nextChild->GetMinKey());
    }
}
