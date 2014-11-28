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
char* __newStringLen(int size) {

    return (char*)__newAlloc(size + 1);
}
char* __newString(char *s) {

    int size = strlen(s);
    char * newS = __newStringLen(size);
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
            int size    = strlen(s);
            char * newS = (char*)__newAllocOnly(size+1);
            strcpy(newS, s);
            ma->size    = size+1;
            ma->data    = newS;
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
    char* buffer = __newStringLen(MEMORYM_MAX_REPORT_SIZE);
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
struct tm * __newDate() {

    time_t temp            = time(NULL);  // Get a tm structure -- not re entrant
    struct tm * tick_time  = localtime(&temp);
    struct tm * tick_time2 = (struct tm *)__newAlloc(sizeof(struct tm));

    memcpy(tick_time2, tick_time, sizeof(struct tm));
    return tick_time2;
}

struct tm * __newDateTime(int year, int month, int day, int hour, int minutes, int seconds) {

    struct tm * d = __newDate();

    d->tm_sec    = seconds;
    d->tm_min    = minutes;
    d->tm_hour   = hour;
    d->tm_min    = minutes;
    d->tm_mday   = day;
    d->tm_mon    = month - 1;
    d->tm_year   = year - 1900;
    //d->tm_wday = ?;
    //d->tm_yday = ?;
    d->tm_isdst  = 0;

    return d;
}

static char MemoryM_Buffer[32];

char* __formatDateTime(struct tm *date, char* format) {

    strftime(MemoryM_Buffer, sizeof(MemoryM_Buffer), format, date);
    return __newString(MemoryM_Buffer);
}

char* __reFormatDateTime(struct tm *date, char* format, char * previousAllocation) {
    if (previousAllocation == NULL) {
        return __formatDateTime(date, format);
    }
    else {
        MemoryAllocation * ma = __getMemoryAllocation(previousAllocation);
        if (ma == NULL) {
            return NULL;
        }
        else {
            MemoryAllocation_FreeAllocation(ma);
            strftime(MemoryM_Buffer, sizeof(MemoryM_Buffer), format, date);
            int size    = strlen(MemoryM_Buffer);
            char * newS = (char*)__newAllocOnly(size + 1);
            strcpy(newS, MemoryM_Buffer);
            ma->size    = size + 1;
            ma->data    = newS;
            return newS;
        }

    }
}
#if !defined(WINFORMEBBLE)

    void assertString(char *s1, char *s2) {
        assert(!strcmp(s1, s2));
    }
    void assertDate(struct tm * date, int year, int month, int day, int hour, int minutes, int seconds) {

        assert((year - 1900) == date->tm_year);
        assert(month         == date->tm_mon + 1); // Month start a 0
        assert(day           == date->tm_mday);
        assert(hour          == date->tm_hour);
        assert(minutes       == date->tm_min);
        assert(seconds       == date->tm_sec);
    }

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

    // *************************************************
    // UnitTests
    // *************************************************

    bool __UnitTests_Basic() {

        memoryM()->PopContext(); // Restore memory to initialization state
        memoryM()->PushContext();
        assert(0 == memoryM()->GetMemoryUsed());
        
        bool * b1 = memoryM()->NewBool(); // Verify bool allocation
        bool * b2 = memoryM()->NewBool();
        assert(2 == memoryM()->GetMemoryUsed());
        
        int  * i1 = memoryM()->NewInt(); // Verify int allocation
        int  * i2 = memoryM()->NewInt();
        assert(10 == memoryM()->GetMemoryUsed());
        
        char * s1 = memoryM()->NewStringLen(10); // Verify string allocation
        char * s2 = memoryM()->NewStringLen(100);
        assert(10 + 11 + 101 == memoryM()->GetMemoryUsed());

        char * helloWorld = "Hello World"; // Verify allocation of a string with a static string
        char * s3 = memoryM()->NewString(helloWorld);
        assertString(helloWorld, s3);
        assert(10 + 11 + 101 + 12 == memoryM()->GetMemoryUsed());
        
        int a1    = memoryM()->GetMemoryUsed(); // Verify that FreeAllocation() free the memory
        char * s5 = memoryM()->NewString(helloWorld);
        int a2    = memoryM()->GetMemoryUsed();
        memoryM()->FreeAllocation(s5);
        assert(a1 == memoryM()->GetMemoryUsed());

        // test Free()
        // Go back to Context 0 automatically pushed when the singleton is created
        memoryM()->PopContext();
        memoryM()->PushContext(); // Re push just in case

        assert(0 == memoryM()->GetMemoryUsed());

        char * testDataString1 = "0123456789";
        int testDataString1Len = strlen(testDataString1) + 1;
        char * s22 = memoryM()->NewString(testDataString1);
        char * s33 = memoryM()->ReNewString(testDataString1, NULL);
        assert(testDataString1Len*2 == memoryM()->GetMemoryUsed());
        
        assert(0 == memoryM()->Free(2, s22, s33));
        assert(1 == memoryM()->Free(1, 4354543));
        assert(00 == memoryM()->GetMemoryUsed());

        return true;
    }

    bool __UnitTests_BasicDate() {

        memoryM()->PopContext(); // Restore memory to initialization state
        memoryM()->PushContext();
        assert(0 == memoryM()->GetMemoryUsed());

        int tmLen = sizeof(struct tm);
        struct tm * now = memoryM()->NewDate();

        struct tm * codeCamp22Date = memoryM()->NewDateTime(2014, 11, 22, 1, 2, 3);
        assertDate(codeCamp22Date, 2014, 11, 22, 1, 2, 3);
        
        int a = memoryM()->GetMemoryUsed();
        assert(tmLen *2 == memoryM()->GetMemoryUsed());

        char * f1 = NULL;
        f1 = memoryM()->ReFormatDateTime(codeCamp22Date, "%a - %b %d", f1);
        assertString("Thu - Nov 22", f1);

        f1 = memoryM()->ReFormatDateTime(codeCamp22Date, "%Y-%m-%d", f1);
        assertString("2014-11-22", f1);

        f1 = memoryM()->ReFormatDateTime(codeCamp22Date, "%X", f1);
        assertString("01:02:03", f1);

        return true;
    }

    bool __UnitTests_PushPopContext() {

        memoryM()->PopContext(); // Restore memory to initialization state
        memoryM()->PushContext();
        assert(0 == memoryM()->GetMemoryUsed());

        char * testDataString1 = "0123456789";
        int testDataString1Len = strlen(testDataString1) + 1;

        char *s22 = memoryM()->NewString(testDataString1);
        assert(testDataString1Len == memoryM()->GetMemoryUsed());

        bool * b1 = memoryM()->NewBool();
        bool * b2 = memoryM()->NewBool();
        int  * i1 = memoryM()->NewInt();
        int  * i2 = memoryM()->NewInt();

        int totalAllocated = memoryM()->GetMemoryUsed();
        assert(21 == memoryM()->GetMemoryUsed());

        memoryM()->PushContext(); // Re push just in case

            char *s33 = memoryM()->ReNewString(testDataString1, NULL);
            assert(totalAllocated + testDataString1Len == memoryM()->GetMemoryUsed());

        memoryM()->PopContext();
        assert(totalAllocated == memoryM()->GetMemoryUsed());

        memoryM()->PopContext();
        assert(0 == memoryM()->GetMemoryUsed());

        memoryM()->PushContext();

        return true;
    }

    bool __UnitTests_ReNewString() {

        memoryM()->PopContext(); // Restore memory to initialization state
        memoryM()->PushContext();
        assert(0 == memoryM()->GetMemoryUsed());
        
        char * testDataString1 = "0123456789";
        int testDataString1Len = strlen(testDataString1) + 1;
        char * testDataString2 = "01234567890123456789";
        int testDataString2Len = strlen(testDataString2) + 1;

        // Test ReNewString
        char * s33 = NULL;
        s33 = memoryM()->ReNewString(testDataString1, s33);
        assert(testDataString1Len == memoryM()->GetMemoryUsed());
        assertString(testDataString1, s33);

        s33 = memoryM()->ReNewString("01234567890123456789", s33);
        assertString(testDataString2, s33);
        int a = memoryM()->GetMemoryUsed();
        assert(testDataString2Len == memoryM()->GetMemoryUsed());

        s33 = memoryM()->ReNewString(testDataString1, s33);
        assert(testDataString1Len == memoryM()->GetMemoryUsed());
        assertString(testDataString1, s33);

        return true;
    }

    bool __UnitTests_StringFormat() {

        memoryM()->PopContext(); // Restore memory to initialization state
        memoryM()->PushContext();
        assert(0 == memoryM()->GetMemoryUsed());

        // Verify allocation of a string with a static string
        char * helloWorld = "Hello World";
        int helloWorldLen = strlen(helloWorld)+1;
        char * s3         = memoryM()->NewString(helloWorld);
        assertString(helloWorld, s3);
        assert(helloWorldLen == memoryM()->GetMemoryUsed());

        // Verify Format()
        assertString("b:true, b:false", memoryM()->Format("b:%b, b:%b", true, false));
        assertString("n:128, u:128, x:80, X:80", memoryM()->Format("n:%d, u:%u, x:%x, X:%X", 128, 128, 128, 128));
        assertString("s:ok les filles, a:Yes", memoryM()->Format("s:%s, a:%s", "ok les filles", "Yes"));
        assertString("c:A, c:z", memoryM()->Format("c:%c, c:%c", 'A', 'z'));
        assertString("1%", memoryM()->Format("%d%%", 1));
        
        return true;
    }

    //////////////////////////////////////////////////////////////////
    /// __UnitTests
    bool __UnitTests() {

        if (!__UnitTests_Basic())          return false;
        if (!__UnitTests_ReNewString())    return false;
        if (!__UnitTests_PushPopContext()) return false;
        if (!__UnitTests_StringFormat())   return false;
        if (!__UnitTests_BasicDate())      return false;

        return true;
    }

#endif

MemoryManager * memoryM() {

    if (__localMemoryM.NewBool == NULL) {

        __localMemoryM.NewBool          = __newBool;
        __localMemoryM.NewInt           = __newInt;
        __localMemoryM.NewString        = __newString;
        __localMemoryM.ReNewString      = __reNewString;
        __localMemoryM.FreeAll          = __freeAll;
        __localMemoryM.GetCount         = __getCount;
        __localMemoryM.NewStringLen     = __newStringLen;

        __localMemoryM.Format           = __format;
        __localMemoryM.GetReport        = __getReport;
        __localMemoryM.GetMemoryUsed    = __getMemoryUsed;
        __localMemoryM.FreeAllocation   = __freeAllocation;
        __localMemoryM.PushContext      = __PushContext;
        __localMemoryM.PopContext       = __PopContext;
        __localMemoryM.Free             = __free;

        __localMemoryM.NewDate          = __newDate;
        __localMemoryM.NewDateTime      = __newDateTime;
        __localMemoryM.FormatDateTime   = __formatDateTime;
        __localMemoryM.ReFormatDateTime = __reFormatDateTime;

        #if !defined(WINFORMEBBLE)
            __localMemoryM.UnitTests  = __UnitTests;
        #endif

        __Initialize();
    }
    return &__localMemoryM;
}

/*

http://api.thingspeak.com/update?key=N7RV4GSNJWDTTNT6&field1=1111&field2=2222
http://api.thingspeak.com/channels/19324/feed.json?key=N7RV4GSNJWDTTNT6

http://api.openweathermap.org/data/2.5/weather?lat=42.414747044171925&lon=-71.50293471631109

*/
