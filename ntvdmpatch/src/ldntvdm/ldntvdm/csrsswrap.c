/* Project: ldntvdm
 * Module : csrsswrap
 * Author : leecher@dose.0wnz.at
 * Descr. : This module implements wrapping functions to call the CSRSS 
 *          Subsystem on a 64bit Windows machine by wrapping the 32bit
 *          to 64bit calls. This normally is the task of the wow64.dll,
 *          but wrappers for these functions weren't implemented there,
 *          because NTVDM normally isn't available on x64 Windows.
 *          So if Microsoft decides to implement NTVDM on x64, these
 *          wrappers should go into wow64.dll
 * Changes: 01.04.2016  - Created
 */

#ifndef _WIN64
#include "ldntvdm.h"
#include "basemsg64.h"
#include "iathook.h"

#define STARTUP_INFO_RETURNED    0x200

PVOID NTAPI CsrAllocateCaptureBuffer(ULONG ArgumentCount, ULONG BufferSize);
VOID NTAPI CsrFreeCaptureBuffer(struct _CSR_CAPTURE_BUFFER *CaptureBuffer);
ULONG NTAPI CsrAllocateMessagePointer(struct _CSR_CAPTURE_BUFFER *CaptureBuffer, ULONG MessageLength, PVOID *CaptureData);
NTSTATUS NTAPI CsrClientCallServer(struct _CSR_API_MESSAGE *Request, struct _CSR_CAPTURE_BUFFER *CaptureBuffer OPTIONAL,
	ULONG ApiNumber, ULONG RequestLength);
typedef NTSTATUS (NTAPI *fpCsrClientCallServer)(struct _CSR_API_MESSAGE *Request, struct _CSR_CAPTURE_BUFFER *CaptureBuffer OPTIONAL,
	ULONG ApiNumber, ULONG RequestLength);
fpCsrClientCallServer CsrClientCallServerReal = CsrClientCallServer;


NTSTATUS CallBaseUpdateVDMEntry(BASE_API_MSG *m32, CSR_API_NUMBER ApiNumber)
{
	BASE_API_MSG64 m;
	BASE_UPDATE_VDM_ENTRY_MSG64 *b = (BASE_UPDATE_VDM_ENTRY_MSG64*)&m.u.UpdateVDMEntry;
	BASE_UPDATE_VDM_ENTRY_MSG *b32 = (BASE_UPDATE_VDM_ENTRY_MSG*)&m32->u.UpdateVDMEntry;
	NTSTATUS Status;

	b->iTask = b32->iTask;
	b->BinaryType = b32->BinaryType;
	b->ConsoleHandle = (ULONGLONG)b32->ConsoleHandle;
	b->VDMProcessHandle = (ULONGLONG)b32->VDMProcessHandle;
	b->WaitObjectForParent = (ULONGLONG)b32->WaitObjectForParent;
	b->EntryIndex = b32->EntryIndex;
	b->VDMCreationState = b32->VDMCreationState;

	Status = CsrClientCallServer((struct _CSR_API_MESSAGE*)&m, NULL, ApiNumber, sizeof(*b));
	m32->ReturnValue = m.ReturnValue;
	b32->WaitObjectForParent = b->WaitObjectForParent;

	return Status;
}

NTSTATUS CallBaseIsFirstVDM(BASE_API_MSG *m32, CSR_API_NUMBER ApiNumber)
{
	BASE_API_MSG64 m;
	BASE_IS_FIRST_VDM_MSG64 *b = (BASE_IS_FIRST_VDM_MSG64*)&m.u.IsFirstVDM;
	BASE_IS_FIRST_VDM_MSG *b32 = (BASE_IS_FIRST_VDM_MSG*)&m32->u.IsFirstVDM;
	NTSTATUS Status;

	Status = CsrClientCallServer((struct _CSR_API_MESSAGE*)&m, NULL, ApiNumber, sizeof(*b));
	b32->FirstVDM = b->FirstVDM;
	m32->ReturnValue = m.ReturnValue;

	return Status;
}


