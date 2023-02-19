# CZ4031Project1

Project 1 repository for CZ4031.

## HOW TO RUN

Under root folder, execute

```sh
mkdir build
cd build
cmake ..
make
./CZ4031Project1
```

or

```sh
chmod u+x run.sh
./run.sh
```

Code Style: https://google.github.io/styleguide/cppguide.html

## Project Structure

### Storage
- 100 - 500 MB
- storage pointer
- allocate new block
- copy block to memory
- write block from memory to disk
- free block

### Block
- 200B
- block pointer, block type

### RecordBlock
- slots

### BPTreeBlock
- behaves like vector
