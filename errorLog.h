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

//errorlog.h
#include <stdio.h>

//ERROR DEFINITIONS
void systemErr(){
	printf("\n--Error--\n\tInternal Error :\n\tError Code : 0x00\n\tError stat : sytem error\n\t-- Unhandled exception.\n\tpleaseBug report.\n");
}	//report an unhandled exception
void jobfileErr(){
	printf("\n--Error--\n\tInternal Error :\n\tError Code : 0x01\n\tError stat : invalid job_file.sys\n\t-- posible causes : \n\t\t1. corrupted job_file.sys. \n\t\t2.system error 0x00\n");
}	//report job_file.sys reading error
void argumentErr(){
	printf("\n--Error--\n\tExternal Error :\n\tError Code : 0x03\n\tError stat : invalid number of arguments.\n\t-- posible causes : \n\t\t1. Usage : scheduler <t-seconds> \n");
}	//reporting usage error
void argumentValueErr(){
	printf("\n--Error--\n\tExternal Error :\n\tError Code : 0x04\n\tError stat : Invalid argument\n\t-- posible causes : \n\t\t1. Usage : scheduler <t-seconds>.\nThe specified <t-seconds> is not an integer\n");
}	//reporting intentional usage bug.
void simulationlogErr(){
	printf("\n--Error--\n\tInternal Error :\n\tError Code : 0x05\n\tError stat : invalid simulationLog.sys\n\t-- posible causes : \n\t\t1. corrupted simulationLog.sys \n\t\t2.permission denied.\n\t\t3.memory error not enough memory\n\t\t4.system error 0x00\n");
}

void simulationInterruptErr(){
	printf("\n--Error--\n\tExternal Error :\n\tError Code : 0x06\n\tError stat : Keyboard INTERRUPT ERROR\n\t-- posible causes : \n\t\t1.Insufficient number of processes simulated.\n\t\t2.system error 0x00\n\n");
}
	
//ERROR reporting function
void errorReport(int err_id)
{
	switch(err_id)
	{
		case 0 : systemErr();
				break;
		case 1 : jobfileErr();
				break;
		case 3 : argumentErr();
				break;
		case 4 : argumentValueErr();
				break; 
		case 5 : simulationlogErr();
				break;
		case 6 : simulationInterruptErr();
				break;
		default : systemErr();
				break;
	}
}


