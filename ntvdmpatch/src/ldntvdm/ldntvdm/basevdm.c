/*Project: ldntvdm
* Module : basevdm
* Author : (C) 1989-1993 Microsoft Corporation, leecher@dose.0wnz.at
* Descr. : This module contains the vdm-functions that are not
*          present in kernel32 in x64 on Windows XP
*          Fortunately, they are available in later Windows versions
* Changes: 31.05.2020  - Created
*/

#include <ntstatus.h>
#define WIN32_NO_STATUS
#include "ldntvdm.h"
#include "ntrtl.h"
#include "ntregapi.h"
#include "xpwrap.h"
#include "basevdm.h"
#include "csrsswrap.h"
#ifdef NEED_BASEVDM

#define MAXIMUM_VDM_COMMAND_LENGTH	128
#define MAXIMUM_VDM_ENVIRONMENT 	32*1024
#define MAXIMUM_VDM_CURRENT_DIR 	64

#define ENV_NAME_TYPE_NO_PATH		1
#define ENV_NAME_TYPE_SINGLE_PATH	2
#define ENV_NAME_TYPE_MULTIPLE_PATH	3


#define CONSOLE_HANDLE_SIGNATURE 0x00000003
#define CONSOLE_HANDLE(HANDLE) (((ULONG)(HANDLE) & CONSOLE_HANDLE_SIGNATURE) == CONSOLE_HANDLE_SIGNATURE)


#define MAX_VDM_CFG_LINE   256

#define FULL_INFO_BUFFER_SIZE (sizeof(KEY_VALUE_FULL_INFORMATION) + MAX_VDM_CFG_LINE)

#define STD_ENV_NAME_COUNT	    5

// Define environment variables we need to convert their values to
// short path name in BaseCreateVdmEnvironment function.
#define ENV_NAME_PATH		L"PATH"
#define ENV_NAME_PATH_LEN	4

#define ENV_NAME_WINDIR 	L"WINDIR"
#define ENV_NAME_WINDIR_LEN	6

#define ENV_NAME_SYSTEMROOT	L"SYSTEMROOT"
#define ENV_NAME_SYSTEMROOT_LEN 10

#define ENV_NAME_TEMP		L"TEMP"
#define ENV_NAME_TEMP_LEN	4

#define ENV_NAME_TMP		L"TMP"
#define ENV_NAME_TMP_LEN	3

#define ENV_NAME_TYPE_NO_PATH		1
#define ENV_NAME_TYPE_SINGLE_PATH	2
#define ENV_NAME_TYPE_MULTIPLE_PATH	3

typedef struct	_ENV_INFO {
	UINT    NameType;
	UINT    NameLength;
	WCHAR   *Name;
} ENV_INFO, *PENV_INFO;


//
// Message format for messages sent from the client to the server
//


typedef BASE_API_MSG *PBASE_API_MSG;
typedef BASE_CHECKVDM_MSG *PBASE_CHECKVDM_MSG;
typedef BASE_UPDATE_VDM_ENTRY_MSG *PBASE_UPDATE_VDM_ENTRY_MSG;



NTSYSCALLAPI NTSTATUS NTAPI NtOpenKey(PHANDLE KeyHandle, ACCESS_MASK DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes);
NTSYSAPI
NTSTATUS
NTAPI
NtQueryInformationToken(
	IN HANDLE TokenHandle,
	IN TOKEN_INFORMATION_CLASS TokenInformationClass,
	OUT PVOID TokenInformation,
	IN ULONG TokenInformationLength,
	OUT PULONG ReturnLength
	);


NTSTATUS NTAPI BaseGetVdmLuid(HANDLE TokenHandle, LUID *AuthenticationId)
{
	NTSTATUS Status;
	TOKEN_STATISTICS *Statistics;
	ULONG ReturnLength;

	Status = NtQueryInformationToken(TokenHandle, TokenStatistics, NULL, 0, &ReturnLength);
	if (Status == STATUS_BUFFER_TOO_SMALL)
	{
		Statistics = (TOKEN_STATISTICS *)RtlAllocateHeap(RtlProcessHeap(), HEAP_CREATE_ENABLE_TRACING, ReturnLength);
		if (Statistics)
		{
			Status = NtQueryInformationToken(TokenHandle, TokenStatistics, Statistics, ReturnLength, &ReturnLength);
			if (NT_SUCCESS(Status))
			{
				if (AuthenticationId)
					*AuthenticationId = Statistics->AuthenticationId;
			}
			RtlFreeHeap(RtlProcessHeap(), 0, Statistics);
		}
		else
		{
			Status = STATUS_BUFFER_TOO_SMALL;
		}
	}
	return Status;
}

