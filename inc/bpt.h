//
// Created by hengwei on 2/19/2023.
//

#ifndef BPT_H
#define BPT_H

#include "storage.h"

class BPT {
public:
    explicit BPT(char *pRoot);

    ~BPT();

    void setRoot(char *pRoot);

<<<<<<< HEAD
    char *getRoot();

    void setInitialized(bool initialvalue);

    bool getInitialized();

    void initializeBPT(int[] indexlist, char*[] addresslist);
=======
    void initializeBPT(int[] indexlist, char *[] addresslist);
>>>>>>> ef1179ff35c019b5c955017ca74dd1bee8161c9f

private:
    char *root_;
    bool intialized_;
    Storage storage_;
};

#endif //BPT_H
