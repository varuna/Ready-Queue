/*******************************************************************************
* This file is part of ready queue simulator project.
* Maintained by : Varuna Lekamwasam <vrlekamwasam@gmail.com>
* Authored By : Varuna Lekamwasam <vrlekamwasam@gmail.com>
*
* Ready queue simulator project is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* Ready queue is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
*******************************************************************************/

//cpu.h
//preprocessor commands
#include "ready_Q.h"
#include <time.h>
#include <pthread.h>

//defining data type structures
typedef struct 
{
	int _running;
	int _idle;
}stat_t;	//data structure to notify CPU status

typedef struct
{
	int _id;
	proc_t * _running;
	stat_t stat;
	struct tm *service_t;
	int num_processes;
	int total_waiting_time;
	int total_turn_around_time;
}cpu_t;		//data structure to represent a CPU

typedef struct
{
	ready_Q *rq;
	cpu_t *cpu;
}cpu_data_bind_t;	//data structure to be used in cpu data binding
			// while passing data to threads


//defining data structure functioanlities

//setting status of the CPU's
void idle_cpu(cpu_t * c)
{
	c->stat._idle = 1;
	c->stat._running = 0;
}//idle state
void busy_cpu(cpu_t * c)
{
	c->stat._idle = 0;
	c->stat._running = 1;
}//busy state

void set_stat(cpu_t * cp, int _idle )
{
	switch(_idle)
	{
		case 1 : idle_cpu(cp);
				break;
		case 0 : busy_cpu(cp);
				break;
		default : idle_cpu(cp);
				break;
	}
}//state setting functionality

cpu_t getInstance(int id)	//creating a cpu_t object for a give ID;
{
	cpu_t cp;
	cp._id = id;
	cp._running = NULL;
	cp.num_processes = 0;
	cp.total_waiting_time = 0;
	cp.total_turn_around_time = 0;
	set_stat(&cp,1);
	return cp;
}
//attach a given process to the cpu_t cp taking from the ready_Q rq.
void attach_process(cpu_t * cp,ready_Q * rq)
{
	if (rq != NULL && (isQFull(rq) != 0)){
		cp->_running = NULL;
		proc_t *p = pop(rq);
		time_t tp;
		time_t lt = time(&tp);
		struct tm *ty = localtime(&lt);
		cp->service_t = ty;	
		if (p != NULL){
			int hrs = ty->tm_hour - p->arrival_t.tm_hour;
			int min = ty->tm_min - p->arrival_t.tm_min;
			int sec = ty->tm_sec - p->arrival_t.tm_sec;
			int total = hrs*3600 + min*60 + sec;
			cp->total_waiting_time = cp->total_waiting_time + total;
			cp->num_processes++;
			cp->_running = p;	
			set_stat(cp,0);
		}
	}
}
//detach the process from the cpu. 
void dettach_process(cpu_t * cp, ready_Q *rq)
{
	cp->_running = NULL;
	set_stat(cp,1);
}
void execute_process(cpu_t *cpu)
{
  if (cpu->_running != NULL){
	int _burst = cpu->_running->burst_t;
	sleep(_burst);
	time_t t;
	time_t tl = time(&t);
	struct tm *cmp = localtime(&tl);
	int hrs = cmp->tm_hour;
	int min = cmp->tm_min;
	int sec = cmp->tm_sec;
	struct tm tp = cpu->_running->arrival_t;
	hrs = hrs - tp.tm_hour;
	min = min - tp.tm_min;
	sec = sec - tp.tm_sec;
	cpu->total_turn_around_time = cpu->total_turn_around_time + (hrs * 3600) + (min*60 + sec);
  }
}
//simulation recording abstraction providing functionality
int CPU_simulation_service(cpu_t *cpu, int service, int report)
{
                int t = -1;
                int id = cpu->_id;
                int pid = cpu->_running->proc_ID;
                struct tm arr = cpu->_running->arrival_t;
                if (service == 1){
                        struct tm *serv = cpu->service_t;
                        t = add_CPUSERVICE_simulationEntry(id,pid,arr,serv);
                        t+=10;
                }else if (service == 0){
                        t = add_CPUCOMPLETE_simulationEntry(id,pid,arr);                        
                }
                if (report == 1)
                        if (t == -1){
                                printf("service unspecified.\n");
                        }else if (t == 10){
                                printf("CPUSERVICE successfully reported.\n");                  
                        }else if (t == 11){
                                printf("CPUSERVICE could not be reported.\n");
                        }else if (t == 0){
                                printf("CPUCOMPLETE successfully reported.\n");
                        }else if (t == 1){
                                printf("CPUCOMPLETE could not be reported.\n");
                        }else{  printf("Unrecognized functionality.\n");  }
}