NTSTATUS
BaseCheckVDMp(
	IN  ULONG BinaryType,
	IN  PCWCH lpApplicationName,
	IN  PCWCH lpCommandLine,
	IN  PCWCH lpCurrentDirectory,
	IN  ANSI_STRING *pAnsiStringEnv,
	IN  PBASE_API_MSG m,
	IN OUT PULONG iTask,
	IN  DWORD dwCreationFlags,
	LPSTARTUPINFOW lpStartupInfo,
	IN HANDLE hUserToken OPTIONAL
	)
{

	NTSTATUS Status = STATUS_UNSUCCESSFUL;
	PBASE_CHECKVDM_MSG b = (PBASE_CHECKVDM_MSG)&m->u.CheckVDM;
	PCSR_CAPTURE_HEADER CaptureBuffer;
	ANSI_STRING AnsiStringCurrentDir, AnsiStringDesktop;
	ANSI_STRING AnsiStringReserved, AnsiStringPif;
	OEM_STRING OemStringCmd, OemStringAppName, OemStringTitle;
	UNICODE_STRING UnicodeString;
	PCHAR pch, Buffer = NULL;
	ULONG Len;
	ULONG bufPointers;
	LPWSTR wsBuffer;
	LPWSTR wsAppName;
	LPWSTR wsPifName;
	LPWSTR wsCmdLine;
	LPWSTR wsPif = (PWSTR)".\0p\0i\0f\0\0";    // L".pif"
	LPWSTR wsSharedWowPif = L"wowexec.pif";
	PWCHAR pwch;
	BOOLEAN bNewConsole;
	DWORD   dw, dwTotal, Length;
	WCHAR   wchBuffer[MAX_PATH + 1];
	ULONG BinarySubType;
	LPWSTR lpAllocatedReserved = NULL;
	LUID AuthenticationId = {0};
	DWORD   HandleFlags;
	xpPRTL_USER_PROCESS_PARAMETERS ProcessParameters;

	if (!ARGUMENT_PRESENT(pAnsiStringEnv) ||
		pAnsiStringEnv->Length > MAXIMUM_VDM_ENVIRONMENT) {
		BaseSetLastNTError(STATUS_INVALID_PARAMETER);
		return STATUS_INVALID_PARAMETER;
	}

	wsCmdLine = wsAppName = NULL;
	OemStringCmd.Buffer = NULL;
	OemStringAppName.Buffer = NULL;
	AnsiStringCurrentDir.Buffer = NULL;
	AnsiStringDesktop.Buffer = NULL;
	AnsiStringPif.Buffer = NULL;
	OemStringTitle.Buffer = NULL;
	AnsiStringReserved.Buffer = NULL;
	wsBuffer = NULL;
	wsPifName = NULL;

	BinarySubType = BinaryType & BINARY_SUBTYPE_MASK;
	BinaryType = BinaryType & ~BINARY_SUBTYPE_MASK;
	ProcessParameters = (xpPRTL_USER_PROCESS_PARAMETERS)NtCurrentPeb()->ProcessParameters;
	bNewConsole = !ProcessParameters->ConsoleHandle ||
		(dwCreationFlags & CREATE_NEW_CONSOLE);

		if (BinaryType == BINARY_TYPE_DOS) {

			if (lpStartupInfo && lpStartupInfo->dwFlags & STARTF_USESTDHANDLES) {
				b->StdIn = lpStartupInfo->hStdInput;
				b->StdOut = lpStartupInfo->hStdOutput;
				b->StdErr = lpStartupInfo->hStdError;

			}
			else {
				b->StdIn = ProcessParameters->StandardInput;
				b->StdOut = ProcessParameters->StandardOutput;
				b->StdErr = ProcessParameters->StandardError;

				if (ProcessParameters->WindowFlags &
					(STARTF_USEHOTKEY | STARTF_HASSHELLDATA)) {

					if (b->StdIn && !CONSOLE_HANDLE(b->StdIn) &&
						!GetHandleInformation(b->StdIn, &HandleFlags))
						b->StdIn = 0;
					if (b->StdOut && !CONSOLE_HANDLE(b->StdOut) &&
						!GetHandleInformation(b->StdOut, &HandleFlags)) {
						if (b->StdErr == b->StdOut)
							b->StdErr = 0;
						b->StdOut = 0;
					}
					if (b->StdErr && b->StdErr != b->StdOut &&
						!CONSOLE_HANDLE(b->StdErr) &&
						!GetHandleInformation(b->StdErr, &HandleFlags))
						b->StdErr = 0;
				}
			}
			if (CONSOLE_HANDLE((b->StdIn)))
				b->StdIn = 0;

			if (CONSOLE_HANDLE((b->StdOut)))
				b->StdOut = 0;

			if (CONSOLE_HANDLE((b->StdErr)))
				b->StdErr = 0;
		}


		if (BinaryType == BINARY_TYPE_SEPWOW) {
			bNewConsole = TRUE;
		}

		while (*lpApplicationName == (WCHAR)' ' || *lpApplicationName == (WCHAR)'\t') {
			lpApplicationName++;
		}

		Len = wcslen(lpApplicationName);
		dwTotal = Len + 1 + MAX_PATH;
		wsAppName = RtlAllocateHeap(RtlProcessHeap(),
			MAKE_TAG(VDM_TAG),
			dwTotal * sizeof(WCHAR)
			);
		if (wsAppName == NULL) {
			Status = STATUS_NO_MEMORY;
			goto BCVTryExit;
		}

		dw = GetShortPathNameW(lpApplicationName, wsAppName, dwTotal);

		if (0 == dw || dw > dwTotal) {
			Status = STATUS_OBJECT_PATH_INVALID;
			goto BCVTryExit;
		}

		RtlInitUnicodeString(&UnicodeString, wsAppName);
		Status = RtlUnicodeStringToOemString(&OemStringAppName,
			&UnicodeString,
			TRUE
			);
		if (!NT_SUCCESS(Status)) {
			goto BCVTryExit;
		}


		dw = OemStringAppName.Length;
		pch = OemStringAppName.Buffer;
		Length = 1;
		while (dw-- && *pch != '.') {
			if (*pch == '\\') {
				Length = 1;
			}
			else {
				Length++;
			}
			pch++;
		}

		Len = wcslen(lpApplicationName);

		if (L'"' == lpCommandLine[0]) {

			pwch = (LPWSTR)&lpCommandLine[1];
			while (*pwch && L'"' != *pwch++) {
				;
			}

		}
		else if (Len <= wcslen(lpCommandLine) &&
			0 == __wcsnicmp(lpApplicationName, lpCommandLine, Len)) {

			pwch = (LPWSTR)lpCommandLine + Len;
		}
		else {
			pwch = (LPWSTR)lpCommandLine;

			while (*pwch != UNICODE_NULL &&
				(*pwch == (WCHAR) ' ' || *pwch == (WCHAR) '\t')) {
				pwch++;
			}

			if (*pwch == (WCHAR) '\"') {
				pwch++;
				while (*pwch && *pwch++ != '\"') {
					;
				}
			}
			else {
				while (*pwch && *pwch != ' ' && *pwch != '\t') {
					pwch++;
				}
			}
		}

		while (*pwch && (L' ' == *pwch || L'\t' == *pwch)) {
			pwch++;
		}

		wsCmdLine = pwch;
		dw = wcslen(wsCmdLine);

		UnicodeString.Length = (USHORT)(dw * sizeof(WCHAR));
		UnicodeString.MaximumLength = UnicodeString.Length + sizeof(WCHAR);
		UnicodeString.Buffer = wsCmdLine;
		Status = RtlUnicodeStringToOemString(
			&OemStringCmd,
			&UnicodeString,
			TRUE);

		if (!NT_SUCCESS(Status)) {
			goto BCVTryExit;
		}

		if (OemStringCmd.Length >= MAXIMUM_VDM_COMMAND_LENGTH - Length) {
			Status = STATUS_INVALID_PARAMETER;
			goto BCVTryExit;
		}

		wsBuffer = RtlAllocateHeap(RtlProcessHeap(), MAKE_TAG(VDM_TAG), MAX_PATH*sizeof(WCHAR));
		if (!wsBuffer) {
			Status = STATUS_NO_MEMORY;
			goto BCVTryExit;
		}

		wsPifName = RtlAllocateHeap(RtlProcessHeap(), MAKE_TAG(VDM_TAG), MAX_PATH*sizeof(WCHAR));
		if (!wsPifName) {
			Status = STATUS_NO_MEMORY;
			goto BCVTryExit;
		}

		if (BinaryType == BINARY_TYPE_WIN16) {
			wcscpy(wsBuffer, wsSharedWowPif);
			Len = 0;
		}
		else {
			wcscpy(wsBuffer, lpApplicationName);

			pwch = _wcsrchr(wsBuffer, (WCHAR)'.');
			if (pwch == NULL) {
				Status = STATUS_INVALID_PARAMETER;
				goto BCVTryExit;
			}
			wcscpy(pwch, wsPif);
			Len = GetFileAttributesW(wsBuffer);
			if (Len == (DWORD)(-1) || (Len & FILE_ATTRIBUTE_DIRECTORY)) {
				Len = 0;
			}
			else {
				Len = wcslen(wsBuffer) + 1;
				_wcsncpy(wsPifName, wsBuffer, Len);
			}
		}

		if (!Len) {
			pwch = _wcsrchr(wsBuffer, (WCHAR)'\\');
			if (!pwch) {
				pwch = _wcsrchr(wsBuffer, (WCHAR)':');
			}

			if (pwch++) {
				while (*pwch != UNICODE_NULL &&
					*pwch != (WCHAR)' '   && *pwch != (WCHAR)'\t')
				{
					wsBuffer[Len++] = *pwch++;
				}
				wsBuffer[Len] = UNICODE_NULL;
			}

			if (Len) {
				Len = SearchPathW(
					NULL,
					wsBuffer,
					wsPif,              // L".pif"
					MAX_PATH,
					wsPifName,
					NULL
					);
				if (Len >= MAX_PATH) {
					Status = STATUS_NO_MEMORY;
					goto BCVTryExit;
				}
			}
		}

		if (!Len)
			*wsPifName = UNICODE_NULL;



		if (!ARGUMENT_PRESENT(lpCurrentDirectory)) {

			dw = RtlGetCurrentDirectory_U(sizeof(wchBuffer), wchBuffer);

			wchBuffer[dw / sizeof(WCHAR)] = UNICODE_NULL;
			dw = GetShortPathNameW(wchBuffer,
				wchBuffer,
				sizeof(wchBuffer) / sizeof(WCHAR)
				);
			if (dw > sizeof(wchBuffer) / sizeof(WCHAR))
				goto BCVTryExit;

			else if (dw == 0) {
				RtlInitUnicodeString(&UnicodeString, wchBuffer);
				dw = UnicodeString.Length / sizeof(WCHAR);
			}
			else {
				UnicodeString.Length = (USHORT)(dw * sizeof(WCHAR));
				UnicodeString.Buffer = wchBuffer;
				UnicodeString.MaximumLength = (USHORT)sizeof(wchBuffer);
			}
			if (dw - 2 <= MAXIMUM_VDM_CURRENT_DIR) {
				Status = RtlUnicodeStringToAnsiString(
					&AnsiStringCurrentDir,
					&UnicodeString,
					TRUE
					);
			}
			else {
				Status = STATUS_INVALID_PARAMETER;
				goto BCVTryExit;
			}

			if (!NT_SUCCESS(Status)) {
				goto BCVTryExit;
			}
		}
		else {

			dw = GetFullPathNameW(lpCurrentDirectory,
				sizeof(wchBuffer) / sizeof(WCHAR),
				wchBuffer,
				NULL);
			if (0 != dw && dw <= sizeof(wchBuffer) / sizeof(WCHAR)) {
				dw = GetShortPathNameW(wchBuffer,
					wchBuffer,
					sizeof(wchBuffer) / sizeof(WCHAR));
			}
			if (dw > sizeof(wchBuffer) / sizeof(WCHAR))
				goto BCVTryExit;

			if (dw != 0) {
				UnicodeString.Buffer = wchBuffer;
				UnicodeString.Length = (USHORT)(dw * sizeof(WCHAR));
				UnicodeString.MaximumLength = sizeof(wchBuffer);
			}
			else
				RtlInitUnicodeString(&UnicodeString, lpCurrentDirectory);

			Status = RtlUnicodeStringToAnsiString(
				&AnsiStringCurrentDir,
				&UnicodeString,
				TRUE);

			if (!NT_SUCCESS(Status)) {
				goto BCVTryExit;
			}

			if ((AnsiStringCurrentDir.Length - 2) > MAXIMUM_VDM_CURRENT_DIR) {
				Status = STATUS_INVALID_PARAMETER;
				goto BCVTryExit;
			}
		}

		if (AnsiStringCurrentDir.Buffer[0] <= 'Z')
			b->CurDrive = AnsiStringCurrentDir.Buffer[0] - 'A';
		else
			b->CurDrive = AnsiStringCurrentDir.Buffer[0] - 'a';

		if (lpStartupInfo && lpStartupInfo->dwFlags & STARTF_USEHOTKEY) {

			DWORD cbAlloc = sizeof(WCHAR) *
				(20 +
					(lpStartupInfo->lpReserved
						? wcslen(lpStartupInfo->lpReserved)
						: 0
						) +
					1
					);


			lpAllocatedReserved = RtlAllocateHeap(RtlProcessHeap(),
				MAKE_TAG(VDM_TAG),
				cbAlloc
				);
			if (lpAllocatedReserved) {

				__swprintf(lpAllocatedReserved,
					L"hotkey.%u %s",
					HandleToUlong(lpStartupInfo->hStdInput),
					lpStartupInfo->lpReserved ? lpStartupInfo->lpReserved : L""
					);

				lpStartupInfo->dwFlags &= ~STARTF_USEHOTKEY;
				lpStartupInfo->hStdInput = 0;
				lpStartupInfo->lpReserved = lpAllocatedReserved;

			}

		}


		bufPointers = 2;
		Len = ROUND_UP((OemStringCmd.Length + 3), 4);

		Len += ROUND_UP((OemStringAppName.Length + 1), 4);

		if (pAnsiStringEnv->Length) {
			bufPointers++;
			Len += ROUND_UP(pAnsiStringEnv->Length, 4);
		}

		if (AnsiStringCurrentDir.Length) {
			bufPointers++;
			Len += ROUND_UP((AnsiStringCurrentDir.Length + 1), 4); // 1 for NULL
		}


		if (wsPifName && *wsPifName != UNICODE_NULL) {
			bufPointers++;
			RtlInitUnicodeString(&UnicodeString, wsPifName);
			Status = RtlUnicodeStringToAnsiString(&AnsiStringPif,
				&UnicodeString,
				TRUE
				);
			if (!NT_SUCCESS(Status)) {
				goto BCVTryExit;
			}

			Len += ROUND_UP((AnsiStringPif.Length + 1), 4);
		}

		if (lpStartupInfo) {
			Len += ROUND_UP(sizeof(STARTUPINFOA), 4);
			bufPointers++;
			if (lpStartupInfo->lpDesktop) {
				bufPointers++;
				RtlInitUnicodeString(&UnicodeString, lpStartupInfo->lpDesktop);
				Status = RtlUnicodeStringToAnsiString(
					&AnsiStringDesktop,
					&UnicodeString,
					TRUE);

				if (!NT_SUCCESS(Status)) {
					goto BCVTryExit;
				}
				Len += ROUND_UP((AnsiStringDesktop.Length + 1), 4);
			}

			if (lpStartupInfo->lpTitle) {
				bufPointers++;
				RtlInitUnicodeString(&UnicodeString, lpStartupInfo->lpTitle);
				Status = RtlUnicodeStringToOemString(
					&OemStringTitle,
					&UnicodeString,
					TRUE);

				if (!NT_SUCCESS(Status)) {
					goto BCVTryExit;
				}
				Len += ROUND_UP((OemStringTitle.Length + 1), 4);
			}

			if (lpStartupInfo->lpReserved) {
				bufPointers++;
				RtlInitUnicodeString(&UnicodeString, lpStartupInfo->lpReserved);
				Status = RtlUnicodeStringToAnsiString(
					&AnsiStringReserved,
					&UnicodeString,
					TRUE);

				if (!NT_SUCCESS(Status)) {
					goto BCVTryExit;
				}
				Len += ROUND_UP((AnsiStringReserved.Length + 1), 4);
			}
			if (hUserToken) {
				bufPointers++;
				if (NT_SUCCESS(BaseGetVdmLuid(hUserToken, &AuthenticationId)))
					Len += sizeof(AuthenticationId);
			}
		}

		CaptureBuffer = CsrAllocateCaptureBuffer(bufPointers, Len);
		if (CaptureBuffer == NULL) {
			Status = STATUS_NO_MEMORY;
			goto BCVTryExit;
		}

		myCsrAllocateMessagePointer(CaptureBuffer,
			ROUND_UP((OemStringCmd.Length + 3), 4),
			(PVOID *)&b->CmdLine
			);

		RtlMoveMemory(b->CmdLine, OemStringCmd.Buffer, OemStringCmd.Length);
		b->CmdLine[OemStringCmd.Length] = 0xd;
		b->CmdLine[OemStringCmd.Length + 1] = 0xa;
		b->CmdLine[OemStringCmd.Length + 2] = 0;
		b->CmdLen = (USHORT)(OemStringCmd.Length + 3);

		myCsrAllocateMessagePointer(CaptureBuffer,
			ROUND_UP((OemStringAppName.Length + 1), 4),
			(PVOID *)&b->AppName
			);

		RtlMoveMemory(b->AppName,
			OemStringAppName.Buffer,
			OemStringAppName.Length
			);
		b->AppName[OemStringAppName.Length] = 0;
		b->AppLen = OemStringAppName.Length + 1;

		if (AnsiStringPif.Buffer) {
			myCsrAllocateMessagePointer(CaptureBuffer,
				ROUND_UP((AnsiStringPif.Length + 1), 4),
				(PVOID *)&b->PifFile
				);

			RtlMoveMemory(b->PifFile,
				AnsiStringPif.Buffer,
				AnsiStringPif.Length);

			b->PifFile[AnsiStringPif.Length] = 0;
			b->PifLen = AnsiStringPif.Length + 1;

		}
		else {
			b->PifLen = 0;
			b->PifFile = NULL;
		}

		if (pAnsiStringEnv->Length) {
			myCsrAllocateMessagePointer(CaptureBuffer,
				ROUND_UP((pAnsiStringEnv->Length), 4),
				(PVOID *)&b->Env
				);

			RtlMoveMemory(b->Env,
				pAnsiStringEnv->Buffer,
				pAnsiStringEnv->Length);

			b->EnvLen = pAnsiStringEnv->Length;
		}
		else {
			b->EnvLen = 0;
			b->Env = NULL;
		}


		if (AnsiStringCurrentDir.Length) {
			myCsrAllocateMessagePointer(CaptureBuffer,
				ROUND_UP((AnsiStringCurrentDir.Length + 1), 4),
				(PVOID *)&b->CurDirectory
				);
			RtlMoveMemory(b->CurDirectory,
				AnsiStringCurrentDir.Buffer,
				AnsiStringCurrentDir.Length + 1);

			b->CurDirectoryLen = AnsiStringCurrentDir.Length + 1;
		}
		else {
			b->CurDirectory = NULL;
			b->CurDirectoryLen = 0;
		}

		if (lpStartupInfo) {
			myCsrAllocateMessagePointer(CaptureBuffer,
				ROUND_UP(sizeof(STARTUPINFOA), 4),
				(PVOID *)&b->StartupInfo
				);

			b->StartupInfo->dwX = lpStartupInfo->dwX;
			b->StartupInfo->dwY = lpStartupInfo->dwY;
			b->StartupInfo->dwXSize = lpStartupInfo->dwXSize;
			b->StartupInfo->dwYSize = lpStartupInfo->dwYSize;
			b->StartupInfo->dwXCountChars = lpStartupInfo->dwXCountChars;
			b->StartupInfo->dwYCountChars = lpStartupInfo->dwYCountChars;
			b->StartupInfo->dwFillAttribute = lpStartupInfo->dwFillAttribute;
			b->StartupInfo->dwFlags = lpStartupInfo->dwFlags;
			b->StartupInfo->wShowWindow = lpStartupInfo->wShowWindow;
			b->StartupInfo->cb = sizeof(STARTUPINFOA);
		}
		else {
			b->StartupInfo = NULL;
		}

		if (AnsiStringDesktop.Buffer) {
			myCsrAllocateMessagePointer(CaptureBuffer,
				ROUND_UP((AnsiStringDesktop.Length + 1), 4),
				(PVOID *)&b->Desktop
				);
			RtlMoveMemory(b->Desktop,
				AnsiStringDesktop.Buffer,
				AnsiStringDesktop.Length + 1);
			b->DesktopLen = AnsiStringDesktop.Length + 1;
		}
		else {
			b->Desktop = NULL;
			b->DesktopLen = 0;
		}

		if (OemStringTitle.Buffer) {
			myCsrAllocateMessagePointer(CaptureBuffer,
				ROUND_UP((OemStringTitle.Length + 1), 4),
				(PVOID *)&b->Title
				);
			RtlMoveMemory(b->Title,
				OemStringTitle.Buffer,
				OemStringTitle.Length + 1);
			b->TitleLen = OemStringTitle.Length + 1;
		}
		else {
			b->Title = NULL;
			b->TitleLen = 0;
		}

		if (AnsiStringReserved.Buffer) {
			myCsrAllocateMessagePointer(CaptureBuffer,
				ROUND_UP((AnsiStringReserved.Length + 1), 4),
				(PVOID *)&b->Reserved
				);
			RtlMoveMemory(b->Reserved,
				AnsiStringReserved.Buffer,
				AnsiStringReserved.Length + 1);
			b->ReservedLen = AnsiStringReserved.Length + 1;
		}
		else {
			b->Reserved = NULL;
			b->ReservedLen = 0;
		}

		if (hUserToken) {
			myCsrAllocateMessagePointer(CaptureBuffer,
				sizeof(AuthenticationId),
				(PVOID *)&b->AuthenticationId
				);
			*b->AuthenticationId = AuthenticationId;
		}
		else {
			b->AuthenticationId = NULL;
		}

		if (BinaryType == BINARY_TYPE_WIN16)
			b->ConsoleHandle = (HANDLE)-1;
		else if (bNewConsole)
			b->ConsoleHandle = 0;
		else
			b->ConsoleHandle = ProcessParameters->ConsoleHandle;

		b->VDMState = FALSE;
		b->BinaryType = BinaryType;
		b->CodePage = (ULONG)GetConsoleCP();
		b->dwCreationFlags = dwCreationFlags;


		Status = myCsrClientCallServer(
			(struct _CSR_API_MESSAGE*)m,
			CaptureBuffer,
			CSR_MAKE_API_NUMBER(BASESRV_SERVERDLL_INDEX,
				BasepCheckVDM
				),
			sizeof(*b)
			);

		CsrFreeCaptureBuffer(CaptureBuffer);

		if (!NT_SUCCESS(Status) || !NT_SUCCESS((NTSTATUS)m->ReturnValue)) {
			if (NT_SUCCESS(Status)) Status = (NTSTATUS)m->ReturnValue;
			goto BCVTryExit;
		}

		*iTask = b->iTask;
		Status = STATUS_SUCCESS;
	BCVTryExit:;

		if (Buffer != NULL)
			RtlFreeHeap(RtlProcessHeap(), 0, (PVOID)Buffer);

		if (wsBuffer != NULL)
			RtlFreeHeap(RtlProcessHeap(), 0, (PVOID)wsBuffer);

		if (wsPifName != NULL)
			RtlFreeHeap(RtlProcessHeap(), 0, (PVOID)wsPifName);

		if (OemStringCmd.Buffer != NULL)
			RtlFreeOemString(&OemStringCmd);

		if (OemStringAppName.Buffer != NULL)
			RtlFreeOemString(&OemStringAppName);

		if (AnsiStringPif.Buffer != NULL)
			RtlFreeAnsiString(&AnsiStringPif);

		if (AnsiStringCurrentDir.Buffer != NULL)
			RtlFreeAnsiString(&AnsiStringCurrentDir);

		if (AnsiStringDesktop.Buffer != NULL)
			RtlFreeAnsiString(&AnsiStringDesktop);

		if (OemStringTitle.Buffer != NULL)
			RtlFreeAnsiString(&OemStringTitle);

		if (AnsiStringReserved.Buffer != NULL)
			RtlFreeAnsiString(&AnsiStringReserved);

		if (wsAppName != NULL)
			RtlFreeHeap(RtlProcessHeap(), 0, wsAppName);

		if (lpAllocatedReserved != NULL)
			RtlFreeHeap(RtlProcessHeap(), 0, lpAllocatedReserved);


	return Status;
}

