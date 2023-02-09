#include <iostream>
#include "storage.h"

using namespace std;

int main() {
    int DISK_CAPACITY = 500000000;      // 500MB of memory, can be modified
    int BLOCK_SIZE = 200;               // 200B

    Storage storage(DISK_CAPACITY, BLOCK_SIZE);

    cout << storage.getNumOfRecords() << endl;
    cout << storage.getNumOfBlocks() << endl;
    cout << sizeof(RecordMovie) << endl;
    return 0;
}