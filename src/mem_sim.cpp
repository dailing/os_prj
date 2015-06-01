#include "mem_sim.h"
#include "task_structure.h"
#include <assert.h>

mem_sim::mem_sim(int _pages):
	read_page_time(1000)
{
	printf("MEM_SIM\n");
	pages = _pages;
}

void mem_sim::statics(task_structure* current){
	assert(current != NULL);
	int pg  = current->memseq.front();
	if(current->addmap.find(pg) != current->addmap.end()){
		current->num_hit++;
	}else{
		current->num_pgf++;
	}

}