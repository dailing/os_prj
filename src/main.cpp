#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include "simulator.h"
#include <string>

int main(int argc, char const *argv[]){
	if (argc < 5){
		printf("ERRPR: argument not correct\n");
		return -1;
	}
	int pages = atoi(argv[1]);
	int quantum = atoi(argv[2]);
	string pr_policy(argv[3]);
	string trace_file(argv[4]);
	
	simulator sim(pages,quantum,pr_policy,trace_file);
	for(int i = 0; ; i++){
		if(sim.clock_tick())
			break;
	}
	return 0;
}