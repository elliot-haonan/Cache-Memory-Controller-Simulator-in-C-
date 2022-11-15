# Cache-Memory Controller Simulator in C++
## Introduction
This C++ program implements a cache and memory controller. The address of the memory is block addressable with a block size of 4 bytes. The program simulates the behaviour of the cache and memory during the data loading and storing.
## Memory Hierarchy
The memory hierarchy in this program has two levels of cache (i.e. L1 cache and L2 cache) and a main memory. The L1 cache is a direct-mapped cache with 1 block per line and a total of 16 lines. The L2 cache is an 8-way set-associative cache with 16 sets. The main memory has 4096 lines and each line contains a 4-bytes block. Each block has four properties: tag, data, valid bit, lru_position(only for set-associative). Least-Recently-Used replacement policy (LRU) is used to decide wich cache line to evict in each fully-associate set in set-associate cache. 
## Trace Driven
The program is trace-driven, each line in the trace.txt has four values (MemR, MemW, adr, data). The first two indicating whether this is a LOAD or StORE. The address is a integer between 0 and 4095, the data is a signed integer value.
## Output 
The program monitors the statistics for L1 and L2 cache at each trace. It outputs the miss times/hit times/access times/miss rate for L1 and L2 cache and finally computes the average access time for this memory hierarchy.
## Running
Code should be compiled with following command
```bash
 g++ *.cpp -o memory_simulator
```
and run like this
```bash
 ./memory_simulator <inputfile.txt>
```
