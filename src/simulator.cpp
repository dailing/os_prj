#include <stdlib.h>
#include <stdio.h>
#include <string>
#include "simulator.h"
#include "mem_sim.h"
#include "task_structure.h"
#include "event.h"

simulator::simulator(int _pages,int _quantum,
		string _pr,string trace_fi):
		circles_per_second(100000),
		context_switch_time(50),
		read_page_time(1000),
		current(NULL),
		quantum(_quantum),
		sys_count(0)
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
	printf("simulator initialed\n");
	printf("-----------------------------------------\n");
}

void simulator::schedule(void){
	printf("switch %s to %s at %ld\n", 
		current==NULL?"EMPTY":current->pname.c_str(),
		readyQueue.empty()?"EMPTY":readyQueue.front()->pname.c_str(),
		sys_count);
	if(current != NULL){
		readyQueue.push(current);
	}
	if(readyQueue.size() > 1 && current != NULL){
		sys_count += context_switch_time;
	}
	if(!readyQueue.empty()){
		eventQueue.push(event(sys_count+quantum,EVENT_RR_TIMEOUT));
		current = readyQueue.front();
		current->last_sched_in = sys_count;
		readyQueue.pop();
	}else{
		// printf("readyQueue empty\n");
		current = NULL;
	}

	// printf("leave schedule\n");
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
	while(!eventQueue.empty() && sys_count >= eventQueue.top().start_time){
		string processname;
		switch(eventQueue.top().type){
			case EVENT_NEW_PROCESS:
				processname = string(eventQueue.top().data.pname);
				printf("nwe process:%s at %ld\n", processname.c_str(),sys_count);
				readyQueue.push(new task_structure(processname,filePath+processname+".mem") );
				break;
			case EVENT_RR_TIMEOUT:
				if(current != NULL && eventQueue.top().start_time-current->last_sched_in == quantum){
					schedule();
				}
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
	if(current != NULL && current->clock_tick()){
		printf("process :%s finished at %ld \n", current->pname.c_str(),sys_count);
		delete current;
		current = NULL;
	}
	sys_count++;
	if(current == NULL && eventQueue.empty() && readyQueue.empty())
		return true;
	else
		return false;
}