NTSTATUS 
BASEP_CALL
BaseCheckVDM(
	IN  ULONG BinaryType,
	IN  PCWCH lpApplicationName,
	IN  PCWCH lpCommandLine,
	IN  PCWCH lpCurrentDirectory,
	IN  ANSI_STRING *pAnsiStringEnv,
	IN  PBASE_API_MSG m,
	IN OUT PULONG iTask,
	IN  DWORD dwCreationFlags,
	LPSTARTUPINFOW lpStartupInfo,
	IN HANDLE hUserToken OPTIONAL
	)
{
	NTSTATUS Status;
	WCHAR *lpDesktop;

	Status = BaseCheckVDMp(
		BinaryType,
		lpApplicationName,
		lpCommandLine,
		lpCurrentDirectory,
		pAnsiStringEnv,
		m,
		iTask,
		dwCreationFlags,
		lpStartupInfo,
		hUserToken);
	if (Status == STATUS_ACCESS_DENIED)
	{
		lpDesktop = lpStartupInfo->lpDesktop;
		lpStartupInfo->lpDesktop = ((xpPRTL_USER_PROCESS_PARAMETERS)NtCurrentTeb()->ProcessEnvironmentBlock->ProcessParameters)->DesktopInfo.Buffer;
		Status = BaseCheckVDMp(
			BinaryType,
			lpApplicationName,
			lpCommandLine,
			lpCurrentDirectory,
			pAnsiStringEnv,
			m,
			iTask,
			dwCreationFlags,
			lpStartupInfo,
			hUserToken);
		if (!NT_SUCCESS(Status))
			lpStartupInfo->lpDesktop = lpDesktop;
	}
	return Status;
}

