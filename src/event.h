#ifndef _EVENT_HEAD_
#define _EVENT_HEAD_ 

#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "task_structure.h"
using namespace std;

enum event_type{
	EVENT_NEW_PROCESS = 1,
	EVENT_RR_TIMEOUT,
	EVENT_PAGE_FETCHED,
	EVENT_ERASE_PAGE
};

struct event_erase_pagre_data{
	task_structure* process;
	int   vpage;
	int   ppage;
};

union event_data{
	char pname[20];
	event_erase_pagre_data eepd;
};

class event
{
public:
	// friend bool operator <(const event &a, const event &b);
	friend bool operator <(const event &a, const event &b){
		return a.start_time > b.start_time;
	}
	int start_time;
	int type;
	event_data data;
	// new process event;
	event(int _start_time,int _type, char*pname):
		start_time(_start_time),
		type(_type)
	{
		strcpy(data.pname,pname);
	}
	// new cputime out event;
	event(int _start_time,int _type):
		start_time(_start_time),
		type(_type)
	{
		
	}
	// new erase event, new page fetched event
	event(int _start_time,int _type,task_structure* p,int ppage,int vpage):
		start_time(_start_time),
		type(_type)
	{
		data.eepd.process = p;
		data.eepd.vpage = vpage;
		data.eepd.ppage = ppage;
	}
};


#endif