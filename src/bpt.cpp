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

BPT::BPT(char *pRoot, Storage storage) : root_(pRoot), intialized_(false), storage_(storage), noofnodes(1),
                                         nooflevels(1), noofindexnodes(1), noofdatablocks(0), avgavgrating(0) {}

BPT::~BPT() {

}

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

int BPT::getNoofNodes() {
    return noofnodes;
}

int BPT::getNoofLevels() {
    return nooflevels;
}

int BPT::getIndexNodes() {
    return noofindexnodes;
}

int BPT::getDataBlocks() {
    return noofdatablocks;
}

int BPT::getAvgAvgRating() {
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
    BPTNode *leafnode = new BPTNode(getRoot());
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
            nodeAddress = storage_.AllocateBlock();
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
    if (nodes.size() > 0) {
        assert(curKeyCount >= floor((MAX_KEYS + 1) / 2));
    }

    // Recursively create levels. If nodes.size == 0, means that only one node in the level.
    while (nodes.size() > 0) {
        nooflevels++;
        noofnodes++;


        maxKeyCount = MAX_KEYS;
        setmax = false;
        count = 0;


        nodeAddress = storage_.AllocateBlock();
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

                nodeAddress = storage_.AllocateBlock();
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

        if (nodes.size() > 0) {
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

        BPTNode *node = new BPTNode(pNode);

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

// void BPT::search(int lowerBoundKey, int upperBoundKey) //take in lower and upper range
// {
//   BPTNode *node = new BPTNode(getRoot()); // Traverse the tree, start with the root and move left and right accordingly
//   bool stop = false; // default is false because not found any keys
//   int totalavgrating = 0;
//   int numavgrating = 0;

//   if (node == nullptr) // tree contains no elements or tree is empty
//   {
//     return; // will not perform search since tree is empty
//   }
//   else // tree is not empty
//   {
//     while (!node->IsLeaf()) // while not reached a leaf node (not leaf node)
//     {
//       noofindexnodes++;

//       for (int i = 0; i < node->GetNumKeys(); i++) // visit every key in the current node
//       {

//         if (lowerBoundKey < node->GetKey(i)) // if lowerBoundKey is less than current key (greater), go left.
//         {
//           node = new BPTNode(node->GetChild(i)); 
//           break;
//         }
//         if (i == node->GetNumKeys() - 1) // if visited all the keys in current node (i.e., all keys are smaller), go right
//         {
//           node = new BPTNode(node->GetChild(i+1)); 
//           break;
//         }
//       }
//     }
//     while (stop == false) // continue search over the entire range (reach leaf node)
//     {
//       int i;
//       for (i = 0; i < node->GetNumKeys(); i++)
//       {

//         if (node->GetKey(i) > upperBoundKey) // continue till you reach upperBoundKey
//         {
//           stop = true;
//           break; // reach the upper bound key
//         }
//         if (node->GetKey(i) >= lowerBoundKey && node->GetKey(i) <= upperBoundKey)
//         {
//           // print the movie record, found 
//           char *pRecord = node->GetChild(i);
//           int offset = (pRecord - storage_.GetLatestBlock()) % BLOCK_SIZE; 
//           char *pBlockMem = pRecord - offset;
//           RecordMovie *recordMovie = dbtypes::ReadRecordMovie(pBlockMem, offset);
//           totalavgrating = recordMovie->avg_rating + totalavgrating;
//           numavgrating++;
//           cout << "Movie Record -- tconst: " << recordMovie->tconst << " avgRating: " << recordMovie->avg_rating 
//           << " numVotes: " << recordMovie->num_votes << endl;
//         }
//       }
//     }
//   }
//   if (numavgrating == 0){
//       avgavgrating = 0;
//   }
//   else {
//       avgavgrating = totalavgrating / numavgrating;
//   }
//   return;
// }

void BPT::search(int lowerBoundKey, int upperBoundKey) //take in lower and upper range
{
    BPTNode *node = new BPTNode(
            getRoot()); // Traverse the tree, start with the root and move left and right accordingly
    bool stop = false; // default is false because not found any keys
    int totalavgrating = 0;
    int numavgrating = 0;

    if (node == nullptr) // tree contains no elements or tree is empty
    {
        return; // will not perform search since tree is empty
    }
    else // tree is not empty
    {
        while (!node->IsLeaf()) // while not reached a leaf node (not leaf node)
        {
            noofindexnodes++;

            for (int i = 0; i < node->GetNumKeys(); i++) // visit every key in the current node
            {

                if (lowerBoundKey < node->GetKey(i)) // if lowerBoundKey is less than current key (greater), go left.
                {
                    node = new BPTNode(node->GetChild(i));
                    break;
                }
                if (i == node->GetNumKeys() -
                         1) // if visited all the keys in current node (i.e., all keys are smaller), go right
                {
                    node = new BPTNode(node->GetChild(i + 1));
                    break;
                }
            }
        }
        while (stop == false) // continue search over the entire range (reach leaf node)
        {
            int i;
            for (i = 0; i < node->GetNumKeys(); i++) {

                if (node->GetKey(i) > upperBoundKey) // continue till you reach upperBoundKey
                {
                    stop = true;
                    break; // reach the upper bound key
                }
                if (node->GetKey(i) >= lowerBoundKey && node->GetKey(i) <= upperBoundKey) {
                    // print the movie record, found
                    char *pRecord = node->GetChild(i);
                    int offset = (pRecord - storage_.GetAddress()) % BLOCK_SIZE;
                    char *pBlockMem = pRecord - offset;
                    RecordMovie *recordMovie = dbtypes::ReadRecordMovie(pBlockMem, offset);
                    totalavgrating = recordMovie->avg_rating + totalavgrating;
                    numavgrating++;
                    cout << "Movie Record -- tconst: " << recordMovie->tconst << " avgRating: "
                         << recordMovie->avg_rating
                         << " numVotes: " << recordMovie->num_votes << endl;
                }
            }

            // Follow the right sibling pointer to the next leaf node
            if (!stop && node->GetChild(node->GetNumKeys()) != nullptr) {
                node = new BPTNode(node->GetChild(node->GetNumKeys()));
            }
            else {
                stop = true; // no right sibling or end of search range
            }
        }
    }
    if (numavgrating == 0) {
        avgavgrating = 0;
    }
    else {
        avgavgrating = totalavgrating / numavgrating;
    }
    return;
}

char *BPT::SearchLeafNode(int key) {
    BPTNode *node = new BPTNode(root_);
    while (!node->IsLeaf()) {
        node = new BPTNode(node->GetChild(SearchKeyIndex(node, key)));
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
void BPT::InsertToParent(char *node, int middle_key, char *node_l_block, char *node_r_block) {
    BPTNode *current_node = new BPTNode(node);
    BPTNode *node_l = new BPTNode(node_l_block);
    BPTNode *node_r = new BPTNode(node_r_block);
    if (node == root_) {
        // allocate a new block for root
        char *new_root_block = storage_.AllocateBlock();
        BPTNode *new_root = new BPTNode(new_root_block);
        node_l->SetParent(new_root_block);
        node_r->SetParent(new_root_block);
        // initialize root and insert values
        new_root->SetLeaf(false);
        new_root->InsertKey(0, middle_key);
        new_root->InsertChild(0, node_l_block);
        new_root->InsertChild(1, node_r_block);
        // update root address
        root_ = new_root_block;
    }
    else {
        // if node is not root, we find its parent
        char *parent = current_node->GetParent();
        BPTNode *parent_node = new BPTNode(parent);
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
    int middle_key;
    int size_l = (MAX_KEYS + 1) / 2;  // 7 keys & 8 children
    int size_r = MAX_KEYS - size_l;  // 7 keys & 8 children

    // initialize new blocks
    char *node_l_block = storage_.AllocateBlock();
    char *node_r_block = storage_.AllocateBlock();
    BPTNode *node_l = new BPTNode(node_l_block);
    BPTNode *node_r = new BPTNode(node_r_block);
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
        BPTNode *temp = new BPTNode(children[i]);
        cout << temp->GetNumKeys();
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

    storage_.FreeBlock(node);
}

void BPT::SplitLeaf(char *leaf, int keys[], char *children[]) {
    int size_l = (MAX_KEYS + 1) / 2;  // 7 keys
    int size_r = MAX_KEYS + 1 - size_l;  // 8 keys
    int middle_key = keys[size_l];

    char *node_l_block = storage_.AllocateBlock();
    char *node_r_block = storage_.AllocateBlock();
    BPTNode *node_l = new BPTNode(node_l_block);
    BPTNode *node_r = new BPTNode(node_r_block);
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

    InsertToParent(leaf, middle_key, node_l_block, node_r_block);

    storage_.FreeBlock(leaf);
}

void BPT::Insert(int key, char *address) {
    BPTNode *leaf_node = new BPTNode(SearchLeafNode(key));
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