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


int _tmain(int argc, _TCHAR* argv[])
{
	bool * b1 = memoryM()->NewBool();
	bool * b2 = memoryM()->NewBool();
	int  * i1 = memoryM()->NewInt();
	int  * i2 = memoryM()->NewInt();
	char * s1 = memoryM()->NewString(10);
	char * s2 = memoryM()->NewString(100);
	char * s3 = memoryM()->String("Hello World");

	printf(memoryM()->GetReport()); // The report buffer is allocated using memoryM.
	printf("Total Used %d", memoryM()->GetMemoryUsed());

	memoryM()->FreeAll();
	
	return 0;
}

