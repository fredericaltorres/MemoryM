// MemoryM.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <time.h> 
#include <string.h>
#include "darray.h"
#include "MemoryM.h"

#include<stdarg.h>


int sum(int num_args, ...)
{
    int val = 0;
    va_list ap;
    int i;

    va_start(ap, num_args);
    for (i = 0; i < num_args; i++)
    {
        val += va_arg(ap, int);
    }
    va_end(ap);

    return val;
}


int _tmain(int argc, _TCHAR* argv[])
{

    printf("Sum of 10, 20 and 30 = %d\n", sum(3, 10, 20, 30));

	bool * b1 = memoryM()->NewBool();
	bool * b2 = memoryM()->NewBool();
	int  * i1 = memoryM()->NewInt();
	int  * i2 = memoryM()->NewInt();
	char * s1 = memoryM()->NewString(10);
	char * s2 = memoryM()->NewString(100);
	char * s3 = memoryM()->String("Hello World");

    char * s4 = memoryM()->Format("Hello b:%b, Number:%d, s:%s, c:%c, b:%b \r\n", true, 10, "ok les filles", 'A', false);
    printf(s4);

    char* report = memoryM()->GetReport();
	printf(report); 
    // The report buffer is allocated using memoryM. Let's free the report
    memoryM()->FreeAllocation(report); 

	printf("Total Used %d", memoryM()->GetMemoryUsed());

    

	memoryM()->FreeAll();
	
	return 0;
}

