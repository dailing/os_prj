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

int mem_sim_fifo::handle_page_event(priority_queue<event>&eventQueue, long stime){
	if(requestQueue.empty())
		return 0;
	int start_time = max(last_load_page+read_page_time,stime);
	last_load_page = start_time;
	if(!freePages.empty()){
		int ppage = freePages.front();
		firstVisit[ppage] = 1;
		eventQueue.push(
			event(start_time+read_page_time,
				EVENT_PAGE_FETCHED,
				requestQueue.front().data.eepd.process,
				ppage, // phisical page
				requestQueue.front().data.eepd.vpage  // virtual page#
				) );
		// printf("add   vpage %d ppage %d for %s at %ld\n", 
		// 	requestQueue.front().data.eepd.vpage,
		// 	ppage,requestQueue.front().data.eepd.process->pname.c_str(),stime);
		ppage_pid_map[ppage] = requestQueue.front().data.eepd.process;
		ppage_vpage_map[ppage] = requestQueue.front().data.eepd.vpage;
		loadQueue.push(ppage);
		freePages.pop();
		requestQueue.pop();
	}else{
		int vpage = requestQueue.front().data.eepd.vpage;
		int ppage = loadQueue.front();
		if(firstVisit[ppage] == 0){
			firstVisit[ppage] = 1;
			if(!ppage_pid_map[ppage]->finished){
				// printf("erase vpage %d ppage %d for %s at %ld\n", 
				// 	ppage_vpage_map[ppage],ppage,
				// 	ppage_pid_map[ppage]->pname.c_str(),stime);
				assert(ppage_pid_map[ppage]->addmap.find(ppage_vpage_map[ppage])
					!= ppage_pid_map[ppage]->addmap.end());
				ppage_pid_map[ppage]->addmap.erase(ppage_vpage_map[ppage]);
			}
			// printf("add   vpage %d ppage %d for %s at %ld\n", 
			// 	vpage,ppage,ppage_pid_map[ppage]->pname.c_str(),stime);
			eventQueue.push(
				event(start_time+read_page_time,
					EVENT_PAGE_FETCHED,
					requestQueue.front().data.eepd.process,
					ppage, // phisical page
					vpage  // virtual page#
					) );
			ppage_pid_map[ppage] = requestQueue.front().data.eepd.process;
			ppage_vpage_map[ppage] = requestQueue.front().data.eepd.vpage;
			requestQueue.pop();
		}
	}
	return 1;
}
int mem_sim_fifo::clock_tick(task_structure*current,priority_queue<event>&eventQueue, long stime){
	// printf("clock_tick %s vpage %d \n",current->pname.c_str(), current->memseq.front());
	assert(current != NULL);
	int pg  = current->memseq.front();
	if(current->addmap.find(pg) != current->addmap.end()){
		current->memseq.pop();
		firstVisit[current->addmap[pg]] = 0;
		if(current->memseq.empty()){
			// printf("%s finished\n", current->pname.c_str());
			for(map<int,int>::iterator i = current->addmap.begin();
				i != current->addmap.end(); i++)
			{
				// printf("free  page %d for %s at %ld\n", i->second, current->pname.c_str(), stime);
				firstVisit[i->second] = 0;
				freePages.push(i->second);
			}
			current->finished = 1;
			return TASK_END;
		}else{
			return TASK_NORMAL;
		}
	}else{
		int start_time = last_load_page;
		requestQueue.push(
			event(start_time+read_page_time,
				EVENT_PAGE_FETCHED,
				current,
				0, // phisical page
				pg  // virtual page#
				) );
		handle_page_event(eventQueue,stime);
		return TASK_PGF;
	}
}