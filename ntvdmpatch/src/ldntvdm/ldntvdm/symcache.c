/*Project: ldntvdm
* Module : symcache
* Author : leecher@dose.0wnz.at
* Descr. : Symbol caching routines needed on Windows Versions prior to
*          Windows 8, as some routines are internal to kernel32.dll there
*          and cannot be resolved on module startup
* Changes: 01.04.2016  - Created
*/

#include "ldntvdm.h"
#include "symcache.h"
#include "symeng.h"
#include "reg.h"
#include "injector32.h"

#ifdef USE_SYMCACHE

typedef struct {
	char *pszFunction;
	LPWSTR lpKeyName;
} REGKEY_PAIR;

typedef struct {
	char *pszDLL;
	LPWSTR lpDLLKey;
	REGKEY_PAIR *keys;
} REGKEY_SYMS;

#define _E(x) {x,L##x}

static REGKEY_PAIR m_aSymsKERNEL32[] =
{
	_E("BaseCreateVDMEnvironment"),
	_E("BaseGetVdmConfigInfo"),
	_E("BaseCheckVDM"),
#if defined(TARGET_WIN7)
	{ "BasepProcessInvalidImage", REGKEY_BasepProcessInvalidImage },
	{ "BaseIsDosApplication", REGKEY_BaseIsDosApplication },
#endif // TARGET_WIN7
#if defined(TARGET_WINXP)
	_E("NtVdm64CreateProcess"),
	_E("BaseIsDosApplication"),
	_E("BaseCreateStack"),
	_E("BaseFormatObjectAttributes"),
	_E("BasepIsProcessAllowed"),
	_E("BasePushProcessParameters"),
	_E("BasepIsRealtimeAllowed"),
	_E("BaseInitializeContext"),
	_E("BaseStaticServerData"),
	_E("StuffStdHandle"),
	_E("BasepFreeAppCompatData"),
	_E("BasepReplaceProcessThreadTokens"),
	_E("BasepCheckBadapp"),
	_E("BasepCheckWinSaferRestrictions"),
#ifndef _WIN64
	_E("CsrBasepCreateProcess"),
#endif
	_E("BuildSubSysCommandLine"),
#endif // TARGET_WINXP
	{ NULL, NULL }
};

#if defined(_WIN64) && !defined(TARGET_WINXP) && (defined(TARGET_WIN7) || defined(HOOK_CONHOSTV2))
static REGKEY_PAIR m_aSymsCONHOST[] = 
{
#if defined(TARGET_WIN7)
	_E("dwConBaseTag"),
	_E("FindProcessInList"),
	_E("CreateConsoleBitmap"),
#endif
#if defined(HOOK_CONHOSTV2)
	_E("ShouldUseConhostV2"),
#endif
	{ NULL, NULL }
};
#endif
#ifdef TARGET_WIN11
static REGKEY_PAIR m_aSymsCONHOSTV1[] =
{
	_E("InitializeCustomCP"),
	{ NULL, NULL }
};
#endif
#ifdef NEED_APPINFO
static REGKEY_PAIR m_aSymsAPPINFO[] =
{
	_E("AiOpenWOWStubs"),
	{ NULL, NULL }
};
#endif
#ifdef METHOD_HOOKLDR
static REGKEY_PAIR m_aSymsNTDLL[] =
{
	_E("LdrpInitializeProcess"),
	{ NULL, NULL }
};
#endif
#undef _E

static REGKEY_SYMS m_aSyms[] = {
	{"kernel32.dll", L"kernel32.dll", m_aSymsKERNEL32},
	#if defined(_WIN64) && !defined(TARGET_WINXP) && (defined(TARGET_WIN7) || defined(HOOK_CONHOSTV2)) // consbmp.c fix
	{"conhost.exe", L"conhost.exe", m_aSymsCONHOST},
	#endif
	#if defined(TARGET_WIN11) && defined(_WIN64)
	{ "conhostV1.dll", L"conhostV1.dll", m_aSymsCONHOSTV1 },
	#endif
	#ifdef NEED_APPINFO
	{"appinfo.dll", L"appinfo.dll", m_aSymsAPPINFO},
	#endif
	#ifdef METHOD_HOOKLDR
	{"ntdll.dll", L"ntdll.dll", m_aSymsNTDLL},
	#endif
};

extern void EnsureWin7Symbols(HMODULE hKrnl32);

