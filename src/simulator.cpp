#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <assert.h>
#include "simulator.h"
#include "mem_sim_fifo.h"

simulator::simulator(int _pages,int _quantum,
		string _pr,string trace_fi):
		circles_per_second(100000),
		context_switch_time(50),
		read_page_time(1000),
		current(NULL),
		quantum(_quantum),
		sys_count(0),
		memmodule(NULL)
{
	printf("-----------------------------------------\n");
	printf("pages:%d quantum:%d\n", _pages,_quantum);
	printf("pr:              %s\n", _pr.c_str());
	printf("trace file:      %s\n", trace_fi.c_str());
	unsigned int found = trace_fi.find_last_of("/\\");
	if(found !=  string::npos)
		filePath = trace_fi.substr(0,found) + "/";
	else
		filePath = "";
	printf("trace file path: %s\n", filePath.c_str());
	
	float starttime,cputime;
	int iocount;
	FILE * file = fopen(trace_fi.c_str(),"r");
	char namebuf[100];
	while(EOF != fscanf(file,"%s%f%f%d",namebuf,&starttime,&cputime,&iocount)){
		starttime *= circles_per_second;
		cputime   *= circles_per_second;
		printf("%-5s  %8.0f %8.0f %9d\n", namebuf,starttime,cputime,iocount);
		eventQueue.push(event((int)starttime,EVENT_NEW_PROCESS,namebuf));
	}
	memmodule = new mem_sim_fifo(_pages);
	printf("simulator initialed\n");
	printf("-----------------------------------------\n");
}

/*	add current to end of ready queue and run the fitst
	process in ready queue.
	context switch time is add if context switch is needed 
*/
void simulator::schedule(void){
	// printf("switch %d to %d at %ld\n", 
	// 	current==NULL?0:current->pid,
	// 	readyQueue.empty()?0:readyQueue.front()->pid,
	// 	sys_count);
	if(readyQueue.size() > 0 && current != NULL){
		sys_count += context_switch_time;
	}
	if(current != NULL){
		readyQueue.push(current);
		current = readyQueue.front();
		current->last_sched_in = sys_count;
		eventQueue.push(event(sys_count+quantum,EVENT_RR_TIMEOUT));
		readyQueue.pop();
	}else{
		if(!readyQueue.empty()){
			current = readyQueue.front();
			current->last_sched_in = sys_count;
			eventQueue.push(event(sys_count+quantum,EVENT_RR_TIMEOUT));
			readyQueue.pop();
		}
	}

	// printf("switch end\n");
}
/*
	this function put the current process to waiting 
	queue. and call schedule to get a process in readyqueue
	to run.
*/
void simulator::switch_out(void){
	if(current != NULL){
		// printf("switch out %d at %ld\n", current->pid, sys_count);
		// waitingQueue.insert(current);
		// printf("schedule\n");
		current = NULL;
		schedule();
		// printf("schedule END\n");
	}
}
/*
	interact with readyQueue, eventQueue, current

	if(current) execute current process
	else if(readyQueue) get a new process and run
	else nothing to do, jump to most recent event

	on event:
		if new process ,add it to ready queue
		if timeout: if event is already switched 
			out, ignore, else context switch
		if I/O interrupt, page fetched, add waiting
			task to ready queue

	return true for end of simulation
*/
bool simulator::clock_tick(){
	// handle event
	// printf("TEST0\n");
	while(!eventQueue.empty() && sys_count >= eventQueue.top().start_time){
		string processname;
		event_erase_pagre_data eepd;
		switch(eventQueue.top().type){
			case EVENT_NEW_PROCESS:
				// printf("#1\n");
				processname = string(eventQueue.top().data.pname);
				printf("nwe process:%s at %ld\n", processname.c_str(),sys_count);
				readyQueue.push(
					new task_structure(
						processname,filePath+processname+".mem") );
				// processes.insert(readyQueue.back());
				// printf("#2\n");
				break;
			case EVENT_RR_TIMEOUT:
				// printf("#3\n");
				if(current != NULL && 
					eventQueue.top().start_time-current->last_sched_in == quantum)
				{
					schedule();
				}
				// printf("#4\n");
				break;
			case EVENT_PAGE_FETCHED:
				// printf("#5\n");
				eepd = eventQueue.top().data.eepd;
				// assert(waitingQueue.find(eepd.process) != waitingQueue.end());
				assert(finishedSet.find(eepd.process) == finishedSet.end() );
				// waitingQueue.erase(eepd.process);
				// printf("#5_1\n");
				readyQueue.push(eepd.process);
				// printf("#5_2\n");
				assert(eepd.process != NULL);
				eepd.process->addmap[eepd.vpage] = eepd.ppage;
				// printf("add page , process %d , vpage %x ,ppage %d at %ld\n", eepd.process->pid,
					// eepd.vpage,eepd.ppage, sys_count);
				// printf("#6\n");
				break;
			case EVENT_ERASE_PAGE:
				// printf("#7\n");
				assert(!eventQueue.empty());
				eepd = eventQueue.top().data.eepd;
				if(finishedSet.find(eepd.process) != finishedSet.end()){
					// printf("process %s , erase vpahg %d ppage %d at %ld\n", eepd.process->pname.c_str(),
						// eepd.vpage, eepd.ppage,sys_count);
					break;
				}
				assert(finishedSet.find(eepd.process) == finishedSet.end() );
				// printf("#7_1\n");
				assert(eepd.process != NULL);
				// printf("#7_2\n");
				assert(eepd.process->addmap.find(eepd.vpage) != eepd.process->addmap.end());
				// printf("#7_3\n");
				// printf("erase vpage %6d ppage %6d for %s\n", eepd.vpage,eepd.ppage, eepd.process->pname.c_str());
				// printf("#7_4\n");
				eepd.process->addmap.erase(eepd.vpage);
				// printf("#8\n");
				break;
			default:
				printf("ERROR:unrecognized event type\n");
				break;
		}
		eventQueue.pop();
	}
	// check current;
	if(current == NULL){
		schedule();
	}
	if(current == NULL && !eventQueue.empty()){
		sys_count = eventQueue.top().start_time;
	}
	// run process
	// printf("TEST1\n");
	if(current != NULL){
		switch(memmodule->clock_tick(current,eventQueue,sys_count)){
		case TASK_END:
			// printf("#1\n");
			printf("process :%d finished at %ld \n", current->pid,sys_count);
			// processes.erase(current);
			// delete current;
			finishedSet.insert(current);
			current = NULL;
			// printf("#2\n");
			break;
		case TASK_PGF:
			// printf("#3\n");
			switch_out();
			// printf("#4\n");
			break;
		case TASK_NORMAL:
			break;
		}
	}
	sys_count++;
	// printf("TEST2\n");
	if(current == NULL && eventQueue.empty() && readyQueue.empty())
		return true;
	else
		return false;
}