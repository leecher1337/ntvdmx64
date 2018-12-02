#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "Winternl.h"
#include <tchar.h>

#define LDNTVDM_NAME _T("ldntvdm.dll")

// If set (i.e. via project workspace settings), this is Windows 7/Server 2008 version, if not set, it's Win10 version
//#define TARGET_WIN7	

// Enable debug tracing via Debug console. Recommended, as it helps diagnosing loader issues, but makes DLL bigger
// due to debug strings
#define TRACING

// Not really useful method, but maybe we need it some day, leave disabled
//#define APPCERT_DLL

#if !defined(APPCERT_DLL)
/* If this is set, CreateProcess() APIs are hooked. This is just needed because
 * the AppInit-Hooks don't invade console applications.
 * If not set, ldntvdm, which gets injected into conhost.exe via AppInit,
 * instead tries to inject itself into the target application from conhost.exe,
 * which may be a more stable solution than hooking CreateProcess
 */
//#define CREATEPROCESS_HOOK
#endif

#ifdef TRACING
static char szDbgBuf[2048];
typedef int (NTAPI *fpsprintf)(char * str, const char * format, ...);
extern fpsprintf sprintf;
#define TRACE(...) { sprintf(szDbgBuf, __VA_ARGS__); OutputDebugStringA(szDbgBuf); }
#else
#define TRACE(...)
#endif


/* This is the simplest method of entering the NTVDM.
 * It has the advantage that it doesn't need symbol server for internal
 * symbol resoution to execute loader code, however it has some problems
 * with starting DOS-applications from within DOS and it needs NTVDM
 * to be patched to ignore -w switch for WOW.
 */
//#define WOW_HACK

#undef RtlMoveMemory
EXTERN_C void _declspec(dllimport) WINAPI RtlMoveMemory(PVOID, const VOID*, SIZE_T);
#undef RtlZeroMemory
EXTERN_C void _declspec(dllimport) WINAPI RtlZeroMemory(PVOID, SIZE_T);
