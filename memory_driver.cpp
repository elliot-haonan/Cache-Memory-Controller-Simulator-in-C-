/*
Engineer: Haonan Liu, University of California, Los Angeles
Date: 2022/11/13
*/

#include <fstream>
#include <iostream>
#include <cstring>
#include <sstream>
#include <vector>
#include "cache.h"
using namespace std;

struct trace
{
	bool MemR; 
	bool MemW; 
	int adr; 
	int data; 
};

int main (int argc, char* argv[]) // the program runs like this: ./program <filename> <mode>
{
	// input file (i.e., test.txt)
	string filename = argv[1];
	ifstream fin;
    
	// opening file
	fin.open(filename.c_str());
	if (!fin){ // making sure the file is correctly opened
		cout << "Error opening " << filename << endl;
		exit(1);
	}
	
	// reading the text file
	string line;
	vector<trace> myTrace;
	int TraceSize = 0;
	string s1,s2,s3,s4;
    while( getline(fin,line) )
      	{
            stringstream ss(line);
            getline(ss,s1,','); 
            getline(ss,s2,','); 
            getline(ss,s3,','); 
            getline(ss,s4,',');
            myTrace.push_back(trace()); 
            myTrace[TraceSize].MemR = stoi(s1);
            myTrace[TraceSize].MemW = stoi(s2);
            myTrace[TraceSize].adr = stoi(s3);
            myTrace[TraceSize].data = stoi(s4);
            //cout<<myTrace[TraceSize].MemW << endl;
            TraceSize+=1;
        }
    
	// Defining cache and stat
    cache myCache;
    int myMem[MEM_SIZE]; 


	int traceCounter = 0;
	bool cur_MemR; 
	bool cur_MemW; 
	int cur_adr;
	int cur_data;

	cout << "--------------------------Cache Statistics----------------------"<< endl;
	// this is the main loop of the code
	while(traceCounter < TraceSize){
		
		cur_MemR = myTrace[traceCounter].MemR;
		cur_MemW = myTrace[traceCounter].MemW;
		cur_data = myTrace[traceCounter].data;
		cur_adr = myTrace[traceCounter].adr;
		traceCounter += 1;
		cout <<"TraceCounter:"<<traceCounter<<"	";
		myCache.controller (cur_MemR, cur_MemW, &cur_data, cur_adr, myMem); 
	}
	
	float L1_miss_rate, L2_miss_rate, AAT; 
	
	L1_miss_rate = (float)myCache.getMissL1()/myCache.getAccL1(); 
	L2_miss_rate = (float)myCache.getMissL2()/myCache.getAccL2(); 
	AAT = 1 + L1_miss_rate*(8 + L2_miss_rate*100);

	cout<< "(" << L1_miss_rate<<","<<L2_miss_rate<<","<<AAT<<")"<<endl;
	// closing the file
	fin.close();
    
	return 0;
}
