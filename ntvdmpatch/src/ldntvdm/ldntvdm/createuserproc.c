/*Project: ldntvdm
* Module : createuserproc
* Author : leecher@dose.0wnz.at
* Descr. : Create a process without using NtCreateUserProcess, thus allowing
*          the allocation of memory at address 0 by moving ApiSetMap out of
*          the way.
* Changes: 01.02.2024  - Created
*/
#include <ntstatus.h>
#define WIN32_NO_STATUS
#include "ldntvdm.h"
#include "ntrtl.h"
#include "basemsg64.h"
#include "basevdm.h"
#include "createuserproc.h"
#include "ntpsapi.h"
#include "ntpeb.h"
#include "injector64.h"
#include "mappage0.h"
#include "xpwrap.h"

#if defined(USE_OWN_RTLCREATEUSERPROCESSEX) && !defined(TARGET_WINXP)

#define NtCurrentProcess() ( (HANDLE)(LONG_PTR) -1 )
#define IsConsoleHandle(h) (((ULONG_PTR)(h) & 0x10000003) == 0x3)

NTSYSAPI
NTSTATUS
NTAPI
NtDuplicateObject(
	IN HANDLE SourceProcessHandle,
	IN HANDLE SourceHandle,
	IN HANDLE TargetProcessHandle OPTIONAL,
	OUT PHANDLE TargetHandle OPTIONAL,
	IN ACCESS_MASK DesiredAccess,
	IN ULONG HandleAttributes,
	IN ULONG Options
	);

NTSYSCALLAPI
NTSTATUS
NTAPI
NtWriteVirtualMemory(
	IN HANDLE ProcessHandle,
	OUT PVOID BaseAddress,
	IN CONST VOID *Buffer,
	IN SIZE_T BufferSize,
	OUT PSIZE_T NumberOfBytesWritten OPTIONAL
	);

NTSYSAPI
NTSTATUS
NTAPI
RtlCreateUserThread(
	HANDLE Process,
	PSECURITY_DESCRIPTOR ThreadSecurityDescriptor,
	BOOLEAN CreateSuspended,
	ULONG StackZeroBits,
	SIZE_T MaximumStackSize OPTIONAL,
	SIZE_T InitialStackSize OPTIONAL,
	PUSER_THREAD_START_ROUTINE StartAddress,
	PVOID Parameter,
	PHANDLE Thread,
	PCLIENT_ID ClientId
	);



#define DUPLICATE_SAME_ATTRIBUTES   0x00000004

NTSYSAPI
PRTL_USER_PROCESS_PARAMETERS
NTAPI
RtlNormalizeProcessParams(
	PRTL_USER_PROCESS_PARAMETERS ProcessParameters
	);

NTSYSAPI
PRTL_USER_PROCESS_PARAMETERS
NTAPI
RtlDeNormalizeProcessParams(
	PRTL_USER_PROCESS_PARAMETERS ProcessParameters
	);


typedef NTSTATUS(*PUSER_THREAD_START_ROUTINE)(
	PVOID ThreadParameter
	);

NTSTATUS
RtlpOpenImageFile(
	IN PUNICODE_STRING ImagePathName,
	IN ULONG Attributes,
	OUT PHANDLE FileHandle,
	IN BOOLEAN ReportErrors
	)
{
	NTSTATUS Status;
	OBJECT_ATTRIBUTES ObjectAttributes;
	HANDLE File;
	IO_STATUS_BLOCK IoStatus;

	*FileHandle = NULL;

	InitializeObjectAttributes(&ObjectAttributes,
		ImagePathName,
		Attributes,
		NULL,
		NULL
		);
	Status = NtOpenFile(&File,
		SYNCHRONIZE | FILE_EXECUTE | FILE_READ_ATTRIBUTES,  // NB: FILE_READ_ATTRIBUTES not originally there, but without it, ApphelpCacheControlValidateParameters in NtApphelpCacheControl will fail later
		&ObjectAttributes,
		&IoStatus,
		FILE_SHARE_READ | FILE_SHARE_DELETE,
		FILE_NON_DIRECTORY_FILE
		);

	if (!NT_SUCCESS(Status)) {
#if DBG
		if (ReportErrors) {
			DbgPrint("NTRTL: RtlpOpenImageFile - NtCreateFile( %wZ ) failed.  Status == %X\n",
				ImagePathName,
				Status
				);
		}
#endif // DBG
		return(Status);
	}

	*FileHandle = File;
	return(STATUS_SUCCESS);
}

#define TYPE32(x)   ULONG
#define TYPE64(x)   ULONGLONG

typedef struct _STRING32 {
	USHORT   Length;
	USHORT   MaximumLength;
	ULONG  Buffer;
} STRING32;
typedef STRING32 *PSTRING32;

typedef STRING32 UNICODE_STRING32;
typedef UNICODE_STRING32 *PUNICODE_STRING32;

typedef struct _CURDIR32 {
	UNICODE_STRING32 DosPath;
	TYPE32(HANDLE) Handle;
} CURDIR32, *PCURDIR32;

typedef struct _RTL_DRIVE_LETTER_CURDIR32 {
	USHORT Flags;
	USHORT Length;
	ULONG TimeStamp;
	STRING32 DosPath;
} RTL_DRIVE_LETTER_CURDIR32, *PRTL_DRIVE_LETTER_CURDIR32;

