#ifndef _TASK_STRUCTURE_HEADER_
#define _TASK_STRUCTURE_HEADER_

#include <string>
#include <queue>
#include <map>
// #include "mem_sim.h"

using namespace std;

enum TASK_RET_TYPE{
	TASK_END,
	TASK_PGF,
	TASK_NORMAL
};

class task_structure
 {
 public:
 	static int task_count;
 	string p_mem_file;
 	FILE *mem_file;
 	queue<int> memseq;
 	// map virtual to phical
 	map<int,int> addmap;

 	long start_time;
 	long end_time;
 	long num_pgf;
 	long num_hit;
 	unsigned int numInsts;
 	bool finished;

 public:
 	int pid;
 	long last_sched_in;
 	string pname;
 	task_structure(string name,string mem_file);
	// int clock_tick();
 };


#endif