#include <iostream>

#include "config.h"
#include "storage.h"

using namespace std;

int main() {
    Storage storage(DISK_CAPACITY);

    cout << storage.getNumOfRecords() << endl;
    cout << storage.getNumOfBlocks() << endl;
    cout << RECORD_SIZE << endl;
    return 0;
}