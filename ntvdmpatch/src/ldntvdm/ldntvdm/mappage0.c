/*Project: ldntvdm
* Module : mappage0
* Author : leecher@dose.0wnz.at
* Descr. : Tries to use our little driver to Map page 0 to applications running on
*          Windows 8 or above, as OS doesn't allow it there
* Changes: 05.05.2024  - Created
*/

#include <ntstatus.h>
#define WIN32_NO_STATUS
#include "ldntvdm.h"
#include "symcache.h"
#include "reg.h"
#include <winioctl.h>
#include "../../mapzero/mapzero/mapzero.h"
#include "mappage0.h"

#ifdef USE_MAP0DRV
static HANDLE Map0DeviceHandle = INVALID_HANDLE_VALUE;

static BOOL OpenDriver(INITPTRSPARAMS *Params)
{
	if (Map0DeviceHandle == INVALID_HANDLE_VALUE)
	{
		DWORD len;

		Map0DeviceHandle = CreateFile(_T("\\\\.\\MapZeroPage"), 
			GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (Map0DeviceHandle == INVALID_HANDLE_VALUE)
		{
			TRACE("Cannot open MapZeroPage driver: %d", GetLastError());
			return FALSE;
		}
		if (!DeviceIoControl(Map0DeviceHandle, IOCTL_INITPTRS, Params, sizeof(INITPTRSPARAMS), NULL, 0, &len, NULL))
		{
			TRACE("Initializing the MapZero driver failed: %d", GetLastError());
			Map0Page_CloseDriver();
			return FALSE;
		}
	}
	return TRUE;
}

void Map0Page_CloseDriver(void)
{
	if (Map0DeviceHandle != INVALID_HANDLE_VALUE)
	{
		if (CloseHandle(Map0DeviceHandle))
			Map0DeviceHandle = INVALID_HANDLE_VALUE;
	}
}

BOOL Map0Page(IN HANDLE ProcessId)
{
	NTSTATUS Status;
	HKEY hKey;
	DWORD64 dwAddress;
	INITPTRSPARAMS Params;
	DWORD len;

	// Did we look up the pointers?
#ifndef USE_SYMCACHE
	return FALSE;
#endif
	if (!ProcessId)
	{
		TRACE("Process ID %d is invalid?", ProcessId);
		return FALSE;
	}

	if (NT_SUCCESS(Status = REG_OpenLDNTVDM(KEY_READ | KEY_WRITE, &hKey)))
	{
		if (SymCache_GetDLLKey(hKey, L"ntoskrnl.exe", TRUE))
		{
			if ((dwAddress = (DWORD64)SymCache_GetProcAddress(hKey, L"MiAllocateVad")) &&
				(Params.MiAllocateVad = (PVOID)dwAddress) &&
				(dwAddress = (DWORD64)SymCache_GetProcAddress(hKey, L"MiInsertVadCharges")) &&
				(Params.MiInsertVadCharges = (PVOID)dwAddress) &&
				(dwAddress = (DWORD64)SymCache_GetProcAddress(hKey, L"MiInsertVad")) &&
				(Params.MiInsertVad = (PVOID)dwAddress) &&
				(dwAddress = (DWORD64)SymCache_GetProcAddress(hKey, L"MiInsertPrivateVad")))
			{
				Params.MiInsertPrivateVad = (PVOID)dwAddress;
			}
			else
			{
				OutputDebugStringA("NTVDM: Resolving ntoskrnl.exe symbols failed.");
				Status = STATUS_UNSUCCESSFUL;
			}
		}
		else Status = STATUS_UNSUCCESSFUL;
		REG_CloseKey(hKey);
	}
	if (!NT_SUCCESS(Status)) return FALSE;
	if (!OpenDriver(&Params)) return FALSE;

	if (!DeviceIoControl(Map0DeviceHandle, IOCTL_ALLOCPAGE0, &ProcessId, sizeof(ProcessId), NULL, 0, &len, NULL))
	{
		TRACE("Allocating page 0 for Process with PID %d failed: %d", ProcessId, GetLastError());
		return FALSE;
	}

	return TRUE;
}
#endif
