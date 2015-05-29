#ifndef _TASK_STRUCTURE_HEADER_
#define _TASK_STRUCTURE_HEADER_

#include <string>
#include <queue>

using namespace std;

class task_structure
 {
 public:
 	int pid;
 	long last_sched_in;
 	string pname;
 	string p_mem_file;
 	FILE *mem_file;
 	queue<int> memseq;

 public:
 	task_structure(string name,string mem_file);
	bool clock_tick();
 };


#endif