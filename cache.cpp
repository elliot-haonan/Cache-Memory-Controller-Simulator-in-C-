#include "cache.h"

cache::cache()
{
	for (int i=0; i<L1_CACHE_SETS; i++)
		L1[i].valid = false; 
	for (int i=0; i<L2_CACHE_SETS; i++)
		for (int j=0; j<L2_CACHE_WAYS; j++)
			L2[i][j].valid = false; 

	this->myStat.missL1 =0;
	this->myStat.missL2 =0;
	this->myStat.accL1 =0;
	this->myStat.accL2 =0;
	this->myStat.hitL1 =0;
	this->myStat.hitL2 =0;
}

int cache::getMissL1()
{
	return this->myStat.missL1;
}

int cache::getMissL2()
{
	return this->myStat.missL2;
}

int cache::getAccL1()
{
	return this->myStat.accL1;
}

int cache::getAccL2()
{
	return this->myStat.accL2;
}

/*
Address is decomposed like this:
L1 Cache:
-------28-bit tag----------|--4-bit index--
L2 Cache:
-------28-bit tag----------|--4-bit index--
*/
void cache::controller(bool MemR, bool MemW, int* data, int adr, int* myMem)
{
	int addr_index;
	int addr_tag;
	int found = 0;
	addr_index = (bitset<4>((bitset<32>(adr)).to_string().substr(28,4))).to_ulong();
	addr_tag = (bitset<28>((bitset<32>(adr)).to_string().substr(0,28))).to_ulong();

	if(MemW){ // store
	    // search in L1 cache
		this->myStat.accL1 += 1;
		if((this->L1[addr_index].tag == addr_tag) && (this->L1[addr_index].valid == true)){
			this->L1[addr_index].data = *data;
			this->myStat.hitL1 += 1;
		}
		// not found in L1 cache, search L2 cache
		else{

			this->myStat.missL1 += 1;
			this->myStat.accL2 += 1;
			
			int i;
			for(i=0; i<L2_CACHE_WAYS; i++){
				if((this->L2[addr_index][i].tag == addr_tag) && (this->L2[addr_index][i].valid == true)){

					this->myStat.hitL2 += 1;
					this->L2[addr_index][i].data = *data;

					// swap the cache line in L1 and L2
					int temp_tag = this->L1[addr_index].tag;
					int temp_data = this->L1[addr_index].data;
					
					this->L1[addr_index].tag = addr_tag;
					this->L1[addr_index].data = *data;
					this->L1[addr_index].valid = true;

					this->L2[addr_index][i].tag = temp_tag;
					this->L2[addr_index][i].data = temp_data;
                    this->L2[addr_index][i].valid = true;

					// update the LRU position table
					for(int j=0; j<L2_CACHE_WAYS; j++){
						if((L2[addr_index][j].lru_position > L2[addr_index][i].lru_position) && (L2[addr_index][j].valid == true)){
							L2[addr_index][j].lru_position -= 1;
						}
					}
					this->L2[addr_index][i].lru_position = 7; // most recently used
					break;
				}
			}
			if(i>=L2_CACHE_WAYS)
				this->myStat.missL2 += 1;
		}
		// write-through & write-no-allocate, update the main memory
		myMem[adr] = *data;
	} 

	if(MemR){ // load
		// search in L1 cache
		this->myStat.accL1 += 1;
		if((this->L1[addr_index].tag == addr_tag) && (this->L1[addr_index].valid == true)){
			found = 1;
			this->myStat.hitL1 += 1;
		}
		// not found in L1, search L2
		else{
			// update stat
			this->myStat.missL1 += 1;
			this->myStat.accL2 += 1;

			int i;
			for(i=0; i<L2_CACHE_WAYS; i++){
				if((this->L2[addr_index][i].tag == addr_tag) && (this->L2[addr_index][i].valid == true)){

					this->myStat.hitL2 += 1;

					// swap the cache line in L1 and L2
					int temp_tag = this->L1[addr_index].tag;
					int temp_data = this->L1[addr_index].data;
					
					this->L1[addr_index].tag = addr_tag;
					this->L1[addr_index].data = *data;
					this->L1[addr_index].valid = true;

					this->L2[addr_index][i].tag = temp_tag;
					this->L2[addr_index][i].data = temp_data;
                    this->L2[addr_index][i].valid = true;
					
					// update the LRU position table
					for(int j=0; j<L2_CACHE_WAYS; j++){
						if((L2[addr_index][j].lru_position > L2[addr_index][i].lru_position) && (L2[addr_index][j].valid == true)){
							L2[addr_index][j].lru_position -= 1;
						}
					}
					this->L2[addr_index][i].lru_position = 7; // most recently used
					found = 1;
					break;
				}
			}
			if(i>=L2_CACHE_WAYS)
				this->myStat.missL2 += 1;
		}
		// not found in Cache, go to main memory
		if(!found){
			int temp_data = this->L1[addr_index].data;
			int temp_tag = this->L1[addr_index].tag;
			int temp_valid = this->L1[addr_index].valid;

			// bring the cache line to L1
			this->L1[addr_index].data = *data;
			this->L1[addr_index].valid = true;
			this->L1[addr_index].tag = addr_tag;


			// put the evicted line from L1 to L2
			if(temp_valid){
				int i;
				for(i=0; i<L2_CACHE_WAYS; i++){
					if(this->L2[addr_index][i].valid == false){ // put the cache line in the invalid entry
						this->L2[addr_index][i].data = temp_data;
						this->L2[addr_index][i].tag = temp_tag;
						this->L2[addr_index][i].valid = true;

						// update the LRU position table
						for(int j=0; j<L2_CACHE_WAYS; j++)
							if(L2[addr_index][j].valid == true)
								L2[addr_index][j].lru_position -= 1;
						
						this->L2[addr_index][i].lru_position = 7;
						break;					
					}
				}
				if(i >= L2_CACHE_WAYS){ // if all the lines are valid, put the cache line in the least-recently-used entry
					for(int k=0; k<L2_CACHE_WAYS;k++)
						if(this->L2[addr_index][k].lru_position == 0){
							this->L2[addr_index][k].data = temp_data;
							this->L2[addr_index][k].tag = temp_tag;
							this->L2[addr_index][k].valid = true;
							this->L2[addr_index][k].lru_position = 7;

							for(int j=0; j<L2_CACHE_WAYS; j++)
								if(j!=k)
									L2[addr_index][j].lru_position -= 1;
						}
				}
			}	
		}
	}
	cout<<"| #L1 Access:" <<this->myStat.accL1<<"	| #L2 Access:" << this->myStat.accL2 <<"	| #L1 Hit:"<< this->myStat.hitL1
	<<"	| #L2 Hit:"<< this->myStat.hitL2 << "	| #L1 Miss:"<<
	this->myStat.missL1 << "	| #L1 Miss:" << this->myStat.missL2 << endl;
}