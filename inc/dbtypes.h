#ifndef DBTYPES_H
#define DBTYPES_H

#include "config.h"

typedef unsigned short size_type;

typedef enum class BlockType : unsigned char {
    UNUSED = 0,
    RECORD = 1,
    BPTREE = 2
} block_type;

typedef struct BlockHeader {
    bool used: 1;
    block_type type: 2;
    size_type used_size: 13;
} BlockHeader;  // 2 Bytes

typedef struct RecordMovie {
    char tconst[TCONST_SIZE];
    float avg_rating;
    unsigned int num_votes;
} RecordMovie;  // 20 Bytes

const size_type BLOCK_HEADER_SIZE = sizeof(BlockHeader);
const size_type RECORD_SIZE = sizeof(RecordMovie);
const size_type PACKED_RECORD_SIZE = TCONST_SIZE + sizeof(float) + sizeof(int);
const unsigned short RECORD_PER_BLOCK = (BLOCK_SIZE - BLOCK_HEADER_SIZE) / (PACKED_RECORD_SIZE + 1);

typedef struct RecordBlockHeader {
    bool occupied[RECORD_PER_BLOCK];
} RecordBlockHeader;  // extra 1 byte for boolean flag

const size_type RECORD_BLOCK_HEADER_SIZE = sizeof(RecordBlockHeader);
const size_type RECORD_BLOCK_OFFSET = BLOCK_HEADER_SIZE + RECORD_BLOCK_HEADER_SIZE;

namespace dbtypes {

// typecasting only as block header is auto packed
BlockHeader *ReadBlockHeader(char *pSrc);

// reads 18 bytes starting from pSrc and parse into RecordMovie structure
RecordMovie *ReadRecordMovie(char *pSrc);

// pack RecordMovie structure to 18 bytes and save to pDest
void WriteRecordMovie(char *pDest, RecordMovie *pSrc);

// typecasting only
RecordBlockHeader *ReadRecordBlockHeader(char *pBlock);

}

#endif