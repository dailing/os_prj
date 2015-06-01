#include "task_structure.h"
#include <stdlib.h>
#include <stdio.h>

task_structure::task_structure(string name, string mem_file_name)
	:pname(name),
	finished(false)
{
	task_count ++;
	pid = task_count;
	mem_file = fopen(mem_file_name.c_str(),"r");
	int tmp;
	while(EOF != fscanf(mem_file,"%i",&tmp)){
		memseq.push(tmp);
	}
}

int task_structure::task_count = 0;