NTSTATUS CallBaseSetReenterCount(BASE_API_MSG *m32, CSR_API_NUMBER ApiNumber)
{
	BASE_API_MSG64 m;
	BASE_SET_REENTER_COUNT_MSG64 *b = (BASE_SET_REENTER_COUNT_MSG64*)&m.u.SetReenterCount;
	BASE_SET_REENTER_COUNT_MSG *b32 = (BASE_SET_REENTER_COUNT_MSG*)&m32->u.SetReenterCount;
	NTSTATUS Status;

	b->ConsoleHandle = b32->ConsoleHandle;
	b->fIncDec = b32->fIncDec;
	Status = CsrClientCallServer((struct _CSR_API_MESSAGE*)&m, NULL, ApiNumber, sizeof(*b));
	m32->ReturnValue = m.ReturnValue;

	return Status;
}

NTSTATUS CallBaseGetVDMExitCode(BASE_API_MSG *m32, CSR_API_NUMBER ApiNumber)
{
	BASE_API_MSG64 m;
	BASE_GET_VDM_EXIT_CODE_MSG64 *b = (BASE_GET_VDM_EXIT_CODE_MSG64*)&m.u.GetVDMExitCode;
	BASE_GET_VDM_EXIT_CODE_MSG *b32 = (BASE_GET_VDM_EXIT_CODE_MSG*)&m32->u.GetVDMExitCode;
	NTSTATUS Status;

	b->ConsoleHandle = b32->ConsoleHandle;
	b->hParent = b32->hParent;
	Status = CsrClientCallServer((struct _CSR_API_MESSAGE*)&m, NULL, ApiNumber, sizeof(*b));
	m32->ReturnValue = m.ReturnValue;
	b32->ExitCode = b->ExitCode;

	return Status;
}

NTSTATUS CallBaseExitVDM(BASE_API_MSG *m32, CSR_API_NUMBER ApiNumber)
{
	BASE_API_MSG64 m;
	BASE_EXIT_VDM_MSG64 *b = (BASE_EXIT_VDM_MSG64*)&m.u.ExitVDM;
	BASE_EXIT_VDM_MSG *b32 = (BASE_EXIT_VDM_MSG*)&m32->u.ExitVDM;
	NTSTATUS Status;

	b->ConsoleHandle = (ULONGLONG)b32->ConsoleHandle;
	b->iWowTask = b32->iWowTask;
	Status = CsrClientCallServer((struct _CSR_API_MESSAGE*)&m, NULL, ApiNumber, sizeof(*b));
	b32->WaitObjectForVDM = (ULONGLONG)b->WaitObjectForVDM;
	m32->ReturnValue = m.ReturnValue;

	return Status;
}

