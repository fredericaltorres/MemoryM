#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <time.h> 
#include <string.h>

#include "darray.h"
#include "MemoryM.h"

/* ============== MemoryM  ==================

A memory manager for C
*/


DArray*				MemoryAllocation_New()                                              { return darray_init(); }
void				MemoryAllocation_Push(DArray *array, int size, void *data)			{
	MemoryAllocation* ma = (MemoryAllocation*)malloc(sizeof(MemoryAllocation));
	ma->data = data;
	ma->size = size;
	MemoryAllocation_PushA(array, ma);
}
void				MemoryAllocation_PushA(DArray *array, MemoryAllocation *s)          { darray_push(array, s); }
MemoryAllocation*	MemoryAllocation_Pop(DArray *array)                                 { return (MemoryAllocation *)darray_pop(array); }
MemoryAllocation*	MemoryAllocation_Get(DArray *array, int index)                      { return (MemoryAllocation *)darray_get(array, index); }
void				MemoryAllocation_Set(DArray *array, int index, MemoryAllocation *s) { darray_set(array, index, s); }
void				MemoryAllocation_Destructor(DArray *array)                          { darray_free(array); }
int					MemoryAllocation_GetLength(DArray *array)                           { return array->last; }

MEMORYM_CLASS __localMemoryM; // Single instance allocated






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
char* __String(char *s) {

	int size = strlen(s);
	char * newS = __newString(size + 1);
	strcpy(newS, s);
	return newS;
}
void __freeAll() {

	// Free all registered memory allocation
	int count = __getCount();
	for (int i = 0; i <= count; i++) {

		MemoryAllocation* ma = MemoryAllocation_Get(__localMemoryM._memoryAllocation, i);
		free(ma->data);
	}
	// Free the MemoryAllocation dynamic array
	MemoryAllocation_Destructor(__localMemoryM._memoryAllocation);
}
char * __GetReport() {

	char  buffer2[100];
	char* buffer = __newString(1024 * 4);
	int   count = __getCount();
	for (int i = 0; i <= count; i++) {

		MemoryAllocation* ma = MemoryAllocation_Get(__localMemoryM._memoryAllocation, i);
		sprintf(buffer2, "[%d] %d - %d\r\n", i, ma->size, ma->data);
		strcat(buffer, buffer2);
	}
	return buffer;
}
int __GetMemoryUsed() {

	int total = 0;
	int count = __getCount();
	for (int i = 0; i <= count; i++) {

		MemoryAllocation* ma = MemoryAllocation_Get(__localMemoryM._memoryAllocation, i);
		total += ma->size;
	}
	return total;
}
void __Initialize() {

	__localMemoryM._memoryAllocation = MemoryAllocation_New();
}
MEMORYM memoryM() {

	if (__localMemoryM.NewBool == NULL) {

		__localMemoryM.NewBool = __newBool;
		__localMemoryM.NewInt = __newInt;
		__localMemoryM.NewString = __newString;
		__localMemoryM.FreeAll = __freeAll;
		__localMemoryM.GetCount = __getCount;
		__localMemoryM.String = __String;
		__localMemoryM.GetReport = __GetReport;
		__localMemoryM.GetMemoryUsed = __GetMemoryUsed;

		__Initialize();
	}
	return &__localMemoryM;
}