BOOL
GetVDMConfigValue(
	HANDLE hKey,
	LPWSTR Keyword,
	LPWSTR UnicodeBuffer
	)
{
	NTSTATUS NtStatus;
	UNICODE_STRING ValueName;
	PKEY_VALUE_FULL_INFORMATION pKeyValueInformation = (PVOID)UnicodeBuffer;
	ULONG ValueLength;

	RtlInitUnicodeString(&ValueName, Keyword);
	NtStatus = NtQueryValueKey(hKey,
		&ValueName,
		KeyValueFullInformation,
		pKeyValueInformation,
		FULL_INFO_BUFFER_SIZE,
		&ValueLength);

	if (NT_SUCCESS(NtStatus))
		return TRUE;
	else {
		BaseSetLastNTError(NtStatus);
		return FALSE;
	}
}


UINT
BaseGetEnvNameType_U(WCHAR * Name, DWORD NameLength)
{
	static ENV_INFO     EnvInfoTable[STD_ENV_NAME_COUNT] = {
		{ ENV_NAME_TYPE_MULTIPLE_PATH, 4, ENV_NAME_PATH },
		{ ENV_NAME_TYPE_SINGLE_PATH, 6, ENV_NAME_WINDIR },
		{ ENV_NAME_TYPE_SINGLE_PATH, 10, ENV_NAME_SYSTEMROOT }
	};



	UINT NameType;
	int	i;


	NameType = ENV_NAME_TYPE_NO_PATH;

	for (i = 0; i < STD_ENV_NAME_COUNT; i++) {
		if (EnvInfoTable[i].NameLength == NameLength &&
			!__wcsnicmp(EnvInfoTable[i].Name, Name, NameLength)) {
			NameType = EnvInfoTable[i].NameType;
			break;
		}
	}
	return NameType;
}

