MEMORYM
=======

## Overview

MemoryM.c

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

void __Samples() {

    bool * b1 = memoryM()->NewBool();
    int  * i1 = memoryM()->NewInt();

    // Allocate string
    char * s1 = memoryM()->NewString(10);    
    char * s2 = memoryM()->String("Hello World");
    
    // Format and allocate string
    char * s3 = memoryM()->Format("b:%b, b:%b", true, false);
    char * s4 = memoryM()->Format("n:%d, u:%u, x:%x, X:%X", 128, 128, 128, 128);
    char * s5 = memoryM()->Format("s:%s, a:%s", "ok les filles", "Yes");
    char * s6 = memoryM()->Format("c:%c, c:%c", 'A', 'z');
    char * s7 = memoryM()->Format("%d%%", 1);
    char * s8 = memoryM()->String("Hello World");
    memoryM()->FreeAllocation(s8); // Just free an allocation

    // Push/Pop memory context and free all allocations after previous Push
    memoryM()->PushContext();

        char * s22   = memoryM()->NewString(100);
        char* report = memoryM()->GetReport(); // Get allocation report
        printf(report);

    memoryM()->PopContext(); // Free all allocation since previous push

    memoryM()->FreeAll();
}    
	

```