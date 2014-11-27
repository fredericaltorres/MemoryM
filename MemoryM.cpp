/*
    MemoryM
    A Simple memory manager for C.

    (C) Torres Frederic 2014
    MIT License

    This library was created for PEBBLE Watch development used with the library
    WinFormebble (https://github.com/fredericaltorres/WinFormebble)

 */

#if !defined(WINFORMEBBLE)
    #include <stdlib.h>
    #include <stdio.h>
    #include <assert.h>
    #include <time.h> 
    #include <string.h>
    #include <stdarg.h>
    #include "darray.h"
    #include "MemoryM.h"
#endif

/*
    snprintf support for Microsoft C compiler
    http://stackoverflow.com/questions/2915672/snprintf-and-visual-studio-2010
*/
#ifdef _MSC_VER

    #define snprintf c99_snprintf

    inline int c99_vsnprintf(char* str, size_t size, const char* format, va_list ap) {

        int count = -1;

        if (size != 0)
            count = _vsnprintf_s(str, size, _TRUNCATE, format, ap);
        if (count == -1)
            count = _vscprintf(format, ap);

        return count;
    }
    inline int c99_snprintf(char* str, size_t size, const char* format, ...) {

        int count;
        va_list ap;

        va_start(ap, format);
        count = c99_vsnprintf(str, size, format, ap);
        va_end(ap);

        return count;
    }

#endif // _MSC_VER

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
void* __newAllocOnly(int size) {

    void * d = malloc(size);
    memset(d, 0, size);
    return d;
}
void* __newAlloc(int size) {

    void * d = __newAllocOnly(size);
    MemoryAllocation_Push(__localMemoryM._memoryAllocation, size, d);
    return d;
}
bool* __newBool() {

    return (bool*)__newAlloc(sizeof(bool));
}
int* __newInt() {

    return (int*)__newAlloc(sizeof(int));
}
char* __newStringX(int size) {

    return (char*)__newAlloc(size + 1);
}
char* __newString(char *s) {

    int size = strlen(s);
    char * newS = __newStringX(size);
    strcpy(newS, s);
    return newS;
}

char* __reNewString(char *s, char* previousAllocation) {

    if (previousAllocation == NULL) {
        return __newString(s);
    }
    else {
        MemoryAllocation * ma = __getMemoryAllocation(previousAllocation);
        if (ma == NULL) {
            return NULL;
        }
        else {
            MemoryAllocation_FreeAllocation(ma);
            int size = strlen(s);
            char * newS = (char*)__newAllocOnly(size + 1);
            strcpy(newS, s);
            ma->size = size;
            ma->data = newS;

            return newS;
        }
    }
}

bool __freeAllocation(void* data) {

    MemoryAllocation* ma = __getMemoryAllocation(data);
    if (ma == NULL)  {
        return false;
    }
    else {
        MemoryAllocation_FreeAllocation(ma);
        return true;
    }
}
//////////////////////////////////////////////////////////////////
/// __format
/// Format and allocate a string following the sprintf format
///     http://www.tutorialspoint.com/c_standard_library/c_function_sprintf.htm
/// 
/// For now there is a limit for the formated string of 1k.
/// Formating padding is not implemented yet.
char * __format(char *format, ...) {

    char * formated = (char*)__newAllocOnly(MEMORYM_MAX_FORMATED_TEMP_STRING_SIZE + 1);
    char tmpBuf[(MEMORYM_MAX_FORMATED_TEMP_STRING_SIZE / 2)+1];
    va_list argptr;
    va_start(argptr, format);

    while(*format != '\0') {

        if (*format == '%') {
            format++;
            if (*format == '%') { // string
                strcat(formated, "%");
            }
            else if (*format == 's') { // string
                char* s = va_arg(argptr, char *); strcat(formated, s);
            }
            else if (*format == 'c') { // character
                char c = (char)va_arg(argptr, int);
                snprintf(tmpBuf, sizeof(tmpBuf), "%c", c); strcat(formated, tmpBuf);
            }
            else if (*format == 'd') { // integer
                int d = va_arg(argptr, int);
                snprintf(tmpBuf, sizeof(tmpBuf), "%d", d); strcat(formated, tmpBuf);
            }
            else if (*format == 'u') { // un signed integer
                unsigned int ui = va_arg(argptr, unsigned int);
                snprintf(tmpBuf, sizeof(tmpBuf), "%u", ui); strcat(formated, tmpBuf);
            }
            else if (*format == 'x') { // un signed integer hexa
                unsigned int ui = va_arg(argptr, unsigned int);
                snprintf(tmpBuf, sizeof(tmpBuf), "%x", ui); strcat(formated, tmpBuf);
            }
            else if (*format == 'X') { // un signed integer hexa uppercase
                unsigned int ui = va_arg(argptr, unsigned int);
                snprintf(tmpBuf, sizeof(tmpBuf), "%X", ui); strcat(formated, tmpBuf);
            }
            else if (*format == 'f') { // float
                double d = va_arg(argptr, double);
                snprintf(tmpBuf, sizeof(tmpBuf), "%f", d); strcat(formated, tmpBuf);
            }
            else if (*format == 'b') { // boolean not standard
                
                //bool d = va_arg(argptr, bool);
                int d = va_arg(argptr, int);
                if (d)
                    strcpy(tmpBuf, MEMORYM_TRUE);
                else
                    strcpy(tmpBuf, MEMORYM_FALSE);
                strcat(formated, tmpBuf);
            }
        }
        else {
            char c = format[0];
            snprintf(tmpBuf, sizeof(tmpBuf), "%c", c);
            strcat(formated, tmpBuf);
        }
        format++;
    }
    va_end(argptr);
    // Allocate a new string for the exact size of the formated result
    char* r = __newString(formated);
    free(formated);
    return r;
}