typedef struct _RTL_USER_PROCESS_PARAMETERS32 {
	ULONG MaximumLength;
	ULONG Length;

	ULONG Flags;
	ULONG DebugFlags;

	TYPE32(HANDLE) ConsoleHandle;
	ULONG  ConsoleFlags;
	TYPE32(HANDLE) StandardInput;
	TYPE32(HANDLE) StandardOutput;
	TYPE32(HANDLE) StandardError;

	CURDIR32 CurrentDirectory;        // ProcessParameters
	UNICODE_STRING32 DllPath;         // ProcessParameters
	UNICODE_STRING32 ImagePathName;   // ProcessParameters
	UNICODE_STRING32 CommandLine;     // ProcessParameters
	TYPE32(PVOID) Environment;              // NtAllocateVirtualMemory

	ULONG StartingX;
	ULONG StartingY;
	ULONG CountX;
	ULONG CountY;
	ULONG CountCharsX;
	ULONG CountCharsY;
	ULONG FillAttribute;

	ULONG WindowFlags;
	ULONG ShowWindowFlags;
	UNICODE_STRING32 WindowTitle;     // ProcessParameters
	UNICODE_STRING32 DesktopInfo;     // ProcessParameters
	UNICODE_STRING32 ShellInfo;       // ProcessParameters
	UNICODE_STRING32 RuntimeData;     // ProcessParameters
	RTL_DRIVE_LETTER_CURDIR32 CurrentDirectores[RTL_MAX_DRIVE_LETTERS];
#ifndef TARGET_WINXP
	ULONG EnvironmentSize;
	ULONG EnvironmentVersion;
#endif
#ifndef TARGET_WIN7
	TYPE32(PVOID) PackageDependencyData;
	ULONG ProcessGroupId;
#endif
} xpRTL_USER_PROCESS_PARAMETERS32, *xpPRTL_USER_PROCESS_PARAMETERS32;

#ifndef TARGET_WINXP
VOID
Wow64pCopyString(
	PCHAR *p,
	PUNICODE_STRING32 str32,
	PUNICODE_STRING str64
	)
{
	/* Process parameters are not using pointers but offsets, so just do a dumb copy
	*p = (PCHAR)ROUND_UP((SIZE_T)*p, sizeof(ULONG));

	str32->Length = str64->Length;
	str32->MaximumLength = str64->MaximumLength;
	RtlMoveMemory(*p, str64->Buffer, str64->MaximumLength);
	str32->Buffer = PtrToUlong(*p);

	*p += str64->MaximumLength;
	*/
	str32->Length = str64->Length;
	str32->MaximumLength = str64->MaximumLength;
	if (str64->Buffer)
		str32->Buffer = (ULONG_PTR)str64->Buffer - (ULONG_PTR)*p;
	else
		str32->Buffer = 0;
}

