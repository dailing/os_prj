#ifndef _MEM_SIM_FIFO_
#define _MEM_SIM_FIFO_ 

#include <stdio.h>
#include <stdlib.h>
#include <queue>
#include <map>
#include "mem_sim.h"

using namespace std;

class mem_sim_fifo
	:public mem_sim
{
private:


public:
	mem_sim_fifo(int _pages);
	int access(int pid,int pgnum);
	int allocate(int pid);
};

#endif