int __free(int n, ...)
{
    int error = 0;
    va_list vl;
    va_start(vl, n);
    for(int i = 0; i<n; i++) {
        void * a = va_arg(vl, void*);
        bool r = __freeAllocation(a);
        if (!r)
            error += 1;
    }
    va_end(vl);
    return error;
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

    char  buffer2[100]; // TODO: Fix this
    char* buffer = __newStringX(MEMORYM_MAX_REPORT_SIZE);
    int   count  = __getCount();
    for (int i = 0; i <= count; i++) {

        MemoryAllocation* ma = MemoryAllocation_Get(__localMemoryM._memoryAllocation, i);
        snprintf(buffer2, sizeof(buffer2) ,"[%3d] %6d - %X\r\n", i, ma->size, (unsigned int)ma->data);
        strcat(buffer, buffer2);
    }
    snprintf(buffer2, sizeof(buffer2), "Total Used:%d, Count:%d\r\n", memoryM()->GetMemoryUsed(), memoryM()->GetCount());
    strcat(buffer, buffer2);
    return buffer;
}
int __getMemoryUsed() {

    int total = 0;
    int count = __getCount();
    for (int i = 0; i <= count; i++) {

        MemoryAllocation* ma = MemoryAllocation_Get(__localMemoryM._memoryAllocation, i);
        if (ma->data != NULL)
            total += ma->size;
    }
    return total;
}
void __Initialize() {

    __localMemoryM._memoryAllocation  = MemoryAllocation_New();
    __localMemoryM._contextStackIndex = -1;
    __localMemoryM.PushContext(); // Always save a context a 0
}
//////////////////////////////////////////////////////////////////
/// __PushContext
/// 
/// Push in the stack the current state of the memory manager
bool __PushContext() {

    __localMemoryM._contextStackIndex++;

    if (__localMemoryM._contextStackIndex < MEMORYM_STACK_CONTEXT_SIZE) {
        __localMemoryM._contextStack[__localMemoryM._contextStackIndex] = __getCount();
        return true;
    }
    else {
        __localMemoryM._contextStackIndex--;
        return false;
    }
}
//////////////////////////////////////////////////////////////////
/// __PopContext
/// 
/// Restore the state of the memory manager based on the last push
bool __PopContext() {

    if (__localMemoryM._contextStackIndex > -1) {

        int numberOfAllocToPop = __getCount() - __localMemoryM._contextStack[__localMemoryM._contextStackIndex];
        __localMemoryM._contextStackIndex--;

        for(int i = 0; i < numberOfAllocToPop; i++) {
            
            MemoryAllocation* ma = MemoryAllocation_Pop(__localMemoryM._memoryAllocation); // Remove and return the allocation at the end of the array
            MemoryAllocation_FreeAllocation(ma); // Free the allocation
        }
        return true;
    }
    else 
        return false;
}