BOOL BaseCreateVDMEnvironment(
	PWCHAR lpEnvironment,
	ANSI_STRING * pAStringEnv,
	UNICODE_STRING  *pUStringEnv
	)
{
	WCHAR  *pEnv, *pDst, *EnvStrings, *pTmp, *pNewEnv;
	DWORD   cchEnv, dw, Length, dwRemain;
	NTSTATUS    Status;
	UINT        NameType;
	BOOL        bRet = FALSE;
	SIZE_T      EnvSize;

	if (!ARGUMENT_PRESENT(pAStringEnv) || !ARGUMENT_PRESENT(pUStringEnv)) {
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}
		// the environment strings are shared by every thread of the same
		// process. Since we have no idea of what the caller process
		// is, we have to grab the entire environment to our local buffer in one
		// shot then we can walk through the strings.
		// Note that if another thread makes call to RtlSetEnvironmentVariable
		// then we are out of sync. It is a problem of process structure and
		// I don't want to think about it now.
		// The funny thing is that we have to assume the environment
		// is a block of strings(otherwise, how can we do it?)t, nothing more and
		// nothing less. If someday and somebody dares to change it, he will be
		// the one to blame. If the caller(CreateProcess)
		// provides the environment, we assume it is safe to walk through it.
		//

		if (lpEnvironment == NULL) {
			// create a new environment and inherit the current process env
			Status = RtlCreateEnvironment(TRUE, (PVOID *)&EnvStrings);
			if (!NT_SUCCESS(Status))
				goto bveTryExit;
		}
		else
			EnvStrings = lpEnvironment;

		if (EnvStrings == NULL) {
			SetLastError(ERROR_BAD_ENVIRONMENT);
			goto bveTryExit;
		}
		// figure out how long the environment is
		// why can Rtl just provides such a function for us?
		//
		cchEnv = 0;
		pEnv = EnvStrings;
		// environment is double-null terminated
		while (!(*pEnv++ == UNICODE_NULL && *pEnv == UNICODE_NULL))
			cchEnv++;
		// count the last two NULLs
		cchEnv += 2;
		// we don't want to change the original environment, so
		// make a local buffer for it.
		EnvSize = (cchEnv + MAX_PATH) * sizeof(WCHAR);
		pNewEnv = NULL;
		Status = NtAllocateVirtualMemory(NtCurrentProcess(),
			&pNewEnv,
			0,
			&EnvSize,
			MEM_COMMIT,
			PAGE_READWRITE
			);
		if (!NT_SUCCESS(Status)) {
			SetLastError(ERROR_NOT_ENOUGH_MEMORY);
			goto bveTryExit;
		}
		// give the last two for null
		dwRemain = MAX_PATH - 2;
		// now walk through the environment string
		pEnv = EnvStrings;
		// the new environmet will be
		pDst = pNewEnv;
		while (*pEnv != UNICODE_NULL) {
			// current directory environment has the form as:
			// "=d:=d:\pathname" where d: is the drive designator.
			if (pEnv[0] == L'=')
			{
				if ((pEnv[1] >= L'A' && pEnv[1] <= L'Z' || pEnv[1] >= L'a' && pEnv[1] <= L'z') &&
					pEnv[2] == L':' && pEnv[3] == L'=' && wcslen(pEnv) >= 7)
				{
					// hack hack!!!!
					// if the path points to the root directory,
					// bypass the conversion. Dos or Wow keeps current directory
					// for every valid drive. If we do the conversion for
					// every current directory, it could take several
					// seconds on removable drives, especially, on
					// floppy drives.
					if (pEnv[7] == UNICODE_NULL &&
						(pEnv[6] == L'\\' || pEnv[6] == L'/') &&
						pEnv[5] == L':' &&
						(pEnv[4] >= L'A' && pEnv[4] <= L'Z' ||
							pEnv[4] >= L'a' && pEnv[4] <= L'z'))
					{
						NameType = ENV_NAME_TYPE_NO_PATH;
					}
					else
					{
						// copy "=N:=", where N is the drive letter
						*pDst++ = *pEnv++; *pDst++ = *pEnv++;
						*pDst++ = *pEnv++; *pDst++ = *pEnv++;
						NameType = ENV_NAME_TYPE_SINGLE_PATH;
					}
				}
				else {
					// a weird environment was detected.
					// treat it as no path
					NameType = ENV_NAME_TYPE_NO_PATH;
				}
			}
			else {
				pTmp = pEnv;
				// copy down the name and the '='
				while (*pEnv != UNICODE_NULL && (*pDst++ = *pEnv++) != L'=')
					;
				NameType = BaseGetEnvNameType_U(pTmp, (DWORD)(pEnv - pTmp) - 1);
			}

			if (NameType == ENV_NAME_TYPE_NO_PATH) {
				while ((*pDst++ = *pEnv++) != UNICODE_NULL)
					;
			}
			else if (NameType == ENV_NAME_TYPE_SINGLE_PATH) {
				Length = wcslen(pEnv) + 1;
				dw = GetShortPathNameW(pEnv, pDst, Length + dwRemain);
				// if the conversion failed, we simply pass down the original
				// one no matter what the reason is. This is done because we
				// are doing the environment strings.
				if (dw == 0 || dw >= Length + dwRemain) {
					RtlMoveMemory(pDst, pEnv, Length * sizeof(WCHAR));
					dw = Length - 1;
				}
				pDst += dw + 1;
				pEnv += Length;
				if (dw > Length)
					dwRemain -= dw - Length;
			}
			else {
				// multiple path name found.
				// the character ';' is used for seperator
				pTmp = pEnv;
				while (*pEnv != UNICODE_NULL) {
					if (*pEnv == L';') {
						// length not include the ';'
						Length = (DWORD)(pEnv - pTmp);
						if (Length > 0) {
							*pEnv = UNICODE_NULL;
							dw = GetShortPathNameW(pTmp, pDst, Length + 1 + dwRemain);
							// again, if the conversion failed, use the original one
							if (dw == 0 || dw > Length + dwRemain) {
								RtlMoveMemory(pDst, pTmp, Length * sizeof(WCHAR));
								dw = Length;
							}
							pDst += dw;
							*pDst++ = *pEnv++ = L';';
							if (dw > Length)
								dwRemain -= dw - Length;
						}
						// skip all consecutive ';'
						while (*pEnv == L';')
							*pDst++ = *pEnv++;
						pTmp = pEnv;
					}
					else
						pEnv++;
				}
				// convert the last one
				if ((Length = (DWORD)(pEnv - pTmp)) != 0) {
					dw = GetShortPathNameW(pTmp, pDst, Length + 1 + dwRemain);
					if (dw == 0 || dw > Length) {
						RtlMoveMemory(pDst, pTmp, Length * sizeof(WCHAR));
						dw = Length;
					}
					pDst += dw;
					if (dw > Length)
						dwRemain -= dw - Length;
				}
				*pDst++ = *pEnv++;
			}
		}
		*pDst++ = UNICODE_NULL;
		cchEnv = (ULONG)((ULONG_PTR)pDst - (ULONG_PTR)pNewEnv);
		pUStringEnv->MaximumLength = pUStringEnv->Length = (USHORT)cchEnv;
		pUStringEnv->Buffer = pNewEnv;
		Status = RtlUnicodeStringToAnsiString(pAStringEnv,
			pUStringEnv,
			TRUE
			);

		if (!NT_SUCCESS(Status)) {
			BaseSetLastNTError(Status);
		}
		else
			bRet = TRUE;
	bveTryExit:;
	if (lpEnvironment == NULL && EnvStrings != NULL)
		RtlDestroyEnvironment(EnvStrings);
	return bRet;
}

