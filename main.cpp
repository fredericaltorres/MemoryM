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


int _tmain(int argc, _TCHAR* argv[])
{
    memoryM()->UnitTests();
    memoryM()->FreeAll();
	return 0;
}