#if !defined(WINFORMEBBLE)

    void assertString(char *s1, char *s2) {
        assert(!strcmp(s1, s2));
    }

    //////////////////////////////////////////////////////////////////
    /// MemoryM.c 
    /// A simple memory manager for C.
    ///  memoryM() returns singleton object
    void __Samples() {

        bool * b1 = memoryM()->NewBool();
        int  * i1 = memoryM()->NewInt();

        // Allocate string
        char * s1 = memoryM()->NewStringX(10);    
        char * s2 = memoryM()->NewString("Hello World");
    
        // Format and allocate string
        char * s3 = memoryM()->Format("b:%b, b:%b", true, false);
        char * s4 = memoryM()->Format("n:%d, u:%u, x:%x, X:%X", 128, 128, 128, 128);
        char * s5 = memoryM()->Format("s:%s, a:%s", "ok les filles", "Yes");
        char * s6 = memoryM()->Format("c:%c, c:%c", 'A', 'z');
        char * s7 = memoryM()->Format("%d%%", 1);
        char * s8 = memoryM()->NewString("Hello World");
        memoryM()->FreeAllocation(s8); // Just free an allocation

        // Push/Pop memory context and free all allocation after previous Push
        memoryM()->PushContext();

            char * s22   = memoryM()->NewStringX(100);
            char* report = memoryM()->GetReport(); // Get allocation report
            printf(report);

        memoryM()->PopContext(); // Force to free all allocated since previous push

        memoryM()->FreeAll();
    }

    //////////////////////////////////////////////////////////////////
    /// __UnitTests
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
        char * s1 = memoryM()->NewStringX(10);
        char * s2 = memoryM()->NewStringX(100);
        assert(10+11+101 == memoryM()->GetMemoryUsed());

        // Verify allocation of a string with a static string
        char * helloWorld = "Hello World";
        char * s3         = memoryM()->NewString(helloWorld);
        assertString(helloWorld, s3);
        assert(10 + 11 + 101 + 12 == memoryM()->GetMemoryUsed());
    
        // Verify Format()
        assertString("b:true, b:false",          memoryM()->Format("b:%b, b:%b", true, false));
        assertString("n:128, u:128, x:80, X:80", memoryM()->Format("n:%d, u:%u, x:%x, X:%X", 128, 128, 128, 128));
        assertString("s:ok les filles, a:Yes",   memoryM()->Format("s:%s, a:%s", "ok les filles", "Yes"));
        assertString("c:A, c:z",                 memoryM()->Format("c:%c, c:%c", 'A','z'));
        assertString("1%",                       memoryM()->Format("%d%%", 1));
    
        // Verify that FreeAllocation() free the memory
        int a1     = memoryM()->GetMemoryUsed();
        char * s5  = memoryM()->NewString(helloWorld);
        int a2     = memoryM()->GetMemoryUsed();
                     memoryM()->FreeAllocation(s5);
        int a3     = memoryM()->GetMemoryUsed();
        assert(a1 == a3);

        memoryM()->PushContext();

        int v1 = memoryM()->GetCount();
        int m1 = memoryM()->GetMemoryUsed();

        char * s22 = memoryM()->NewStringX(100);
        char* report = memoryM()->GetReport();
        printf(report);
        printf("Total Used:%d, Count:%d\r\n", memoryM()->GetMemoryUsed(), memoryM()->GetCount());

        memoryM()->PopContext(); // Force to free all allocated since previous push

        printf("Total Used:%d, Count:%d\r\n", memoryM()->GetMemoryUsed(), memoryM()->GetCount());

        int v2 = memoryM()->GetCount();
        int m2 = memoryM()->GetMemoryUsed();

        assert(v1 == v2);
        assert(m1 == m2);

        // Go back to Context 0 automatically pushed when the singleton is created
        memoryM()->PopContext(); 
        v2 = memoryM()->GetCount();
        m2 = memoryM()->GetMemoryUsed();
        assert(-1 == v2);
        assert(0 == m2);
        memoryM()->PushContext(); // Re push just in case

        char * testDataString1 = "0123456789";
        char * testDataString2 = "01234567890123456789";

        // Test ReNewString
        char * s33 = NULL;
        s33 = memoryM()->ReNewString(testDataString1, s33);
        assert(11 == memoryM()->GetMemoryUsed());
                
        assertString(testDataString1, s33);
        s33 = memoryM()->ReNewString("01234567890123456789", s33);
        assertString(testDataString2, s33);
        assert(20 == memoryM()->GetMemoryUsed());

        // test Free()
        // Go back to Context 0 automatically pushed when the singleton is created
        memoryM()->PopContext();
        memoryM()->PushContext(); // Re push just in case
        assert(0 == memoryM()->GetMemoryUsed());
        
        s22 = memoryM()->NewString(testDataString1);
        s33 = memoryM()->ReNewString(testDataString1, NULL);
        assert(22 == memoryM()->GetMemoryUsed());

        assert(0 == memoryM()->Free(2, s22, s33));
        assert(1 == memoryM()->Free(1, 4354543));
        assert(00 == memoryM()->GetMemoryUsed());

        memoryM()->FreeAll();
        return true;
    }

#endif

MemoryManager * memoryM() {

    if (__localMemoryM.NewBool == NULL) {

        __localMemoryM.NewBool        = __newBool;
        __localMemoryM.NewInt         = __newInt;
        __localMemoryM.NewString      = __newString;
        __localMemoryM.ReNewString    = __reNewString;
        __localMemoryM.FreeAll        = __freeAll;
        __localMemoryM.GetCount       = __getCount;
        __localMemoryM.NewStringX     = __newStringX;

        __localMemoryM.Format         = __format;
        __localMemoryM.GetReport      = __getReport;
        __localMemoryM.GetMemoryUsed  = __getMemoryUsed;
        __localMemoryM.FreeAllocation = __freeAllocation;
        __localMemoryM.PushContext    = __PushContext;
        __localMemoryM.PopContext     = __PopContext;
        __localMemoryM.Free           = __free;

        #if !defined(WINFORMEBBLE)
            __localMemoryM.UnitTests  = __UnitTests;
        #endif

        __Initialize();
    }
    return &__localMemoryM;
}
