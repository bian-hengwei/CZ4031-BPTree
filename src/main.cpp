#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>

#include "block.h"
#include "bptnode.h"
#include "config.h"
#include "dbtypes.h"
#include "storage.h"

using namespace std;

int main() {
//    Storage storage(DISK_CAPACITY);
//
//    // read data
//    cout << "Read data from data/data.tsv..." << endl;
//    ifstream data_tsv("../data/data.tsv");
//    string line;
//
//    bool isFirstLine = true;
//
//    while (getline(data_tsv, line)) {
//        if (isFirstLine) {
//            isFirstLine = false;
//            continue;
//        }
//        RecordMovie record{};
//        stringstream ss(line);
//        string dataItem;
//        vector<string> result;
//        while (getline(ss, dataItem, '\t')) {
//            result.push_back(dataItem);
//        }
//
//        strcpy(record.tconst, result[0].c_str());  // does not end with \0, may result in print issues
//        record.avg_rating = stof(result[1]);
//        record.num_votes = stoi(result[2]);
//
//        cout << "current record read -- tconst: " << record.tconst << " avgRating: " << record.avg_rating
//             << " numVotes: " << record.num_votes
//             << endl;
//
//        char *pBlock = storage.AllocateBlock();
//        char *pBlockMem = storage.ReadBlock(pBlock);
//
//        block::Initialize(pBlockMem, BlockType::RECORD);
//        block::record::Initialize(pBlockMem);
//        unsigned short slot = block::record::AllocateSlot(pBlockMem);
//        dbtypes::WriteRecordMovie(pBlockMem, slot, &record);
//        storage.WriteBlock(pBlock, pBlockMem);
//        break;
//    }
//    data_tsv.close();
//
//    cout << "--- after storage statistics ---" << endl;
//
//    // get statistics
//    cout << "number of records: " << storage.getNumOfRecords() << endl;
//    cout << "number of blocks: " << storage.getNumOfBlocks() << endl;
//    cout << "Record size is: " << RECORD_SIZE << endl;

    char *pBlock = static_cast<char *>(operator new(BLOCK_SIZE));
    char *pBlockMem = static_cast<char *>(operator new(BLOCK_SIZE));

    cout << "pblock " << static_cast<void *>(pBlock) << endl;
    cout << "pblockmem " << static_cast<void *>(pBlockMem) << endl << endl;

    std::memcpy(pBlockMem, pBlock, BLOCK_SIZE);

    block::Initialize(pBlockMem, BlockType::RECORD);
    block::record::Initialize(pBlockMem);

    cout << "pblockmem used " << block::IsUsed(pBlockMem) << endl;
    cout << "pblockmem type record " << (block::GetBlockType(pBlockMem) == BlockType::RECORD) << endl;
    cout << "pblockmem used size " << block::GetUsedSize(pBlockMem) << endl << endl;

    RecordMovie *record_movie = new RecordMovie();
    std::strncpy(record_movie->tconst, "tt12345678", TCONST_SIZE);
    record_movie->avg_rating = 1;
    record_movie->num_votes = 10000;
    unsigned short slot = block::record::AllocateSlot(pBlockMem);
    dbtypes::WriteRecordMovie(pBlockMem, slot, record_movie);

    cout << "pblockmem used size " << block::GetUsedSize(pBlockMem) << endl << endl;

    std::memcpy(pBlock, pBlockMem, BLOCK_SIZE);
    pBlockMem = static_cast<char *>(operator new(BLOCK_SIZE));
    std::memcpy(pBlockMem, pBlock, BLOCK_SIZE);
    cout << "pblockmem used size " << block::GetUsedSize(pBlockMem) << endl << endl;

    record_movie = dbtypes::ReadRecordMovie(pBlockMem, slot);
    cout << "current record read -- tconst: " << record_movie->tconst
         << " avgRating: " << record_movie->avg_rating
         << " numVotes: " << record_movie->num_votes
         << endl;

    block::record::FreeSlot(pBlockMem, slot);
    cout << "pblockmem used size " << block::GetUsedSize(pBlockMem) << endl << endl;

    for (int i = 0; i < RECORD_PER_BLOCK; i++) {
        cout << i << " ";
        slot = block::record::AllocateSlot(pBlockMem);
        cout << slot << endl;
        dbtypes::WriteRecordMovie(pBlockMem, slot, record_movie);
    }

    block::record::FreeSlot(pBlockMem, slot);
    cout << "pblockmem used size " << block::GetUsedSize(pBlockMem) << endl << endl;

    return 0;
}