#include "mem_sim_fifo.h"
#include <assert.h>

mem_sim_fifo::mem_sim_fifo(int _pages):
	mem_sim(_pages),
	last_load_page(0)
{
	for (int i = 0; i < pages; ++i){
		freePages.push(i);
	}
}



int mem_sim_fifo::clock_tick(task_structure*current,priority_queue<event>&eventQueue, long stime){
	assert(current != NULL);
	int pg  = current->memseq.front();
	if(current->addmap.find(pg) != current->addmap.end()){
		current->memseq.pop();
		if(current->memseq.empty()){
			printf("%s finisged\n", current->pname.c_str());
			for(map<int,int>::iterator i = current->addmap.begin();
				i != current->addmap.end(); i++)
			{
				// printf("erase page %d for %s at %ld\n", i->second, current->pname.c_str(), stime);
				assert(ppage_pid_map.find(i->second)!= ppage_pid_map.end());
				if(ppage_pid_map[i->second] == current){
					freePages.push(i->second);
				}
			}
			return TASK_END;
		}else{
			return TASK_NORMAL;
		}
	}else{
		int start_time = max(last_load_page+read_page_time,stime);
		last_load_page = start_time;
		if(freePages.empty()){
			// printf("%d\n", loadQueue.front());
			assert(ppage_pid_map.find(loadQueue.front()) != ppage_pid_map.end());
			eventQueue.push(
				event(start_time,EVENT_ERASE_PAGE,
					ppage_pid_map[loadQueue.front()],
					loadQueue.front(), // phisical page
					ppage_vpage_map[loadQueue.front()]  // virtual page#
					) );
			eventQueue.push(
				event(start_time+read_page_time,
					EVENT_PAGE_FETCHED,
					current,
					loadQueue.front(), // phisical page
					pg  // virtual page#
					) );
			ppage_vpage_map[loadQueue.front()] = pg;
			ppage_pid_map[loadQueue.front()] = current;
			// printf("replace page vpage %6d , ppage %6d, for %s \n", ppage_vpage_map[loadQueue.front()], loadQueue.front(),ppage_pid_map[loadQueue.front()]->pname.c_str());
			// printf("add page vpage %6d , ppahe %6d, for %s \n", pg,loadQueue.front(),current->pname.c_str());
			loadQueue.push(loadQueue.front());
			loadQueue.pop();
		}else{
			eventQueue.push(
				event(start_time+read_page_time,
					EVENT_PAGE_FETCHED,
					current,
					freePages.front(), // phisical page
					pg  // virtual page#
					) );
			ppage_vpage_map[freePages.front()] = pg;
			ppage_pid_map[freePages.front()] = current;
			// printf("add page vpage %6d , ppahe %6d, for %s \n", pg,freePages.front(),current->pname.c_str());
			loadQueue.push(freePages.front());
			freePages.pop();
		}
		return TASK_PGF;
	}
}