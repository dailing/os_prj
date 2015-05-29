#include "task_structure.h"
#include <stdlib.h>
#include <stdio.h>

task_structure::task_structure(string name, string mem_file_name)
	:pname(name)
{
	mem_file = fopen(mem_file_name.c_str(),"r");
	int tmp;
	while(EOF != fscanf(mem_file,"%i",&tmp)){
		memseq.push(tmp);
	}
}

/*
	return true, end of simulation
*/
bool task_structure::clock_tick(){
	if(memseq.empty())
		return true;
	else{
		memseq.pop();
		return false;
	}
}

