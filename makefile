VPATH = build : src

compilerFlags = -Wall

mem‐sim: main.o simulator.o mem_sim.o task_structure.o mem_sim_fifo.o mem_sim_lru.o mem_sim_2ch.o mem_sim_ed.o
	cd build && g++ main.o simulator.o task_structure.o mem_sim.o mem_sim_2ch.o mem_sim_ed.o mem_sim_lru.o mem_sim_fifo.o -o ../mem‐sim

main.o: main.cpp simulator.h simulator.o mem_sim_lru.o mem_sim_2ch.o mem_sim_ed.o
	cd src && g++ -c $(compilerFlags) main.cpp -o ../build/main.o

mem_sim.o: mem_sim.h mem_sim.cpp task_structure.h
	cd src && g++ -c $(compilerFlags) mem_sim.cpp -o ../build/mem_sim.o

simulator.o: simulator.h simulator.cpp task_structure.h event.h mem_sim.h mem_sim_fifo.h mem_sim_lru.h mem_sim_ed.h
	cd src && g++ -c $(compilerFlags) simulator.cpp -o ../build/simulator.o

task_structure.o: task_structure.cpp task_structure.h mem_sim.h
	cd src && g++ -c $(compilerFlags) task_structure.cpp -o ../build/task_structure.o

mem_sim_fifo.o: mem_sim_fifo.h mem_sim_fifo.cpp mem_sim.cpp	mem_sim.h
	cd src && g++ -c $(compilerFlags) mem_sim_fifo.cpp -o ../build/mem_sim_fifo.o

mem_sim_lru.o: mem_sim_lru.h mem_sim_lru.cpp mem_sim.cpp mem_sim.h
	cd src && g++ -c $(compilerFlags) mem_sim_lru.cpp -o ../build/mem_sim_lru.o

mem_sim_2ch.o: mem_sim_2ch.cpp mem_sim_2ch.h mem_sim.cpp mem_sim.h
	cd src && g++ -c $(compilerFlags) mem_sim_2ch.cpp -o ../build/mem_sim_2ch.o

mem_sim_ed.o: mem_sim_ed.cpp mem_sim_ed.h mem_sim.cpp mem_sim.h
	cd src && g++ -c $(compilerFlags) mem_sim_ed.cpp -o ../build/mem_sim_ed.o

run: run_ed run_lru run_fifo run_2ch

run_ed:
	./mem‐sim 200 10000 ed data/sche_traces_small.txt 
	
run_lru:
	./mem‐sim 200 10000 lru data/sche_traces_small.txt 

run_fifo:
	./mem‐sim 200 10000 fifo data/sche_traces_small.txt 

run_2ch:
	./mem‐sim 200 10000 2ch‐alg data/sche_traces_small.txt 

clean:
	rm build/* mem‐sim