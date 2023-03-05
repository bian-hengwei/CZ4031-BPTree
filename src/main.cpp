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

std::vector<char *> ScanRecords(char *storage, int lo, int hi) {
    std::vector<char *> records;
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
    auto *storage = new Storage(DISK_CAPACITY);

    char *pRoot = storage->AllocateBlock();
    block::bpt::Initialize_D(pRoot);

    BPT *bpt = new BPT(pRoot, storage);
    auto *root = new BPTNode(pRoot);
    root->SetLeaf(true);

    // read data
    std::cout << "Read data from data/data.tsv..." << std::endl;
    std::ifstream data_tsv("../data/data.tsv");
    std::string line;
    getline(data_tsv, line);

    int cnt = 0;

    // experiment variables
    // 1
    int num_of_records = 0;
    int size_of_record = PACKED_RECORD_SIZE;
    int num_of_records_in_block = RECORD_PER_BLOCK;
    int num_of_blocks_storing_data = 0;
    // 2
    int parameter_n = MAX_KEYS;
    int num_of_nodes_in_bpt;
    int num_of_levels_in_bpt;

    char *pBlock = nullptr;

    while (getline(data_tsv, line)) {
        num_of_records++;

        auto *record_movie = new RecordMovie();
        std::stringstream ss(line);
        std::string dataItem;
        std::vector<std::string> result;
        while (getline(ss, dataItem, '\t')) {
            result.push_back(dataItem);
        }
        std::strncpy(record_movie->tconst, result[0].c_str(), TCONST_SIZE);
        record_movie->avg_rating = stof(result[1]);
        record_movie->num_votes = stoi(result[2]);

        if (pBlock == nullptr || block::record::IsFull_D(pBlock)) {
            pBlock = storage->AllocateBlock();
            block::record::Initialize_D(pBlock);
            num_of_blocks_storing_data++;
        }
        unsigned short slot = block::record::AllocateSlot_D(pBlock);
        dbtypes::WriteRecordMovie_D(pBlock, slot, record_movie);

        bpt->Insert(record_movie->num_votes, pBlock + slot);

        if (!(++cnt % 100000)) {
            std::cout << "Loading line " << cnt << std::endl;
        }
    }
    std::cout << std::endl;
    data_tsv.close();

    num_of_nodes_in_bpt = bpt->getNumOfNodes();
    num_of_levels_in_bpt = bpt->getNumOfLevels();

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

    std::cout << "--- EXPERIMENT 3 ---" << std::endl;
    std::cout << "--- Retrieving movies with the “numVotes” equal to 500  ---" << std::endl;
    std::vector<char *> v;
    int record_count = 0;
    int index_block_count = 0;
    int data_block_count = 0;
    float avg_rating = 0.;
    auto start_time = high_resolution_clock::now();
    v = bpt->Search(500, 500, record_count, index_block_count, data_block_count, avg_rating);
    auto end_time = high_resolution_clock::now();
    std::chrono::duration<double> duration = end_time - start_time;
    std::cout << "found " << record_count << " records" << std::endl;
    std::cout << "average rating " << avg_rating << std::endl;
    std::cout << "number of index blocks " << index_block_count << std::endl;
    std::cout << "number of data blocks " << data_block_count << std::endl;
    std::cout << "the running time of the retrieval process: " << duration.count() << std::endl;
    start_time = high_resolution_clock::now();
    v = ScanRecords(storage->GetAddress(), 500, 500);
    std::cout << "found " << v.size() << " records" << std::endl;
    end_time = high_resolution_clock::now();
    duration = end_time - start_time;
    std::cout << "the number of data blocks that would be accessed "
            << "by a brute-force linear scan method and its running time : "
            << num_of_blocks_storing_data << ", " << duration.count() << std::endl;
    std::cout << std::endl;

    std::cout << "--- EXPERIMENT 4 ---" << std::endl;
    std::cout << "--- Retrieving movies with the “numVotes” between 30000 and 40000  ---" << std::endl;
    record_count = 0;
    index_block_count = 0;
    data_block_count = 0;
    avg_rating = 0.;
    start_time = high_resolution_clock::now();
    v = bpt->Search(30000, 40000, record_count, index_block_count, data_block_count, avg_rating);
    end_time = high_resolution_clock::now();
    duration = end_time - start_time;
    std::cout << "found " << record_count << " records" << std::endl;
    std::cout << "average rating " << avg_rating << std::endl;
    std::cout << "number of index blocks " << index_block_count << std::endl;
    std::cout << "number of data blocks " << data_block_count << std::endl;
    std::cout << "the running time of the retrieval process: " << duration.count() << std::endl;
    start_time = high_resolution_clock::now();
    v = ScanRecords(storage->GetAddress(), 30000, 40000);
    std::cout << "found " << v.size() << " records" << std::endl;
    end_time = high_resolution_clock::now();
    duration = end_time - start_time;
    std::cout << "the number of data blocks that would be accessed "
         << "by a brute-force linear scan method and its running time : "
         << num_of_blocks_storing_data << ", " << duration.count() << std::endl;
    std::cout << std::endl;

    std::cout << "--- EXPERIMENT 5 ---" << std::endl;
    std::cout << "--- Delete those movies with the attribute “numVotes” equal to 1,000  ---" << std::endl;
    int keyToDelete = 1000;
    // start timer
    auto start = std::chrono::high_resolution_clock::now();
    while (bpt->DeleteRecord(keyToDelete));
    // end timer
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    std::cout << "All records containing the key " << keyToDelete << " have been deleted already:)" << std::endl;
    std::cout << "the number of nodes of the updated B+ tree: " << bpt->getNumOfNodes() << std::endl;
    std::cout << "the number of levels of the updated B+ tree: " << bpt->getNumOfLevels() << std::endl;
    std::cout << "the content of the root node of the updated B+ tree: ";
    root = new BPTNode(bpt->getRoot());
    for (int i = 0; i < root->GetNumKeys(); i++) {
        std::cout << root->GetKey(i) << " ";
    }
    std::cout << std::endl;
    std::cout << "Elapsed time for running deletion: " << elapsed.count() << "s" << std::endl;

    start_time = high_resolution_clock::now();
    v = ScanRecords(storage->GetAddress(), 1000, 1000);
    end_time = high_resolution_clock::now();
    duration = end_time - start_time;
    std::cout << "the number of data blocks that would be accessed "
              << "by a brute-force linear scan method and its running time : "
              << num_of_blocks_storing_data << ", " << duration.count() << std::endl;
}