// This function was moved to Kernel as PspWow64SetupUserProcessAddressSpace in recent Windows Versions (> Xp)
// and for some unknown (stupid?) reason isn't done in WOW64 anymore, so we have to setup WOW64 process address
// space manually, as it's done in NtCreateUserProcess (these is no Context given in PspAllocateProcess call
// on NtCreateProcessEx, thus PspSetupUserProcessAddressSpace isn't called and no Wow64 context is setup)
NTSTATUS
Wow64pThunkProcessParameters(
	HANDLE Process,
	ULONGLONG Peb64Address,
	PPEB32 Peb32,
	xpPRTL_USER_PROCESS_PARAMETERS Params64
	)
	/*++

	Routine Description:

	This function copies the process parameters from the 64bit peb to the 32bit peb.

	Arguments:

	Process- Handle to process
	Peb32  - Supplies the pointer to the 32bit peb that will recieve the process parameters.
	Peb    - Supplies the pointer to the 64bit peb that will supply the process parameters.

	Return Value:

	Status.

	--*/
{

	SIZE_T AllocSize, StructSize;
	xpPRTL_USER_PROCESS_PARAMETERS32 Params32, Params32Target;
	SIZE_T Index;
	PCHAR p;
	PVOID Base;
	SIZE_T RegionSize;
	NTSTATUS st;


	// Process Parameters should have been normalized by ntdll.

	if (NULL == Params64) {
		Peb32->ProcessParameters = (TYPE32(PRTL_USER_PROCESS_PARAMETERS))0;
		return STATUS_SUCCESS;
	}

	//
	//  Compute the required space for the continous memory region.

	AllocSize = sizeof(xpRTL_USER_PROCESS_PARAMETERS32);
	AllocSize += ROUND_UP(Params64->CurrentDirectory.DosPath.MaximumLength, sizeof(ULONG));
	AllocSize += ROUND_UP(Params64->DllPath.MaximumLength, sizeof(ULONG));
	AllocSize += ROUND_UP(Params64->ImagePathName.MaximumLength, sizeof(ULONG));
	AllocSize += ROUND_UP(Params64->CommandLine.MaximumLength, sizeof(ULONG));
	AllocSize += ROUND_UP(Params64->WindowTitle.MaximumLength, sizeof(ULONG));
	AllocSize += ROUND_UP(Params64->DesktopInfo.MaximumLength, sizeof(ULONG));
	AllocSize += ROUND_UP(Params64->ShellInfo.MaximumLength, sizeof(ULONG));
	AllocSize += ROUND_UP(Params64->RuntimeData.MaximumLength, sizeof(ULONG));

	for (Index = 0; Index < RTL_MAX_DRIVE_LETTERS; Index++) {
		AllocSize += ROUND_UP(Params64->CurrentDirectores[Index].DosPath.MaximumLength, sizeof(ULONG));
	}

	StructSize = AllocSize;
	//AllocSize += Params64->EnvironmentSize;

	Base = NULL;
	RegionSize = AllocSize;
	st = NtAllocateVirtualMemory(GetCurrentProcess(),
		&Base,
		0,
		&RegionSize,
		MEM_COMMIT | MEM_RESERVE,
		PAGE_READWRITE);

	if (!NT_SUCCESS(st)) {
		TRACE("ThunkProcessParameters: NtAllocateVirtualMemory failed allocating process parameters locally, error %x.\n", st);
		return st;
	}
	Params32 = (xpPRTL_USER_PROCESS_PARAMETERS32)Base;

	Base = NULL;
	RegionSize = AllocSize;
	st = NtAllocateVirtualMemory(Process,
		&Base,
		0,
		&RegionSize,
		MEM_COMMIT | MEM_RESERVE,
		PAGE_READWRITE);

	if (!NT_SUCCESS(st)) {
		TRACE("ThunkProcessParameters: NtAllocateVirtualMemory failed allocating process parameters, error %x.\n", st);
		return st;
	}

	Params32Target = (xpPRTL_USER_PROCESS_PARAMETERS32)Base;
	Peb32->ProcessParameters = (TYPE32(PRTL_USER_PROCESS_PARAMETERS))PtrToUlong(Params32Target);
	//p = (PCHAR)Params32 + sizeof(xpRTL_USER_PROCESS_PARAMETERS32);
	p = (PCHAR)(sizeof(xpRTL_USER_PROCESS_PARAMETERS) - sizeof(xpRTL_USER_PROCESS_PARAMETERS32));

	Params32->MaximumLength = Params32->Length = (ULONG)AllocSize;
	Params32->Flags = Params64->Flags;
	Params32->DebugFlags = Params64->DebugFlags;
	Params32->ConsoleHandle = (TYPE32(HANDLE))PtrToUlong(Params64->ConsoleHandle);
	Params32->ConsoleFlags = (ULONG)Params64->ConsoleFlags;
	Params32->StandardInput = (TYPE32(HANDLE)) PtrToUlong(Params64->StandardInput);
	Params32->StandardOutput = (TYPE32(HANDLE)) PtrToUlong(Params64->StandardOutput);
	Params32->StandardError = (TYPE32(HANDLE)) PtrToUlong(Params64->StandardError);

	Params32->CurrentDirectory.Handle = (TYPE32(HANDLE)) PtrToUlong(Params64->CurrentDirectory.Handle);
	Wow64pCopyString(&p, &Params32->CurrentDirectory.DosPath, &Params64->CurrentDirectory.DosPath);

	Wow64pCopyString(&p, &Params32->DllPath, &Params64->DllPath);
	Wow64pCopyString(&p, &Params32->ImagePathName, &Params64->ImagePathName);
	Wow64pCopyString(&p, &Params32->CommandLine, &Params64->CommandLine);
	Params32->Environment = PtrToUlong(Params64->Environment);

	Params32->StartingX = Params64->StartingX;
	Params32->StartingY = Params64->StartingY;
	Params32->CountX = Params64->CountX;
	Params32->CountY = Params64->CountY;
	Params32->CountCharsX = Params64->CountCharsX;
	Params32->CountCharsY = Params64->CountCharsY;
	Params32->FillAttribute = Params64->FillAttribute;

	Params32->WindowFlags = Params64->WindowFlags;
	Params32->ShowWindowFlags = Params64->ShowWindowFlags;
	Wow64pCopyString(&p, &Params32->WindowTitle, &Params64->WindowTitle);
	Wow64pCopyString(&p, &Params32->DesktopInfo, &Params64->DesktopInfo);
	Wow64pCopyString(&p, &Params32->ShellInfo, &Params64->ShellInfo);

	// RuntimeData is mis-prototyped as a UNICODE_STRING.  However,
	// it is really used by the C runtime as a mechanism to pass file
	// handles around.  Thunk it as such.  See sdktools\vctools\crtw32
	// \exec\dospawn.c and lowio\ioinit.c for the gory details.
	if (Params64->RuntimeData.Length && Params64->RuntimeData.Buffer) {
		int cfi_len;
		char *posfile64;
		UINT_PTR UNALIGNED *posfhnd64;
		char *posfile32;
		UINT UNALIGNED *posfhnd32;
		int i;

		p = (PCHAR)Params64->RuntimeData.Buffer;
		cfi_len = *(int UNALIGNED *)Params64->RuntimeData.Buffer;

		Params32->RuntimeData.Length = Params64->RuntimeData.Length - cfi_len * sizeof(ULONG);
		Params32->RuntimeData.MaximumLength = Params32->RuntimeData.Length;
		Params32->RuntimeData.Buffer = PtrToUlong(p);
		p += Params32->RuntimeData.Length;

		posfile64 = (char *)((UINT_PTR)Params64->RuntimeData.Buffer + sizeof(int));
		posfhnd64 = (UINT_PTR UNALIGNED *)(posfile64 + cfi_len);
		posfile32 = (char *)((ULONG_PTR)Params32->RuntimeData.Buffer + sizeof(int));
		posfhnd32 = (UINT UNALIGNED *)(posfile32 + cfi_len);

		*(int *)Params32->RuntimeData.Buffer = cfi_len;
		for (i = 0; i<cfi_len; ++i) {
			*posfile32 = *posfile64;
			*posfhnd32 = (ULONG)*posfhnd64;
			posfile32++;
			posfile64++;
			posfhnd32++;
			posfhnd64++;
		}

		// Any bytes past the end of 4+(cfi_len*(sizeof(UINT_PTR)+sizeof(UINT))
		// must be copied verbatim.  They are probably from a non-MS C runtime.
		RtlMoveMemory(posfhnd32, posfhnd64, (Params64->RuntimeData.Length - ((ULONG_PTR)posfhnd64 - (ULONG_PTR)Params64->RuntimeData.Buffer)));
	}

	for (Index = 0; Index < RTL_MAX_DRIVE_LETTERS; Index++) {
		Params32->CurrentDirectores[Index].Flags = Params64->CurrentDirectores[Index].Flags;
		Params32->CurrentDirectores[Index].Length = Params64->CurrentDirectores[Index].Length ? sizeof(RTL_DRIVE_LETTER_CURDIR) : 0;
		Params32->CurrentDirectores[Index].TimeStamp = Params64->CurrentDirectores[Index].TimeStamp;
		Wow64pCopyString(&p, (PUNICODE_STRING32)&Params32->CurrentDirectores[Index].DosPath, (PUNICODE_STRING)&Params64->CurrentDirectores[Index].DosPath);
	}

	// Keep it from above, is in its own memory block, so we can reuse the pointer
	//Params32->Environment = (ULONG)Base + StructSize;
	Params32->EnvironmentSize = Params64->EnvironmentSize;
	Params32->EnvironmentVersion = Params64->EnvironmentVersion;
#ifndef TARGET_WIN7
	Params32->ProcessGroupId = Params64->ProcessGroupId;
#endif

	NtReadVirtualMemory(Process,
		(PVOID)(Peb64Address + sizeof(xpRTL_USER_PROCESS_PARAMETERS)),
		(PVOID)((ULONG_PTR)Params32 + sizeof(xpRTL_USER_PROCESS_PARAMETERS32)),
		(ULONG)StructSize - sizeof(xpRTL_USER_PROCESS_PARAMETERS32),
		NULL
		);

	/*
	NtReadVirtualMemory(Process,
	Params64->Environment,
	(ULONG_PTR)Params32 + StructSize,
	Params32->EnvironmentSize,
	NULL
	);
	*/
	NtWriteVirtualMemory(Process,
		Params32Target,
		Params32,
		AllocSize,
		NULL);

	return STATUS_SUCCESS;
}
/* ULONG_PTR starting with 1703, otherwise ULONG 
typedef struct _PS_MITIGATION_OPTIONS_MAP
{
	ULONG_PTR Map[3]; // 2 < 20H1
} PS_MITIGATION_OPTIONS_MAP, *PPS_MITIGATION_OPTIONS_MAP;

// private
typedef struct _PS_MITIGATION_AUDIT_OPTIONS_MAP
{
	ULONG_PTR Map[3]; // 2 < 20H1
} PS_MITIGATION_AUDIT_OPTIONS_MAP, *PPS_MITIGATION_AUDIT_OPTIONS_MAP;

typedef struct _PS_SYSTEM_DLL_INIT_BLOCK
{
	ULONG Size;
	ULONG_PTR SystemDllWowRelocation;
	ULONG_PTR SystemDllNativeRelocation;
	ULONG_PTR Wow64SharedInformation[16]; // use WOW64_SHARED_INFORMATION as index
	ULONG RngData;
	union
	{
		ULONG Flags;
		struct
		{
			ULONG CfgOverride : 1;
			ULONG Reserved : 31;
		};
	};
	PS_MITIGATION_OPTIONS_MAP MitigationOptionsMap;
	ULONG_PTR CfgBitMap;
	ULONG_PTR CfgBitMapSize;
	ULONG_PTR Wow64CfgBitMap;
	ULONG_PTR Wow64CfgBitMapSize;
	PS_MITIGATION_AUDIT_OPTIONS_MAP MitigationAuditOptionsMap; // REDSTONE3
} PS_SYSTEM_DLL_INIT_BLOCK, *PPS_SYSTEM_DLL_INIT_BLOCK;
*/

