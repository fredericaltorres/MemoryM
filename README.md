MEMORYM
=======

## Overview

A Simple memory manager for C.
This library was created for PEBBLE Watch development to be used with 
the library [WinFormebble](https://github.com/fredericaltorres/WinFormebble)

## Dependencies

- ***darray*** library
    Dynamic array for C based on darray.h from https://gist.github.com/dce/5187025 - David Eisinger
    Modified by Frederic Torres 2014

## License

MIT

## Samples

```C
    
    bool * b1 = memoryM()->NewBool();
	bool * b2 = memoryM()->NewBool();
	int  * i1 = memoryM()->NewInt();
	int  * i2 = memoryM()->NewInt();
	char * s1 = memoryM()->NewString(10);
	char * s2 = memoryM()->NewString(100);
	char * s3 = memoryM()->String("Hello World");

    char* report = memoryM()->GetReport();
	printf(report); 
    // The report buffer is allocated using memoryM.
    // Let's free the report
    memoryM()->FreeAllocation(report); 

	printf("Total Used %d", memoryM()->GetMemoryUsed());

```