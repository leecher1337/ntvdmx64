/* Project: ldntvdm
* Module : pifprop
* Author : leecher@dose.0wnz.at
* Descr. : On x64 Windows, CShellLink::_LoadFromPIF uses 
*          PifMgr_OpenProperties, but this function is stubbed in
*          shell32.dll, so creating shell links via appwiz.cpl fails.
*          We implemented our own pifmgr.dll and export the missing 
*          functions there, we need to make the CShellLink::Save function
*          aware of it. 
* Changes: 20.04.2024  - Created
*/

#include "ldntvdm.h"
#include "iathook.h"
#include "pifprop.h"

typedef HANDLE (STDAPICALLTYPE *fpSHELL32_PifMgr_OpenProperties)(LPCTSTR lpszApp, LPCTSTR lpszPIF, UINT hInf, UINT flOpt);
typedef int (STDAPICALLTYPE  *fpSHELL32_PifMgr_GetProperties)(HANDLE hProps, LPCSTR lpszGroup, void *lpProps, int cbProps, UINT flOpt);
typedef int (STDAPICALLTYPE *fpSHELL32_PifMgr_SetProperties)(HANDLE hProps, LPCSTR lpszGroup, void *lpProps, int cbProps, UINT flOpt);
typedef HANDLE (STDAPICALLTYPE  *fpSHELL32_PifMgr_CloseProperties)(HANDLE hProps, UINT flOpt);
FARPROC WINAPI DelayLoadFailureHook(
	_In_ LPCSTR pszDllName,
	_In_ LPCSTR pszProcName
	);

fpSHELL32_PifMgr_OpenProperties SHELL32_PifMgr_OpenProperties = NULL;
fpSHELL32_PifMgr_GetProperties SHELL32_PifMgr_GetProperties = NULL;
fpSHELL32_PifMgr_SetProperties SHELL32_PifMgr_SetProperties = NULL;
fpSHELL32_PifMgr_CloseProperties SHELL32_PifMgr_CloseProperties = NULL;

// We mustn't load Pifmgr.dll upon DLL initialization like here in ldntvdm, otherwise bad things
// will happen. Therefore do poor man's delay loading in case of function call:
static HANDLE STDAPICALLTYPE PifMgr_OpenProperties(LPCTSTR lpszApp, LPCTSTR lpszPIF, UINT hInf, UINT flOpt)
{
	if (!SHELL32_PifMgr_OpenProperties) PifProp_LoadDLL();
	if (SHELL32_PifMgr_OpenProperties) return SHELL32_PifMgr_OpenProperties(lpszApp, lpszPIF, hInf, flOpt);
	return NULL;
}
static int STDAPICALLTYPE  PifMgr_GetProperties(HANDLE hProps, LPCSTR lpszGroup, void *lpProps, int cbProps, UINT flOpt)
{
	if (!SHELL32_PifMgr_GetProperties) PifProp_LoadDLL();
	if (SHELL32_PifMgr_GetProperties) return SHELL32_PifMgr_GetProperties(hProps, lpszGroup, lpProps, cbProps, flOpt);
	return 0;
}
static int STDAPICALLTYPE PifMgr_SetProperties(HANDLE hProps, LPCSTR lpszGroup, void *lpProps, int cbProps, UINT flOpt)
{
	if (!SHELL32_PifMgr_SetProperties) PifProp_LoadDLL();
	if (SHELL32_PifMgr_SetProperties) return SHELL32_PifMgr_SetProperties(hProps, lpszGroup, lpProps, cbProps, flOpt);
	return 0;
}
static HANDLE STDAPICALLTYPE  PifMgr_CloseProperties(HANDLE hProps, UINT flOpt)
{
	if (!SHELL32_PifMgr_CloseProperties) PifProp_LoadDLL();
	if (SHELL32_PifMgr_CloseProperties) return SHELL32_PifMgr_CloseProperties(hProps, flOpt);
	return NULL;
}

static BOOL PifProp_LoadDLL(void)
{
	HANDLE hPifProp;
	BOOL fRet;

	if (!(hPifProp = GetModuleHandle(_T("pifprop.dll"))) && !(hPifProp = LoadLibrary(_T("pifprop.dll"))))
	{
		TRACE("LDNTVDM: PifProp.dll not present\n");
		return FALSE;
	}
	fRet = (SHELL32_PifMgr_OpenProperties = GetProcAddress(hPifProp, "PifMgr_OpenProperties")) &&
		(SHELL32_PifMgr_GetProperties = GetProcAddress(hPifProp, "PifMgr_GetProperties")) &&
		(SHELL32_PifMgr_SetProperties = GetProcAddress(hPifProp, "PifMgr_SetProperties")) &&
		(SHELL32_PifMgr_CloseProperties = GetProcAddress(hPifProp, "PifMgr_CloseProperties"));
	if (!fRet)
	{
		TRACE("LDNTVDM: Pifprop doesn't export Pifmgr functions, maybe better update DLL?\n");
		return FALSE;
	}
	return TRUE;
}

static BOOL PifProp_Hook_IAT(HMODULE hModule)
{
	return Hook_IAT_x64((LPBYTE)hModule, "ext-ms-win-shell-exports-internal-l1-1-0.dll",
		"SHELL32_PifMgr_OpenProperties", PifMgr_OpenProperties) &&
		Hook_IAT_x64((LPBYTE)hModule, "ext-ms-win-shell-exports-internal-l1-1-0.dll",
			"SHELL32_PifMgr_GetProperties", PifMgr_GetProperties) &&
		Hook_IAT_x64((LPBYTE)hModule, "ext-ms-win-shell-exports-internal-l1-1-0.dll",
			"SHELL32_PifMgr_SetProperties", PifMgr_SetProperties) &&
		Hook_IAT_x64((LPBYTE)hModule, "ext-ms-win-shell-exports-internal-l1-1-0.dll",
			"SHELL32_PifMgr_CloseProperties", PifMgr_CloseProperties);
}

BOOL PifProp_Install(void)
{
#if defined(TARGET_WIN10) || defined(TARGET_WIN11)		// FIXME: Check, which Windows Versions this applies to
	/* On Windows 10, unlike on earlier Windows Versions, the calls to PifProp are still
	 * present, so all we need to do is patch IAT in order to use them
	 */
	HMODULE hModule, hPifProp;
	BOOL fRet = FALSE;

	/* We could also hook DelayLoadFailureHook in kernelbase.dll to redirect potential
	 * callers to the real functions to automatically resolve the dependencies, but
	 * it's currently just in windows.storage.dll, so in case it was already resolved,
	 * better do it directly to be sure.
	if (hModule = GetModuleHandle(_T("kernelbase.dll")))
	{
		DelayLoadFailureHookReal = (fpDelayLoadFailureHook)Hook_Inline(GetCurrentProcess(), hModule, 
			(PVOID)GetProcAddress(hModule, "DelayLoadFailureHook"), MyDelayLoadFailureHook);
	}
	*/

	/* We could also patch _GetNewLinkInfo in shell32.dll to obey .PIF file, but we would need to
	 * make in internal Patch in case SHGetFileInfoA(...SHGFI_EXETYPE) returns 'MZ' 
	 * and it's only cosmetical, so for new, we just focus on the functions that cause real trouble 
	 */

	// Creating a link to a DOS-Application will fail if not patched:
	if (hModule = GetModuleHandle(_T("windows.storage.dll")))
		return PifProp_Hook_IAT(hModule);
	return fRet;
#endif
}
