#include <cassert>
#include <cstring>

#include "bptnode.h"

BPTNode::BPTNode(char *pBlock) : pBlock_(pBlock) {
    node_head_ = (BPTNodeHead *)(pBlock + BLOCK_HEADER_SIZE);
    keys_ = (int *)(pBlock + BLOCK_HEADER_SIZE + NODE_HEAD_SIZE);
    children_ = (char **)(pBlock + BLOCK_HEADER_SIZE + NODE_HEAD_SIZE + sizeof(int) * MAX_KEYS);
}

BPTNode::~BPTNode() = default;

bool BPTNode::IsLeaf() const {
    return node_head_->leaf_;
}

void BPTNode::SetLeaf(bool leaf) {
    node_head_->leaf_ = leaf;
}

int BPTNode::NumKeys() const {
    return node_head_->num_keys_;
}

void BPTNode::SetNumKeys(int num_keys) {
    node_head_->num_keys_ = num_keys;
}

char *BPTNode::Parent() const {
    return node_head_->parent_;
}

void BPTNode::SetParent(char *parent) {
    node_head_->parent_ = parent;
}

int BPTNode::GetMinKey() const {
    return keys_[0];
}

int BPTNode::GetMaxKey() const {
    return keys_[node_head_->num_keys_ - 1];
}

void BPTNode::Insert(int index, int key, char *value) {
    assert(0 <= index);
    assert(index <= NumKeys());
    assert(index < MAX_KEYS);
    int move_size = (NumKeys() - index);
    std::memmove(keys_ + index + 1, keys_ + index, move_size * (int) sizeof(int));
    std::memmove(children_ + index + 1, children_ + index, move_size * (int) sizeof(void *));
    *(keys_ + index) = key;
    *(children_ + index) = value;
}

void BPTNode::Remove(int index) {
    assert(0 <= index);
    assert(index < NumKeys());
    assert(index < MAX_KEYS);
    int move_size = (NumKeys() - index);
    std::memmove(keys_ + index, keys_ + index + 1, move_size * (int) sizeof(int));
    std::memmove(children_ + index, children_ + index + 1, move_size * (int) sizeof(void *));
}