NTSTATUS CallBaseCheckVDM(BASE_API_MSG *m32, CSR_API_NUMBER ApiNumber)
{
	BASE_API_MSG64 m;
	NTSTATUS Status;
	BASE_CHECKVDM_MSG64 *b = (BASE_CHECKVDM_MSG64*)&m.u.CheckVDM;
	BASE_CHECKVDM_MSG *b32 = (BASE_CHECKVDM_MSG*)&m32->u.CheckVDM;
	PCSR_CAPTURE_HEADER CaptureBuffer;
	ULONG bufPointers, Len;

	/* We unfortunately need to allocate a new capture buffer, as LPSTARTUPINFOA needs more space in 64 bit Windows */
	RtlZeroMemory(&m, sizeof(m));
	b->StdIn = (ULONGLONG)b32->StdIn;
	b->StdOut = (ULONGLONG)b32->StdOut;
	b->StdErr = (ULONGLONG)b32->StdErr;
	b->CurDrive = b32->CurDrive;
	bufPointers = 2; // CmdLine, AppName
	Len = ROUND_UP(b32->CmdLen, 8);
	Len += ROUND_UP(b32->AppLen, 8);
	if (b32->Env)
	{
		Len += ROUND_UP(b32->EnvLen, 8);
		bufPointers++;
	}
	if (b32->CurDirectory)
	{
		Len += ROUND_UP(b32->CurDirectoryLen, 8);
		bufPointers++;
	}
	if (b32->PifFile)
	{
		Len += ROUND_UP(b32->PifLen, 8);
		bufPointers++;
	}
	if (b32->StartupInfo)
	{
		Len += ROUND_UP(sizeof(STARTUPINFOA64), 8);
		bufPointers++;
	}
	if (b32->Desktop)
	{
		Len += ROUND_UP(b32->DesktopLen, 8);
		bufPointers++;
	}
	if (b32->Title)
	{
		Len += ROUND_UP(b32->TitleLen, 8);
		bufPointers++;
	}
	if (b32->Reserved)
	{
		Len += ROUND_UP(b32->ReservedLen, 8);
		bufPointers++;
	}
	if (!(CaptureBuffer = (ULONG_PTR)CsrAllocateCaptureBuffer(bufPointers, Len)))
	{
		SetLastError(ERROR_NOT_ENOUGH_MEMORY);
		return STATUS_NO_MEMORY;
	}
	CsrAllocateMessagePointer(CaptureBuffer, ROUND_UP(b32->CmdLen, 8), (PVOID *)&b->CmdLine);
	b->CmdLen = b32->CmdLen;
	RtlMoveMemory(b->CmdLine, b32->CmdLine, b32->CmdLen);
	CsrAllocateMessagePointer(CaptureBuffer, ROUND_UP(b32->AppLen, 8), (PVOID *)&b->AppName);
	b->AppLen = b32->AppLen;
	RtlMoveMemory(b->AppName, b32->AppName, b32->AppLen);
	if (b32->PifFile)
	{
		CsrAllocateMessagePointer(CaptureBuffer, ROUND_UP(b32->PifLen, 8), (PVOID *)&b->PifFile);
		RtlMoveMemory(b->PifFile, b32->PifFile, b32->PifLen - 1);
		b->PifLen = b32->PifLen;
	}
	if (b32->Env)
	{
		CsrAllocateMessagePointer(CaptureBuffer, ROUND_UP(b32->EnvLen, 8), (PVOID *)&b->Env);
		RtlMoveMemory(b->Env, b32->Env, b32->EnvLen);
		b->EnvLen = b32->EnvLen;
	}
	if (b32->CurDirectory)
	{
		CsrAllocateMessagePointer(CaptureBuffer, ROUND_UP(b32->CurDirectoryLen, 8), (PVOID *)&b->CurDirectory);
		RtlMoveMemory(b->CurDirectory, b32->CurDirectory, b32->CurDirectoryLen);
		b->CurDirectoryLen = b32->CurDirectoryLen;
	}
	if (b32->StartupInfo)
	{
		LPSTARTUPINFOA lpStartupInfo = b32->StartupInfo;
		LPSTARTUPINFOA64 lpStartupInfo64;
		CsrAllocateMessagePointer(CaptureBuffer, ROUND_UP(sizeof(STARTUPINFOA64), 8), (PVOID *)&b->StartupInfo);
		lpStartupInfo64 = b->StartupInfo;
		lpStartupInfo64->dwX = lpStartupInfo->dwX;
		lpStartupInfo64->dwY = lpStartupInfo->dwY;
		lpStartupInfo64->dwXSize = lpStartupInfo->dwXSize;
		lpStartupInfo64->dwYSize = lpStartupInfo->dwYSize;
		lpStartupInfo64->dwXCountChars = lpStartupInfo->dwXCountChars;
		lpStartupInfo64->dwYCountChars = lpStartupInfo->dwYCountChars;
		lpStartupInfo64->dwFillAttribute = lpStartupInfo->dwFillAttribute;
		lpStartupInfo64->dwFlags = lpStartupInfo->dwFlags;
		lpStartupInfo64->wShowWindow = lpStartupInfo->wShowWindow;
		lpStartupInfo64->cb = sizeof(STARTUPINFOA64);
	}
	if (b32->Desktop)
	{
		CsrAllocateMessagePointer(CaptureBuffer, ROUND_UP(b32->DesktopLen, 8), (PVOID *)&b->Desktop);
		RtlMoveMemory(b->Desktop, b32->Desktop, b32->DesktopLen);
		b->DesktopLen = b32->DesktopLen;
	}
	if (b32->Title)
	{
		CsrAllocateMessagePointer(CaptureBuffer, ROUND_UP(b32->TitleLen, 8), (PVOID *)&b->Title);
		RtlMoveMemory(b->Title, b32->Title, b32->TitleLen);
		b->TitleLen = b32->TitleLen;
	}
	if (b32->Reserved)
	{
		CsrAllocateMessagePointer(CaptureBuffer, ROUND_UP(b32->ReservedLen, 8), (PVOID *)&b->Reserved);
		RtlMoveMemory(b->Reserved, b32->Reserved, b32->ReservedLen);
		b->ReservedLen = b32->ReservedLen;
	}
	b->ConsoleHandle = b32->ConsoleHandle;
	b->VDMState = b32->VDMState;
	b->BinaryType = b32->BinaryType;
	b->CodePage = b32->CodePage;
	b->dwCreationFlags = b32->dwCreationFlags;
	b->iTask = b32->iTask;

	Status = CsrClientCallServer(&m, CaptureBuffer, ApiNumber, sizeof(*b));

	m32->ReturnValue = m.ReturnValue;
	b32->iTask = b->iTask;
	b32->VDMState = b->VDMState;
	b32->WaitObjectForParent = b->WaitObjectForParent;

	CsrFreeCaptureBuffer(CaptureBuffer);

	return Status;
}

