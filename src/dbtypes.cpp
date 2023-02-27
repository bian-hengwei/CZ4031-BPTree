//
// Created by hengwei on 2/19/2023.
//

#include <cassert>
#include <cstring>

#include "dbtypes.h"
#include "storage.h"

namespace dbtypes {

BlockHeader *ReadBlockHeader(char *pBlockMem) {
    return (BlockHeader *) pBlockMem;
}

RecordMovie *ReadRecordMovie(char *pBlockMem, unsigned short offset) {
    assert(RECORD_BLOCK_OFFSET_TOTAL <= offset);
    assert(offset <= BLOCK_SIZE - RECORD_SIZE);
    assert((offset - RECORD_BLOCK_OFFSET_TOTAL) % PACKED_RECORD_SIZE == 0);
    auto *record_movie = new RecordMovie();
    std::strncpy(record_movie->tconst, pBlockMem + offset, TCONST_SIZE);
    record_movie->avg_rating = *(float *) (pBlockMem + offset + TCONST_SIZE);
    record_movie->num_votes = *(unsigned int *) (pBlockMem + offset + TCONST_SIZE + sizeof(float));
    return record_movie;
}

void WriteRecordMovie(char *pBlockMem, unsigned short offset, RecordMovie *record_movie) {
    assert(RECORD_BLOCK_OFFSET_TOTAL <= offset);
    assert(offset <= BLOCK_SIZE - RECORD_SIZE);
    assert((offset - RECORD_BLOCK_OFFSET_TOTAL) % PACKED_RECORD_SIZE == 0);
    std::strncpy(pBlockMem + offset, record_movie->tconst, TCONST_SIZE);
    *(float *) (pBlockMem + offset + TCONST_SIZE) = record_movie->avg_rating;
    *(unsigned int *) (pBlockMem + offset + TCONST_SIZE + sizeof(float)) = record_movie->num_votes;
}

RecordBlockHeader *ReadRecordBlockHeader(char *pBlockMem) {
    assert(ReadBlockHeader(pBlockMem)->type == BlockType::RECORD);
    return (RecordBlockHeader *) (pBlockMem + BLOCK_HEADER_SIZE);
}

BlockHeader *ReadBlockHeader_D(char *pBlock) {
    char *pBlockMem = static_cast<char *>(operator new(BLOCK_SIZE));
    Storage::ReadBlock(pBlockMem, pBlock);
    BlockHeader *block_header = ReadBlockHeader(pBlockMem);
    operator delete(pBlockMem);
    return block_header;
}

RecordMovie *ReadRecordMovie_D(char *pBlock, unsigned short offset) {
    char *pBlockMem = static_cast<char *>(operator new(BLOCK_SIZE));
    Storage::ReadBlock(pBlockMem, pBlock);
    RecordMovie *record_movie = ReadRecordMovie(pBlockMem, offset);
    operator delete(pBlockMem);
    return record_movie;
}

void WriteRecordMovie_D(char *pBlock, unsigned short offset, RecordMovie *record_movie) {
    char *pBlockMem = static_cast<char *>(operator new(BLOCK_SIZE));
    Storage::ReadBlock(pBlockMem, pBlock);
    WriteRecordMovie(pBlockMem, offset, record_movie);
    Storage::WriteBlock(pBlock, pBlockMem);
    operator delete(pBlockMem);
}

RecordBlockHeader *ReadRecordBlockHeader_D(char *pBlock) {
    char *pBlockMem = static_cast<char *>(operator new(BLOCK_SIZE));
    Storage::ReadBlock(pBlockMem, pBlock);
    RecordBlockHeader *record_block_header = ReadRecordBlockHeader(pBlockMem);
    operator delete(pBlockMem);
    return record_block_header;
}

}  // dbtypes