#ifndef _MEM_SIM_FIFO_
#define _MEM_SIM_FIFO_ 

#include <stdio.h>
#include <stdlib.h>
#include <queue>
#include <map>
#include <set>
#include "mem_sim.h"

using namespace std;



class mem_sim_fifo
	:public mem_sim
{
private:
	queue<int> freePages;
	// map phisical page to pids
	map<int, task_structure*> ppage_pid_map;
	// map phisical page to virtual page
	map<int,int> ppage_vpage_map;
	queue<int> loadQueue;
	queue<event> requestQueue;
	vector<int> firstVisit;
	long last_load_page;
public:
	mem_sim_fifo(int _pages);
	int access(int pid,int pgnum);
	int clock_tick(task_structure*,priority_queue<event> &,long stime);

	// int allocate(int pid);
};

#endif