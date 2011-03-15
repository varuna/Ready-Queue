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

//main.c

//Preprocessor commands
#include "subsystems.h"					//preprocessor command to integrate subsystems
#include <stdio.h>					//including basic C commands
#include <pthread.h>					//including POSIX thread library
#include <stdlib.h>
#include <signal.h>

//GLOBAL CONSTANTS
#define THREADS 4

//GLOBAL VARIABLES
pthread_cond_t queue_not_empty;			//thread condition variable
pthread_mutex_t SIM_LOG;
report_f fr[THREADS];				//CPU data reporting data structures

//BugFixes
#define SIM_SEC 5				//Security constant
int SSEC;					//Security variable for exitting

//Thread routines
void * job(void * param)			//ready_Q loading function for thread routine
{
	load_proc((load_proc_t *)param,&queue_not_empty);
	pthread_exit(NULL);			//exit thread execution after reaching EOF in job_file.sys
}

void * cpu(void * param)			//CPU managing function
{
	cpu_data_bind_t * data = (cpu_data_bind_t *)param;	//bind parsed data to given CPU
	ready_Q * rq = data->rq;
	cpu_t *ct = data->cpu;
	printf("CPU:%d functional\n",ct->_id);			//reporting CPU status
	while (1 == 1){
			//store the current CPU statistics to create final report
		fr[ct->_id]._nProc = ct->num_processes;
		fr[ct->_id]._turn =  ct->total_turn_around_time;
		fr[ct->_id]._wait =  ct->total_waiting_time;
			//locking ready_Q
		pthread_mutex_lock(&rq->mutex);
		printf("CPU:%d is idle\n",ct->_id);		//Report CPU status
		SSEC++;						//Security protocol
		int h = isQFull(rq);				//Check for Queue is Empty
		if (h != 0){
			//make the CPU wait till signal on condition variable is recieved 
			pthread_cond_wait(&queue_not_empty,&rq->mutex);
		}
		attach_process(ct,rq);				//attach given process to CPU
		printf("\n\t--> process %d attached to CPU : %d\n\n",ct->_running->proc_ID,ct->_id);
		pthread_mutex_unlock(&rq->mutex);

		pthread_mutex_lock(&SIM_LOG);
		CPU_simulation_service(ct,1,0);
		pthread_mutex_unlock(&SIM_LOG);
		execute_process(ct);
		pthread_mutex_lock(&SIM_LOG);
		CPU_simulation_service(ct,0,0);
		pthread_mutex_unlock(&SIM_LOG);
	}
	pthread_exit(NULL);
}
void sigINT_handle()
{
	if(SSEC > SIM_SEC){
		fr[0]._nProc = 0,fr[0]._turn = 0, fr[0]._wait = 0;
		int i=0;
    pthread_mutex_lock(&SIM_LOG);
		for (i=0;i<THREADS;i++){
			fr[0]._nProc += fr[i]._nProc;
			fr[0]._turn += fr[i]._turn;
			fr[0]._wait += fr[i]._wait;
		}
		pthread_mutex_unlock(&SIM_LOG);
		int p = finalReport(&fr[0]);
		printf("\nFinal statistics are reported.\nSimulator exitting...\n");
		exit(0);
	}else{
		errorReport(6);
		signal(SIGINT,sigINT_handle);
	}
}
void sigQUIT_handle()
{
	printf("\n\nExitting simulator..\n\n");
	exit(0);
}
void interactor(int i)
{
	switch(i){
	case 1 : readJobFile();
		 printf("press any key to continue..");
		 getchar();
			printf("\n");
			break;
	default : printf("\ncontinue simulator operations\n\n");
			break;
	}
}
//Main thread
int main(int argc, char *argv[])
{
			//WAITING TILL MAIN IS INITIALIZED...
	signal(SIGINT,sigINT_handle);		//signal handler for keyboard INTERRUPT
	signal(SIGQUIT,sigQUIT_handle);		//signal handler for keyboard QUIT
	int wait=0,read = -1;			//Initializing waiting time to zero.
	if (argc != 2){
		errorReport(3);			//Invalid usage reporting.
		return -1;
	}else{
		if (strcmp(argv[1],"/0")==0)
			wait = 0;		//handling zero special case.
		else if (atoi(argv[1]) == 0){
			errorReport(4);		//invalid argument reporting
			return -1;
		}else
			wait = atoi(argv[1]);	//valid argument setting up.
	}
	resetSimulation();			//Reset this simulation from previouse simulations.
	int k = createJobFile();		//creating the job_file.sys
	if (k != 0)
		return -1;			//Exceptional exit. cannot create file.
	else
		printf("\n------\n\tSimulator initializing completed\n\tUsage info: keyboard generated interrupt(ctrl + 'C') will quit the simulator\n\tTo exit without reporting, press \" ctrl + \\ \"  \n------\n\n");
	SSEC = 0;
			//INITIALIZE COMPLETED

	char sx;	//user interaction handling
	int ix = 0;
	printf("press 1 to view job file log : ");
	scanf("%s",&sx);
	ix = atoi(&sx);	
	interactor(ix); 
	getchar();
	//to remove user interactions comment out this section

	ready_Q rq;				//create ready_Q object.
	init_q(&rq);				//Initialize ready_Q object
	load_proc_t lpt;
	lpt.readyq = &rq;
	lpt.w = wait;				//setting up completed for parameter transfer
			
			//thread setup
	pthread_t tid[THREADS];			//creating unique ID's for threads to special purposes.				
	pthread_attr_t attr;			//creating a pthread_attr_t object
	pthread_attr_init(&attr);		//initializing pthread_attr_t value with defaults.
	
	pthread_mutex_init(&rq.mutex,NULL); //initialize the mutexes
	pthread_mutex_init(&SIM_LOG,NULL);
	pthread_cond_init(&queue_not_empty,NULL);
	//created threads aare joinable.[With respect to POSIX standard]
	//by default POSIX pthreads are joinable
	pthread_create(&tid[0],&attr,(void *)job,(void *)&lpt);	//ready_Q is loaded with processes
	//creating CPU instances
	cpu_t cpu1 = getInstance(1);
	cpu_t cpu2 = getInstance(2);
	cpu_t cpu3 = getInstance(3);
	//creating data binders for each CPU
	cpu_data_bind_t c1_dt;
	cpu_data_bind_t c2_dt;
	cpu_data_bind_t c3_dt;
	//attach data binders for each CPU
	c1_dt.cpu = &cpu1;	//CPU 1
	c1_dt.rq = &rq;

	c2_dt.cpu = &cpu2;	//CPU 2
	c2_dt.rq = &rq;
	
	c3_dt.cpu = &cpu3;	//CPU 3
	c3_dt.rq = &rq;
	//create CPU thread(s)
	pthread_create(&tid[1],&attr,(void *)cpu,(void *)&c1_dt);
	pthread_create(&tid[2],&attr,(void *)cpu,(void *)&c2_dt);
	pthread_create(&tid[3],&attr,(void *)cpu,(void *)&c3_dt);

	int i =0;
	//suspend main thread till all processing is done.
	for (i = 0;i<THREADS;i++)
		pthread_join(tid[i],NULL);

	return 0;				//normal exit status is reported.
}

