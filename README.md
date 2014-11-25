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
    
    bool __UnitTests() {
    
    // Verify bool allocation
    bool * b1 = memoryM()->NewBool();
    bool * b2 = memoryM()->NewBool();
    assert(2 == memoryM()->GetMemoryUsed());

    // Verify int allocation
    int  * i1 = memoryM()->NewInt();
    int  * i2 = memoryM()->NewInt();
    assert(10 == memoryM()->GetMemoryUsed());

    // Verify string allocation
    char * s1 = memoryM()->NewString(10);
    char * s2 = memoryM()->NewString(100);
    assert(10+11+101 == memoryM()->GetMemoryUsed());

    // Verify allocation of a string with a static string
    char * helloWorld = "Hello World";
    char * s3         = memoryM()->String(helloWorld);
    assertString(helloWorld, s3);
    assert(10 + 11 + 101 + 13 == memoryM()->GetMemoryUsed());
    
    // Verify Format()
    assertString("b:true, b:false",          memoryM()->Format("b:%b, b:%b", true, false));
    assertString("n:128, u:128, x:80, X:80", memoryM()->Format("n:%d, u:%u, x:%x, X:%X", 128, 128, 128, 128));
    assertString("s:ok les filles, a:Yes",   memoryM()->Format("s:%s, a:%s", "ok les filles", "Yes"));
    assertString("c:A, c:z",                 memoryM()->Format("c:%c, c:%c", 'A','z'));
    assertString("1%",                       memoryM()->Format("%d%%", 1));
    
    // Verify that FreeAllocation() free the memory
    int a1     = memoryM()->GetMemoryUsed();
    char * s5  = memoryM()->String(helloWorld);
    int a2     = memoryM()->GetMemoryUsed();
                 memoryM()->FreeAllocation(s5);
    int a3     = memoryM()->GetMemoryUsed();
    assert(a1 == a3);

    memoryM()->PushContext();

    int v1 = memoryM()->GetCount();
    int m1 = memoryM()->GetMemoryUsed();

    char * s22 = memoryM()->NewString(100);
    char* report = memoryM()->GetReport();
    printf(report);
    printf("Total Used:%d, Count:%d\r\n", memoryM()->GetMemoryUsed(), memoryM()->GetCount());

    memoryM()->PopContext(); // Force to free all allocated since previous push

    printf("Total Used:%d, Count:%d\r\n", memoryM()->GetMemoryUsed(), memoryM()->GetCount());

    int v2 = memoryM()->GetCount();
    int m2 = memoryM()->GetMemoryUsed();

    assert(v1 == v2);
    assert(m1 == m2);

    memoryM()->FreeAll();

    return true;
}
	

```