//
// Created by hengwei on 2/19/2023.
//

#ifndef BPT_H
#define BPT_H

#include "storage.h"

class BPT {
public:
    explicit BPT(char *pRoot, Storage storage);

    ~BPT();

    void setRoot(char *pRoot);

    char *getRoot();

    void setInitialized(bool initialvalue);

    bool getInitialized();

    void initializeBPT(vector<int> keylist, vector<char *> addresslist);

private:
    char *root_;
    bool intialized_;
    Storage storage_;
};

#endif //BPT_H