NTSTATUS CallBaseGetNextVDMCommand(BASE_API_MSG *m32, CSR_API_NUMBER ApiNumber)
{
	NTSTATUS Status;
	BASE_API_MSG64 m;
	BASE_GET_NEXT_VDM_COMMAND_MSG64 *b = (BASE_GET_NEXT_VDM_COMMAND_MSG64*)&m.u.GetNextVDMCommand;
	BASE_GET_NEXT_VDM_COMMAND_MSG *b32 = (BASE_GET_NEXT_VDM_COMMAND_MSG*)&m32->u.GetNextVDMCommand;
	PCSR_CAPTURE_HEADER CaptureBuffer;
	ULONG bufPointers, Len;

	/* We unfortunately need to allocate a new capture buffer, as LPSTARTUPINFOA needs more space in 64 bit Windows */
	RtlZeroMemory(&m, sizeof(m));
	b->iTask = b32->iTask;
	b->ConsoleHandle = (ULONGLONG)b32->ConsoleHandle;
	b->WaitObjectForVDM = (ULONGLONG)b32->WaitObjectForVDM;
	b->StdIn = (ULONGLONG)b32->StdIn;
	b->StdOut = (ULONGLONG)b32->StdOut;
	b->StdErr = (ULONGLONG)b32->StdErr;
	b->CodePage = b32->CodePage;
	b->dwCreationFlags = b32->dwCreationFlags;
	b->ExitCode = b32->ExitCode;
	b->CurrentDrive = b32->CurrentDrive;
	b->VDMState = b32->VDMState;
	b->fComingFromBat = b32->fComingFromBat;

	bufPointers = 2; // CmdLine, AppName
	Len = ROUND_UP(b32->CmdLen, 8);
	Len += ROUND_UP(b32->AppLen, 8);
	if (b32->Env)
	{
		Len += ROUND_UP(b32->EnvLen, 8);
		bufPointers++;
	}
	if (b32->CurDirectory)
	{
		Len += ROUND_UP(b32->CurDirectoryLen, 8);
		bufPointers++;
	}
	if (b32->PifFile)
	{
		Len += ROUND_UP(b32->PifLen, 8);
		bufPointers++;
	}
	if (b32->StartupInfo)
	{
		Len += ROUND_UP(sizeof(STARTUPINFOA64), 8);
		bufPointers++;
	}
	if (b32->Desktop)
	{
		Len += ROUND_UP(b32->DesktopLen, 8);
		bufPointers++;
	}
	if (b32->Title)
	{
		Len += ROUND_UP(b32->TitleLen, 8);
		bufPointers++;
	}
	if (b32->Reserved)
	{
		Len += ROUND_UP(b32->ReservedLen, 8);
		bufPointers++;
	}
	if (!(CaptureBuffer = (ULONG_PTR)CsrAllocateCaptureBuffer(bufPointers, Len)))
	{
		SetLastError(ERROR_NOT_ENOUGH_MEMORY);
		return STATUS_NO_MEMORY;
	}
	CsrAllocateMessagePointer(CaptureBuffer, ROUND_UP(b32->CmdLen, 8), (PVOID *)&b->CmdLine);
	b->CmdLen = b32->CmdLen;
	CsrAllocateMessagePointer(CaptureBuffer, ROUND_UP(b32->AppLen, 8), (PVOID *)&b->AppName);
	b->AppLen = b32->AppLen;
	if (b32->PifFile)
	{
		CsrAllocateMessagePointer(CaptureBuffer, ROUND_UP(b32->PifLen, 8), (PVOID *)&b->PifFile);
		RtlMoveMemory(b->PifFile, b32->PifFile, b32->PifLen - 1);
		b->PifLen = b32->PifLen;
	}
	if (b32->Env)
	{
		CsrAllocateMessagePointer(CaptureBuffer, ROUND_UP(b32->EnvLen, 8), (PVOID *)&b->Env);
		RtlMoveMemory(b->Env, b32->Env, b32->EnvLen);
		b->EnvLen = b32->EnvLen;
	}
	if (b32->CurDirectory)
	{
		CsrAllocateMessagePointer(CaptureBuffer, ROUND_UP(b32->CurDirectoryLen, 8), (PVOID *)&b->CurDirectory);
		RtlMoveMemory(b->CurDirectory, b32->CurDirectory, b32->CurDirectoryLen);
		b->CurDirectoryLen = b32->CurDirectoryLen;
	}
	if (b32->StartupInfo)
	{
		CsrAllocateMessagePointer(CaptureBuffer, ROUND_UP(sizeof(STARTUPINFOA64), 8), (PVOID *)&b->StartupInfo);
	}
	if (b32->Desktop)
	{
		CsrAllocateMessagePointer(CaptureBuffer, ROUND_UP(b32->DesktopLen, 8), (PVOID *)&b->Desktop);
		RtlMoveMemory(b->Desktop, b32->Desktop, b32->DesktopLen);
		b->DesktopLen = b32->DesktopLen;
	}
	if (b32->Title)
	{
		CsrAllocateMessagePointer(CaptureBuffer, ROUND_UP(b32->TitleLen, 8), (PVOID *)&b->Title);
		RtlMoveMemory(b->Title, b32->Title, b32->TitleLen);
		b->TitleLen = b32->TitleLen;
	}
	if (b32->Reserved)
	{
		CsrAllocateMessagePointer(CaptureBuffer, ROUND_UP(b32->ReservedLen, 8), (PVOID *)&b->Reserved);
		RtlMoveMemory(b->Reserved, b32->Reserved, b32->ReservedLen);
		b->ReservedLen = b32->ReservedLen;
	}

	Status = CsrClientCallServer(&m, CaptureBuffer, ApiNumber, sizeof(*b));

	m32->ReturnValue = m.ReturnValue;
	if (b->VDMState & STARTUP_INFO_RETURNED)
	{
		LPSTARTUPINFOA64 lpsi = (LPSTARTUPINFOA64)b->StartupInfo;

		b32->StartupInfo->cb = sizeof(STARTUPINFOA);
		b32->StartupInfo->lpReserved = lpsi->lpReserved;
		b32->StartupInfo->lpDesktop = lpsi->lpDesktop;
		b32->StartupInfo->lpTitle = lpsi->lpTitle;
		b32->StartupInfo->dwX = lpsi->dwX;
		b32->StartupInfo->dwY = lpsi->dwY;
		b32->StartupInfo->dwXSize = lpsi->dwXSize;
		b32->StartupInfo->dwYSize = lpsi->dwYSize;
		b32->StartupInfo->dwXCountChars = lpsi->dwXCountChars;
		b32->StartupInfo->dwYCountChars = lpsi->dwYCountChars;
		b32->StartupInfo->dwFillAttribute = lpsi->dwFillAttribute;
		b32->StartupInfo->dwFlags = lpsi->dwFlags;
		b32->StartupInfo->wShowWindow = lpsi->wShowWindow;
		b32->StartupInfo->cbReserved2 = lpsi->cbReserved2;
		b32->StartupInfo->lpReserved2 = lpsi->lpReserved2;
		b32->StartupInfo->hStdInput = lpsi->hStdInput;
		b32->StartupInfo->hStdOutput = lpsi->hStdOutput;
		b32->StartupInfo->hStdError = lpsi->hStdError;
	}

	if (NT_SUCCESS(Status))
	{
		if (b32->CmdLen) RtlMoveMemory(b32->CmdLine, b->CmdLine, b->CmdLen);
		if (b32->AppLen) RtlMoveMemory(b32->AppName, b->AppName, b->AppLen);
		if (b32->PifLen) RtlMoveMemory(b32->PifFile, b->PifFile, b->PifLen);
		if (b32->EnvLen) RtlMoveMemory(b32->Env, b->Env, b->EnvLen);
		if (b32->CurDirectoryLen) RtlMoveMemory(b32->CurDirectory, b->CurDirectory, b->CurDirectoryLen);
		if (b32->DesktopLen) RtlMoveMemory(b32->Desktop, b->Desktop, b->DesktopLen);
		if (b32->TitleLen) RtlMoveMemory(b32->Title, b->Title, b->TitleLen);
		if (b32->ReservedLen) RtlMoveMemory(b32->Reserved, b->Reserved, b->ReservedLen);
	}

	b32->CmdLen = b->CmdLen;
	b32->AppLen = b->AppLen;
	b32->PifLen = b->PifLen;
	b32->EnvLen = b->EnvLen;
	b32->CurDirectoryLen = b->CurDirectoryLen;
	b32->DesktopLen = b->DesktopLen;
	b32->TitleLen = b->TitleLen;
	b32->ReservedLen = b->ReservedLen;

	b32->iTask = b->iTask;
	b32->ConsoleHandle = b->ConsoleHandle;
	b32->WaitObjectForVDM = b->WaitObjectForVDM;
	b32->StdIn = b->StdIn;
	b32->StdOut = b->StdOut;
	b32->StdErr = b->StdErr;
	b32->CodePage = b->CodePage;
	b32->dwCreationFlags = b->dwCreationFlags;
	b32->ExitCode = b->ExitCode;
	b32->CurrentDrive = b->CurrentDrive;
	b32->VDMState = b->VDMState;
	b32->fComingFromBat = b->fComingFromBat;

	CsrFreeCaptureBuffer(CaptureBuffer);

	return Status;
}


