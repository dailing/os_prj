#include "mem_sim_fifo.h"
#include <assert.h>

mem_sim_fifo::mem_sim_fifo(int _pages):
	mem_sim(_pages),
	last_load_page(0)
{
	firstVisit.resize(pages);
	for (int i = 0; i < pages; ++i){
		freePages.push(i);
		firstVisit[i] = 0;
	}
}



int mem_sim_fifo::clock_tick(task_structure*current,priority_queue<event>&eventQueue, long stime){
	assert(current != NULL);
	int pg  = current->memseq.front();
	if(current->addmap.find(pg) != current->addmap.end()){
		current->memseq.pop();
		firstVisit[current->addmap[pg]] = 0;
		if(current->memseq.empty()){
			printf("%s finisged\n", current->pname.c_str());
			for(map<int,int>::iterator i = current->addmap.begin();
				i != current->addmap.end(); i++)
			{
				// printf("erase page %d for %s at %ld\n", i->second, current->pname.c_str(), stime);
				assert(ppage_pid_map.find(i->second)!= ppage_pid_map.end());
				// assert(ppage_pid_map[i->second] == current);
				firstVisit[i->second] = 0;
					freePages.push(i->second);
				if(ppage_pid_map[i->second] == current){
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
			requestQueue.push(
				event(start_time,EVENT_ERASE_PAGE,
					ppage_pid_map[loadQueue.front()],
					loadQueue.front(), // phisical page
					ppage_vpage_map[loadQueue.front()]  // virtual page#
					) );
			requestQueue.push(
				event(start_time+read_page_time,
					EVENT_PAGE_FETCHED,
					current,
					loadQueue.front(), // phisical page
					pg  // virtual page#
					) );
			ppage_vpage_map[loadQueue.front()] = pg;
			ppage_pid_map[loadQueue.front()] = current;
			// printf("replace page vpage %6d , ppage %6d, for %s \n", ppage_vpage_map[loadQueue.front()], loadQueue.front(),ppage_pid_map[loadQueue.front()]->pname.c_str());
			// printf("add page vpage %6d , ppage %6d, for %s \n", pg,loadQueue.front(),current->pname.c_str());
			loadQueue.push(loadQueue.front());
			loadQueue.pop();
		}else{
			requestQueue.push(
				event(start_time+read_page_time,
					EVENT_PAGE_FETCHED,
					current,
					freePages.front(), // phisical page
					pg  // virtual page#
					) );
			ppage_vpage_map[freePages.front()] = pg;
			ppage_pid_map[freePages.front()] = current;
			// printf("add page vpage %6d , ppage %6d, for %s \n", pg,freePages.front(),current->pname.c_str());
			loadQueue.push(freePages.front());
			freePages.pop();
		}
		if(requestQueue.front().type == EVENT_ERASE_PAGE){
			if(firstVisit[requestQueue.front().data.eepd.ppage] == 0){
				firstVisit[requestQueue.front().data.eepd.ppage] = 1;
				requestQueue.front().start_time = start_time;
				eventQueue.push(requestQueue.front());
				requestQueue.pop();
				assert(requestQueue.front().type == EVENT_PAGE_FETCHED);
				requestQueue.front().start_time = start_time + read_page_time;
				eventQueue.push(requestQueue.front());
				requestQueue.pop();
			}
		}else{
			assert(requestQueue.front().type == EVENT_PAGE_FETCHED);
			requestQueue.front().start_time = start_time + read_page_time;
			eventQueue.push(requestQueue.front());
			requestQueue.pop();
		}
		return TASK_PGF;
	}
}