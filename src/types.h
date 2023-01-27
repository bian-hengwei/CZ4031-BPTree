#ifndef TYPES_H
#define TYPES_H

//Defines an address of an record.
struct Address
{
    void *blockAddress;
    short int offset;
};

struct Record
{
    char tconst[10];
    short int averageRating;
    int numVotes;
};

int record_size = sizeof(Record);
#endif