#pragma once

#define VDM_PARTIALLY_CREATED	    1
#define VDM_FULLY_CREATED	    2
#define VDM_BEING_REUSED	    4
#define VDM_CREATION_SUCCESSFUL     8

#define BINARY_TYPE_DOS 	    0x10
#define BINARY_TYPE_WIN16           0x20
#define BINARY_TYPE_SEPWOW          0x40

#define BINARY_SUBTYPE_MASK         0xF
#define BINARY_TYPE_DOS_EXE	    01
#define BINARY_TYPE_DOS_COM	    02
#define BINARY_TYPE_DOS_PIF         03
#define BINARY_TYPE_WOW_EX      128

#define VDM_NOT_PRESENT 	    1
#define VDM_PRESENT_NOT_READY	    2
#define VDM_PRESENT_AND_READY	    4 

#define VDM_STATE_MASK		    7 

#define STARTF_USESHOWWINDOW    0x00000001
#define STARTF_USESIZE          0x00000002
#define STARTF_USEPOSITION      0x00000004
#define STARTF_FORCEONFEEDBACK  0x00000040
#define STARTF_FORCEOFFFEEDBACK 0x00000080
#define STARTF_USEHOTKEY        0x00000200  // ;4.0
#define STARTF_HASSHELLDATA     0x00000400  // ;Internal

// Update VDM entry indexes

#define UPDATE_VDM_UNDO_CREATION    0
#define UPDATE_VDM_PROCESS_HANDLE   1
#define UPDATE_VDM_HOOKED_CTRLC     2

#define TMP_TAG 0
#define VDM_TAG 1
#define MAKE_TAG( t ) 0 // We don't know basesrv tag

#define NtCurrentPeb() (NtCurrentTeb()->ProcessEnvironmentBlock)
#define RtlProcessHeap() GetProcessHeap()

#define ARGUMENT_PRESENT(ArgumentPointer)    ((CHAR *)(ArgumentPointer) != (CHAR *)(NULL) )

typedef struct _CSR_CLIENTCONNECT_MSG {
	IN ULONG ServerDllIndex;
	IN OUT PVOID ConnectionInformation;
	IN OUT ULONG ConnectionInformationLength;
} CSR_CLIENTCONNECT_MSG, *PCSR_CLIENTCONNECT_MSG;

#define CSR_NORMAL_PRIORITY_CLASS   0x00000010
#define CSR_IDLE_PRIORITY_CLASS     0x00000020
#define CSR_HIGH_PRIORITY_CLASS     0x00000040
#define CSR_REALTIME_PRIORITY_CLASS 0x00000080

#define RTL_MAX_DRIVE_LETTERS 32
#define RTL_DRIVE_LETTER_VALID (USHORT)0x0001

typedef struct _CSR_API_CONNECTINFO {
	IN ULONG ExpectedVersion;
	OUT ULONG CurrentVersion;
	OUT HANDLE ObjectDirectory;
	OUT PVOID SharedSectionBase;
	OUT PVOID SharedStaticServerData;
	OUT PVOID SharedSectionHeap;
	OUT ULONG DebugFlags;
	OUT ULONG SizeOfPebData;
	OUT ULONG SizeOfTebData;
	OUT ULONG NumberOfServerDllNames;
} CSR_API_CONNECTINFO, *PCSR_API_CONNECTINFO;

typedef struct _CSR_API_MSG {
	PORT_MESSAGE h;
	union {
		CSR_API_CONNECTINFO ConnectionRequest;
		struct {
			PCSR_CAPTURE_HEADER CaptureBuffer;
			CSR_API_NUMBER ApiNumber;
			ULONG ReturnValue;
			ULONG Reserved;
			union {
				CSR_CLIENTCONNECT_MSG ClientConnect;
				ULONG_PTR ApiMessageData[39];
			} u;
		};
	};
} CSR_API_MSG, *PCSR_API_MSG;

typedef struct _RTL_DRIVE_LETTER_CURDIR {
	USHORT Flags;
	USHORT Length;
	ULONG TimeStamp;
	STRING DosPath;
} RTL_DRIVE_LETTER_CURDIR, *PRTL_DRIVE_LETTER_CURDIR;

typedef struct _CURDIR {
	UNICODE_STRING DosPath;
	HANDLE Handle;
} CURDIR, *PCURDIR;

typedef struct _xpRTL_USER_PROCESS_PARAMETERS {
	ULONG MaximumLength;
	ULONG Length;

	ULONG Flags;
	ULONG DebugFlags;

	HANDLE ConsoleHandle;
	ULONG  ConsoleFlags;
	HANDLE StandardInput;
	HANDLE StandardOutput;
	HANDLE StandardError;

	CURDIR CurrentDirectory;        // ProcessParameters
	UNICODE_STRING DllPath;         // ProcessParameters
	UNICODE_STRING ImagePathName;   // ProcessParameters
	UNICODE_STRING CommandLine;     // ProcessParameters
	PVOID Environment;              // NtAllocateVirtualMemory

	ULONG StartingX;
	ULONG StartingY;
	ULONG CountX;
	ULONG CountY;
	ULONG CountCharsX;
	ULONG CountCharsY;
	ULONG FillAttribute;

	ULONG WindowFlags;
	ULONG ShowWindowFlags;
	UNICODE_STRING WindowTitle;     // ProcessParameters
	UNICODE_STRING DesktopInfo;     // ProcessParameters
	UNICODE_STRING ShellInfo;       // ProcessParameters
	UNICODE_STRING RuntimeData;     // ProcessParameters
	RTL_DRIVE_LETTER_CURDIR CurrentDirectores[RTL_MAX_DRIVE_LETTERS];
} xpRTL_USER_PROCESS_PARAMETERS, *xpPRTL_USER_PROCESS_PARAMETERS;


typedef ULONG(BASEP_CALL *fpBaseIsDosApplication)(
	IN PUNICODE_STRING  	PathName,
	IN NTSTATUS  	Status
	);
extern fpBaseIsDosApplication BaseIsDosApplication;

#ifdef NEED_BASEVDM
NTSTATUS
BASEP_CALL
BaseCheckVDM(
	IN  ULONG BinaryType,
	IN  PCWCH lpApplicationName,
	IN  PCWCH lpCommandLine,
	IN  PCWCH lpCurrentDirectory,
	IN  ANSI_STRING *pAnsiStringEnv,
	IN  BASE_API_MSG *m,
	IN OUT PULONG iTask,
	IN  DWORD dwCreationFlags,
	LPSTARTUPINFOW lpStartupInfo,
	IN HANDLE hUserToken OPTIONAL
	);
BOOL
BaseGetVdmConfigInfo(
	IN  LPCWSTR CommandLine,
	IN  ULONG   DosSeqId,
	IN  ULONG   BinaryType,
	IN  PUNICODE_STRING CmdLineString,
	OUT PULONG VdmSize
	);
BOOL BaseCreateVDMEnvironment(
	PWCHAR lpEnvironment,
	ANSI_STRING * pAStringEnv,
	UNICODE_STRING  *pUStringEnv
	);
BOOL
BaseUpdateVDMEntry(
	IN ULONG UpdateIndex,
	IN OUT HANDLE *WaitHandle,
	IN ULONG IndexInfo,
	IN ULONG BinaryType
	);
BOOL
BaseDestroyVDMEnvironment(
	ANSI_STRING *pAStringEnv,
	UNICODE_STRING *pUStringEnv
	);
#endif