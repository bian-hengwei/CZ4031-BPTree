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
#include <set>
#include <vector>


BPT::BPT(char *pRoot, Storage *storage) : root_(pRoot), initialized_(false), storage_(storage), num_of_nodes_(1),
                                          num_of_levels_(1) {}

BPT::~BPT() = default;

void BPT::setRoot(char *pRoot) {
    this->root_ = pRoot;
}

char *BPT::getRoot() {
    return this->root_;
}

void BPT::setInitialized(bool initialvalue) {
    this->initialized_ = initialvalue;
}

bool BPT::getInitialized() const {
    return this->initialized_;
}

int BPT::getNumOfNodes() const {
    return num_of_nodes_;
}

int BPT::getNumOfLevels() const {
    return num_of_levels_;
}

// Keylist and addresslist are sorted
void BPT::initializeBPT(std::vector<int> keylist, std::vector<char *> addresslist) {
    if (getInitialized()) {
        return;
    }
    int maxKeyCount = MAX_KEYS;
    std::vector<BPTNode *> nodes;
    std::vector<BPTNode *> tempnodes;
    std::vector<char *> addresses;
    std::vector<char *> tempaddresses;
    // lowerbounds will be used later for inserting keys of 2nd and above levels.
    std::vector<int> lowerbounds;
    std::vector<int> templowerbounds;
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
            num_of_nodes_++;

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
        num_of_levels_++;
        num_of_nodes_++;

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
                num_of_nodes_++;
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
            }
            else {
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
    std::queue<char *> node_q;
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

std::vector<char *> BPT::Search(unsigned int lower, unsigned int upper, int &record_count, int &index_block_count,
                           int &data_block_count, float &avg_rating) {
    // Traverse the tree, start with the root and move left and right accordingly
    auto *node = new BPTNode(getRoot());
    index_block_count++;
    bool stop = false; // default is false because not found any keys
    float total_avg = 0;
    std::set<char *> data_blocks;
    std::vector<char *> return_data;
    while (!node->IsLeaf()) {
        index_block_count++;
        node = new BPTNode(node->GetChild(SearchKeyIndex(node, lower)));
    }
    while (!stop) {
        for (int i = 0; i < node->GetNumKeys(); i++) {
            unsigned int num_votes = node->GetKey(i);
            // print the movie record, found
            if (num_votes > upper) {
                stop = true;
                break; // reach the upper bound key
            }
            if (num_votes >= lower && num_votes <= upper) {
                char *pRecord = node->GetChild(i);
                int offset = ((int) (pRecord - storage_->GetAddress())) % BLOCK_SIZE;
                char *pRecordBlock = pRecord - offset;
                data_blocks.insert(pRecordBlock);
                RecordMovie *recordMovie = dbtypes::ReadRecordMovie(pRecordBlock, offset);
                total_avg += recordMovie->avg_rating;
                record_count++;
                return_data.push_back(pRecord);
            }
        }

        // Follow the right sibling pointer to the next leaf node
        if (!stop && node->GetChild(node->GetNumKeys()) != nullptr) {
            index_block_count++;
            node = new BPTNode(node->GetChild(node->GetNumKeys()));
        }
        else {
            stop = true; // no right sibling or end of search range
        }
    }
    avg_rating = record_count ? total_avg / (float) record_count : 0;
    data_block_count = (int) data_blocks.size();
    return return_data;
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

char *BPT::SearchLeafNode(unsigned int key) {
    auto *node = new BPTNode(root_);
    while (!node->IsLeaf()) {
        node = new BPTNode(node->GetChild(SearchKeyIndex(node, key)));
    }
    return node->GetAddress();
}

int BPT::SearchKeyIndex(BPTNode *node, unsigned int key) {
    int i = 0;
    while (i < node->GetNumKeys() && key > node->GetKey(i)) {
        i += 1;
    }
    return i;
}

void BPT::InsertToArray(BPTNode *node, int index, unsigned int key, char *address, unsigned int *keys, char **children) {
    for (int j = 0; j < node->GetNumKeys() + 1; j++) {
        if (j < index) {
            keys[j] = node->GetKey(j);
            children[j] = node->GetChild(j);
        }
        else if (index == j) {
            keys[j] = key;
            children[j] = address;
        }
        else {
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
void BPT::InsertToParent(char *node, unsigned int middle_key, char *node_l_block, char *node_r_block) {
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
        num_of_levels_++;
    }
    else {
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
        unsigned int parent_keys[MAX_KEYS + 1];
        char *parent_children[MAX_KEYS + 2];
        InsertToArray(parent_node, i, middle_key,
                      node_l_block, parent_keys, parent_children);
        SplitNonLeaf(parent_node->GetAddress(), parent_keys, parent_children);
    }
}

/**
 * Splits a non-leaf node
 */
void BPT::SplitNonLeaf(char *node, unsigned int *keys, char *children[]) {
    num_of_nodes_++;
    unsigned int middle_key;
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
        }
        else if (i == size_l) {
            middle_key = keys[i];
        }
        else {
            node_r->InsertKey(i - size_l - 1, keys[i]);
        }
    }

    // copy children
    for (int i = 0; i < MAX_KEYS + 2; i++) {
        auto *temp = new BPTNode(children[i]);
        if (i <= size_l) {
            node_l->InsertChild(i, children[i]);
            temp->SetParent(node_l->GetAddress());
        }
        else {
            node_r->InsertChild(i - size_l - 1, children[i]);
            temp->SetParent(node_r->GetAddress());
        }
    }

    InsertToParent(node, middle_key, node_l_block, node_r_block);

    storage_->FreeBlock(node);
}

void BPT::SplitLeaf(char *leaf, unsigned int *keys, char *children[]) {
    num_of_nodes_++;
    int size_l = (MAX_KEYS + 1) / 2;  // 7 keys; size_r = MAX_KEYS + 1 - size_l;  // 8 keys
    unsigned int middle_key = keys[size_l];

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
        }
        else {
            node_r->InsertKey(i - size_l, keys[i]);
        }
    }

    // copy children
    for (int i = 0; i < MAX_KEYS + 2; i++) {
        if (i < size_l) {
            node_l->InsertChild(i, children[i]);
        }
        else {
            node_r->InsertChild(i - size_l, children[i]);
        }
    }
    node_l->SetChild(size_l, node_r->GetAddress());

    char *left_leaf = Find_Left_Leaf(leaf);
    if (left_leaf) {
        auto *left_leaf_node = new BPTNode(left_leaf);
        left_leaf_node->SetChild(left_leaf_node->GetNumKeys(), node_l_block);
    }

    InsertToParent(leaf, middle_key, node_l_block, node_r_block);

    storage_->FreeBlock(leaf);
}

