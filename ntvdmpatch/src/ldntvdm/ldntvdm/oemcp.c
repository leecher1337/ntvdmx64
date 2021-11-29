/*Project: ldntvdm
* Module : oemcp
* Author : leecher@dose.0wnz.at
* Descr. : The retards at Micro$haft recently removed OemCodePageData pointer 
*          from PEB in Windows 11. This leads to a crash in ConHost when using
*          OEM codepage there, because GlyphCP doesn't get initialized due to
*          the lack of PEB->OemCodePageData.
*          This in turn leads to crashes in conhost functions: 
*
*             ConvertOutputToUnicode
*             FalseUnicodeToRealUnicode
*             SB_CharToWcharGlyph
*
*          The practical result is that conhost crashes when i.e. not using raster 
*          font in console, but TTF like "Lucidia Console" and NTVDM application 
*          runs with OEM codepage.
*          Thanks for shipping another buggy Windows version, do you guys even do
*          some basic testing before rolling out the releases?
* Changes: 01.11.2021  - Created
*/
#define WIN32_NO_STATUS
#include <ntstatus.h>
#include "ldntvdm.h"
#include "ntrtl.h"
#include "ntregapi.h"
#include "Winternl.h"
#include "ntpeb.h"
#include "symcache.h"

#define NLS_TABLE_KEY \
        L"\\Registry\\Machine\\System\\CurrentControlSet\\Control\\Nls\\CodePage"

typedef BOOL(*pInitializeCustomCP)();

BOOL GetNlsTablePath(
	UINT CodePage,
	PWCHAR PathBuffer
	)
{
	NTSTATUS NtStatus;
	BOOL Result = FALSE;
	HANDLE RegistryKeyHandle;
	OBJECT_ATTRIBUTES ObjectAttributes;
	UNICODE_STRING UnicodeString;
	PKEY_VALUE_PARTIAL_INFORMATION KeyValueInformation;

	RtlInitUnicodeString(&UnicodeString, NLS_TABLE_KEY);

	InitializeObjectAttributes(&ObjectAttributes,
		&UnicodeString,
		OBJ_CASE_INSENSITIVE,
		NULL,
		NULL);

	NtStatus = NtOpenKey(&RegistryKeyHandle, GENERIC_READ, &ObjectAttributes);

	if (NT_SUCCESS(NtStatus))
	{
		WCHAR ResultBuffer[MAX_PATH + (sizeof(KEY_VALUE_FULL_INFORMATION) / sizeof(WCHAR) + 1)];
		ULONG ValueReturnedLength, BufferSize = sizeof(ResultBuffer);
		WCHAR CodePageStringBuffer[20];
		__swprintf(CodePageStringBuffer, L"%d", CodePage);

		RtlInitUnicodeString(&UnicodeString, CodePageStringBuffer);

		KeyValueInformation = (PKEY_VALUE_PARTIAL_INFORMATION)ResultBuffer;

		NtStatus = NtQueryValueKey(RegistryKeyHandle,
			&UnicodeString,
			KeyValuePartialInformation,
			KeyValueInformation,
			BufferSize,
			&BufferSize);

		if (NT_SUCCESS(NtStatus))
		{
			__swprintf(PathBuffer + GetSystemDirectoryW(PathBuffer, MAX_PATH), 
				L"\\%ws",
				(WCHAR *)&(KeyValueInformation->Data[0]));
			Result = TRUE;
		}
		else
		{
			TRACE("GetNlsTablePath failed to get NLS table\n");
		}

		NtClose(RegistryKeyHandle);
	}
	else
	{
		TRACE("GetNlsTablePath failed to open NLS key\n");
	}

	return(Result);
}


BOOL OEMCP_FixNLSTable(void)
{
	WCHAR wszPath[MAX_PATH];
	PPEB_ARCH Peb = NtCurrentPeb();

	if (!Peb->OemCodePageData && GetNlsTablePath(GetOEMCP(), wszPath))
	{
		HANDLE hFile;
		BOOL bRet = FALSE;

		hFile = CreateFileW(wszPath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

		if (hFile != INVALID_HANDLE_VALUE)
		{
			HANDLE hMap;

			if (hMap = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL))
			{
				if (Peb->OemCodePageData = (PBYTE)MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, 0))
				{
					return TRUE;
				}
				CloseHandle(hMap);
				TRACE("Codepage MapViewOfFile failed: %d\n", GetLastError());
			}
			CloseHandle(hFile);
			TRACE("Codepage CreateFileW failed: %d\n", GetLastError());
		}
	}
	return FALSE;
}

#ifdef _WIN64
BOOL OEMCP_CallInitializeCustomCP()
{
	DWORD64 dwAddress;
	HMODULE nt = GetModuleHandleW(L"conhostV1.dll");
	pInitializeCustomCP fnInitializeCustomCP = NULL;
#ifdef USE_SYMCACHE
	NTSTATUS Status;
	HKEY hKey;

	if (NT_SUCCESS(Status = REG_OpenLDNTVDM(KEY_READ | KEY_WRITE, &hKey)))
	{
		if (SymCache_GetDLLKey(hKey, L"conhostV1.dll", FALSE) &&
			(dwAddress = (DWORD64)SymCache_GetProcAddress(hKey, L"InitializeCustomCP")))
		{
			dwAddress += (DWORD64)nt;
			fnInitializeCustomCP = (pInitializeCustomCP)dwAddress;
		}
		REG_CloseKey(hKey);
	}
#else
	DWORD64 dwBase;
	char szNTDLL[MAX_PATH];

	GetSystemDirectoryA(szNTDLL, sizeof(szNTDLL) / sizeof(szNTDLL[0]));
	strcat(szNTDLL, "\\conhostV1.dll");
	if (SymEng_LoadModule(szNTDLL, &dwBase) == 0)
	{
		if ((dwAddress = SymEng_GetAddr(dwBase, "InitializeCustomCP")))
		{
			dwAddress += (DWORD64)nt;
			fnInitializeCustomCP = (pInitializeCustomCP)dwAddress;
		}
		else
		{
			OutputDebugStringA("NTVDM: Resolving symbols failed.");
		}
		SymEng_UnloadModule(dwBase);
	}
#endif
	if (!nt || !fnInitializeCustomCP) return FALSE;
	return fnInitializeCustomCP();
}
#endif