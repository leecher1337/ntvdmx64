#pragma once
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#ifdef TARGET_WINXP
#define _WIN32_WINNT _WIN32_WINNT_WINXP
#endif
#include <Windows.h>
#include "Winternl.h"
#include <tchar.h>
#include "config.h"

#define LDNTVDM_NAME _T("ldntvdm.dll")

#ifdef TRACING
static char szDbgBuf[2048];
typedef int (*fpsprintf)(char * str, const char * format, ...);
extern fpsprintf sprintf;
#ifdef TRACE_FILE
void Trace(char *pszLine);
#define WRITE_TRACE Trace
#else
#define WRITE_TRACE OutputDebugStringA
#endif
#define TRACE(...) { if (sprintf) {sprintf(szDbgBuf, __VA_ARGS__); WRITE_TRACE(szDbgBuf);} }
#else
#define TRACE(...)
#endif
	
typedef int (__cdecl *fpstrcmp)(char * str, char *str2);
typedef int (__cdecl *fp_stricmp)(const char * str, const char *str2);
typedef int (__cdecl *fp_wcsicmp)(PWCHAR str, PWCHAR str2);
typedef int(__cdecl *fpswprintf)(wchar_t *buffer, const wchar_t *format, ...);
extern fp_stricmp __stricmp;
extern fp_wcsicmp __wcsicmp;
extern fpstrcmp _strcmp;
extern fpswprintf __swprintf;
#ifdef NEED_BASEVDM
typedef wchar_t * (__cdecl *fpwcsncpy)(wchar_t *strDest, const wchar_t *strSource, size_t count);
typedef int(__cdecl *fp_wcsnicmp)(const wchar_t *string1, const wchar_t *string2, size_t count);
typedef wchar_t * (__cdecl *fpwcsrchr)(const wchar_t *str, wchar_t c);
typedef char *(__cdecl *fpstrstr)(const char *str, const char *strSearch);
extern fpwcsncpy _wcsncpy;
extern fp_wcsnicmp __wcsnicmp;
extern fpwcsrchr _wcsrchr;
extern fpstrstr _strstr;
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

__kernel_entry NTSTATUS
NTAPI
NtGetNextThread(
	_In_ HANDLE ProcessHandle,
	_In_ HANDLE ThreadHandle,
	_In_ ACCESS_MASK DesiredAccess,
	_In_ ULONG HandleAttributes,
	_In_ ULONG Flags,
	_Out_ PHANDLE NewThreadHandle
	);

#ifdef _WIN64
#define BASEP_CALL __fastcall
#else
#define BASEP_CALL WINAPI
#endif

#ifndef STATUS_SUCCESS
#   define STATUS_SUCCESS 0
#endif

#ifdef CRYPT_LDR
#pragma section(".code",execute, read, write)
#pragma comment(linker,"/SECTION:.code,ERW")
#pragma code_seg(".code")
#endif
