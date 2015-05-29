#include "task_structure.h"
#include <stdlib.h>
#include <stdio.h>

task_structure::task_structure(string name, string mem_file_name,mem_sim*_mem)
	:memmodule(_mem),
	pname(name)
{
	task_count ++;
	pid = task_count;
	mem_file = fopen(mem_file_name.c_str(),"r");
	int tmp;
	while(EOF != fscanf(mem_file,"%i",&tmp)){
		memseq.push(tmp);
	}
}

/*
	return true, end of simulation
*/
int task_structure::clock_tick(){
	if(addmap.find(memseq.front()) == addmap.end() ){
		int pgn = memmodule->allocate(pid);
		if(pgn >= 0){
			addmap[memseq.front()] = pgn;
		}
		return TASK_PGF;
	}else{
		int pgn = addmap[memseq.front()];
		if(memmodule->access(pid,pgn) == MEM_ACCESS_PGF){
			return TASK_PGF;
		}else{
			memseq.pop();
			if(memseq.empty())
				return TASK_END;
			else{
				return TASK_NORMAL;
			}
		}
	}
}

int task_structure::task_count = 0;