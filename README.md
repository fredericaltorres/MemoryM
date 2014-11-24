MEMORYM
=======

## Overview

A simple memory manager for C to be used in embeded development
like the PEBBLE Watch

## Dependencies

- ***darray*** library
    Dynamic array for C based on darray.h from https://gist.github.com/dce/5187025 - David Eisinger
    Modified by Frederic Torres 2014

## Samples

```C
    bool * b1 = memoryM()->NewBool();
	int  * i1 = memoryM()->NewInt();
	char * s1 = memoryM()->NewString(10);
	char * s3 = memoryM()->String("Hello World");

	printf(memoryM()->GetReport()); // The report buffer is allocated using memoryM.
	printf("Total Used %d", memoryM()->GetMemoryUsed());

	memoryM()->FreeAll();
```