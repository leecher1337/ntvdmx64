#pragma once

/* Which methods do we support= */
#define METHOD_CREATETHREAD 0
//#define METHOD_APC 1
//#define METHOD_MODIFYSTARTUP 2
#define METHOD_INTERCEPTTHREAD 3

BOOL injectLdrLoadDLL(HANDLE hProcess, HANDLE hThread, WCHAR *szDLL, UCHAR method);
