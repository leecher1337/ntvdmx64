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

#ifdef USE_SYMCACHE

typedef struct {
	char *pszFunction;
	LPWSTR lpKeyName;
} REGKEY_PAIR;
static BOOL UpdateSymsForModule(HKEY hKey, char *pszDLL, LPWSTR lpDLLKey, REGKEY_PAIR *keys)
{
	char szKernel32[MAX_PATH];
	DWORD dwAddress, i;
	DWORD64 dwBase = 0;
	FILETIME tm = { 0 };
	HANDLE hFile;

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
			if (!keys[i].lpKeyName) return TRUE; // No update needed, everything fine
		}
		CloseHandle(hFile);
	}

	// Update needed
	if (SymEng_LoadModule(szKernel32, &dwBase) == 0 || GetLastError() == 0x1E7)
	{
		if (!dwBase) dwBase = (DWORD64)GetModuleHandleA(pszDLL);
		TRACE("UpdateSymbolCache() loading %s symbols\n", pszDLL);
		for (i = 0; keys[i].lpKeyName; i++)
		{
			if (dwAddress = SymEng_GetAddr(dwBase, keys[i].pszFunction))
				REG_SetDWORD(hKey, keys[i].lpKeyName, dwAddress);
		}
		SymEng_UnloadModule(dwBase);
		if (tm.dwLowDateTime)
		{
			REG_SetQWORD(hKey, lpDLLKey, *((PULONGLONG)&tm));
		}
		return TRUE;
	}
	return FALSE;
}

BOOL UpdateSymbolCache()
{
	HKEY hKey = NULL;
	DWORD64 dwBase = 0;
	NTSTATUS Status;
	static BOOL bUpdated = FALSE;

	if (bUpdated) return TRUE;
	TRACE("UpdateSymboCache()\n");
	if (NT_SUCCESS(Status = REG_OpenLDNTVDM(KEY_READ | KEY_WRITE, &hKey)))
	{
		{
			REGKEY_PAIR Keys[] = {
#if !defined(TARGET_WINXP)
				{ "BasepProcessInvalidImage", REGKEY_BasepProcessInvalidImage },
#endif
				{ "BaseIsDosApplication", REGKEY_BaseIsDosApplication },
				{ NULL, NULL }
			};
			bUpdated = UpdateSymsForModule(hKey, "kernel32.dll", L"kernel32.dll", Keys);
			TRACE("UpdateSymboCache updated = %d\n", bUpdated);
		}

#if !defined(TARGET_WINXP)
		{
			REGKEY_PAIR Keys[] = {
				{ "dwConBaseTag", L"dwConBaseTag" },
				{ "FindProcessInList", L"FindProcessInList" },
				{ "CreateConsoleBitmap",  L"CreateConsoleBitmap" },
				{ NULL, NULL }
			};
			bUpdated &= UpdateSymsForModule(hKey, "conhost.exe", L"conhost.exe", Keys);
		}
#endif
		REG_CloseKey(hKey);
	}
	else
	{
		TRACE("RegCreateKeyEx failed: %08X\n", Status);
	}

	return bUpdated;
}
#endif /* TARGET_WIN7 */
