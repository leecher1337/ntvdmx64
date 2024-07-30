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
#include "symcachep.h"
#include "symeng.h"
#include "reg.h"
#include "injector32.h"
#include "matcheng.h"

#ifdef USE_SYMCACHE

#include "symcachefunc.h"
#ifdef _WIN64
#define m_aSyms g_aSyms64
extern REGKEY_SYMS g_aSyms32[];
extern const int g_aSyms32Size;
#else
#define m_aSyms g_aSyms32
#endif


extern void EnsureWin7Symbols(HMODULE hKrnl32);

static BOOL UpdateSymsForModule(HKEY hKey, char *pszDLL, LPWSTR lpDLLKey, REGKEY_PAIR *keys, BOOL fUpdate, BOOL fWOW64)
{
	char szKernel32[MAX_PATH];
	DWORD dwAddress, i;
	DWORD64 dwBase = 0;
	FILETIME tm = { 0 };
	HANDLE hFile;
	MATCHENG_INST hMatchEng;

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
#ifdef NEED_APPINFO
			else if (pszDLL == "appinfo.dll")
				goto appinfo_load;
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
#ifdef NEED_APPINFO
	else
	{
appinfo_load:
		if (fUpdate && pszDLL == "appinfo.dll" && MatchEng_LoadModule(szKernel32, &hMatchEng) == 0)
		{
			// Special treatment for appinfo.dll, where unfortunately, debug Symbols are often missing, so 
			// Pattern matching is needed

			for (i = 0; keys[i].lpKeyName; i++)
			{
				if (keys[i].pszFunction == "AiOpenWOWStubs" && (dwAddress = (DWORD)MatchEng_FindSig(&hMatchEng, "\x48\x89\x5C\x24\x10\x4C\x89", 7, 2)))
					REG_SetDWORD(hKey, keys[i].lpKeyName, dwAddress);
				else tm.dwLowDateTime = 0;
			}
			MatchEng_UnloadModule(&hMatchEng);
			if (tm.dwLowDateTime)
			{
				REG_SetQWORD(hKey, lpDLLKey, *((PULONGLONG)&tm));
			}
		}
	}
#endif


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
			TRACE("SymCache_GetDLLKey(%S) found, but update failed\n", lpDLLKey);
			return NULL;
		}
	TRACE("SymCache_GetDLLKey(%S) not found\n", lpDLLKey);
	return NULL;
}

#ifdef _WIN64
HANDLE SymCache_GetDLLKeyWOW64(HKEY hKey, LPWSTR lpDLLKey, BOOL fUpdate)
{
	int i;

	for (i = 0; i < g_aSyms32Size; i++)
		if (g_aSyms32[i].lpDLLKey == lpDLLKey)
		{
			if (UpdateSymsForModule(hKey, g_aSyms32[i].pszDLL, g_aSyms32[i].lpDLLKey, g_aSyms32[i].keys, fUpdate, TRUE))
				return (HANDLE)&g_aSyms32[i];
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
	int i;
	NTSTATUS Status;
	static BOOL fUpdated = FALSE;

	if (fUpdated) return TRUE;
	TRACE("UpdateSymbolCache()\n");
	if (NT_SUCCESS(Status = REG_OpenLDNTVDM(KEY_READ | KEY_WRITE, &hKey)))
	{
		for (i = 0, fUpdated = TRUE; i < sizeof(m_aSyms) / sizeof(m_aSyms[0]); i++)
#ifndef _WIN64
			if (!m_aSyms[i].fx64Only)
#endif
			fUpdated &= UpdateSymsForModule(hKey, m_aSyms[i].pszDLL, m_aSyms[i].lpDLLKey, m_aSyms[i].keys, fUpdate, FALSE);
		REG_CloseKey(hKey);

#ifdef _WIN64
		if (NT_SUCCESS(Status = REG_OpenLDNTVDMWOW64(KEY_READ | KEY_WRITE, &hKey)))
		{
			for (i = 0; i < g_aSyms32Size; i++)
				if (!g_aSyms32[i].fx64Only) fUpdated &= UpdateSymsForModule(hKey, g_aSyms32[i].pszDLL, g_aSyms32[i].lpDLLKey, g_aSyms32[i].keys, fUpdate, TRUE);
			REG_CloseKey(hKey);
		}
#endif
	}
	else
	{
		TRACE("RegCreateKeyEx failed: %08X\n", Status);
	}

	return fUpdated;
}
#endif /* USE_SYMCACHE */
