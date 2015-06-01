#ifndef _SIMULATOR_HEADER_
#define _SIMULATOR_HEADER_

#include <queue>
#include <string>
#include <queue>
#include <set>
#include "event.h"
#include "task_structure.h"
#include "mem_sim.h"

using namespace std;

class simulator
{
private:
	const int circles_per_second;
	const int context_switch_time;
	const int read_page_time;
	// task record
	task_structure *current;
	queue<task_structure*> readyQueue;
	set<task_structure*> waitingQueue;
	set<task_structure*> finishedSet;
	// scheduler 
	int quantum;
	long sys_count;
	priority_queue<event> eventQueue;
	// enviroment
	string filePath;

	mem_sim * memmodule;

/* 	this call will switch current procrss
	out and switch process from ready queue
	in.
*/
	void schedule(void);
	void switch_out(void);

public:
	void addProcess();
	bool clock_tick();
	simulator(int,int,string,string);
};

#endif