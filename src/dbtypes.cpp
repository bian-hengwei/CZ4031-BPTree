//
// Created by hengwei on 2/19/2023.
//

#include <cstring>

#include "dbtypes.h"

namespace dbtypes {

BlockHeader *ReadBlockHeader(char *pSrc) {
    return (BlockHeader *) pSrc;
}

RecordMovie *ReadRecordMovie(char *pSrc) {
    RecordMovie *record_movie = new RecordMovie();
    std::strncpy(record_movie->tconst, pSrc, TCONST_SIZE);
    record_movie->avg_rating = *(float *) (pSrc + TCONST_SIZE);
    record_movie->num_votes = *(unsigned int *) (pSrc + TCONST_SIZE + sizeof(float));
    return record_movie;
}

void WriteRecordMovie(char *pDest, RecordMovie *record_movie) {
    std::strncpy(pDest, record_movie->tconst, TCONST_SIZE);
    *(float *) (pDest + TCONST_SIZE) = record_movie->avg_rating;
    *(unsigned int *) (pDest + TCONST_SIZE + sizeof(float)) = record_movie->num_votes;
}

RecordBlockHeader *ReadRecordBlockHeader(char *pBlock) {
    return (RecordBlockHeader *) (pBlock + BLOCK_HEADER_SIZE);
}

}  // dbtypes