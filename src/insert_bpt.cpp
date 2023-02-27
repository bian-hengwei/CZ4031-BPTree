#include <vector>
#include <algorithm>
using namespace std;

const int MIN_CHILDREN = 2;
const int MAX_CHILDREN = 4;

// B+ tree node class
class Node {
public:
    vector<int> keys; // keys stored in the node
    vector<Node*> children; // pointers to children nodes
    Node* parent; // pointer to parent node
    // constructor
    Node() {
        parent = nullptr;
    }
    
    // destructor
    ~Node() {
        for (auto child : children) {
            delete child;
        }
    }
    
    // check if the node is a leaf
    bool is_leaf() const {
        return children.empty();
    }
    
    // find the index of the first key greater than or equal to the given key
    int find_key(int key) const {
        return std::lower_bound(keys.begin(), keys.end(), key) - keys.begin();
    }
    
    // insert a key and optional value pointer into the node
    void insert(int key, void* value) {
        if (is_leaf()) {
            // insert key into sorted position
            int index = find_key(key);
            keys.insert(keys.begin() + index, key);
            values.insert(values.begin() + index, value);
            if (keys.size() > MAX_CHILDREN) {
                // split node into two
                Node* right_sibling = new Node();
                right_sibling->keys = std::vector<int>(keys.begin() + MIN_CHILDREN, keys.end());
                right_sibling->values = std::vector<void*>(values.begin() + MIN_CHILDREN, values.end());
                keys.erase(keys.begin() + MIN_CHILDREN, keys.end());
                values.erase(values.begin() + MIN_CHILDREN, values.end());
                right_sibling->parent = parent;
                if (parent) {
                    int index = parent->find_key(right_sibling->keys.front());
                    parent->insert_child(index + 1, right_sibling);
                } else {
                    // create new root node
                    parent = new Node();
                    parent->insert_child(0, this);
                    parent->insert_child(1, right_sibling);
                }
            }
        } else {
            // insert key into appropriate child node
            int index = find_key(key);
            children[index]->insert(key, value);
        }
    }
    
    // insert a child node into the node's children vector
    void insert_child(int index, Node* child) {
        children.insert(children.begin() + index, child);
        if (!child->is_leaf()) {
            child->parent = this;
        }
        if (children.size() > MAX_CHILDREN) {
            // split node into two
            Node* right_sibling = new Node();
            right_sibling->children = std::vector<Node*>(children.begin() + MIN_CHILDREN, children.end());
            children.erase(children.begin() + MIN_CHILDREN, children.end());
            right_sibling->parent = parent;
            if (parent) {
                int index = parent->find_key(right_sibling->children.front()->keys.front());
                parent->insert_child(index + 1, right_sibling);
            } else {
                // create new root node
                parent = new Node();
                parent->insert_child(0, this);
                parent->insert_child(1, right_sibling);
            }
        }
    }
    
    // fix an underflow in the node's children
    void fix_underflow() {
        int index = -1;
        for (int i = 0; i < parent->children.size(); ++i) {
            if (parent->children[i] == this) {
                index = i;
                break;
            }
        }
        if (index > 0 && parent->children[index - 1]->keys.size() > MIN_CHILDREN) {
            // redistribute keys from
        // left sibling
        Node* left_sibling = parent->children[index - 1];
        int transfer_key = left_sibling->keys.back();
        void* transfer_value = left_sibling->values.back();
        left_sibling->keys.pop_back();
        left_sibling->values.pop_back();
        keys.insert(keys.begin(), transfer_key);
        values.insert(values.begin(), transfer_value);
        if (!is_leaf()) {
            Node* transfer_child = left_sibling->children.back();
            left_sibling->children.pop_back();
            children.insert(children.begin(), transfer_child);
            transfer_child->parent = this;
        }
        parent->keys[index - 1] = keys.front();
    } else if (index < parent->children.size() - 1 && parent->children[index + 1]->keys.size() > MIN_CHILDREN) {
        // redistribute keys from right sibling
        Node* right_sibling = parent->children[index + 1];
        int transfer_key = right_sibling->keys.front();
        void* transfer_value = right_sibling->values.front();
        right_sibling->keys.erase(right_sibling->keys.begin());
        right_sibling->values.erase(right_sibling->values.begin());
        keys.push_back(transfer_key);
        values.push_back(transfer_value);
        if (!is_leaf()) {
            Node* transfer_child = right_sibling->children.front();
            right_sibling->children.erase(right_sibling->children.begin());
            children.push_back(transfer_child);
            transfer_child->parent = this;
        }
        parent->keys[index] = right_sibling->keys.front();
    } else {
        // merge with left or right sibling
        if (index > 0) {
            Node* left_sibling = parent->children[index - 1];
            left_sibling->keys.insert(left_sibling->keys.end(), keys.begin(), keys.end());
            left_sibling->values.insert(left_sibling->values.end(), values.begin(), values.end());
            if (!is_leaf()) {
                left_sibling->children.insert(left_sibling->children.end(), children.begin(), children.end());
                for (auto child : children) {
                    child->parent = left_sibling;
                }
            }
            parent->keys.erase(parent->keys.begin() + index - 1);
            parent->children.erase(parent->children.begin() + index);
            parent->fix_underflow();
            delete this;
        } else {
            Node* right_sibling = parent->children[index + 1];
            right_sibling->keys.insert(right_sibling->keys.begin(), keys.begin(), keys.end());
            right_sibling->values.insert(right_sibling->values.begin(), values.begin(), values.end());
            if (!is_leaf()) {
                right_sibling->children.insert(right_sibling->children.begin(), children.begin(), children.end());
                for (auto child : children) {
                    child->parent = right_sibling;
                }
            }
            parent->keys.erase(parent->keys.begin() + index);
            parent->children.erase(parent->children.begin() + index);
            parent->fix_underflow();
            delete this;
        }
    }
}

// remove a key from the node
void remove(int key) {
    if (is_leaf()) {
        int index = find_key(key);
        if (index < keys.size() && keys[index] == key) {
            // remove key and value pointer
            keys.erase(keys.begin() + index);
            values.erase(values.begin() + index);
            if (keys.size() < MIN_CHILDREN && parent) {
                fix_underflow();
            }
        }
    } else {
        int index = find_key(key);
        if (index < children.size()) {
            children[index]->remove(key);
            if (children[index]->keys.size() < MIN_CHILDREN && parent) {
                children[index]->fix_underflow();
           
            }
        }
    }
}

// find the value associated with a key
void* find(int key) {
    int index = find_key(key);
    if (index < keys.size() && keys[index] == key) {
        return values[index];
    } else if (is_leaf()) {
        return nullptr;
    } else {
        return children[index]->find(key);
    }
}

// find the index of the first key greater than or equal to a given key
int find_key(int key) {
    int index = 0;
    while (index < keys.size() && keys[index] < key) {
        index++;
    }
    return index;
}

// check if the node is a leaf node
bool is_leaf() {
    return children.empty();
}

// print the keys of the node and its children (for debugging purposes)
void print() {
    cout << "[";
    for (int i = 0; i < keys.size(); i++) {
        cout << keys[i];
        if (i != keys.size() - 1) {
            cout << " ";
        }
    }
    cout << "]";
    if (!is_leaf()) {
        cout << " (";
        for (int i = 0; i < children.size(); i++) {
            children[i]->print();
            if (i != children.size() - 1) {
                cout << " ";
            }
        }
        cout << ")";
    }
}
};