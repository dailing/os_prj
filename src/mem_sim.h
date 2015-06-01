#ifndef _MEM_SIM_HEADER_
#define _MEM_SIM_HEADER_ 
#include "task_structure.h"
#include "event.h"

enum MEM_ACCESS_SRARUS
{
	MEM_ACCESS_SUCCED,
	MEM_ACCESS_PGF
};


class mem_sim
{
public:
	int read_page_time;
	int pages;
	virtual int clock_tick(task_structure*,priority_queue<event> &, long stime)=0;
	mem_sim(int _pages);
};

#endif