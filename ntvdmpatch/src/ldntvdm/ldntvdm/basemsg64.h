#ifndef CSHORT
#define CSHORT short
#endif
#ifndef BASESRV_FIRST_API_NUMBER
#define BASESRV_FIRST_API_NUMBER 0
#endif
#ifndef ROUND_UP
#define ROUND_UP(n,size)	(((ULONG)(n) + (size - 1)) & ~(size - 1)) 
#endif

#ifndef DEFINE_BASEMSG32
#define HANDLE ULONGLONG
#define LPWSTR ULONGLONG
#define PCHAR ULONGLONG
#define LPSTARTUPINFOA ULONGLONG
#define LPSTR ULONGLONG
#define LPBYTE ULONGLONG
#define ULONG_PTR ULONGLONG

#define BASESRV_API_CONNECTINFO BASESRV_API_CONNECTINFO64
#define BASESRV_API_NUMBER BASESRV_API_NUMBER64
#define PORT_MESSAGE PORT_MESSAGE64
#define BASESRV_API_CONNECTINFO BASESRV_API_CONNECTINFO64
#define BASE_NLS_SET_USER_INFO_MSG BASE_NLS_SET_USER_INFO_MSG64
#define BASE_NLS_SET_MULTIPLE_USER_INFO_MSG BASE_NLS_SET_MULTIPLE_USER_INFO_MSG64
#define BASE_NLS_CREATE_SORT_SECTION_MSG BASE_NLS_CREATE_SORT_SECTION_MSG64
#define BASE_NLS_PRESERVE_SECTION_MSG BASE_NLS_PRESERVE_SECTION_MSG64
#define BASE_CREATEPROCESS_MSG BASE_CREATEPROCESS_MSG64
#define BASE_CREATETHREAD_MSG BASE_CREATETHREAD_MSG64
#define BASE_DEBUGPROCESS_MSG BASE_DEBUGPROCESS_MSG64
#define BASE_CHECKVDM_MSG BASE_CHECKVDM_MSG64
#define BASE_UPDATE_VDM_ENTRY_MSG BASE_UPDATE_VDM_ENTRY_MSG64
#define BASE_GET_NEXT_VDM_COMMAND_MSG BASE_GET_NEXT_VDM_COMMAND_MSG64
#define BASE_EXIT_VDM_MSG BASE_EXIT_VDM_MSG64
#define BASE_SET_REENTER_COUNT_MSG BASE_SET_REENTER_COUNT_MSG64
#define BASE_GET_VDM_EXIT_CODE_MSG BASE_GET_VDM_EXIT_CODE_MSG64
#define BASE_GET_SET_VDM_CUR_DIRS_MSG BASE_GET_SET_VDM_CUR_DIRS_MSG64
#define BASE_BAT_NOTIFICATION_MSG BASE_BAT_NOTIFICATION_MSG64
#define BASE_REGISTER_WOWEXEC_MSG BASE_REGISTER_WOWEXEC_MSG64
#define BASE_REFRESHINIFILEMAPPING_MSG BASE_REFRESHINIFILEMAPPING_MSG64
#define BASE_DEFINEDOSDEVICE_MSG BASE_DEFINEDOSDEVICE_MSG64
#define BASE_GETTEMPFILE_MSG BASE_GETTEMPFILE_MSG64
#define BASE_API_MSG BASE_API_MSG64
#define BASE_SHUTDOWNPARAM_MSG BASE_SHUTDOWNPARAM_MSG64
#define BASE_EXITPROCESS_MSG BASE_EXITPROCESS_MSG64
#define BASE_IS_FIRST_VDM_MSG BASE_IS_FIRST_VDM_MSG64
#define BASE_SOUNDSENTRY_NOTIFICATION_MSG BASE_SOUNDSENTRY_NOTIFICATION_MSG64
#define CLIENT_ID CLIENT_ID64
#define UNICODE_STRING UNICODE_STRING64

//
// Message format for messages sent from the client to the server
//

typedef enum {
	BasepCreateProcess = BASESRV_FIRST_API_NUMBER,
	BasepCreateThread,
	BasepGetTempFile,
	BasepExitProcess,
	BasepDebugProcess,
	BasepCheckVDM,
	BasepUpdateVDMEntry,
	BasepGetNextVDMCommand,
	BasepExitVDM,
	BasepIsFirstVDM,
	BasepGetVDMExitCode,
	BasepSetReenterCount,
	BasepSetProcessShutdownParam,
	BasepGetProcessShutdownParam,
	BasepNlsSetUserInfo,
	BasepNlsSetMultipleUserInfo,
	BasepNlsCreateSortSection,
	BasepNlsPreserveSection,
	BasepSetVDMCurDirs,
	BasepGetVDMCurDirs,
	BasepBatNotification,
	BasepRegisterWowExec,
	BasepSoundSentryNotification,
	BasepRefreshIniFileMapping,
	BasepDefineDosDevice,
	BasepMaxApiNumber
} BASESRV_API_NUMBER;