NTSTATUS NTAPI myCsrClientCallServer(BASE_API_MSG *m, PCSR_CAPTURE_HEADER CaptureHeader, CSR_API_NUMBER ApiNumber, ULONG ArgLength)
{
	char szMsg[256];
	NTSTATUS Status;

	if (m && CSR_APINUMBER_TO_SERVERDLLINDEX(ApiNumber) == BASESRV_SERVERDLL_INDEX)
	{
		switch (CSR_APINUMBER_TO_APITABLEINDEX(ApiNumber))
		{
		case BasepExitVDM:
			return CallBaseExitVDM(m, ApiNumber);
		case BasepIsFirstVDM:
			return CallBaseIsFirstVDM(m, ApiNumber);
		case BasepGetVDMExitCode:
			return CallBaseGetVDMExitCode(m, ApiNumber);
		case BasepSetReenterCount:
			return CallBaseSetReenterCount(m, ApiNumber);
		case BasepCheckVDM:
			return CallBaseCheckVDM(m, ApiNumber);
		case BasepUpdateVDMEntry:
			return CallBaseUpdateVDMEntry(m, ApiNumber);
		case BasepGetNextVDMCommand:
			return CallBaseGetNextVDMCommand(m, ApiNumber);
		}
	}
	Status = CsrClientCallServerReal(m, CaptureHeader, ApiNumber, ArgLength);
	wsprintfA(szMsg, "Unpatched CSR call %d returns %08X", CSR_APINUMBER_TO_APITABLEINDEX(ApiNumber), Status);
	OutputDebugStringA(szMsg);
	return Status;
}

