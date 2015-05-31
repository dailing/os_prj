#include "mem_sim.h"
#include "task_structure.h"

mem_sim::mem_sim(int _pages):
	read_page_time(1000)
{
	pages = _pages;
}