typedef struct _STARTUPINFOA64 {
	ULONG_PTR cb;
	LPSTR   lpReserved;
	LPSTR   lpDesktop;
	LPSTR   lpTitle;
	DWORD   dwX;
	DWORD   dwY;
	DWORD   dwXSize;
	DWORD   dwYSize;
	DWORD   dwXCountChars;
	DWORD   dwYCountChars;
	DWORD   dwFillAttribute;
	DWORD   dwFlags;
	WORD    wShowWindow;
	WORD    cbReserved2;
	DWORD   _align;
	LPBYTE  lpReserved2;
	HANDLE  hStdInput;
	HANDLE  hStdOutput;
	HANDLE  hStdError;
} STARTUPINFOA64, *LPSTARTUPINFOA64;

#ifndef CSR_MAKE_API_NUMBER
#define CSR_MAKE_API_NUMBER( DllIndex, ApiIndex ) \
    (CSR_API_NUMBER)(((DllIndex) << 16) | (ApiIndex))

#define CSR_APINUMBER_TO_SERVERDLLINDEX( ApiNumber ) \
    ((ULONG)((ULONG)(ApiNumber) >> 16) & 0xFFF)

#define CSR_APINUMBER_TO_APITABLEINDEX( ApiNumber ) \
    ((ULONG)((USHORT)(ApiNumber)))
#endif
#define BASESRV_SERVERDLL_INDEX 1

#pragma pack(8)
#endif

#define PCSR_CAPTURE_HEADER ULONG_PTR
#define CSR_API_NUMBER ULONG_PTR


#if !defined(DEFINE_BASEMSG32) || !defined(_WINTERNL_)
typedef struct {
	USHORT Length;
	USHORT MaximumLength;
	LPWSTR  Buffer;
} UNICODE_STRING;
#endif

typedef struct
{
	ULONG_PTR UniqueProcess;
	ULONG_PTR UniqueThread;
} CLIENT_ID;

typedef struct {
	union {
		struct {
			CSHORT DataLength;
			CSHORT TotalLength;
		} s1;
		ULONG Length;					// +04	+04
	} u1;
	union {
		struct {
			CSHORT Type;
			CSHORT DataInfoOffset;
		} s2;
		ULONG ZeroInit;					// +08	+08
	} u2;
	union {
		CLIENT_ID ClientId;
		double DoNotUseThisField;		// +16	+16
	};
	ULONG_PTR MessageId;				// +20	+24
	union {
		ULONG_PTR ClientViewSize;		// +24	+32
		ULONG_PTR CallbackId;
	};
	//  UCHAR Data[];						// +24	+32
} PORT_MESSAGE;

typedef struct {
    IN ULONG ExpectedVersion;
    OUT HANDLE DefaultObjectDirectory;
    OUT ULONG WindowsVersion;
    OUT ULONG CurrentVersion;
    OUT ULONG DebugFlags;
    OUT WCHAR WindowsDirectory[ MAX_PATH ];
    OUT WCHAR WindowsSystemDirectory[ MAX_PATH ];
} BASESRV_API_CONNECTINFO;

#define BASESRV_VERSION 0x10000

typedef struct {
    LPWSTR pValue;
    LPWSTR pCacheString;
    LPWSTR pData;
    ULONG DataLength;
} BASE_NLS_SET_USER_INFO_MSG;

typedef struct {
    ULONG Flags;
    ULONG DataLength;
    LPWSTR pPicture;
    LPWSTR pSeparator;
    LPWSTR pOrder;
    LPWSTR pTLZero;
    LPWSTR pTimeMarkPosn;
} BASE_NLS_SET_MULTIPLE_USER_INFO_MSG;

typedef struct {
    UNICODE_STRING SectionName;
    HANDLE hNewSection;
    LARGE_INTEGER SectionSize;
} BASE_NLS_CREATE_SORT_SECTION_MSG;

typedef struct {
    HANDLE hSection;
} BASE_NLS_PRESERVE_SECTION_MSG;

typedef struct {
    ULONG ShutdownLevel;
    ULONG ShutdownFlags;
} BASE_SHUTDOWNPARAM_MSG;

typedef struct {
    HANDLE ProcessHandle;
    HANDLE ThreadHandle;
    CLIENT_ID ClientId;
    CLIENT_ID DebuggerClientId;
    ULONG CreationFlags;
    ULONG IsVDM;
    HANDLE hVDM;
} BASE_CREATEPROCESS_MSG;

typedef struct {
    HANDLE ThreadHandle;
    CLIENT_ID ClientId;
} BASE_CREATETHREAD_MSG;

