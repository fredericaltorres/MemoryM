#ifndef _MEMORYM_H_
#define _MEMORYM_H_

    #include <stdlib.h>
    #include <stdio.h>
    #include <assert.h>
    #include <time.h> 
    #include <string.h>
    #include "darray.h"

    /* ============== MemoryM  ==================

    A memory manager for C
    */

    // First implement a dynamic array to store all allocation
    typedef struct {

	    int size;
	    void * data;
    } MemoryAllocation;

    DArray*				MemoryAllocation_New       ();
    void				MemoryAllocation_PushA     (DArray *array, MemoryAllocation *s);
    void				MemoryAllocation_Push      (DArray *array, int size, void *data);
    MemoryAllocation*	MemoryAllocation_Pop       (DArray *array);
    MemoryAllocation*	MemoryAllocation_Get       (DArray *array, int index);
    void                MemoryAllocation_Set       (DArray *array, int index, MemoryAllocation *s);
    void                MemoryAllocation_Destructor(DArray *array);
    int                 MemoryAllocation_GetLength (DArray *array);




    typedef struct {

        DArray*	_memoryAllocation;

        bool*(*NewBool      )();
        int* (*NewInt       )();
        char*(*NewString    )(int size);
        char*(*String       )(char* s);
        char*(*GetReport    )();
        int  (*GetMemoryUsed)();
        void (*FreeAll      )();
        int  (*GetCount     )();

    } MemoryManager;

    MemoryManager* memoryM(); // Function that return the instance

    #endif