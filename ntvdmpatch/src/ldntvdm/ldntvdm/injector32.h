#pragma once

/* Which methods do we support? */

// --- Methods for already running processes ---
#define METHOD_CREATETHREAD 0
//#define METHOD_APC 1
//#define METHOD_MODIFYSTARTUP 2
#if !defined(CREATEPROCESS_HOOK) && defined(_WIN64)
#define METHOD_INTERCEPTTHREAD 3
#endif

// --- Methods only for processes that got started suspended  ---

/* Sets the PostProcessInitRoutine in the PEB to run the loader
 * This is a neat method, but it only works for console mode applications, because the 
 * user32.dll loader nulls it in its DLL init routine :(
 *
 *         NtCurrentPeb()->PostProcessInitRoutine = NULL;
 *
 */
#define METHOD_POSTPROCESSINIT	4 

/* Inline Hooks LdrpInitializeProcess 
 * This has the same effect as METHOD_POSTPROCESSINIT, but it works on all
 * subsystems
 */
#define METHOD_HOOKLDR	5

BOOL injectLdrLoadDLL(HANDLE hProcess, HANDLE hThread, WCHAR *szDLL, UCHAR method);

#ifdef METHOD_APC
// The context used for injection via inject_via_apcthread
typedef struct _APCCONTEXT
{
	union
	{
		LPVOID lpStartAddress;
		BYTE bPadding1[8];
	} s;

	union
	{
		LPVOID lpParameter;
		BYTE bPadding2[8];
	} p;

	BYTE bExecuted;

} APCCONTEXT, *LPAPCCONTEXT;
#endif