ULONG_PTR PspNtdll32SystemDllInitBlock = 0, PspSystemDllInitBlock = 0;
// PS_SYSTEM_DLL_INIT_BLOCK structure:
PBYTE PsWow64SharedInformation = NULL;
//BYTE PsWow64SharedInformation[0x50];

BOOL PspPrepareSystemDllInitBlock(HANDLE hProcess)
{
	WCHAR exeName[MAX_PATH + 1];
	BOOL bRet = FALSE;
	SIZE_T dwWritten;
	ULONG CB_PsWow64SharedInformation;
	static OSVERSIONINFOW ver = { 0 };

	if (!PsWow64SharedInformation)
	{
#ifdef _WIN64
		HMODULE ntRemote;
		HANDLE ntLocal;

		if (!(ntRemote = GetRemoteModuleHandle32(hProcess, L"ntdll.dll")))
		{
			TRACE("GetRemoteModuleHandle32 failed.\n");
		}
		else if (!(ntLocal = LoadLibraryWindir32(L"ntdll.dll")))
		{
			TRACE("LoadLibraryWindir32 failed.\n");
		}
		else
		{
			if ((PspNtdll32SystemDllInitBlock = (ULONG_PTR)GetRemoteProcAddressX32(ntRemote, ntLocal, "LdrSystemDllInitBlock")) &&
				(PspSystemDllInitBlock = (ULONG_PTR)GetProcAddress(GetModuleHandle(_T("ntdll.dll")), "LdrSystemDllInitBlock")))
			{
				PsWow64SharedInformation = (PBYTE)PspSystemDllInitBlock;
			}
			FreeLibrary32(ntLocal);
		}
#else
		if (PspNtdll32SystemDllInitBlock = GetProcAddress(GetModuleHandle(_T("ntdll.dll")), "LdrSystemDllInitBlock"))
		{
			//HMODULE hNTDLL64 = GetRemoteModuleHandle64(hProcess, L"ntdll.dll");
			PspSystemDllInitBlock = GetProcAddress64(getNTDLL64(), "LdrSystemDllInitBlock");
			PsWow64SharedInformation = PspNtdll32SystemDllInitBlock;
		}
#endif // _WIN64

	}

	if (!PsWow64SharedInformation || !PspNtdll32SystemDllInitBlock || !PspSystemDllInitBlock) return FALSE;
	CB_PsWow64SharedInformation = *((PULONG)PsWow64SharedInformation);

	bRet = WriteProcessMemory(hProcess, (LPVOID)PspNtdll32SystemDllInitBlock, PsWow64SharedInformation, CB_PsWow64SharedInformation, &dwWritten) &&
		WriteProcessMemory(hProcess, (LPVOID)PspSystemDllInitBlock, PsWow64SharedInformation, CB_PsWow64SharedInformation, &dwWritten);
#ifndef TARGET_WIN7
	if (!ver.dwOSVersionInfoSize)
	{
		NTSYSAPI ULONG RtlRandomEx(PULONG Seed);

		ULONG Seed = 1, RngData = RtlRandomEx(&Seed), offs;
		ver.dwOSVersionInfoSize = sizeof(ver);
		GetVersionExW(&ver);
		offs = ver.dwBuildNumber >= 1703 ? 0x98 : 0x50;
		WriteProcessMemory(hProcess, (LPVOID)(PspNtdll32SystemDllInitBlock + offs), &RngData, sizeof(RngData), &dwWritten);
		RngData = RtlRandomEx(&Seed);
		WriteProcessMemory(hProcess, (LPVOID)(PspSystemDllInitBlock + offs), &RngData, sizeof(RngData), &dwWritten);
	}
#endif
	return bRet;
}
#endif


