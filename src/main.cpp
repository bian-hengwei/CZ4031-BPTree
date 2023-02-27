#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <cassert>
#include <vector>

#include "block.h"
#include "bptnode.h"
#include "bpt.h"
#include "config.h"
#include "dbtypes.h"
#include "storage.h"

using namespace std;

int main() {
    Storage storage(DISK_CAPACITY);

    // read data
//    cout << "Read data from data/data.tsv..." << endl;
//    ifstream data_tsv("../data/data.tsv");
//    string line;

//    bool isFirstLine = true;

//    while (getline(data_tsv, line)) {
//        if (isFirstLine) {
//            isFirstLine = false;
//            continue;
//        }
//        auto *record_movie = new RecordMovie();
//        stringstream ss(line);
//        string dataItem;
//        vector<string> result;
//        while (getline(ss, dataItem, '\t')) {
//            result.push_back(dataItem);
//        }
//        std::strncpy(record_movie->tconst, result[0].c_str(), TCONST_SIZE);
//        record_movie->avg_rating = stof(result[1]);
//        record_movie->num_votes = stoi(result[2]);
//
//        cout << "current record read -- tconst: " << record_movie->tconst << " avgRating: " << record_movie->avg_rating
//             << " numVotes: " << record_movie->num_votes
//             << endl;
//
//        char pBuffer[BLOCK_SIZE];
//        if (storage.GetNumOfBlocks() == 0 || storage.IsLatestBlockFull()) {
//            char *pBlock = storage.AllocateBlock();
//            Storage::ReadBlock(pBuffer, pBlock);
//
//            block::Initialize(pBuffer, BlockType::RECORD);
//            block::record::Initialize(pBuffer);
//
//            unsigned short slot = block::record::AllocateSlot(pBuffer);
//            dbtypes::WriteRecordMovie(pBuffer, slot, record_movie);
//
//            Storage::WriteBlock(pBlock, pBuffer);
//        } else {
//            char *pBlock = storage.GetLatestBlock();
//            Storage::ReadBlock(pBuffer, pBlock);
//
//            unsigned short slot = block::record::AllocateSlot(pBuffer);
//            dbtypes::WriteRecordMovie(pBuffer, slot, record_movie);
//
//            Storage::WriteBlock(pBlock, pBuffer);
//        }
//    }
//
//    data_tsv.close();
//
//    cout << "--- EXPERIMENT 1 ---" << endl;
//    cout << "--- after storage statistics ---" << endl;
//
//    // get statistics
//    cout << "number of records: " << storage.GetNumOfRecords() << endl;
//    cout << "number of blocks: " << storage.GetNumOfBlocks() << endl;
//    cout << "number of records in the first block: " << block::record::GetOccupiedCount(storage.GetBlockByIndex(0))
//         << endl;
//    cout << "number of records in the second block: " << block::record::GetOccupiedCount(storage.GetBlockByIndex(1))
//         << endl;
//    cout << "number of records in the last block: " << block::record::GetOccupiedCount(storage.GetLatestBlock())
//         << endl;
//    cout << "Record size is: " << RECORD_SIZE << endl;

    cout << "Testing bptinitialization.." << endl;

    char *pRecord = storage.AllocateBlock();
    char *pRecordMem = static_cast<char *>(operator new(BLOCK_SIZE));
    Storage::ReadBlock(pRecordMem, pRecord);
    block::Initialize(pRecordMem, BlockType::RECORD);
    block::record::Initialize(pRecordMem);

    RecordMovie *record_movie = new RecordMovie();
    std::strncpy(record_movie->tconst, "tt12345678", TCONST_SIZE);
    record_movie->avg_rating = 1;
    record_movie->num_votes = 10000;
    unsigned short slot = block::record::AllocateSlot(pRecordMem);
    dbtypes::WriteRecordMovie(pRecordMem, slot, record_movie);
    char *record_address = pRecord + slot;

    Storage::WriteBlock(pRecord, pRecordMem);

    char *pRoot = storage.AllocateBlock();
    char *pRootMem = static_cast<char *>(operator new(BLOCK_SIZE));
    Storage::ReadBlock(pRootMem, pRoot);
    block::Initialize(pRootMem, BlockType::BPTREE);
    block::bpt::Initialize(pRootMem);
    Storage::WriteBlock(pRoot, pRootMem);

    BPT bpttree = BPT(pRootMem, storage);
    assert(bpttree.getInitialized() == false);

    vector<int> vector1(300,1);
    vector<char *> vector2(300,record_address);
    bpttree.initializeBPT(vector1, vector2);

    cout << "No of Nodes: " << bpttree.getNoofNodes() << endl;
    cout << "No of Levels: "<< bpttree.getNoofLevels() << endl;
    return 0;
}