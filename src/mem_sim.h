#ifndef _MEM_SIM_HEADER_
#define _MEM_SIM_HEADER_ 


enum MEM_ACCESS_SRARUS
{
	MEM_ACCESS_SUCCED,
	MEM_ACCESS_PGF
};


class mem_sim
{
private:
	int pages;
public:
	virtual int access(int pid,int pgnum) = 0;
	virtual int allocate(int pid) = 0;
	mem_sim(int _pages);
};

#endif