typedef struct {
    UINT uUnique;
} BASE_GETTEMPFILE_MSG;

typedef struct {
    UINT uExitCode;
} BASE_EXITPROCESS_MSG;

typedef struct {
    DWORD dwProcessId;
    CLIENT_ID DebuggerClientId;
    PVOID AttachCompleteRoutine;
} BASE_DEBUGPROCESS_MSG;

typedef struct {
    ULONG_PTR  iTask;				// + 38  40
    HANDLE ConsoleHandle;			// + 3C  48
    ULONG_PTR  BinaryType;			// + 40  50
    HANDLE WaitObjectForParent;		// + 44  58
    HANDLE StdIn;					// + 48  60
    HANDLE StdOut;					// + 4C  68
    HANDLE StdErr;					// + 50  70
    ULONG  CodePage;				// + 54  78
    ULONG  dwCreationFlags;			// + 58  7C
    PCHAR  CmdLine;					// + 5C  80
    PCHAR  AppName;					// + 60  88
    PCHAR  PifFile;					// + 64  90
    PCHAR  CurDirectory;			// + 68  98
    PCHAR  Env;						// + 6C  A0
    ULONG_PTR  EnvLen;				// + 70  A8
    LPSTARTUPINFOA StartupInfo;		// + 74  B0
    PCHAR  Desktop;					// + 78  B8
    ULONG_PTR  DesktopLen;			// + 7C  C0
    PCHAR  Title;					// + 80  C8
    ULONG_PTR  TitleLen;			// + 84  D0
    PCHAR  Reserved;				// + 88  D8
    ULONG  ReservedLen;				// + 8C  E0
    USHORT CmdLen;					// + 90  E4
    USHORT AppLen;					// + 92  E6
    USHORT PifLen;					// + 94  E8
    USHORT CurDirectoryLen;			// + 96  EA
    USHORT CurDrive;				// + 98  EC
    USHORT VDMState;				// + 9A  EE
	ULONG_PTR Unknown;				// + 9C  F0
} BASE_CHECKVDM_MSG;

typedef struct {
    ULONG  iTask;
    ULONG  BinaryType;
    HANDLE ConsoleHandle;
    HANDLE VDMProcessHandle;
    HANDLE WaitObjectForParent;
    WORD   EntryIndex;
    WORD   VDMCreationState;
} BASE_UPDATE_VDM_ENTRY_MSG;

typedef struct {
    ULONG_PTR  iTask;
    HANDLE ConsoleHandle;
    HANDLE WaitObjectForVDM;
    HANDLE StdIn;
    HANDLE StdOut;
    HANDLE StdErr;
    ULONG  CodePage;
    ULONG  dwCreationFlags;
    ULONG  ExitCode;
    PCHAR  CmdLine;
    PCHAR  AppName;
    PCHAR  PifFile;
    PCHAR  CurDirectory;
    PCHAR  Env;
    ULONG_PTR  EnvLen;
    LPSTARTUPINFOA StartupInfo;
    PCHAR  Desktop;
    ULONG_PTR DesktopLen;
    PCHAR  Title;
    ULONG_PTR  TitleLen;
    PCHAR  Reserved;
    ULONG  ReservedLen;
    USHORT CurrentDrive;
    USHORT CmdLen;
    USHORT AppLen;
    USHORT PifLen;
    USHORT CurDirectoryLen;
    USHORT VDMState;
    BOOLEAN fComingFromBat;
} BASE_GET_NEXT_VDM_COMMAND_MSG;

typedef struct {
    HANDLE ConsoleHandle;
    ULONG  iWowTask;
    HANDLE WaitObjectForVDM;
} BASE_EXIT_VDM_MSG;

typedef struct {
    HANDLE ConsoleHandle;
    ULONG  fIncDec;
} BASE_SET_REENTER_COUNT_MSG;

typedef struct {
    BOOL    FirstVDM;
} BASE_IS_FIRST_VDM_MSG;

typedef struct {
    HANDLE ConsoleHandle;
    HANDLE hParent;
    ULONG  ExitCode;
} BASE_GET_VDM_EXIT_CODE_MSG;

typedef struct {
    HANDLE ConsoleHandle;
    PCHAR  lpszzCurDirs;
    ULONG  cchCurDirs;
} BASE_GET_SET_VDM_CUR_DIRS_MSG;

typedef struct {
    HANDLE ConsoleHandle;
    ULONG  fBeginEnd;
} BASE_BAT_NOTIFICATION_MSG;

typedef struct {
    HANDLE hwndWowExec;
} BASE_REGISTER_WOWEXEC_MSG;

typedef struct {
    ULONG  VideoMode;
} BASE_SOUNDSENTRY_NOTIFICATION_MSG;

