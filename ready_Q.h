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

//ready_Q.h
	
//preprocessor commands
#include "process.h"
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include "file_manager.h"

//GLOBAL VARIABLE DEFINITIONS
#define MAX_Q 1000

//type structures definitions

typedef struct 		//ready_Q data structure definition
{
	proc_t q[MAX_Q];
	int stat;
	int curr_ptr;
	int proc_count;
	pthread_mutex_t mutex;
}ready_Q;

typedef struct	      //parameter data structure definition
{
	ready_Q * readyq;
	int w;
}load_proc_t;

//data structure functionalities

void init_q(ready_Q *rq)
{	
	rq->stat = 0;
	rq->curr_ptr = -1;
	rq->proc_count = 0;
}
//Queue push
int push(ready_Q * Queue, proc_t pr)
{
	if (Queue->curr_ptr == MAX_Q - 1){
		Queue->stat = 1;
		return -1;
	}else{
		Queue->curr_ptr++;
		Queue->q[Queue->curr_ptr] = pr;
		Queue->proc_count++;
		return 0;
	}
}
proc_t * peek(ready_Q * Queue)
{
	if (Queue->curr_ptr == 0){
		Queue->stat = 0;
		return NULL;
	}else{
		proc_t *p = &Queue->q[Queue->curr_ptr];
		return p;
	}
}
//Queue pop
proc_t * pop(ready_Q * Queue)
{ 
	if (Queue->curr_ptr == -1){
		Queue->stat = 0;
		return NULL;
	}else{
		proc_t *p = &Queue->q[Queue->curr_ptr];
		Queue->curr_ptr--;
		Queue->proc_count--;
		return p;
	}
}
int isQEmpty(ready_Q * Queue)
{
	if (Queue->stat == 0)
		return 0;
	else
		return -1;
}
int isQFull(ready_Q * Queue)
{
	if (Queue->stat == 1)
		return 0;
	else
		return -1;
}
//processes loading to ready queue functionality
void load_proc(load_proc_t *t,pthread_cond_t *condition)
{
	ready_Q *rq = t->readyq;
	int wait = t->w;	
	int state;
	while(1 == 1){
		int lastread;
		FILE *jobLog = fopen("job_file.sys","r");
		if (jobLog != NULL)
		{
			char dta[6];
			int i=0;
			while (1 == 1)
			{
				sleep(wait);
				if (rq->proc_count == MAX_Q)
					break;
				fgets(dta,10,jobLog);
				char *delimiters = ":\n";
				char *str[4];
				str[0] = strtok(dta,delimiters);
				if (str[0] == NULL){
					pthread_cond_signal(condition);
					break;
				}
				int a=0;
				for (a =1 ;a<3;a++)
					str[a] = strtok(NULL,delimiters);
				if (str[1] == NULL){
					pthread_cond_signal(condition);
					if (rq->curr_ptr == -1){
						printf("\n\t------\n\tExitting thread\n\tReady Queue is empty\n\t------\n");
						return;
					}
					break;
				}
				int _id = atoi(str[0]);
				int _burst = atoi(str[1]);
				proc_t tp = creatProcess(_id,_burst);
				time_t lt;
				time_t _t = time(&lt);
				struct tm *temp = localtime(&_t);
				tp.arrival_t = *temp;
				int push_stat;
				pthread_mutex_lock(&rq->mutex);
				push_stat = push(rq,tp);
				printf("Last loaded process id : %d\tburst time : %d\t ready queue element : %d\n",_id,_burst,rq->proc_count);
				int k = addQueueEntry(_id,_burst);
				pthread_cond_signal(condition);
				pthread_mutex_unlock(&rq->mutex);	
				lastread = _id;	
			}
			fclose(jobLog);
			int k = modify_jobfile(lastread);			
		}else{
			break;
		}
		pthread_cond_signal(condition);
		pthread_mutex_unlock(&rq->mutex);
	}
}
