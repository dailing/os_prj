#include "mem_sim_2ch.h"
#include <assert.h>
#include <algorithm>

mem_sim_2ch::mem_sim_2ch(int _pages):
	mem_sim(_pages),
	last_load_page(0)
{
	printf("2CH\n");
	firstVisit.resize(pages);
	reference.resize(pages);
	memcounter = 0;
	for (int i = 0; i < pages; ++i){
		freePages.push(i);
		firstVisit[i] = 0;
		reference[i] = true;
	}
}

int mem_sim_2ch::find_page(void){
	while(1){
		memcounter = memcounter % pages;
		if(reference[memcounter]){
			reference[memcounter] = false;
		}else{
			break;
		}
		memcounter ++;
	}
	memcounter++;
	return memcounter;
}

int mem_sim_2ch::handle_page_event(priority_queue<event>&eventQueue, long stime){
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
		ppage_pid_map[ppage] = requestQueue.front().data.eepd.process;
		ppage_vpage_map[ppage] = requestQueue.front().data.eepd.vpage;
		reference[ppage] = true;
		freePages.pop();
		requestQueue.pop();
	}else{
		int vpage = requestQueue.front().data.eepd.vpage;
		// int ppage = loadQueue.front().ppage;
		int ppage = find_page();
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
			// loadQueue.pop_front();
			// loadQueue.push_back(ppage);
			reference[ppage] = true;
			ppage_pid_map[ppage] = requestQueue.front().data.eepd.process;
			ppage_vpage_map[ppage] = requestQueue.front().data.eepd.vpage;
			requestQueue.pop();
		}
	}
	return 1;
}
int mem_sim_2ch::clock_tick(task_structure*current,priority_queue<event>&eventQueue, long stime){
	mem_sim::statics(current);
	// printf("clock_tick %s vpage %d \n",current->pname.c_str(), current->memseq.front());
	assert(current != NULL);
	int pg  = current->memseq.front();
	if(current->addmap.find(pg) != current->addmap.end()){
		current->memseq.pop();
		firstVisit[current->addmap[pg]] = 0;
		// reference[current->addmap[pg]] = stime;
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