/* wow64!whNtWow64CsrAllocateMessagePointer just pipes through our call to x64 end, but this assumes that destination
 * pointers are 64bit whereas in 32bit they are only DWORDs which leads to memory overwrites in the 32bit 
 * structures being allocated. Therefore we need to catch these calls too and ensure that they do not overwrite
 * anything
 */
ULONG NTAPI myCsrAllocateMessagePointer(struct _CSR_CAPTURE_BUFFER *CaptureBuffer, ULONG MessageLength, PVOID *CaptureData)
{
	ULONGLONG CapturePtr;
	ULONG Status;

	Status = CsrAllocateMessagePointer(CaptureBuffer, MessageLength, &CapturePtr);
	*((ULONG*)CaptureData) = (ULONG)CapturePtr;
	return Status;
}

void HookCsrClientCallServer()
{
	Hook_IAT_x64_IAT((LPBYTE)GetModuleHandle(_T("kernel32.dll")), "ntdll.dll", "CsrClientCallServer", myCsrClientCallServer, &CsrClientCallServerReal);
	Hook_IAT_x64_IAT((LPBYTE)GetModuleHandle(_T("kernel32.dll")), "ntdll.dll", "CsrAllocateMessagePointer", myCsrAllocateMessagePointer, NULL);
}

#endif