static BOOL UpdateSymsForModule(HKEY hKey, char *pszDLL, LPWSTR lpDLLKey, REGKEY_PAIR *keys, BOOL fUpdate, BOOL fWOW64)
{
	char szKernel32[MAX_PATH];
	DWORD dwAddress, i;
	DWORD64 dwBase = 0;
	FILETIME tm = { 0 };
	HANDLE hFile;

#ifdef _WIN64
	if (fWOW64) sprintf(szKernel32 + GetWindowsDirectoryA(szKernel32, sizeof(szKernel32) / sizeof(szKernel32[0])), "\\SysWOW64\\%s", pszDLL); else
#endif
	sprintf(szKernel32 + GetSystemDirectoryA(szKernel32, sizeof(szKernel32) / sizeof(szKernel32[0])), "\\%s", pszDLL);
	hFile = CreateFileA(szKernel32, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		ULONGLONG tmr;

		if (GetFileTime(hFile, NULL, NULL, &tm) &&
			NT_SUCCESS(REG_QueryQWORD(hKey, lpDLLKey, &tmr)) && *((PULONGLONG)&tm) == tmr)
		{
			// So far, it seems unchanged, now check if exports are present
			for (i = 0; keys[i].lpKeyName; i++)
			{
				if (!NT_SUCCESS(REG_QueryDWORD(hKey, keys[i].lpKeyName, &dwAddress)) || !dwAddress) break;
			}
			if (!keys[i].lpKeyName)
			{
				// No update needed, everything fine
				CloseHandle(hFile);
				return TRUE; 
			}
		}
		CloseHandle(hFile);
	}

	// Update needed
	if (fUpdate && (SymEng_LoadModule(szKernel32, &dwBase) == 0 || GetLastError() == 0x1E7))
	{
		if (!dwBase && !fWOW64) dwBase = (DWORD64)GetModuleHandleA(pszDLL);
		TRACE("UpdateSymbolCache() loading %s symbols\n", pszDLL);
		for (i = 0; keys[i].lpKeyName; i++)
		{
			if (dwAddress = SymEng_GetAddr(dwBase, keys[i].pszFunction))
				REG_SetDWORD(hKey, keys[i].lpKeyName, dwAddress);
#ifdef HOOK_CONHOSTV2
			// Special treatment for Symbol with multiple possible names :(
			else if (keys[i].pszFunction == "ShouldUseConhostV2" &&
				(dwAddress = SymEng_GetAddr(dwBase, "ConhostV2ForcedInRegistry")))
				REG_SetDWORD(hKey, keys[i].lpKeyName, dwAddress);
#endif
			else tm.dwLowDateTime = 0;
		}
		SymEng_UnloadModule(dwBase);
		if (tm.dwLowDateTime)
		{
			REG_SetQWORD(hKey, lpDLLKey, *((PULONGLONG)&tm));
		}
#ifdef TARGET_WIN7
		if (lpDLLKey == L"kernel32.dll")
		{
			// If modules get resolved by symcache update hook, ensure that we also get to know them, if not done yet
			HMODULE hKrnl32 = GetModuleHandle(_T("kernel32.dll"));
			if (hKrnl32) EnsureWin7Symbols(hKrnl32);
		}
#endif

		return TRUE;
	}

	return FALSE;
}

// NB: Only pass in static string lpDLLKey so that comparison with pointer to string in Data segment works 
// and no expensive strcmp is needed
HANDLE SymCache_GetDLLKey(HKEY hKey, LPWSTR lpDLLKey, BOOL fUpdate)
{
	int i;

	for (i = 0; i < sizeof(m_aSyms) / sizeof(m_aSyms[0]); i++)
		if (m_aSyms[i].lpDLLKey == lpDLLKey)
		{
			if (UpdateSymsForModule(hKey, m_aSyms[i].pszDLL, m_aSyms[i].lpDLLKey, m_aSyms[i].keys, fUpdate, FALSE))
				return (HANDLE)&m_aSyms[i];
			return NULL;
		}
	return NULL;
}

#ifdef _WIN64
HANDLE SymCache_GetDLLKeyWOW64(HKEY hKey, LPWSTR lpDLLKey, BOOL fUpdate)
{
	int i;

	for (i = 0; i < sizeof(m_aSyms) / sizeof(m_aSyms[0]); i++)
		if (m_aSyms[i].lpDLLKey == lpDLLKey)
		{
			if (UpdateSymsForModule(hKey, m_aSyms[i].pszDLL, m_aSyms[i].lpDLLKey, m_aSyms[i].keys, fUpdate, TRUE))
				return (HANDLE)&m_aSyms[i];
			return NULL;
		}
	return NULL;
}
#endif


DWORD SymCache_GetProcAddress(HKEY hKey, LPWSTR lpKeyName)
{
	NTSTATUS Status;
	DWORD dwAddress;

	if (NT_SUCCESS(Status = REG_QueryDWORD(hKey, lpKeyName, &dwAddress)))
	{
		return dwAddress;
	}
	else
	{
		TRACE("RegQueryValueEx(%S) failed: %08X\n", lpKeyName, Status);
	}
	return 0;
}

BOOL UpdateSymbolCache(BOOL fUpdate)
{
	HKEY hKey = NULL;
	DWORD64 dwBase = 0;
	NTSTATUS Status;
	static BOOL fUpdated = FALSE;

	if (fUpdated) return TRUE;
	TRACE("UpdateSymbolCache()\n");
	if (NT_SUCCESS(Status = REG_OpenLDNTVDM(KEY_READ | KEY_WRITE, &hKey)))
	{
		int i;

		for (i = 0, fUpdated = TRUE; i < sizeof(m_aSyms) / sizeof(m_aSyms[0]); i++)
			fUpdated &= UpdateSymsForModule(hKey, m_aSyms[i].pszDLL, m_aSyms[i].lpDLLKey, m_aSyms[i].keys, fUpdate, FALSE);
		REG_CloseKey(hKey);
	}
	else
	{
		TRACE("RegCreateKeyEx failed: %08X\n", Status);
	}

	return fUpdated;
}
#endif /* USE_SYMCACHE */