BOOL
BaseGetVdmConfigInfo(IN LPCWSTR CommandLineReserved,
	IN ULONG DosSeqId,
	IN ULONG BinaryType,
	IN PUNICODE_STRING CmdLineString,
	OUT PULONG VdmSize)
{
	WCHAR Buffer[MAX_PATH];
	WCHAR CommandLine[MAX_PATH * 2];
	ULONG Length;

	/* Clear the buffer in case we fail */
	CmdLineString->Buffer = 0;

	/* Always return the same size: 16 Mb */
	*VdmSize = 0x1000000;

	/* Get the system directory */
	Length = GetSystemDirectoryW(Buffer, MAX_PATH);
	if (!(Length) || (Length >= MAX_PATH))
	{
		/* Eliminate no path or path too big */
		SetLastError(ERROR_INVALID_NAME);
		return FALSE;
	}

	/* Check if this is VDM with a DOS Sequence ID */
	if (DosSeqId)
	{
		/*
		* Build the VDM string for it:
		* -i%lx : Gives the DOS Sequence ID;
		* %s%c  : Nothing if DOS VDM, -w if WoW VDM, -ws if separate WoW VDM.
		*/
		__swprintf(CommandLine,
			L"\"%s\\ntvdm.exe\" -i%lx %s%c",
			Buffer,
			DosSeqId,
			(BinaryType == BINARY_TYPE_DOS) ? L" " : L"-w",
			(BinaryType == BINARY_TYPE_SEPWOW) ? L's' : L' ');
	}
	else
	{
		/*
		* Build the string for it without the DOS Sequence ID:
		* %s%c  : Nothing if DOS VDM, -w if WoW VDM, -ws if separate WoW VDM.
		*/
		__swprintf(CommandLine,
			L"\"%s\\ntvdm.exe\" %s%c",
			Buffer,
			(BinaryType == BINARY_TYPE_DOS) ? L" " : L"-w",
			(BinaryType == BINARY_TYPE_SEPWOW) ? L's' : L' ');
	}

	/* Create the actual string */
	return RtlCreateUnicodeString(CmdLineString, CommandLine);
}
BOOL
BaseUpdateVDMEntry(
	IN ULONG UpdateIndex,
	IN OUT HANDLE *WaitHandle,
	IN ULONG IndexInfo,
	IN ULONG BinaryType
	)
{
	NTSTATUS Status;
	BASE_API_MSG m;
	PBASE_UPDATE_VDM_ENTRY_MSG c = (PBASE_UPDATE_VDM_ENTRY_MSG)&m.u.UpdateVDMEntry;
	xpPRTL_USER_PROCESS_PARAMETERS ProcessParameters;

	ProcessParameters = (xpPRTL_USER_PROCESS_PARAMETERS)NtCurrentPeb()->ProcessParameters;
	switch (UpdateIndex) {
	case UPDATE_VDM_UNDO_CREATION:
		c->iTask = (ULONG)*WaitHandle;
		c->VDMCreationState = (USHORT)IndexInfo;
		break;
	case UPDATE_VDM_PROCESS_HANDLE:
		c->VDMProcessHandle = *WaitHandle;	// Actually this is VDM handle
		c->iTask = IndexInfo;
		break;
	}

	if (BinaryType == BINARY_TYPE_WIN16)
		c->ConsoleHandle = (HANDLE)-1;
	else if (c->iTask)
		c->ConsoleHandle = 0;
	else
		c->ConsoleHandle = ProcessParameters->ConsoleHandle;

	c->EntryIndex = (WORD)UpdateIndex;
	c->BinaryType = BinaryType;


	Status = myCsrClientCallServer(
		(struct _CSR_API_MESSAGE*)&m,
		NULL,
		CSR_MAKE_API_NUMBER(BASESRV_SERVERDLL_INDEX,
			BasepUpdateVDMEntry
			),
		sizeof(*c)
		);

	if (!NT_SUCCESS(Status) || !NT_SUCCESS((NTSTATUS)m.ReturnValue)) {
		BaseSetLastNTError((NTSTATUS)m.ReturnValue);
		return FALSE;
	}

	switch (UpdateIndex) {
	case UPDATE_VDM_UNDO_CREATION:
		break;
	case UPDATE_VDM_PROCESS_HANDLE:
		*WaitHandle = c->WaitObjectForParent;
		break;
	}
	return TRUE;
}

BOOL
BaseDestroyVDMEnvironment(
	ANSI_STRING *pAStringEnv,
	UNICODE_STRING *pUStringEnv
	)
{
	if (pAStringEnv->Buffer)
		RtlFreeAnsiString(pAStringEnv);
	if (pUStringEnv->Buffer)
		RtlFreeHeap(RtlProcessHeap(), 0, pUStringEnv->Buffer);
	return TRUE;

}

#endif