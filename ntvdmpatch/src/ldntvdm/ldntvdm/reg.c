/* Project: ldntvdm
 * Module : reg.c
 * Author : leecher@dose.0wnz.at
 * Descr. : Registry functions for remembering offsets on Windows 7.
 *          We better not use ADVAPI32.DLL as the new injection method is
 *          a bit picky and wants as few module dependencies as possible
 *          Therefore we use native API to accomplis Registry read/write
 * Changes: 06.01.2019  - Created
 */

#define WIN32_NO_STATUS
#include <ntstatus.h>
#include "ldntvdm.h"
#include "ntrtl.h"
#include "ntregapi.h"
#include "Winternl.h"

#if defined(WOW16_SUPPORT)
BOOL REG_CheckForOTVDM(void)
{
	HKEY hKey;
	UNICODE_STRING uStr;
	OBJECT_ATTRIBUTES ObjectAttributes;
	NTSTATUS Status;

	RtlInitUnicodeString(&uStr, L"\\Registry\\Machine\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\NtVdm64\\0OTVDM");
	InitializeObjectAttributes(&ObjectAttributes, &uStr, OBJ_CASE_INSENSITIVE, NULL, NULL);
	Status = NtOpenKey(&hKey, GENERIC_READ, &ObjectAttributes);
	if (NT_SUCCESS(Status))
	{
		NtClose(hKey);
		return TRUE;
	}
	// Also check for installed WOW. If user hasn't installed it, fallback to default behaviour
	RtlInitUnicodeString(&uStr, L"\\Registry\\Machine\\SOFTWARE\\WOW6432Node\\Microsoft\\Windows NT\\CurrentVersion\\WOW");
	InitializeObjectAttributes(&ObjectAttributes, &uStr, OBJ_CASE_INSENSITIVE, NULL, NULL);
	Status = NtOpenKey(&hKey, GENERIC_READ, &ObjectAttributes);
	if (NT_SUCCESS(Status)) 
	{
		NtClose(hKey);
		return FALSE; 
	}
	return TRUE;
}
#endif

#if defined(USE_SYMCACHE)

NTSTATUS REG_OpenLDNTVDM(DWORD dwAccess, PHKEY phKey)
{
	HKEY hKeyCU;
	NTSTATUS Status;
	UNICODE_STRING uStr;
	OBJECT_ATTRIBUTES ObjectAttributes;
	DWORD Disposition;

	if (!(NT_SUCCESS(Status = RtlOpenCurrentUser(MAXIMUM_ALLOWED, &hKeyCU)))) return Status;
	RtlInitUnicodeString(&uStr, L"Software\\ldntvdm");
	InitializeObjectAttributes(&ObjectAttributes, &uStr, OBJ_CASE_INSENSITIVE, hKeyCU, NULL);
	Status = NtCreateKey(phKey, dwAccess, &ObjectAttributes, 0, NULL, REG_OPTION_NON_VOLATILE, &Disposition);
	NtClose(hKeyCU);
	return Status;
}
#endif

NTSTATUS REG_QueryNum(HKEY hKey, LPWSTR lpKey, PBYTE pdwResult, UINT Type)
{
	NTSTATUS Status;
	UNICODE_STRING uStr;
	ULONG ResultLength;
	BYTE Buffer[sizeof(KEY_VALUE_PARTIAL_INFORMATION) + sizeof(ULONGLONG)];
	KEY_VALUE_PARTIAL_INFORMATION *PartialInfo = (KEY_VALUE_PARTIAL_INFORMATION *)Buffer;

	RtlInitUnicodeString(&uStr, lpKey);
	Status = NtQueryValueKey(hKey, &uStr, KeyValuePartialInformation, PartialInfo, sizeof(Buffer), &ResultLength);
	if (NT_SUCCESS(Status) && PartialInfo->Type == Type)
	{
		switch (Type)
		{
		case REG_DWORD: *(PDWORD)pdwResult = *(PDWORD)PartialInfo->Data; break;
		case REG_QWORD: *(PULONGLONG)pdwResult = *(PULONGLONG)PartialInfo->Data; break;
		}
	}
	return Status;
}

NTSTATUS REG_QueryDWORD(HKEY hKey, LPWSTR lpKey, PDWORD pdwResult)
{
	return REG_QueryNum(hKey, lpKey, (PBYTE)pdwResult, REG_DWORD);
}

NTSTATUS REG_QueryQWORD(HKEY hKey, LPWSTR lpKey, PULONGLONG pdwResult)
{
	return REG_QueryNum(hKey, lpKey, (PBYTE)pdwResult, REG_QWORD);
}

NTSTATUS REG_SetDWORD(HKEY hKey, LPWSTR lpKey, DWORD dwData)
{
	UNICODE_STRING uStr;

	RtlInitUnicodeString(&uStr, lpKey);
	return NtSetValueKey(hKey, &uStr, 0, REG_DWORD, &dwData, sizeof(dwData));
}

NTSTATUS REG_SetQWORD(HKEY hKey, LPWSTR lpKey, ULONGLONG dwData)
{
	UNICODE_STRING uStr;

	RtlInitUnicodeString(&uStr, lpKey);
	return NtSetValueKey(hKey, &uStr, 0, REG_QWORD, &dwData, sizeof(dwData));
}

VOID REG_CloseKey(HKEY hKey)
{
	NtClose(hKey);
}