// Fun fact: RTL_USER_PROC_RESERVE_16MB never gets used by Windows, even though the flag exists. 
// RTL_USER_PROC_RESERVE_1MB would work somewhat, but no A20 wrap area would be allocated with this 
// parameter and nothing there to expand reserved area
// RtlCreateUserProcessEx is my invention, it doesn't really exist, but it is a function that basically implements
// RtlCreateUserProcess with support for PPS_CREATE_INFO and RTL_USER_PROC_RESERVE_16MB in order to be used
// as a Usermode replacement for the Kernel mode NtCreateUserProcess syscall and also contains WOW64 handling
NTSTATUS
RtlCreateUserProcessEx(
	IN PUNICODE_STRING NtImagePathName,
	IN ULONG Attributes,
	IN xpPRTL_USER_PROCESS_PARAMETERS ProcessParameters,
	IN PSECURITY_DESCRIPTOR ProcessSecurityDescriptor OPTIONAL,
	IN PSECURITY_DESCRIPTOR ThreadSecurityDescriptor OPTIONAL,
	IN HANDLE ParentProcess OPTIONAL,
	IN BOOLEAN InheritHandles,
	IN HANDLE DebugPort OPTIONAL,
	IN HANDLE ExceptionPort OPTIONAL,
	OUT PRTL_USER_PROCESS_INFORMATION ProcessInformation,
	PPS_CREATE_INFO CreateInfo
	)

	/*++

	Routine Description:

	This function creates a user mode process with a single thread with
	a suspend count of one.  The address space of the new process is
	initialized with the contents of specified image file.  The caller
	can specify the Access Control List for the new process and thread.
	The caller can also specify the parent process to inherit process
	priority and processor affinity from.  The default is to inherit
	these from the current process.  Finally the caller can specify
	whether the new process is to inherit any of the object handles
	from the specified parent process or not.

	Information about the new process and thread is returned via
	the ProcessInformation parameter.

	Arguments:

	NtImagePathName - A required pointer that points to the NT Path string
	that identifies the image file that is to be loaded into the
	child process.

	ProcessParameters - A required pointer that points to parameters that
	are to passed to the child process.

	ProcessSecurityDescriptor - An optional pointer to the Security Descriptor
	give to the new process.

	ThreadSecurityDescriptor - An optional pointer to the Security Descriptor
	give to the new thread.

	ParentProcess - An optional process handle that will used to inherit
	certain properties from.

	InheritHandles - A boolean value.  TRUE specifies that object handles
	associated with the specified parent process are to be inherited
	by the new process, provided they have the OBJ_INHERIT attribute.
	FALSE specifies that the new process is to inherit no handles.

	DebugPort - An optional handle to the debug port associated with this
	process.

	ExceptionPort - An optional handle to the exception port associated with this
	process.

	ProcessInformation - A pointer to a variable that receives information
	about the new process and thread.

	CreateInfo - CreateInfo structure to fill for caller

	--*/

