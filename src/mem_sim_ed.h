#ifndef _MEM_SIM_ED_
#define _MEM_SIM_ED_ 

#include <stdio.h>
#include <stdlib.h>
#include <queue>
#include <map>
#include <set>
#include "mem_sim.h"

using namespace std;


class mem_sim_ed
	:public mem_sim
{
private:
	queue<int> freePages;
	// map phisical page to pids
	map<int, task_structure*> ppage_pid_map;
	// map phisical page to virtual page
	map<int,int> ppage_vpage_map;
	vector<double> visitTime;
	queue<event> requestQueue;
	vector<int> firstVisit;
	long last_load_page;

	int find_page(void);

	double C;// expontial const
public:
	mem_sim_ed(int _pages);
	int access(int pid,int pgnum);
	int clock_tick(task_structure*,priority_queue<event> &,long stime);
	int handle_page_event(priority_queue<event>&eventQueue, long stime);

	// int allocate(int pid);
};

#endif