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

//file_manager.h
#include "auth.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define NUMBER_OF_PROCESSES 501
#define MAX_BURST_TIME 51

typedef struct
{
	int _nProc;
	int _turn;
	int _wait;
}report_f;

//automating the creation of the job_file.sys file.
int createJobFile()
{	
	int i,Num_jobs=0,j_id,j_bt=0;	
	while(!(Num_jobs > 50)){
		Num_jobs = random() % NUMBER_OF_PROCESSES;		//Creating a random number for number of jobs
	}
	FILE *jobfile_t = fopen("job_file.sys","r");	//open file to check if it exists
	if (jobfile_t != NULL)
	{
		fclose(jobfile_t);
		remove("job_file.sys");			//delete the file if it exists
	}
	FILE *jobfile = fopen("job_file.sys","a+");	//create the file in append mode
	if (jobfile == NULL){
		errorReport(2);				//see errorReport(2) in errorLog.h
		return -1;
	}
	if (authenticate() == 1333)
	for(i = 0; i< Num_jobs;i++)			//create random values for process busrt times
	{
		j_bt = 0;
		j_id = i;
		j_bt = random() % MAX_BURST_TIME;
		fprintf(jobfile,"%d:%d\n", j_id , j_bt);	//write process id & burst time to the file seperated by a ':'
	
	}
	fclose(jobfile);				//close the file descriptor after completing the job
	return 0;
	
}

//Adding the CPU service entry to the simulation_log file.
int add_CPUSERVICE_simulationEntry(int cpu_id,int proc_ID,struct tm proc_arr,struct tm *cpu_serv)
{
	FILE *sm_log = fopen("simulation_log.sys","a+");
	if (sm_log != NULL)			// if file open was successfull
	{
		int phrs=0,pmin=0,psec=0,shrs=0,smin=0,ssec=0;
		phrs = proc_arr.tm_hour;
		pmin = proc_arr.tm_min;
		psec = proc_arr.tm_sec;
		shrs = cpu_serv->tm_hour;
		smin = cpu_serv->tm_min;
		ssec = cpu_serv->tm_sec;
		printf("process[%d] arrival time : %d:%d:%d\tprocess[%d] service time : %d:%d:%d\n",proc_ID,phrs,pmin,psec,proc_ID,shrs,smin,ssec);
		fprintf(sm_log,"\nStatistics for CPU - %d\nProcess ID : %d\nArrival time : %d:%d:%d\nService time %d:%d:%d\n",cpu_id,proc_ID,phrs,pmin,psec,shrs,smin,ssec);
		fclose(sm_log);			//close file descriptor after complete writting.
		return 0;
	}
	return 1;
}
//Adding the CPU completed entry to the simulation_log file.
int add_CPUCOMPLETE_simulationEntry(int cpu_id,int proc_ID,struct tm proc_arr)
{
	FILE *sm_log = fopen("simulation_log.sys","a+");
	if (sm_log == NULL){
		errorReport(5);		
	}	
	if (sm_log != NULL)
	{
		int phrs=0,pmin=0,psec=0,chrs=0,cmin=0,csec=0;
		phrs = proc_arr.tm_hour;
		pmin = proc_arr.tm_min;
		psec = proc_arr.tm_sec;
		
		time_t t;
		time_t tx = time(&t);
		struct tm *cpu_comp = localtime(&tx);	//creating the tm data structure with current date and time.
		
		chrs = cpu_comp->tm_hour;
		cmin = cpu_comp->tm_min;
		csec = cpu_comp->tm_sec;
		fprintf(sm_log,"\nStatistics for CPU - %d\nProcess ID : %d\nArrival time : %d:%d:%d\nCompletion time %d:%d:%d\n",cpu_id,proc_ID,phrs,pmin,psec,chrs,cmin,csec);
		fclose(sm_log);				
		return 0;
	}
	return 1;
}

int addQueueEntry(int jobID, int burst)
{
	FILE *sm_log = fopen("simulation_log.sys","a+");
	if (sm_log == NULL){
		errorReport(5);		
	}	
	
	if (sm_log != NULL)
	{
		int hrs,min,sec;
		time_t t;
		time_t tx = time(&t);
		struct tm *cpu_comp = localtime(&tx);	//creating the tm data structure with current date and time.
		hrs = cpu_comp->tm_hour;
		min = cpu_comp->tm_min;
		sec = cpu_comp->tm_sec;
		fprintf(sm_log,"\n--------------------------------\n%d : %d \nArrivalTime : %d:%d:%d\n--------------------------------\n",jobID,burst,hrs,min,sec);
		fclose(sm_log);				
		return 0;
	}
	return 1;

}
//used to recreate the job_file.sys with values removed
int modify_jobfile(int lastRead)
{
	FILE *jobLog = fopen("job_file.sys","r");
	if (jobLog == NULL){
		errorReport(1);
		return -1;
	}
	char dta[6];
	int exit = 0,ID[500],BT[500],i=0,_burst=-1,_id=-1,check;
	fpos_t pos;
	while (exit == 0){       
	        fgets(dta,10,jobLog);
		check = feof(jobLog);
		if (check != 0 )
			break;
        	char *delimiters = ":\n";
	        char *str[4];
        	str[0] = strtok(dta,delimiters);
	       	int a=0;
                for (a =1 ;a<3;a++)
                	str[a] = strtok(NULL,delimiters);
		_id = atoi(str[0]);
		_burst = atoi(str[1]);    
        	if (_id >= lastRead+1){
			ID[i] = _id;
			BT[i] = _burst;
			i++;
		}
	}
	fclose(jobLog);
	remove("job_file.sys");
	FILE * make = fopen("job_file.sys","w");
	int j = 0;
	while (j < i){
		fprintf(make,"%d:%d \n",ID[j],BT[j]);
		j++;
	}
	fclose(make);
	return 0;
}
int readJobFile()
{
	FILE * jb = fopen("job_file.sys","r");
	if (jb == NULL){
		errorReport(2);
		return -1;		
	}
	char buff[10],cmp[10];
	printf("\njob_file.sys output : \n");
	while (1==1){
		strcpy(cmp,buff);
		fgets(buff,10,jb);
			if (strcmp(buff,cmp) == 0)
				break;
		printf("%s",buff);
		
	}
	printf("\nEOF reached\n\n");
}

int finalReport(report_f *fg)
{
	double __AVGturn = fg->_turn / fg->_nProc;
	double __AVGwait = fg->_wait / fg->_nProc;
	FILE *sm_log = fopen("simulation_log.sys","a");
	fprintf(sm_log,"\nNumber of completed processes : %d\nAverage waiting time : %2f seconds\nAverage turnaround time : %2f seconds\n",fg->_nProc,__AVGwait,__AVGturn);
	printf("\nNumber of completed processes : %d\nAverage waiting time : %2f seconds\nAverage turnaround time : %2f seconds\n",fg->_nProc,__AVGwait,__AVGturn);
	fclose(sm_log);
}
void resetSimulation()
{
	//printf("\n\n-----\n\tRemoving previous simulation logs...\n-----\n\n"); //Testing
	FILE * tp;
	tp = fopen("simulation_log.sys","r");
	if (tp != NULL)
	{
		tp = NULL;
		remove("simulation_log.sys");
	}
	tp = fopen("job_file.sys","r");
	if (tp != NULL)
	{
		tp = NULL;
		remove("job_log.sys");
	}
}
