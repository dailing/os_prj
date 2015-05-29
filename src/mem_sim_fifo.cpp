#include "mem_sim_fifo.h"

mem_sim_fifo::mem_sim_fifo(int _pages):
	mem_sim(_pages)
{

}

int mem_sim_fifo::access(int pid,int pgnum){
	return MEM_ACCESS_SUCCED;
}

int mem_sim_fifo::allocate(int pid){
	static int ret = 0;
	return ret++;
}