typedef struct {
    UNICODE_STRING IniFileName;
} BASE_REFRESHINIFILEMAPPING_MSG;

typedef struct {
    ULONG Flags;
    UNICODE_STRING DeviceName;
    UNICODE_STRING TargetPath;
} BASE_DEFINEDOSDEVICE_MSG;

typedef struct {
    PORT_MESSAGE h;
    PCSR_CAPTURE_HEADER CaptureBuffer;
    CSR_API_NUMBER ApiNumber;
#if defined(DEFINE_BASEMSG32) && !defined(_WIN64)
	BYTE Dummy[16];
#endif
    ULONG ReturnValue;
    ULONG Reserved;
    union {
        BASE_NLS_SET_USER_INFO_MSG NlsSetUserInfo;
        BASE_NLS_SET_MULTIPLE_USER_INFO_MSG NlsSetMultipleUserInfo;
        BASE_NLS_CREATE_SORT_SECTION_MSG NlsCreateSortSection;
        BASE_NLS_PRESERVE_SECTION_MSG NlsPreserveSection;
        BASE_SHUTDOWNPARAM_MSG ShutdownParam;
        BASE_CREATEPROCESS_MSG CreateProcess;
        BASE_CREATETHREAD_MSG CreateThread;
        BASE_GETTEMPFILE_MSG GetTempFile;
        BASE_EXITPROCESS_MSG ExitProcess;
        BASE_DEBUGPROCESS_MSG DebugProcess;
        BASE_CHECKVDM_MSG CheckVDM;
        BASE_UPDATE_VDM_ENTRY_MSG UpdateVDMEntry;
        BASE_GET_NEXT_VDM_COMMAND_MSG GetNextVDMCommand;
        BASE_EXIT_VDM_MSG ExitVDM;
        BASE_IS_FIRST_VDM_MSG IsFirstVDM;
        BASE_GET_VDM_EXIT_CODE_MSG GetVDMExitCode;
        BASE_SET_REENTER_COUNT_MSG SetReenterCount;
        BASE_GET_SET_VDM_CUR_DIRS_MSG GetSetVDMCurDirs;
        BASE_BAT_NOTIFICATION_MSG BatNotification;
        BASE_REGISTER_WOWEXEC_MSG RegisterWowExec;
        BASE_SOUNDSENTRY_NOTIFICATION_MSG SoundSentryNotification;
        BASE_REFRESHINIFILEMAPPING_MSG RefreshIniFileMapping;
        BASE_DEFINEDOSDEVICE_MSG DefineDosDeviceApi;
    } u;
} BASE_API_MSG;


#ifndef DEFINE_BASEMSG32
#undef BASESRV_API_CONNECTINFO
#undef BASESRV_API_NUMBER
#undef PORT_MESSAGE
#undef BASESRV_API_CONNECTINFO
#undef BASE_NLS_SET_USER_INFO_MSG
#undef BASE_NLS_SET_MULTIPLE_USER_INFO_MSG
#undef BASE_NLS_CREATE_SORT_SECTION_MSG
#undef BASE_NLS_PRESERVE_SECTION_MSG
#undef BASE_CREATEPROCESS_MSG
#undef BASE_CREATETHREAD_MSG
#undef BASE_DEBUGPROCESS_MSG
#undef BASE_CHECKVDM_MSG
#undef BASE_UPDATE_VDM_ENTRY_MSG
#undef BASE_GET_NEXT_VDM_COMMAND_MSG
#undef BASE_EXIT_VDM_MSG
#undef BASE_SET_REENTER_COUNT_MSG
#undef BASE_GET_VDM_EXIT_CODE_MSG
#undef BASE_GET_SET_VDM_CUR_DIRS_MSG
#undef BASE_BAT_NOTIFICATION_MSG
#undef BASE_REGISTER_WOWEXEC_MSG
#undef BASE_REFRESHINIFILEMAPPING_MSG
#undef BASE_DEFINEDOSDEVICE_MSG
#undef BASE_API_MSG
#undef BASE_SHUTDOWNPARAM_MSG
#undef BASE_GETTEMPFILE_MSG
#undef BASE_EXITPROCESS_MSG
#undef BASE_IS_FIRST_VDM_MSG
#undef BASE_SOUNDSENTRY_NOTIFICATION_MSG
#undef UNICODE_STRING

#undef HANDLE
#undef LPWSTR
#undef PCHAR
#undef LPSTARTUPINFOA
#undef UNICODE_STRING
#undef PCSR_CAPTURE_HEADER
#undef PORT_MESSAGE
#undef CLIENT_ID
#undef CSR_API_NUMBER
#undef ULONG_PTR
#undef LPSTR
#undef LPBYTE
#pragma pack()
#define DEFINE_BASEMSG32
#include "basemsg64.h"
#endif

#undef CSHORT
