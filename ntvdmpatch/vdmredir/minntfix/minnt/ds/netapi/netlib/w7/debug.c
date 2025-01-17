#include <nt.h>              // IN, LPVOID, etc.

#include <netdebug.h>           // My prototypes.
#include <ntrtl.h>              // RtlAssert().
#include <stdarg.h>             // va_list, etc.
#include <stdio.h>              // vsprintf().


#if DBG

VOID
NetpBreakPoint(
    VOID
    )
{
    DbgBreakPoint();

} // NetpBreakPoint

#endif
