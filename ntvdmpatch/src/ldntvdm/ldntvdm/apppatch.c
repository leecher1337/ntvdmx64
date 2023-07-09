/*Project: ldntvdm
* Module : apppatch
* Author : leecher@dose.0wnz.at
* Descr. : This module contains shims for some well-known applications
*          that show wrong behaviour on 64bit Windows and filter the execution
*          of 16bit Applications, even though they would run with NTVDMx64
* Changes: 06.01.2021  - Created
*/
#include "ldntvdm.h"
#include <shellapi.h>
#include "apppatch.h"
#include "iathook.h"

struct stProcDisp {
	TCHAR *pszProcess;
	void(*fpDisp)(void);
};

#ifdef _WIN64

/*--------------------------------------------------------------------------*
 * Ghisler Total Commander 64bit edition                                    *
 *--------------------------------------------------------------------------*/
typedef DWORD_PTR (*fpSHGetFileInfoW)(_In_ LPCWSTR pszPath, DWORD dwFileAttributes, _Inout_updates_bytes_opt_(cbFileInfo) SHFILEINFOW *psfi,
	UINT cbFileInfo, UINT uFlags);

 DWORD_PTR SHGetFileInfoWHook(
	LPCWSTR     pszPath,
	DWORD       dwFileAttributes,
	SHFILEINFOW *psfi,
	UINT        cbFileInfo,
	UINT        uFlags
	)
{
	/* TOTALCMD64 checks return value of SHGetFileInfoW. If it is NE, it refuses to run the application,
	 * even though it would be able to run. Therefore fake return value to MZ
	 */
	static fpSHGetFileInfoW pSHGetFileInfoW = NULL;
	DWORD_PTR ret;

	// Avoid dependency to shell32.dll
	if (!pSHGetFileInfoW && !(pSHGetFileInfoW = (fpSHGetFileInfoW)GetProcAddress(GetModuleHandleA("shell32.dll"), "SHGetFileInfoW")))
		return 'ZM'; // We are doomed!
	ret = pSHGetFileInfoW(pszPath, dwFileAttributes, psfi, cbFileInfo, uFlags);
	if (LOWORD(ret) == 'EN') return 'ZM'; else return ret;
}

FARPROC WINAPI myGetProcAddress(_In_ HMODULE hModule, _In_ LPCSTR lpProcName)
{
	/* TOTALCMD64 loads SHGetFileInfoW dynamically */
	if (HIWORD(lpProcName) && !_strcmp((char*)lpProcName, "SHGetFileInfoW")) return SHGetFileInfoWHook;
	return GetProcAddress(hModule, lpProcName);
}

void TotalCmd64_Hook(void)
{
	Hook_IAT_x64_IAT((LPBYTE)GetModuleHandle(NULL), "KERNEL32.DLL", "GetProcAddress", myGetProcAddress, NULL);
}
#endif


/*--------------------------------------------------------------------------*
 * Dispacher
 *--------------------------------------------------------------------------*/
void AppPatch_Check(WCHAR *pszProcess)
{
#ifdef _WIN64	// Currently only 64bit apps in list
	struct stProcDisp aDispTbl[] = {
		{_T("totalcmd64.exe"), TotalCmd64_Hook}
	};
	int i;

	for (i = 0; i < sizeof(aDispTbl) / sizeof(aDispTbl[0]); i++)
		if (__wcsicmp(pszProcess, aDispTbl[i].pszProcess) == 0)
		{
			TRACE("Installing LDNTVDM hook for application %S\n", aDispTbl[i].pszProcess);
			aDispTbl[i].fpDisp();
			break;
		}
#endif
}