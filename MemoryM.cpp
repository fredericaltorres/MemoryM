/*
    MemoryM
    A Simple memory manager for C.

    (C) Torres Frederic 2014
    MIT License

    This library was created for PEBBLE Watch development used with the library
    WinFormebble (https://github.com/fredericaltorres/WinFormebble)

 */
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <time.h> 
#include <string.h>
#include <stdarg.h>

#include "darray.h"
#include "MemoryM.h"

// First a dynamic array of MemoryAllocation

DArray*             MemoryAllocation_New()                                              { return darray_init(); }
void                MemoryAllocation_PushA(DArray *array, MemoryAllocation *s)          { darray_push(array, s); }
MemoryAllocation*   MemoryAllocation_Pop(DArray *array)                                 { return (MemoryAllocation *)darray_pop(array); }
MemoryAllocation*   MemoryAllocation_Get(DArray *array, int index)                      { return (MemoryAllocation *)darray_get(array, index); }
void                MemoryAllocation_Set(DArray *array, int index, MemoryAllocation *s) { darray_set(array, index, s); }
void                MemoryAllocation_Destructor(DArray *array)                          { darray_free(array); }
int                 MemoryAllocation_GetLength(DArray *array)                           { return array->last; }

void MemoryAllocation_FreeAllocation(MemoryAllocation *a) {  

    if (a->data != NULL) {
        free(a->data);
        a->data = NULL;
    }
}

void MemoryAllocation_Push(DArray *array, int size, void *data) {

    MemoryAllocation* ma = (MemoryAllocation*)malloc(sizeof(MemoryAllocation));
    ma->data             = data;
    ma->size             = size;
    MemoryAllocation_PushA(array, ma);
}

// Second the MemoryManager Class

MemoryManager __localMemoryM; // Single instance allocated

int __getCount() {

    return MemoryAllocation_GetLength(__localMemoryM._memoryAllocation);
}
void* __newAlloc(int size) {

    void * d = malloc(size);
    memset(d, 0, size);
    MemoryAllocation_Push(__localMemoryM._memoryAllocation, size, d);
    return d;
}
bool* __newBool() {

    return (bool*)__newAlloc(sizeof(bool));
}
int* __newInt() {

    return (int*)__newAlloc(sizeof(int));
}
char* __newString(int size) {

    return (char*)__newAlloc(size + 1);
}
char* __string(char *s) {

    int size = strlen(s);
    char * newS = __newString(size + 1);
    strcpy(newS, s);
    return newS;
}
MemoryAllocation* __getMemoryAllocation(void* data) {

    int count = __getCount();
    for (int i = 0; i <= count; i++) {

        MemoryAllocation * ma = MemoryAllocation_Get(__localMemoryM._memoryAllocation, i);
        if (ma->data == data) {
            return ma;
        }
    }
    return NULL;
}
bool __freeAllocation(void* data) {

    MemoryAllocation* ma = __getMemoryAllocation(data);
    if (ma != NULL) {
        MemoryAllocation_FreeAllocation(ma);
    }
    return true;
}

// http://www.tutorialspoint.com/c_standard_library/c_function_sprintf.htm

char * __format(char *format, ...) {

    char * formated = __newString(MEMORYM_MAX_FORMATED_STRING_SIZE);
    char tmpBuf[MEMORYM_MAX_FORMATED_STRING_SIZE / 2];
    va_list argptr;
    va_start(argptr, format);

    while(*format != '\0') {

        if (*format == '%') {
            *format++;
            if (*format == 's') { // string
                char* s = va_arg(argptr, char *);
                strcat(formated, s);
            }
            else if (*format == 'c') { // character
                char c = (char)va_arg(argptr, int);
                sprintf(tmpBuf, "%c", c);
                strcat(formated, tmpBuf);
            }
            else if (*format == 'd') { // integer
                int d = va_arg(argptr, int);
                sprintf(tmpBuf, "%d", d);
                strcat(formated, tmpBuf);
            }
            else if (*format == 'f') { // float
                float d = va_arg(argptr, float);
                sprintf(tmpBuf, "%f", d);
                strcat(formated, tmpBuf);
            }
            else if (*format == 'b') { // boolean not standard
                bool d = va_arg(argptr, bool);
                if (d)
                    strcpy(tmpBuf, MEMORYM_TRUE);
                else
                    strcpy(tmpBuf, MEMORYM_FALSE);
                strcat(formated, tmpBuf);
            }
        }
        else {
            char c = format[0];
            sprintf(tmpBuf, "%c", c);
            strcat(formated, tmpBuf);
        }
        *format++;
    }
    va_end(argptr);
    // Allocate a new string for the exact size of the formated result
    char* r = __string(formated);
    __freeAllocation(formated);
    return r;
}



void __freeAll() {

    // Free all registered memory allocation first
    int count = __getCount();
    for (int i = 0; i <= count; i++) {
        
        MemoryAllocation_FreeAllocation(MemoryAllocation_Get(__localMemoryM._memoryAllocation, i));
    }
    // Free the MemoryAllocation dynamic array
    MemoryAllocation_Destructor(__localMemoryM._memoryAllocation);
}
char * __getReport() {

    char  buffer2[100];
    char* buffer = __newString(1024 * 4);
    int   count  = __getCount();
    for (int i = 0; i <= count; i++) {

        MemoryAllocation* ma = MemoryAllocation_Get(__localMemoryM._memoryAllocation, i);
        sprintf(buffer2, "[%3d] %6d - %X\r\n", i, ma->size, ma->data);
        strcat(buffer, buffer2);
    }
    return buffer;
}
int __getMemoryUsed() {

    int total = 0;
    int count = __getCount();
    for (int i = 0; i <= count; i++) {

        MemoryAllocation* ma = MemoryAllocation_Get(__localMemoryM._memoryAllocation, i);
        total               += ma->size;
    }
    return total;
}
void __Initialize() {

    __localMemoryM._memoryAllocation = MemoryAllocation_New();
}
MemoryManager * memoryM() {

    if (__localMemoryM.NewBool == NULL) {

        __localMemoryM.NewBool        = __newBool;
        __localMemoryM.NewInt         = __newInt;
        __localMemoryM.NewString      = __newString;
        __localMemoryM.FreeAll        = __freeAll;
        __localMemoryM.GetCount       = __getCount;
        __localMemoryM.String         = __string;
        __localMemoryM.Format         = __format;
        __localMemoryM.GetReport      = __getReport;
        __localMemoryM.GetMemoryUsed  = __getMemoryUsed;
        __localMemoryM.FreeAllocation = __freeAllocation;;

        __Initialize();
    }
    return &__localMemoryM;
}
