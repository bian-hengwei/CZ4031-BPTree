#include "types.h"
#include "bplustree.h"
#include "storage.h"
// include all the libraries needed
#include <vector>
#include <cstring>
#include <iostream>

using namespace std;

void BPlusTree::bulkload(Address[] addresslist, int[] keylist){
    // Assume keys and addresses are sorted
    int maxKeyCount = getMaxKeyCount();
    vector<Node *> nodes;
    vector<Node *> tempnodes;
    vector<Address> addresses;
    vector<Address> tempaddresses;
    vector<Node> levels;
    vector<int> lowerbounds;
    vector<int> templowerbounds;
    size_t count = 0;
    Node nonleafNode;
    if (rootAddress != nullptr){
        // Throw error
    }
    else{
        Node leafNode = new Node(maxKeyCount);
        Address NodeAddress = index -> write((void*)leafNode, nodeSize);
        Node prevNode;

        
        // First initialize all the leaf nodes
        while (count < keys.size){
            if (leafNode -> curKeyCount == maxKeyCount){
                Node nextNode = new Node(maxKeyCount);
                Address nextNodeAddress = index->write((void *)nextNode, nodeSize);
                leafNode -> pointers[maxKeyCount] =  nextNodeAddress;
                prevNode = leafNode;
                leafNode = nextNode;
                nodes.push_back(prevNode);
                addresses.push_back(nextNodeAddress);
                
            }
            leafNode -> keys[leafNode -> curKeyCount] = keylist[count];
            leafNode -> pointers[leafNode -> curKeyCount] = addresslist[count];
            leafNode->curKeyCount++;
            count++;
        }
        // Check leafNodes equal to one
        if(keys.size<=maxKeyCount){}
        // TODO: Check minimum no of keys

        if (leafNode->curKeyCount < floor((maxKeyCount + 1)/2)){
            int noofkeys = floor((maxKeyCount+1)/2) - leafNode->curKeyCount;
            for(int i = noofkeys-1; i>=0;i--){
                leafNode->keys[i+noofkeys] = leafNode->keys[i];
                leafNode->pointers[i+noofkeys] = leafNode->pointers[i];
            }
            for(int j = 0; j<noofkeys; j++){
                leafNode->keys[j] = prevNode->keys[-(noofkeys-j)-1];
                prevNode->keys[-(noofkeys-j)-1] = NULL;
                leafNode->pointers[j] = prevNode->pointers[-(noofkeys-j)-2];
                prevNode->pointers[-(noofkeys-j)-2] = nullptr;
            }
            leafNode-> curKeyCount = floor((maxKeyCount + 1)/2);
            nodes.erase(nodes.end()-1);
            nodes.push_back(prevNode);
            nodes.push_back(leafNode);
        }
        //Setting up vectors...
        nodes.erase(nodes.start() + 1);
        for (Node nodetocheck:nodes){
            lowerbounds.push_back(nodetocheck->keys[0])
        }

        // Now recursive creation of nodes per level
        while (nodes.size > 0){
            nonleafNode = new Node(maxKeyCount);
            nonleafNode -> pointers[0] = NodeAddress;
            NodeAddress = index->write((void *)nonleafNode, nodeSize);
            
            for (int i = 0; i<nodes.size; i++){
                if (nonleafNode->curKeyCount == maxKeyCount){
                    prevNode = nonleafNode;
                    tempnodes.push_back(prevNode);
                    Node nonleafNode = new Node(maxKeyCount);
                    Address nextNodeAddress = index->write((void *)nonleafNode, nodeSize);
                    tempaddresses.push_back(nextNodeAddress);
                    nonleafNode -> pointers[0] = addresses[i];
                    templowerbounds.push_back(lowerbounds[i]);
                
                }
                else{
                    nonleafNode->keys[nonleafNode->curKeyCount] = lowerbounds[i];
                    nonleafNode->pointers[nonleafNode->curKeyCount + 1] = addresses[i];
                    nonleafNode->curKeyCount++;
                }
                

            }

            if (nonleafNode->curKeyCount < floor((maxKeyCount)/2)){
                int noofkeys = floor((maxKeyCount)/2) - nonleafNode->curKeyCount;
                if (nonleafNode->curKeyCount == 0){
                    nonleafNode->pointers[noofkeys] = nonleafNode->pointers[0];
                    nonleafNode->keys[noofkeys] = templowerbounds[-1]; 
                }
                else{
                    for(int i = noofkeys-1; i>=0;i--){
                        nonleafNode->keys[i+noofkeys] = nonleafNode->keys[i];
                        nonleafNode->pointers[i+noofkeys] = nonleafNode->pointers[i+1];
                    }
                    nonleafNode->pointers[noofkeys] = nonleafNode->pointers[0];
                    nonleafNode->keys[noofkeys] = templowerbounds[-1];  
                }
                templowerbounds.erase(templowerbounds.end() - 1);
                for(int j = 0; j<noofkeys-2; j++){
                    leafNode->keys[j] = prevNode->keys[-(noofkeys-j)-1];
                    prevNode->keys[-(noofkeys-j)-1] = NULL;
                    leafNode->pointers[j] = prevNode->pointers[-(noofkeys-j)-2];
                    prevNode->pointers[-(noofkeys-j)-2] = nullptr;
                }
                templowerbounds.push_back(prevNode->keys[-(noofkeys-j)-2]);
                prevNode->keys[-(noofkeys-j)-2] = NULL;
                leafNode-> curKeyCount = floor((maxKeyCount)/2);
                tempnodes.erase(nodes.end()-1);
                tempnodes.push_back(prevNode);
                tempnodes.push_back(leafNode);
            }
            vector<Node *> nodes(tempnodes);
            vector<Address> addresses(tempaddresses);
            vector<int> lowerbounds(templowerbounds);
            tempnodes.clear();
            tempaddresses.clear();
            lowerbounds.clear();
        }
        root = nonleafNode;
        rootAddress = NodeAddress;
    }

}