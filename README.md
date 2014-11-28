MEMORYM
=======

## Overview

A simple memory manager for C.

The library provides a singleton object that 
- Manage memory allocation for int, bool, string (char*) and Datetime (struct tm).
- Manage formating result
- And more...

See samples below

This library was created for PEBBLE Watch development to be used with the library [WinFormebble](https://github.com/fredericaltorres/WinFormebble)

## Dependencies

- ***darray*** library
    Dynamic array for C based on darray.h from https://gist.github.com/dce/5187025 - David Eisinger
    Modified by Frederic Torres 2014

    This library is already included in the source code

## License

MIT

## Samples

```C

    //////////////////////////////////////////////////////////////////
    /// MemoryM.c 
    /// A simple memory manager for C.
    ///  memoryM() returns singleton object
    void __Samples() {

        bool * b1                    = memoryM()->NewBool();
        int  * i1                    = memoryM()->NewInt();
        struct tm * now              = memoryM()->NewDate();
        struct tm * specificDateTime = memoryM()->NewDateTime(2014, 11, 22, 1, 2, 3);

        // Allocate string
        char * s1 = memoryM()->NewStringLen(10);    
        char * s2 = memoryM()->NewString("Hello World");
        memoryM()->Free(2, s1, s2);
    
        // Format and allocate string
        char * s3 = memoryM()->Format("b:%b, b:%b", true, false);
        char * s4 = memoryM()->Format("n:%d, u:%u, x:%x, X:%X", 128, 128, 128, 128);
        char * s5 = memoryM()->Format("s:%s, a:%s", "ok les filles", "Yes");
        char * s6 = memoryM()->Format("c:%c, c:%c", 'A', 'z');
        char * s7 = memoryM()->Format("%d%%", 1);

        // Format date and time
        char * f1 = memoryM()->FormatDateTime(specificDateTime, "%a - %b %d");
        f1 = memoryM()->ReFormatDateTime(specificDateTime, "%Y-%m-%d", f1);

        char * s8 = memoryM()->NewString("Hello World");
        memoryM()->FreeAllocation(s8); // Free one allocation
        memoryM()->Free(2, s1, s2);    // Free multiple allocation
        
        // Push/Pop memory context and free all allocation after previous Push
        memoryM()->PushContext();

        char * s22 = memoryM()->NewStringLen(100);
            char* report = memoryM()->GetReport(); // Get allocation report
            printf(report);

        memoryM()->PopContext(); // Force to free all allocated since previous push

        memoryM()->FreeAll();
    }


```