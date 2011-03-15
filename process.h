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

//process.h

//preprocessor commands
#include<time.h>		//include time structures into the system
#include "errorLog.h"		//include error reporting subsystem

//type definitions
typedef struct			//defining type proc_t
{
	int proc_ID;
	int burst_t;
	struct tm arrival_t;
}proc_t;

//type functionalities

proc_t creatProcess(int id, int b_time)	//proc_t variable creating function
{
	proc_t pr;
	pr.proc_ID = id;
	pr.burst_t = b_time;
	return pr;
}

void setArrival(proc_t *pr, struct tm arr)	//set method for arrival time
{
	pr->arrival_t = arr;
}
