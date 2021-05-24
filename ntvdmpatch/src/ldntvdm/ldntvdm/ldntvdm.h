#pragma once
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include "Winternl.h"
#include <tchar.h>

#define LDNTVDM_NAME _T("ldntvdm.dll")

/* Possible tagets:
	TARGET_WINXP	- Windows XP / Server 2003
	TARGET_WIN7		- Windows 7 / Server 2008
	TARGET_WIN80	- Windows 8 / Server 2012

	Default is TARGET_WIN10, which is set when none of these are set
 */

#if !defined(TARGET_WINXP) && !defined(TARGET_WIN7) && !defined(TARGET_WIN80)
#define TARGET_WIN10	// Default
#endif

// If set, we place some hooks for PrivateExtractIconsW so that 16bit icon support gets enabled again
// which was disabled starting with Windows 7
#ifndef TARGET_WINXP
#define EXTRACTICON_HOOK
#endif

// If set, we also service 16bit Windows applications via WOW32
#define WOW16_SUPPORT

// Enable debug tracing via Debug console. Recommended, as it helps diagnosing loader issues, but makes DLL bigger
// due to debug strings
#define TRACING
//#define TRACE_FILE

// This #define was an experiment to encrypt certain code sections of the loader to evade 
// false positive detection by stupid Antivirus software on x32. 
// However it didn't help at all, so it's deactivated. 
#ifndef _WIN64
//#define CRYPT_LDR
#endif

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

/* This is an experimental patch to enforce ConhostV1 to also load on users
 * who haven't installed NTVDMx64 themselves and therefore, ForceV2 key for
 * console is still TRUE in their user accounts.
 */
#define HOOK_CONHOSTV2

/* On Windows >=7, appinfo.dll needs to get patched inside svchost.exe netsvcs */
#if !defined(TARGET_WINXP) && defined(_WIN64) 
#define NEED_APPINFO
#endif

/* In Windows XP And Windows 7, all the stuff is internal in kernel32.dll :(
 * Windows XP has its own xpcreateproc.c module, therefore doesn't have to lookup
 * all these symbols.
 * All Windows versions >= Windows 7 however have the appinfo.dll for "Run as...",
 * implementation, therefore all of these need to have a symbol cache.
 * Windows 7: We cannot use the symbol loader in the module loading routine, so
 * a symbol cache in the registry needs to be built that can get accessed 
 * during module load
 * Windows >=7: The appinfo.dll is used in the service where the symbol loader also
 * doesn't seem to work, so a logged on user first needs to lookup the symbols and
 * add them to the cache, so the service can read the cache and hook the function
 */
#if defined(TARGET_WIN7) || defined(TARGET_WINXP) || defined(NEED_APPINFO)
#define USE_SYMCACHE
#endif

/* In Windows XP, all the BaseVDM functions are not linked into the kernel32.dll,
 * therefore we need to reimplement them all :( 
 */
#ifdef TARGET_WINXP
#define NEED_BASEVDM
#endif

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
extern fp_stricmp __stricmp;
extern fp_wcsicmp __wcsicmp;
extern fpstrcmp _strcmp;
#ifdef NEED_BASEVDM
typedef wchar_t * (__cdecl *fpwcsncpy)(wchar_t *strDest, const wchar_t *strSource, size_t count);
typedef int(__cdecl *fp_wcsnicmp)(const wchar_t *string1, const wchar_t *string2, size_t count);
typedef wchar_t * (__cdecl *fpwcsrchr)(const wchar_t *str, wchar_t c);
typedef int (__cdecl *fpswprintf)(wchar_t *buffer, const wchar_t *format, ...);
typedef char *(__cdecl *fpstrstr)(const char *str, const char *strSearch);
extern fpwcsncpy _wcsncpy;
extern fp_wcsnicmp __wcsnicmp;
extern fpwcsrchr _wcsrchr;
extern fpswprintf __swprintf;
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


#ifdef CRYPT_LDR
#pragma section(".code",execute, read, write)
#pragma comment(linker,"/SECTION:.code,ERW")
#pragma code_seg(".code")
#endif
