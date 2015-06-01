#include "task_structure.h"
#include <stdlib.h>
#include <stdio.h>

task_structure::task_structure(string name, string mem_file_name)
	:finished(false),
	pname(name)
{
	task_count ++;
	start_time = -1;
	pid = task_count;
	mem_file = fopen(mem_file_name.c_str(),"r");
	int tmp;
	while(EOF != fscanf(mem_file,"%i",&tmp)){
		memseq.push(tmp);
	}
	numInsts = (int)memseq.size();
	// printf("%u\n", memseq.size());
	num_pgf = 0;
	num_hit = 0;
}

int task_structure::task_count = 0;