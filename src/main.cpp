#include <iostream>

#include "config.h"
#include "storage.h"

using namespace std;

int main()
{
    Storage storage(DISK_CAPACITY, BLOCK_SIZE);

    cout << storage.getNumOfRecords() << endl;
    cout << storage.getNumOfBlocks() << endl;
    cout << Storage::RECORD_SIZE << endl;
    return 0;
}