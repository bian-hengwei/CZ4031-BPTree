# Project Documentation

## Basic Ideas

We create a storage by claiming a part of memory, we store the pointer to the start of the storage.

We store blocks in storage.

Whenever we need to access a block, we create a copy of the block in memory.

Whenever we need to save a block, we write the block from memory to storage.

We store block pointers as char pointers.

We access anything inside blocks using offset (with type unsigned short).

We access anything inside bptnode using index (with type unsigned short).

**THINGS TO NOTE:** Although we use the copy of the block, we need to store the address / pointers of the original
block. Either a address conversion function or some extra variables would help.

## Files

### config.h

Basic project configurations, including some pre-set constants

### dbtypes.h

Structures and related constants used in the database system

`size_type` is used to replace the built-in `size_t`. `size_type` is of type `unsigned short`, so it saves space
compared to `size_t` which is defined to be `unsigned long`. **NOTE THAT THIS STORES SIZE IN NUMBER OF BYTES, NOT IN
OTHER UNITS**

enum `BlockType`, with its alias block_type, specifies the type of a specific block, being one
of `UNUSED`, `RECORD`, `BPTREE`. It is stored in `BlockHeader` structure.

`BlockHeader` is stored at the start of any "Block". It stores the following attributes:

- `bool used` whether this block is currently used
- `block_type type` refers to enum `BlockType`
- `size_type used_size` stores how many bytes are occupied in this block
  Note that `BlockHeader` is modified using bit fields so that it saves more space

`RecordMovie` is the struct for a record. It stores the following attributes:
**NOTE THAT THIS STRUCT IS NOT DIRECTLY STORED IN BLOCKS. THIS IS FOR EASY READ/WRITE ONLY. PARSING IS NEEDED
USING `ReadRecordMovie` AND `WriteRecordMovie`**

- ttconst
- avgRating
- numVotes

`RecordBlockHeader` is stored after `BlockHeader` if the "Block" has type `BlockType::RECORD`. It stores:

- a boolean array indicating the status of each record slot, whether occupied or not
- an unsigned short storing the total number of occupied slots

### dbtypes.cpp

Functions related to structures in dbtypes.h

`ReadBlockHeader`
In: pointer to block (memonly)
Out: header of the block

`ReadRecordMovie`
In: pointer to block (memonly), offset in block
Out: record struct

`WriteRecordMovie`
In: pointer to block (memonly), offset in block, record struct

`ReadRecordBlockHeader`
In: pointer to block (memonly)
Out: record header of the block

### block.h

Header file for initializing functions used to operate a block pointer (memonly)

### block.cpp

Functions used to operate a block pointer (memonly)

#### namespace block - shared functions of record blocks and bpt blocks

`Initialize` - Mark block as used, set block type, initialize used_size to the size of the header

`Free` - Mark block as unused, clear used_size

`IsUsed` - block usage status

`GetBlockType` - block type

`GetUsedSize` - occupied size

`GetEmptySize` - unoccupied size

#### namespace record - functions for record blocks only

`Initialize` - Initialize RecordBlockHeader, increment used_size

`GetOccupiedCount`

`GetEmptyCount`

`IsFull`

`AllocateSlot` - Mark one record slot as occupied, modify headers, and return the record slot
In: pointer to block (memonly)
Out: offset of record slot allocated

`FreeSlot` - Mark the record slot as unoccupied, modify headers

### bptnode.h

Header for bptnode.cpp

### bptnode.cpp

Each bptnode is stored in a block.
The block is organized as: (MAX_KEYS = 14)
BlockHeader (2B) - BPTNodeHead (16B) - keys (4B $\times$ 14) - children (8B $\times$ 15)

## Sample Usage
