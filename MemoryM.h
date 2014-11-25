#ifndef _MEMORYM_H_
#define _MEMORYM_H_

    #include <stdlib.h>
    #include <stdio.h>
    #include <assert.h>
    #include <time.h> 
    #include <string.h>
    #include "darray.h"

#define MEMORYM_MAX_FORMATED_TEMP_STRING_SIZE 1024
#define MEMORYM_MAX_REPORT_SIZE (MEMORYM_MAX_FORMATED_TEMP_STRING_SIZE*4)
#define MEMORYM_TRUE "true"
#define MEMORYM_FALSE "false"
#define MEMORYM_STACK_CONTEXT_SIZE 4

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

    void MemoryAllocation_FreeAllocation(MemoryAllocation *a);


    typedef struct {

        DArray*	_memoryAllocation;
        int _contextStack[MEMORYM_STACK_CONTEXT_SIZE];
        int _contextStackIndex;

        bool*(*NewBool       )();
        int *(*NewInt        )();
        char*(*NewString     )(int size);
        char*(*String        )(char* s);
        char*(*Format        )(char* s, ...);
        char*(*GetReport     )();
        int  (*GetMemoryUsed )();
        void (*FreeAll       )();
        int  (*GetCount      )();
        bool (*FreeAllocation)(void* data);

        bool(*PushContext)();
        bool(*PopContext)();

        bool(*UnitTests)();

    } MemoryManager;

    MemoryManager* memoryM(); // Function that return the instance


    char* __format(char *format, ...);

    #endif