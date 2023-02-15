#include <iostream>
#include <fstream>
#include <sstream>
#include "config.h"
#include "storage.h"

using namespace std;

int main() {
    Storage storage(DISK_CAPACITY);

    // read data
    cout << "Read data from data/data.tsv..." << endl;
    ifstream data_tsv("../data/data.tsv");
    string line;

    bool isFirstLine = true;

    while (getline(data_tsv, line)) {
        if (isFirstLine) {
            isFirstLine = false;
            continue;
        }
        RecordMovie record{};
        stringstream ss(line);
        string dataItem;
        vector<string> result;
        while (getline(ss, dataItem, '\t')) {
            result.push_back(dataItem);
        }

        strcpy(record.tconst, result[0].c_str());
        record.avgRating = stof(result[1]);
        record.numVotes = stoi(result[2]);

        cout << "current record read -- tconst: " << record.tconst << " avgRating: " << record.avgRating
             << " numVotes: " << record.numVotes
             << endl;
        char *recordAddr = storage.WriteRecord(&record);
        cout << "record address: " << recordAddr << endl;
        char buffer[RECORD_SIZE];
        Storage::ReadRecord(recordAddr, buffer);
        for (char i : buffer) {
            cout << i;
        }
        cout << endl;
        break;
    }
    data_tsv.close();

    cout << "--- after storage statistics ---" << endl;

    // get statistics
    cout << "number of records: " << storage.getNumOfRecords() << endl;
    cout << "number of blocks: " << storage.getNumOfBlocks() << endl;
    cout << "Record size is: " << RECORD_SIZE << endl;

    return 0;
}