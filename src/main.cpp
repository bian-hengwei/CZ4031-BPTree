#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <vector>
#include <chrono>

#include "block.h"
#include "bptnode.h"
#include "bpt.h"
#include "config.h"
#include "dbtypes.h"
#include "storage.h"


using namespace std::chrono;

vector<char *> ScanRecords(char *storage, int lo, int hi) {
    vector<char *> records;
    char *p = storage;
    while (p < storage + DISK_CAPACITY) {
        if (block::GetBlockType_D(p) != BlockType::RECORD) {
            p += BLOCK_SIZE;
            continue;
        }
        bool occupied[10];
        block::record::GetOccupied_D(p, occupied);
        for (int i = 0; i < RECORD_PER_BLOCK; i++) {
            if (!occupied[i]) {
                continue;
            }
            int offset = RECORD_BLOCK_OFFSET_TOTAL + PACKED_RECORD_SIZE * i;
            RecordMovie *record_movie = dbtypes::ReadRecordMovie_D(p, offset);
            if (lo <= record_movie->num_votes && record_movie->num_votes <= hi) {
                records.push_back(p + offset);
            }
        }
        p += BLOCK_SIZE;
    }
    return records;
}

int main() {
    Storage *storage = new Storage(DISK_CAPACITY);

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

    char *pRecord = storage->AllocateBlock();
    block::record::Initialize_D(pRecord);

    auto *record_movie = new RecordMovie();
    std::strncpy(record_movie->tconst, "tt12345678", TCONST_SIZE);
    record_movie->avg_rating = 1;
    record_movie->num_votes = 10000;

    char *pRoot = storage->AllocateBlock();
    block::bpt::Initialize_D(pRoot);
    BPTNode *root = new BPTNode(pRoot);
    root->SetLeaf(true);

    BPT *bpttree = new BPT(pRoot, storage);

        if (pBlock == nullptr || block::record::IsFull_D(pBlock)) {
            pBlock = storage->AllocateBlock();
            block::record::Initialize_D(pBlock);
            num_of_blocks_storing_data++;
        }
        unsigned short slot = block::record::AllocateSlot_D(pBlock);
        dbtypes::WriteRecordMovie_D(pBlock, slot, record_movie);


        int key = rand() % (2 * n);
        if (!(++cnt % 100000)) {
            std::cout << cnt << endl;
        }
        unsigned short slot = block::record::AllocateSlot_D(pRecord);
        dbtypes::WriteRecordMovie_D(pRecord, slot, record_movie);
        char *record_address = pRecord + slot;

        bpttree->Insert(key, record_address);
        bpttree->PrintTree();
    }
    data_tsv.close();

    num_of_nodes_in_bpt = bpt->getNoofNodes();
    num_of_levels_in_bpt = bpt->getNoofLevels();

    std::cout << "Experiment 1" << std::endl;
    std::cout << "the number of records: " << num_of_records << std::endl;
    std::cout << "the size of a record: " << size_of_record << std::endl;
    std::cout << "the number of records stored in a block: " << num_of_records_in_block << std::endl;
    std::cout << "the number of blocks for storing the data: " << num_of_blocks_storing_data << std::endl;
    std::cout << std::endl;
    std::cout << "Experiment 2" << std::endl;
    std::cout << "the parameter n of the B+ tree: " << parameter_n << std::endl;
    std::cout << "the number of nodes of the B+ tree: " << num_of_nodes_in_bpt << std::endl;
    std::cout << "the number of levels of the B+ tree: " << num_of_levels_in_bpt << std::endl;
    std::cout << "the content of the root node (only the keys): ";
    root = new BPTNode(bpt->getRoot());
    for (int i = 0; i < root->GetNumKeys(); i++) {
        std::cout << root->GetKey(i) << " ";
    }
    std::cout << std::endl << std::endl;

    cout << "--- EXPERIMENT 3 ---" << endl;
    cout << "--- Retrieving movies with the “numVotes” equal to 500  ---" << endl;
    auto starttime = high_resolution_clock::now();
    bpt->search(500, 500);
    auto endtime = high_resolution_clock::now();
    auto duration = duration_cast<seconds>(endtime - starttime);
    cout << "number of index nodes the process accesses: " << bpt->getIndexNodes() << endl;
    cout << "number of data blocks the process accesses: " << bpt->getDataBlocks() << endl;
    cout << "average of “averageRating’s” of the records that are returned: " << bpt->getAvgAvgRating() << endl;
    cout << "the running time of the retrieval process: " << duration.count() << endl;
    starttime = high_resolution_clock::now();
    vector<char *> v = ScanRecords(storage->GetAddress(), 500, 500);
//    for(int i = 0; i < v.size(); i++) {
//        char *addr = v[i];
//        int offset = (addr - storage->GetAddress()) % BLOCK_SIZE;
//        char *p = addr - offset;
//        RecordMovie *recordMovie = dbtypes::ReadRecordMovie(p, offset);
//        cout << "Movie Record -- tconst: " << recordMovie->tconst << " avgRating: "
//             << recordMovie->avg_rating
//             << " numVotes: " << recordMovie->num_votes << endl;
//    }
    cout << "found " << v.size() << " records" << endl;
    endtime = high_resolution_clock::now();
    duration = duration_cast<seconds>(endtime - starttime);
    cout
            << "the number of data blocks that would be accessed by a brute-force linear scan method and its running time : "
            << num_of_blocks_storing_data << ", "  << duration.count() << endl;

    cout << "--- EXPERIMENT 4 ---" << endl;
    cout << "--- Retrieving movies with the “numVotes” between 30000 and 40000  ---" << endl;
    starttime = high_resolution_clock::now();
    bpt->search(30000, 40000);
    endtime = high_resolution_clock::now();
    duration = duration_cast<seconds>(endtime - starttime);
    cout << "number of index nodes the process accesses: " << bpt->getIndexNodes() << endl;
    cout << "number of data blocks the process accesses: " << bpt->getDataBlocks() << endl;
    cout << "average of “averageRating’s” of the records that are returned: " << bpt->getAvgAvgRating() << endl;
    cout << "the running time of the retrieval process: " << duration.count() << endl;
    starttime = high_resolution_clock::now();
    v = ScanRecords(storage->GetAddress(), 30000, 40000);
//    for(int i = 0; i < v.size(); i++) {
//        char *addr = v[i];
//        int offset = (addr - storage->GetAddress()) % BLOCK_SIZE;
//        char *p = addr - offset;
//        RecordMovie *recordMovie = dbtypes::ReadRecordMovie(p, offset);
//        cout << "Movie Record -- tconst: " << recordMovie->tconst << " avgRating: "
//             << recordMovie->avg_rating
//             << " numVotes: " << recordMovie->num_votes << endl;
//    }
    cout << "found " << v.size() << " records" << endl;
    endtime = high_resolution_clock::now();
    duration = duration_cast<seconds>(endtime - starttime);
    cout
            << "the number of data blocks that would be accessed by a brute-force linear scan method and its running time : "
            << num_of_blocks_storing_data << ", "  << duration.count() << endl;
}