{
	NTSTATUS Status;
	HANDLE Section, File;
	OBJECT_ATTRIBUTES ObjectAttributes;
	PRTL_USER_PROCESS_PARAMETERS Parameters;
	SIZE_T ParameterLength;
	PVOID Environment;
	PWCHAR s;
	SIZE_T EnvironmentLength;
	SIZE_T RegionSize;
	PROCESS_BASIC_INFORMATION ProcessInfo;
	PPEB_ARCH Peb;
	ULONG_PTR ApiSetMapNew;

	//
	// Zero output parameter and probe the addresses at the same time
	//

	RtlZeroMemory(ProcessInformation, sizeof(*ProcessInformation));
	ProcessInformation->Length = sizeof(*ProcessInformation);

	//
	// Open the specified image file.
	//

	Status = RtlpOpenImageFile(NtImagePathName,
		Attributes & (OBJ_INHERIT | OBJ_CASE_INSENSITIVE),
		&File,
		TRUE
		);
	if (!NT_SUCCESS(Status)) {
		return(Status);
	}


	//
	// Create a memory section backed by the opened image file
	//

	Status = NtCreateSection(&Section,
		SECTION_ALL_ACCESS,
		NULL,
		NULL,
		PAGE_EXECUTE,
		SEC_IMAGE,
		File
		);
	// Will be closed in CreateProcess
	//NtClose(File);
	if (!NT_SUCCESS(Status)) {
		return(Status);
	}


	//
	// Create the user mode process, defaulting the parent process to the
	// current process if one is not specified.  The new process will not
	// have a name nor will the handle be inherited by other processes.
	//

	if (!ARGUMENT_PRESENT(ParentProcess)) {
		ParentProcess = NtCurrentProcess();
	}

	InitializeObjectAttributes(&ObjectAttributes, NULL, 0, NULL,
		ProcessSecurityDescriptor);

	if (!InheritHandles) {
		ProcessParameters->CurrentDirectory.Handle = NULL;
	}
	Status = NtCreateProcess(&ProcessInformation->Process,
		PROCESS_ALL_ACCESS,
		&ObjectAttributes,
		ParentProcess,
		InheritHandles,
		Section,
		DebugPort,
		ExceptionPort
		);
	if (!NT_SUCCESS(Status)) {
		NtClose(Section);
		return(Status);
	}


	//
	// Retrieve the interesting information from the image header
	//

	Status = NtQuerySection(Section,
		SectionImageInformation,
		&ProcessInformation->ImageInformation,
		sizeof(ProcessInformation->ImageInformation),
		NULL
		);
	if (!NT_SUCCESS(Status)) {
		NtClose(ProcessInformation->Process);
		NtClose(Section);
		return(Status);
	}

	Status = NtQueryInformationProcess(ProcessInformation->Process,
		ProcessBasicInformation,
		&ProcessInfo,
		sizeof(ProcessInfo),
		NULL
		);
	if (!NT_SUCCESS(Status)) {
		NtClose(ProcessInformation->Process);
		NtClose(Section);
		return(Status);
	}

	Peb = (PPEB_ARCH)ProcessInfo.PebBaseAddress;

	//
	// Duplicate Native handles into new process if any specified.
	// Note that the duplicated handles will overlay the input values.
	//
	Status = STATUS_SUCCESS;
	if (ProcessParameters->WindowFlags &  STARTF_USESTDHANDLES)
	{
		if (ProcessParameters->StandardInput && !IsConsoleHandle(ProcessParameters->StandardInput)) {

			Status = NtDuplicateObject(
				ParentProcess,
				ProcessParameters->StandardInput,
				ProcessInformation->Process,
				&ProcessParameters->StandardInput,
				0L,
				0L,
				DUPLICATE_SAME_ACCESS | DUPLICATE_SAME_ATTRIBUTES
				);
		}

		if (NT_SUCCESS(Status) && ProcessParameters->StandardOutput && !IsConsoleHandle(ProcessParameters->StandardOutput)) {

			Status = NtDuplicateObject(
				ParentProcess,
				ProcessParameters->StandardOutput,
				ProcessInformation->Process,
				&ProcessParameters->StandardOutput,
				0L,
				0L,
				DUPLICATE_SAME_ACCESS | DUPLICATE_SAME_ATTRIBUTES
				);
		}

		if (NT_SUCCESS(Status) && ProcessParameters->StandardError && !IsConsoleHandle(ProcessParameters->StandardError)) {

			Status = NtDuplicateObject(
				ParentProcess,
				ProcessParameters->StandardError,
				ProcessInformation->Process,
				&ProcessParameters->StandardError,
				0L,
				0L,
				DUPLICATE_SAME_ACCESS | DUPLICATE_SAME_ATTRIBUTES
				);
		}

		if (!NT_SUCCESS(Status)) {
			NtClose(ProcessInformation->Process);
			NtClose(Section);
			return Status;
		}
	}

	//
	// Possibly reserve some address space in the new process
	//

	// N.B.: Check present in real Windows, but we are IMAGE_SUBSYSTEM_WINDOWS_CUI for NTVDM, so skip check:
	// if (ProcessInformation->ImageInformation.SubSystemType == IMAGE_SUBSYSTEM_NATIVE) 
	{
		// N.B.: Not present in real Windows
		if (ProcessParameters->Flags & RTL_USER_PROC_RESERVE_16MB) {

			ULONG_PTR ApiSetMap;

			Environment = (PVOID)(1);
			RegionSize = (1024 * 1024) * 16L;

			Status = NtReadVirtualMemory(ProcessInformation->Process,
				&Peb->ApiSetMap,
				&ApiSetMap,
				sizeof(ApiSetMap),
				NULL
				);
			if (NT_SUCCESS(Status) && ApiSetMap < RegionSize)
			{
				// apisetschema.dll f*cks up our low address space! Get out of our way you damn, bloody bastard!

				// We are lazy and just copy the contents to a newly allocated section. Any idea if section can just be remapped instead somehow from user mode?
				MEMORY_BASIC_INFORMATION mbi;

				RtlZeroMemory(&mbi, sizeof(mbi));
				if (VirtualQueryEx(ProcessInformation->Process, (LPCVOID)ApiSetMap, &mbi, sizeof(mbi)) == sizeof(MEMORY_BASIC_INFORMATION))
				{
					if (ApiSetMapNew = (ULONG_PTR)VirtualAllocEx(ProcessInformation->Process, (LPVOID)(RegionSize + 0x10000), mbi.RegionSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE))
					{
						ULONG_PTR LocalBuffer = (ULONG_PTR)VirtualAlloc(NULL, mbi.RegionSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

						if (LocalBuffer)
						{
							if (NT_SUCCESS(Status = NtReadVirtualMemory(ProcessInformation->Process,
								(LPVOID)ApiSetMap,
								(LPVOID)LocalBuffer,
								(ULONG)mbi.RegionSize,
								NULL
								)))
							{
								if (NT_SUCCESS(Status = NtWriteVirtualMemory(ProcessInformation->Process,
									(LPVOID)ApiSetMapNew,
									(LPVOID)LocalBuffer,
									mbi.RegionSize,
									NULL
									)))
								{
									// Update PEB with new address
									if (NT_SUCCESS(Status = NtWriteVirtualMemory(ProcessInformation->Process,
										&Peb->ApiSetMap,
										&ApiSetMapNew,
										sizeof(ApiSetMapNew),
										NULL
										)))
									{
										// Now we can unmap old section
										if (!NT_SUCCESS((Status = NtUnmapViewOfSection(ProcessInformation->Process, (LPVOID)ApiSetMap))))
										{
											TRACE("LDNTVDM: Cannot unmap apisetschema: %08x\n", Status);
										}
									}
								}
							}
							VirtualFree((LPVOID)LocalBuffer, mbi.RegionSize, MEM_RELEASE);
						}
					}
				}
			}

			RegionSize = 0x10000 - 1;
			if (NT_SUCCESS(Status)) Status = NtAllocateVirtualMemory(ProcessInformation->Process,
				(PVOID *)&Environment,
				0,
				&RegionSize,
				MEM_RESERVE,
				PAGE_EXECUTE_READWRITE
				);
			if (!NT_SUCCESS(Status)) {
#ifdef USE_MAP0DRV
				// Fallback: We can try our Map0 driver to accomplish the task from Kernel mode
				if (!Map0Page((HANDLE)GetProcessId(ProcessInformation->Process)))
#endif
				{
					TRACE("LDNTVDM: Reserving low 64k for NTVDM failed: %08x. Did you set EnableLowVaAccess?\n", Status);
					NtClose(ProcessInformation->Process);
					NtClose(Section);
					return(Status);
				}
				else Status = STATUS_SUCCESS;
			}

			Environment = (PVOID)0x10000;
			RegionSize = (640 * 1024) - 0x10000;
			if (NT_SUCCESS(Status)) Status = NtAllocateVirtualMemory(ProcessInformation->Process,
				(PVOID *)&Environment,
				0,
				&RegionSize,
				MEM_RESERVE,
				PAGE_EXECUTE_READWRITE
				);
			if (!NT_SUCCESS(Status)) {
				TRACE("LDNTVDM: Reserving low 640k for NTVDM failed: %08x\n", Status);
				NtClose(ProcessInformation->Process);
				NtClose(Section);
				return(Status);
			}

			Environment = (PVOID)(640 * 1024);
			RegionSize = 384 * 1024;
			if (NT_SUCCESS(Status)) Status = NtAllocateVirtualMemory(ProcessInformation->Process,
				(PVOID *)&Environment,
				0,
				&RegionSize,
				MEM_RESERVE,
				PAGE_READWRITE
				);
			if (!NT_SUCCESS(Status)) {
				TRACE("LDNTVDM: Reserving ROM Area for NTVDM failed: %08x\n", Status);
				NtClose(ProcessInformation->Process);
				NtClose(Section);
				return(Status);
			}


			Environment = (PVOID)(1024 * 1024);
			RegionSize = (1024 * 1024) * (RESERVE_MB_INITIAL - 1);
			if (NT_SUCCESS(Status)) Status = NtAllocateVirtualMemory(ProcessInformation->Process,
				(PVOID *)&Environment,
				0,
				&RegionSize,
				MEM_RESERVE,
				PAGE_EXECUTE_READWRITE
				);
			if (!NT_SUCCESS(Status)) {
				TRACE("LDNTVDM: Reserving low 16MB for NTVDM failed: %08x\n", Status);
				NtClose(ProcessInformation->Process);
				NtClose(Section);
				return(Status);
			}


		}
		else

			if (ProcessParameters->Flags & RTL_USER_PROC_RESERVE_1MB) {

				Environment = (PVOID)(4);
				RegionSize = (1024 * 1024) - (256);

				Status = NtAllocateVirtualMemory(ProcessInformation->Process,
					(PVOID *)&Environment,
					0,
					&RegionSize,
					MEM_RESERVE,
					PAGE_READWRITE
					);
				if (!NT_SUCCESS(Status)) {
					NtClose(ProcessInformation->Process);
					NtClose(Section);
					return(Status);
				}
			}
	}

	//
	// Allocate virtual memory in the new process and use NtWriteVirtualMemory
	// to write a copy of the process environment block into the address
	// space of the new process.  Save the address of the allocated block in
	// the process parameter block so the new process can access it.
	//

	if (s = (PWCHAR)ProcessParameters->Environment) {
		while (*s++) {
			while (*s++) {
			}
		}
		EnvironmentLength = (SIZE_T)(s - (PWCHAR)ProcessParameters->Environment) * sizeof(WCHAR);

		Environment = NULL;
		RegionSize = EnvironmentLength;
		Status = NtAllocateVirtualMemory(ProcessInformation->Process,
			(PVOID *)&Environment,
			0,
			&RegionSize,
			MEM_COMMIT,
			PAGE_READWRITE
			);
		if (!NT_SUCCESS(Status)) {
			NtClose(ProcessInformation->Process);
			NtClose(Section);
			return(Status);
		}

		Status = NtWriteVirtualMemory(ProcessInformation->Process,
			Environment,
			ProcessParameters->Environment,
			EnvironmentLength,
			NULL
			);
		if (!NT_SUCCESS(Status)) {
			NtClose(ProcessInformation->Process);
			NtClose(Section);
			return(Status);
		}

		ProcessParameters->Environment = Environment;
	}

	//
	// Allocate virtual memory in the new process and use NtWriteVirtualMemory
	// to write a copy of the process parameters block into the address
	// space of the new process.  Set the initial parameter to the new thread
	// to be the address of the block in the new process's address space.
	//

	Parameters = NULL;
	ParameterLength = ProcessParameters->MaximumLength;
	Status = NtAllocateVirtualMemory(ProcessInformation->Process,
		(PVOID *)&Parameters,
		0,
		&ParameterLength,
		MEM_COMMIT,
		PAGE_READWRITE
		);
	if (!NT_SUCCESS(Status)) {
		NtClose(ProcessInformation->Process);
		NtClose(Section);
		return(Status);
	}

	// leecher1337: Need to do this to get valid pointers in target process
	// NB: Not present in original RtlCreateUserProcess
	RtlDeNormalizeProcessParams((PRTL_USER_PROCESS_PARAMETERS)ProcessParameters);

	Status = NtWriteVirtualMemory(ProcessInformation->Process,
		Parameters,
		ProcessParameters,
		ProcessParameters->Length,
		NULL
		);
	if (!NT_SUCCESS(Status)) {
		NtClose(ProcessInformation->Process);
		NtClose(Section);
		return(Status);
	}

	// leecher1337: Need to do this to get valid pointers in target process
	// NB: Not present in original RtlCreateUserProcess
	RtlNormalizeProcessParams((PRTL_USER_PROCESS_PARAMETERS)ProcessParameters);


	Status = NtWriteVirtualMemory(ProcessInformation->Process,
		&Peb->ProcessParameters,
		&Parameters,
		sizeof(Parameters),
		NULL
		);
	if (!NT_SUCCESS(Status)) {
		NtClose(ProcessInformation->Process);
		NtClose(Section);
		return(Status);
	}

	//
	// Create a suspended thread in the new process.  Specify the size and
	// position of the stack, along with the start address, initial parameter
	// and an SECURITY_DESCRIPTOR.  The new thread will not have a name and its handle will
	// not be inherited by other processes.
	//

	Status = RtlCreateUserThread(
		ProcessInformation->Process,
		ThreadSecurityDescriptor,
		TRUE,
		ProcessInformation->ImageInformation.ZeroBits,
		ProcessInformation->ImageInformation.MaximumStackSize,
		ProcessInformation->ImageInformation.CommittedStackSize,
		(PUSER_THREAD_START_ROUTINE)
		ProcessInformation->ImageInformation.TransferAddress,
		(PVOID)Peb,
		&ProcessInformation->Thread,
		&ProcessInformation->ClientId
		);
	if (!NT_SUCCESS(Status)) {
		NtClose(ProcessInformation->Process);
		NtClose(Section);
		return(Status);
	}

#ifndef TARGET_WIN7
	{
		// Initial thread gets marked in TEB in Windows 8+
		// This is usually done by setting Flag 0x40 (it is set to 0x60, so this includes 0x40)
		// to the pointer in Arg 9 of PspAllocateThread() call within NtCreateUserProcess
		// Unfortunately, NtCreateThreadEx Flags-Parameter doesn't allow to set Flag 0x40
		// on call, that's why we set it here.
		// I don't know if this is necessary, it doesn't really improve the situation
		union
		{
			USHORT SameTebFlags;                                                //0x17ee
			struct
			{
				USHORT SafeThunkCall : 1;                                         //0x17ee
				USHORT InDebugPrint : 1;                                          //0x17ee
				USHORT HasFiberData : 1;                                          //0x17ee
				USHORT SkipThreadAttach : 1;                                      //0x17ee
				USHORT WerInShipAssertCode : 1;                                   //0x17ee
				USHORT RanProcessInit : 1;                                        //0x17ee
				USHORT ClonedThread : 1;                                          //0x17ee
				USHORT SuppressDebugMsg : 1;                                      //0x17ee
				USHORT DisableUserStackWalk : 1;                                  //0x17ee
				USHORT RtlExceptionAttached : 1;                                  //0x17ee
				USHORT InitialThread : 1;                                         //0x17ee
				USHORT SessionAware : 1;                                          //0x17ee
				USHORT SpareSameTebBits : 4;                                      //0x17ee
			};
		} uFlags;
		THREAD_BASIC_INFORMATION tbi;

		if (NT_SUCCESS((Status = NtQueryInformationThread(ProcessInformation->Thread, ThreadBasicInformation, &tbi, sizeof(tbi), NULL))))
		{
			Status = NtReadVirtualMemory(ProcessInformation->Process,
				(LPVOID)((ULONG_PTR)tbi.TebBaseAddress + 0x17ee),
				&uFlags.SameTebFlags,
				sizeof(uFlags.SameTebFlags),
				NULL
				);
			if (NT_SUCCESS(Status))
			{
				uFlags.InitialThread = 1;
				// TEB:
				Status = NtWriteVirtualMemory(ProcessInformation->Process,
					(LPVOID)((ULONG_PTR)tbi.TebBaseAddress + 0x17ee),
					&uFlags.SameTebFlags,
					sizeof(uFlags.SameTebFlags),
					NULL
					);
				// TEB32:
				Status = NtWriteVirtualMemory(ProcessInformation->Process,
					(LPVOID)((ULONG_PTR)tbi.TebBaseAddress + 0x2fca),
					&uFlags.SameTebFlags,
					sizeof(uFlags.SameTebFlags),
					NULL
					);

			}
		}
		
		if (!NT_SUCCESS(Status))
		{
			TRACE("Cannot get ThreadBasicInformation for hThread %X\n", ProcessInformation->Thread);
			// Not critical
		}
	}
#endif

	//
	// Now close the section and file handles.  The objects they represent
	// will not actually go away until the process is destroyed.
	//

	// leecher1337: Will be closed in CreateProcess, therefore leave it open
	//NtClose(Section);

	//
	// Special extension: PspUpdateCreateInfo
	//
	CreateInfo->State = PsCreateSuccess;
	CreateInfo->SuccessState.u2.OutputFlags = 0;
	CreateInfo->SuccessState.FileHandle = File;
	CreateInfo->SuccessState.SectionHandle = Section;
	CreateInfo->SuccessState.CurrentParameterFlags = ProcessParameters->Flags;
	CreateInfo->SuccessState.PebAddressNative = (ULONGLONG)Peb;
	TRACE("CreateInfo->SuccessState.FileHandle = %X", CreateInfo->SuccessState.FileHandle);

	// PspSetupUserProcessAddressSpace:
	Status = NtReadVirtualMemory(ProcessInformation->Process,
		&Peb->ProcessParameters,
		&CreateInfo->SuccessState.UserProcessParametersNative,
		sizeof(CreateInfo->SuccessState.UserProcessParametersNative),
		NULL
		);

	Status = NtQueryInformationProcess(ProcessInformation->Process, ProcessWow64Information, &CreateInfo->SuccessState.PebAddressWow64, sizeof(PVOID), NULL);
	if (NT_SUCCESS(Status) && CreateInfo->SuccessState.PebAddressWow64)
	{
		PEB32 Peb32;
		xpRTL_USER_PROCESS_PARAMETERS Peb64;

		Status = NtReadVirtualMemory(ProcessInformation->Process,
			(LPVOID)CreateInfo->SuccessState.PebAddressWow64,
			&Peb32,
			sizeof(Peb32),
			NULL
			);
		if (NT_SUCCESS(Status))
		{
			Status = NtReadVirtualMemory(ProcessInformation->Process,
				(LPVOID)CreateInfo->SuccessState.UserProcessParametersNative,
				&Peb64,
				sizeof(xpRTL_USER_PROCESS_PARAMETERS),
				NULL
				);
			if (NT_SUCCESS(Status))
			{
#if 0
				char *Ntdll32Exports[] = {
					"LdrInitializeThunk",
					"KiUserExceptionDispatcher",
					"KiUserApcDispatcher",
					"KiUserCallbackDispatcher",
					"LdrHotPatchRoutine",
					"ExpInterlockedPopEntrySListFault",
					"ExpInterlockedPopEntrySListResume",
					"ExpInterlockedPopEntrySListEnd",
					"RtlUserThreadStart",
					"RtlpQueryProcessDebugInformationRemote",
					"EtwpNotificationThread"
				}; // , NtDll32 base
#endif
				Wow64pThunkProcessParameters(ProcessInformation->Process, CreateInfo->SuccessState.UserProcessParametersNative, &Peb32, &Peb64);
				CreateInfo->SuccessState.UserProcessParametersWow64 = Peb32.ProcessParameters;
				Peb32.ApiSetMap = (ULONG)ApiSetMapNew;
				Status = NtWriteVirtualMemory(ProcessInformation->Process,
					(LPVOID)CreateInfo->SuccessState.PebAddressWow64,
					&Peb32,
					sizeof(Peb32),
					NULL
					);
				if (NT_SUCCESS(Status) && !PspPrepareSystemDllInitBlock(ProcessInformation->Process))
					Status = STATUS_UNHANDLED_EXCEPTION;
			}
		}
	}


	//
	// Return success status
	//

	return(Status);
}

#endif