void BPT::Insert(unsigned int key, char *address) {
    auto *leaf_node = new BPTNode(SearchLeafNode(key));
    int i = SearchKeyIndex(leaf_node, key);

    if (leaf_node->GetNumKeys() < MAX_KEYS) {
        leaf_node->InsertKey(i, key);
        leaf_node->InsertChild(i, address);
        return;
    }
    unsigned int keys[MAX_KEYS + 1];
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
bool BPT::DeleteRecord(unsigned int keyToDelete) {
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
        char *nextNodeAddr = leafNode->GetChild(leafNode->GetNumKeys());
        if (nextNodeAddr) {
            auto *nextNode = new BPTNode(nextNodeAddr);

            if (nextNode->GetMinKey() == keyToDelete) {
                leafNode = nextNode;
                recordAddr = leafNode->GetChild(0);
                targetIndex = 0;
            }
            else {
                return false;
            }
        }
        else {
            return false;
        }
    }

    // free record
    long offset = (recordAddr - storage_->GetAddress()) % BLOCK_SIZE;
    // the block where the record resides
    char *pBlock = recordAddr - offset;
    block::record::FreeSlot_D(pBlock, offset);

    // delete the corresponding key & child;
    leafNode->RemoveChild(targetIndex);
    leafNode->RemoveKey(targetIndex);

    // fix the remaining B+ tree
    FixTree(leafNode->GetAddress());

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
            int curIndex = parentNode->GetChildIndex(leafNodeAddr);

            // Try to borrow a key from the left sibling
            if (curIndex > 0) {
                int leftSiblingIndex = curIndex - 1;
                auto *leftSibling = new BPTNode(parentNode->GetChild(leftSiblingIndex));
                if (leftSibling->IsNumOfKeysEnough()) {
                    int indexToBorrow = leftSibling->GetNumKeys() - 1;
                    // get the key to borrow from the left sibling
                    unsigned int borrowedKey = leftSibling->GetKey(indexToBorrow);
                    // get the rightmost child of the left sibling
                    auto *borrowedChildAddr = leftSibling->GetChild(indexToBorrow);
                    // insert the borrowed key and child into the current node
                    curNode->InsertKey(0, borrowedKey);
                    curNode->InsertChild(0, borrowedChildAddr);
                    // remove the borrowed key and child from the left sibling
                    leftSibling->RemoveChild(indexToBorrow);
                    leftSibling->RemoveKey(indexToBorrow);
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
                    unsigned int borrowedKey = rightSibling->GetKey(indexToBorrow);
                    // get the leftmost child of the right sibling
                    auto *borrowedChildAddr = rightSibling->GetChild(indexToBorrow);
                    // insert the borrowed key and child into the current node
                    int numOfKeys = curNode->GetNumKeys();
                    curNode->InsertKey(numOfKeys, borrowedKey);
                    curNode->InsertChild(numOfKeys, borrowedChildAddr);
                    // remove the borrowed key and child from the right sibling
                    rightSibling->RemoveChild(indexToBorrow);
                    rightSibling->RemoveKey(indexToBorrow);
                    break;
                }
            }

            // Merge with the left sibling
            if (curIndex > 0) {
                int leftSiblingIndex = curIndex - 1;
                auto *leftSibling = new BPTNode(parentNode->GetChild(leftSiblingIndex));

                // add all keys and children of current node to the left sibling
                int numLeftKeys = leftSibling->GetNumKeys();
                for (int i = 0; i < curNode->GetNumKeys(); i++) {
                    leftSibling->InsertKey(numLeftKeys + i, curNode->GetKey(i));
                    leftSibling->InsertChild(numLeftKeys + i, curNode->GetChild(i));
                }
                // change the left sibling's last child (ptr to next leaf node) to curNode's last child
                leftSibling->InsertChild(numLeftKeys + curNode->GetNumKeys(), curNode->GetChild(curNode->GetNumKeys()));
                // remove curNode
                parentNode->RemoveChild(curIndex);
                parentNode->RemoveKey(curIndex);

                // update curNode to left sibling
                curNode = leftSibling;
                num_of_nodes_--;
            }
            else {
                // Merge with the right sibling
                int rightSiblingIndex = curIndex + 1;
                auto *rightSibling = new BPTNode(parentNode->GetChild(rightSiblingIndex));
                // add all keys and children of the right sibling to curNode
                int numKeys = curNode->GetNumKeys();
                for (int i = 0; i < rightSibling->GetNumKeys(); i++) {
                    curNode->InsertKey(numKeys + i, rightSibling->GetKey(i));
                    curNode->InsertChild(numKeys + i, rightSibling->GetChild(i));
                }
                // change the curNode's last child (ptr to next leaf node) to right sibling's last child
                curNode->InsertChild(numKeys + rightSibling->GetNumKeys(),
                                     rightSibling->GetChild(rightSibling->GetNumKeys()));

                // remove right sibling
                parentNode->RemoveChild(rightSiblingIndex);
                parentNode->RemoveKey(rightSiblingIndex);
                num_of_nodes_--;
            }
        }

        // update values for non-leaf nodes at current level
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
        num_of_nodes_--;
        num_of_levels_--;
    }
    else {
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
