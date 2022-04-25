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
#include "x64typespush.h"

#define BASESRV_API_NUMBER BASESRV_API_NUMBER64
#define PORT_MESSAGE PORT_MESSAGE64
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
#define BASE_CREATEPROCESS_MSG BASE_CREATEPROCESS_MSG64
#define BASE_SXS_CREATEPROCESS_MSG BASE_SXS_CREATEPROCESS_MSG64
#define BASE_SXS_STREAM BASE_SXS_STREAM64
#define BASE_IS_FIRST_VDM_MSG BASE_IS_FIRST_VDM_MSG64
#define BASE_SOUNDSENTRY_NOTIFICATION_MSG BASE_SOUNDSENTRY_NOTIFICATION_MSG64
#define CLIENT_ID CLIENT_ID64
#define UNICODE_STRING UNICODE_STRING64

typedef enum _BASESRV_API_NUMBER
{
	BasepCreateProcess = BASESRV_FIRST_API_NUMBER,
	BasepCreateThread,
	BasepGetTempFile,
	BasepExitProcess,
	BasepDebugProcess,  // Deprecated
	BasepCheckVDM,
	BasepUpdateVDMEntry,
	BasepGetNextVDMCommand,
	BasepExitVDM,
	BasepIsFirstVDM,
	BasepGetVDMExitCode,
	BasepSetReenterCount,
	BasepSetProcessShutdownParam,
	BasepGetProcessShutdownParam,
#ifdef TARGET_WINXP
	BaseSrvNlsSetUserInfo,
	BaseSrvNlsSetMultipleUserInfo,
	BaseSrvNlsCreateSection,
#endif
	BasepSetVDMCurDirs,
	BasepGetVDMCurDirs,
	BasepBatNotification,
	BasepRegisterWowExec,
	BasepSoundSentryNotification,
	BasepRefreshIniFileMapping,
	BasepDefineDosDevice,
	BasepSetTermsrvAppInstallMode,
	BasepNlsUpdateCacheCount,
	BasepSetTermsrvClientTimeZone,
	BasepSxsCreateActivationContext,
	BasepDebugProcessStop, // Alias to BasepDebugProcess, deprecated
	BasepRegisterThread,
	BasepNlsGetUserInfo,
	BasepMaxApiNumber
} BASESRV_API_NUMBER, *PBASESRV_API_NUMBER;


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

#define CSR_API_NUMBER ULONG

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

#define PCSR_CAPTURE_HEADER LPVOID


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
		ULONG Length;					// +00	+00
	} u1;
	union {
		struct {
			CSHORT Type;
			CSHORT DataInfoOffset;
		} s2;
		ULONG ZeroInit;					// +04	+04
	} u2;
	union {
		CLIENT_ID ClientId;
		double DoNotUseThisField;		// +08  +08
	};
	ULONG MessageId;					// +10	+18
	union {
		ULONG_PTR ClientViewSize;
		ULONG     CallbackId;
	};
	//  UCHAR Data[];					// +14  +1C
} PORT_MESSAGE;							// +18  +24  -> +18  +28 aligned
#define BASESRV_VERSION 0x10000

typedef struct {
    ULONG ShutdownLevel;
    ULONG ShutdownFlags;
} BASE_SHUTDOWNPARAM_MSG;

typedef struct {
    UINT uUnique;
} BASE_GETTEMPFILE_MSG;

typedef struct
{
	BYTE byte0;						// + 00  00
	BYTE byte1;						// + 01  01
	BYTE byte2;						// + 02  02
	UNICODE_STRING FileName;		// + 04  08
	HANDLE FileHandle;				// + 0C  18
	HANDLE SectionHandle;			// + 10  20
#if defined(DEFINE_BASEMSG32) && !defined(_WIN64)
	ULONG unk;
#endif
	ULONG64 Offset;					// + 18  28
	ULONG_PTR Size;					// + 20  30
#if defined(DEFINE_BASEMSG32) && !defined(_WIN64)
	ULONG pad;
#endif
} BASE_SXS_STREAM;					// + 28  38

typedef struct
{
	ULONG Flags;					// + 00  00
	ULONG ProcessParameterFlags;	// + 04  04
	BASE_SXS_STREAM ManifestStream;	// + 08  08
	BASE_SXS_STREAM PolicyStream;	// + 30  40
	UNICODE_STRING AssemblyName;	// + 58  78
} BASE_SXS_CREATEPROCESS_MSG;		// + 60  88

typedef struct {
	HANDLE ProcessHandle;			// + 00  00
	HANDLE ThreadHandle;			// + 04  08
	CLIENT_ID ClientId;				// + 08  10
	ULONG CreationFlags;			// + 10  20
	ULONG VdmBinaryType;			// + 14  24
	ULONG VdmTask;					// + 18  28
	HANDLE hVDM;					// + 1C  30
	BASE_SXS_CREATEPROCESS_MSG Sxs;	// + 20  38
	ULONG64 PebAddressNative;       // + 80  C0
	ULONG_PTR PebAddressWow64;		// + 84  C8
	USHORT ProcessorArchitecture;	// + 88  D0
} BASE_CREATEPROCESS_MSG;			// + 90  D8

typedef struct {
    UINT uExitCode;
} BASE_EXITPROCESS_MSG;

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
	LUID   *AuthenticationId;		// + 9C  F0
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
	HANDLE ConsoleHandle;
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
    PORT_MESSAGE64 h;							// 64       32
	PCSR_CAPTURE_HEADER CaptureBuffer;			// 0x28     0x28
    CSR_API_NUMBER ApiNumber;					// 0x30     0x2C
    ULONG ReturnValue;							// 0x34     0x30
    ULONG Reserved;								// 0x38     0x34
    union {										// 0x3C     0x38
        BASE_SHUTDOWNPARAM_MSG ShutdownParam;
        BASE_GETTEMPFILE_MSG GetTempFile;
		BASE_CREATEPROCESS_MSG CreateProcess;
        BASE_EXITPROCESS_MSG ExitProcess;
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
#undef BASESRV_API_NUMBER
#undef PORT_MESSAGE
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
#undef BASE_CREATEPROCESS_MSG
#undef BASE_SXS_CREATEPROCESS_MSG
#undef BASE_SXS_STREAM
#undef BASE_IS_FIRST_VDM_MSG
#undef BASE_SOUNDSENTRY_NOTIFICATION_MSG
#undef UNICODE_STRING

#undef UNICODE_STRING
#undef PCSR_CAPTURE_HEADER
#undef PORT_MESSAGE
#undef CLIENT_ID
#include "x64typespop.h"
#pragma pack()
#define DEFINE_BASEMSG32
#include "basemsg64.h"
#endif

#undef CSHORT
