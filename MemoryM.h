/*
MemoryM
A Simple memory manager for C.

(C) Torres Frederic 2014
MIT License

This library was created for PEBBLE Watch development used with the library
WinFormebble (https://github.com/fredericaltorres/WinFormebble)

*/
#ifndef _MEMORYM_H_
#define _MEMORYM_H_

#if !defined(WINFORMEBBLE)

    #include <stdlib.h>
    #include <stdio.h>
    #include <assert.h>
    #include <time.h> 
    #include <string.h>
    #include "darray.h"

#endif

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

    DArray*             MemoryAllocation_New       ();
    void                MemoryAllocation_PushA     (DArray *array, MemoryAllocation *s);
    void                MemoryAllocation_Push      (DArray *array, int size, void *data);
    MemoryAllocation*   MemoryAllocation_Pop       (DArray *array);
    MemoryAllocation*   MemoryAllocation_Get       (DArray *array, int index);
    void                MemoryAllocation_Set       (DArray *array, int index, MemoryAllocation *s);
    void                MemoryAllocation_Destructor(DArray *array);
    int                 MemoryAllocation_GetLength (DArray *array);

    void MemoryAllocation_FreeAllocation(MemoryAllocation *a);

    typedef struct {

        DArray*	_memoryAllocation;
        int _contextStack[MEMORYM_STACK_CONTEXT_SIZE];
        int _contextStackIndex;

        // Allocate a new boolean
        bool*(*NewBool)();
        // Allocate a new int
        int *(*NewInt)();
        // Allocate a new string for len size (do not add the extra char for the \0)
        char*(*NewStringLen)(int size);
        // Allocate a new string identical to the string passed
        char*(*NewString)(char* s);
        // Re allocate a new string identical to the string passed, but re use the internal MemoryAllocation object
        char*(*ReNewString)(char* s, char* previousAllocation);
        // Allocate a new DateTime set to now
        struct tm *(*NewDate)();
        // Allocate a new DateTime set to a specific date
        struct tm *(*NewDateTime)(int year, int month, int day, int hour, int minutes, int seconds);

        // Format using sprintf, but return a string allocated by MemoryM
        char*(*Format)(char* s, ...);
        // Format the Date using strftime(), but return a string allocated by MemoryM
        char*(*FormatDateTime)(struct tm *date, char* format);
        // Re allocate and re format the Date using strftime(), but re use the internal MemoryAllocation object
        char*(*ReFormatDateTime)(struct tm *date, char* format, char * previousAllocation);

        // Free a specific allocation
        bool(*FreeAllocation)(void* data);
        // Free multiple specific allocation
        int(*Free)(int n, ...);

        // Return a string allocated by MemoryM, presenting the current memory allocation
        char*(*GetReport)();
        // Return how many total byte are allocated
        int  (*GetMemoryUsed)();
        // Free all
        void (*FreeAll)();
        // Return the total number of allocation created
        int  (*GetCount)();
        
        // Mark the state of the memory manager
        bool(*PushContext)();
        // Restore the state of the memory manager to the previous Push
        bool(*PopContext)();

        bool(*UnitTests)();

    } MemoryManager;

    // Function that return the sigleton instance
    MemoryManager* memoryM(); 

    #endif