//
// Created by hengwei on 2/19/2023.
//

#include "bpt.h"
#include "bptnode.h"

#include <vector>

BPT::BPT(char *pRoot, Storage storage) : root_(pRoot), initialized_(false), storage_(storage) {}

BPT::~BPT() = default;

void BPT::setRoot(char *pRoot){
    root_ = pRoot;
}

void BPT::initializeBPT(int[] indexlist, char*[] addresslist){
    if (this->intialized_){
        std::cout << "Already initialized!" << endl;
        return;
    }

    

}