/*Project: ldntvdm
* Module : appinfo
* Author : leecher@dose.0wnz.at
* Descr. : This module contains a fix for appinfo.dll, whose AiOpenWOWStubs
*          function is optimized for 16bit Setup launchers and not for 
*          csrstub.exe which is necessary to open WOW16 application in context
*          of a different user (Run as...).
*          This is mainly for svchost.exe -k netsvcs which contains the daemon
*          that implements the Run as functionality.
* Changes: 08.01.2021  - Created
*/
#include "ldntvdm.h"
#include "reg.h"
#include "symcache.h"
#include "detour.h"

#ifdef NEED_APPINFO
void BASEP_CALL AiOpenWOWStubs(HANDLE *phHandles)
{
	static WCHAR wszPath[MAX_PATH] = { 0 };

	if (!*wszPath)
		wcscpy(wszPath + GetSystemDirectoryW(wszPath, MAX_PATH), L"\\csrstub.exe");
	phHandles[0] = CreateFileW(wszPath, GENERIC_READ | GENERIC_EXECUTE, FILE_SHARE_DELETE | FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
}

BOOL AppInfo_InstallHook(void)
{
	static BOOL fInstalled = FALSE;
	HMODULE hAppInfo;

	/* This hook is mainly used in the svchost.exe netsvcs, so in a service.
	 * Unfortunately, Symbol server doesn't work within the service, it always
	 * returns 404 for the symbol file, even though the file path is correct.
	 * Maybe there are some Internet communication restrictions for services...?
	 */
	if (!fInstalled && (hAppInfo = GetModuleHandleW(L"appinfo.dll")))
	{
		NTSTATUS Status;
		DWORD64 dwAddress;
		HKEY hKey;

		if (NT_SUCCESS(Status = REG_OpenLDNTVDM(KEY_READ | KEY_WRITE, &hKey)))
		{
			if (SymCache_GetDLLKey(hKey, L"appinfo.dll", TRUE) &&
				(dwAddress = (DWORD64)SymCache_GetProcAddress(hKey, L"AiOpenWOWStubs")))
			{
				dwAddress += (DWORD64)hAppInfo;
				fInstalled = Hook_Inline((PVOID)dwAddress, AiOpenWOWStubs) ? TRUE : FALSE;
			}
			REG_CloseKey(hKey);
		}
	}
	return fInstalled;
}
#endif