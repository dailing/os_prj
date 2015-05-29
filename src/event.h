#ifndef _EVENT_HEAD_
#define _EVENT_HEAD_ 

#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

using namespace std;

enum event_type{
	EVENT_NEW_PROCESS = 1,
	EVENT_RR_TIMEOUT,
	EVENT_PAGE_FETCHED
};

union event_data{
	char pname[20];
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
	
};


#endif