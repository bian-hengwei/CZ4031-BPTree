#include <cassert>
#include <cstring>

#include "bptnode.h"

BPTNode::BPTNode(char *pBlockMem) : pBlockMem_(pBlockMem) {
    bpt_node_head_ = (BPTNodeHead *)(pBlockMem + BLOCK_HEADER_SIZE);
    keys_ = (int *)(pBlockMem + BLOCK_HEADER_SIZE + NODE_HEAD_SIZE);
    children_ = (char **)(pBlockMem + BLOCK_HEADER_SIZE + NODE_HEAD_SIZE + sizeof(int) * MAX_KEYS);
}

BPTNode::~BPTNode() = default;

char *BPTNode::GetAddress() const {
    return pBlockMem_;
};

bool BPTNode::IsLeaf() const {
    return bpt_node_head_->leaf_;
}

void BPTNode::SetLeaf(bool leaf) {
    bpt_node_head_->leaf_ = leaf;
}

int BPTNode::GetNumKeys() const {
    return bpt_node_head_->num_keys_;
}

void BPTNode::SetNumKeys(int num_keys) {
    bpt_node_head_->num_keys_ = num_keys;
}

char *BPTNode::GetParent() const {
    return bpt_node_head_->parent_;
}

void BPTNode::SetParent(char *parent) {
    bpt_node_head_->parent_ = parent;
}

int BPTNode::GetMinKey() const {
    return keys_[0];
}

int BPTNode::GetMaxKey() const {
    return keys_[bpt_node_head_->num_keys_ - 1];
}

void BPTNode::SetKey(unsigned short index, int key) {
    assert(0 <= index);
    assert(index < GetNumKeys());
    assert(index < MAX_KEYS);
    keys_[index] = key;
}

void BPTNode::SetChild(unsigned short index, char *child) {
    assert(0 <= index);
    assert(index <= GetNumKeys());
    assert(index <= MAX_KEYS);
    children_[index] = child;
}

void BPTNode::InsertKey(unsigned short index, int key) {
    assert(0 <= index);
    assert(index <= GetNumKeys());  // if index == GetNumKeys, append
    assert(index < MAX_KEYS);
    int move_size = (GetNumKeys() - index);
    std::memmove(keys_ + index + 1, keys_ + index, move_size * (int) sizeof(int));
    *(keys_ + index) = key;
    SetNumKeys(GetNumKeys() + 1);
}

void BPTNode::InsertChild(unsigned short index, char *child) {
    assert(0 <= index);
    assert(index <= GetNumKeys() + 1);
    assert(index <= MAX_KEYS);
    int move_size = (GetNumKeys() + 1 - index);
    std::memmove(children_ + index + 1, children_ + index, move_size * (int) sizeof(void *));
    *(children_ + index) = child;
}

void BPTNode::RemoveKey(unsigned short index) {
    assert(0 <= index);
    assert(index < GetNumKeys());
    assert(index < MAX_KEYS);
    int move_size = (GetNumKeys() - index);
    std::memmove(keys_ + index, keys_ + index + 1, move_size * (int) sizeof(int));
    SetNumKeys(GetNumKeys() - 1);
}

void BPTNode::RemoveChild(unsigned short index) {
    assert(0 <= index);
    assert(index < GetNumKeys());
    assert(index < MAX_KEYS);
    int move_size = (GetNumKeys() - index);
    std::memmove(children_ + index, children_ + index + 1, move_size * (int) sizeof(void *));
}