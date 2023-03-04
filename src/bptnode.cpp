#include <cassert>
#include <cstring>

#include "bptnode.h"
#include "storage.h"

BPTNode::BPTNode(char *pBlock) : pBlock_(pBlock) {
    pBlockMem_ = static_cast<char *>(operator new(BLOCK_SIZE));
    Storage::ReadBlock(pBlockMem_, pBlock);
    bpt_node_head_ = (BPTNodeHead *) (pBlockMem_ + BLOCK_HEADER_SIZE);
    keys_ = (int *) (pBlockMem_ + BLOCK_HEADER_SIZE + NODE_HEAD_SIZE);
    children_ = (char **) (pBlockMem_ + BLOCK_HEADER_SIZE + NODE_HEAD_SIZE + sizeof(int) * MAX_KEYS);
}

BPTNode::~BPTNode() {
    Storage::WriteBlock(pBlock_, pBlockMem_);
    operator delete(pBlockMem_);
}

void BPTNode::SaveNode() const {
    Storage::WriteBlock(pBlock_, pBlockMem_);
}

char *BPTNode::GetAddress() const {
    return pBlock_;
}

bool BPTNode::IsLeaf() const {
    return bpt_node_head_->leaf_;
}

void BPTNode::SetLeaf(bool leaf) {
    bpt_node_head_->leaf_ = leaf;
    SaveNode();
}

int BPTNode::GetNumKeys() const {
    return bpt_node_head_->num_keys_;
}

void BPTNode::SetNumKeys(int num_keys) {
    bpt_node_head_->num_keys_ = num_keys;
    SaveNode();
}

char *BPTNode::GetParent() const {
    return bpt_node_head_->parent_;
}

void BPTNode::SetParent(char *parent) {
    bpt_node_head_->parent_ = parent;
    SaveNode();
}

int BPTNode::GetMinKey() const {
    return keys_[0];
}

int BPTNode::GetMaxKey() const {
    return keys_[bpt_node_head_->num_keys_ - 1];
}


int BPTNode::GetKey(unsigned short index) const {
    return keys_[index];
}

char *BPTNode::GetChild(unsigned short index) const {
    return children_[index];
}

void BPTNode::SetKey(unsigned short index, int key) {
    assert(0 <= index);
    assert(index < GetNumKeys());
    assert(index < MAX_KEYS);
    keys_[index] = key;
    SaveNode();
}

void BPTNode::SetChild(unsigned short index, char *child) {
    assert(0 <= index);
    //assert(index <= GetNumKeys());
    assert(index <= MAX_KEYS);
    children_[index] = child;
    SaveNode();
}

void BPTNode::InsertKey(unsigned short index, int key) {
    assert(0 <= index);
    assert(index <= GetNumKeys());  // if index == GetNumKeys, append
    assert(index < MAX_KEYS);
    int move_size = (GetNumKeys() - index);
    std::memmove(keys_ + index + 1, keys_ + index, move_size * (int) sizeof(int));
    *(keys_ + index) = key;
    SetNumKeys(GetNumKeys() + 1);
    SaveNode();
}

void BPTNode::InsertChild(unsigned short index, char *child) {
    assert(0 <= index);
    assert(index <= GetNumKeys() + 1);
    assert(index <= MAX_KEYS);
    int move_size = (GetNumKeys() + 1 - index);
    std::memmove(children_ + index + 1, children_ + index, move_size * (int) sizeof(void *));
    *(children_ + index) = child;
    SaveNode();
}

void BPTNode::RemoveKey(unsigned short index) {
    assert(0 <= index);
    assert(index < GetNumKeys());
    assert(index < MAX_KEYS);
    int move_size = (GetNumKeys() - index);
    std::memmove(keys_ + index, keys_ + index + 1, move_size * (int) sizeof(int));
    SetNumKeys(GetNumKeys() - 1);
    SaveNode();
}

void BPTNode::RemoveChild(unsigned short index) {
    assert(0 <= index);
    assert(index < GetNumKeys());
    assert(index < MAX_KEYS);
    int move_size = (GetNumKeys() - index);
    std::memmove(children_ + index, children_ + index + 1, move_size * (int) sizeof(void *));
    SaveNode();
}

int BPTNode::GetChildIndex(const char *childNodeAddress) const {
    for (int i = 0; i < GetNumKeys() + 1; i++) {
        if (GetChild(i) == childNodeAddress) {
            return i;
        }
    }
    // not found
    return -1;
}