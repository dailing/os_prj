#ifndef _TASK_STRUCTURE_HEADER_
#define _TASK_STRUCTURE_HEADER_

#include <string>
#include <queue>
#include <map>
#include "mem_sim.h"

using namespace std;

enum TASK_RET_TYPE{
	TASK_END,
	TASK_PGF,
	TASK_NORMAL
};

class task_structure
 {
 private:
 	static int task_count;
 	string p_mem_file;
 	FILE *mem_file;
 	queue<int> memseq;
 	mem_sim* memmodule;

 	map<int,int> addmap;

 public:
 	int pid;
 	long last_sched_in;
 	string pname;
 	task_structure(string name,string mem_file,mem_sim*);
	int clock_tick();
 };


#endif