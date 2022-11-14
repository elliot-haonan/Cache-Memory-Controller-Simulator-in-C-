#include <iostream>
#include <bitset>
#include <stdio.h>
#include<stdlib.h>
#include <string>
using namespace std;

#define L1_CACHE_SETS 16 // DM cache
#define L2_CACHE_SETS 16 // SA cache
#define L2_CACHE_WAYS 8  
#define MEM_SIZE 4096 //  w.r.t. block
#define BLOCK_SIZE 4 // bytes per block
#define DM 0
#define SA 1

struct cacheBlock
{
	int tag; 
	int lru_position; // for SA only
	int data; // the actual data stored in the cache/memory
	bool valid;
};

struct Stat
{
	int missL1; 
	int missL2; 
	int accL1;
	int accL2;
	int hitL1;
	int hitL2;
};

class cache {
private:
	cacheBlock L1[L1_CACHE_SETS]; // 1 set per row.
	cacheBlock L2[L2_CACHE_SETS][L2_CACHE_WAYS]; // x ways per row 
	Stat myStat;
public:
	cache();
	void controller(bool MemR, bool MemW, int* data, int adr, int* myMem);
	int getMissL1();
	int getMissL2();
	int getAccL1();
	int getAccL2();
};


