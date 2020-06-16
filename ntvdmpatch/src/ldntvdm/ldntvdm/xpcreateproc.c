/*Project: ldntvdm
* Module : xpcreateproc
* Author : leecher@dose.0wnz.at, CreateProcess code taken from ReactOS
* Descr. : Process creation on Windows XP differs a lot from later Windows
*          versions, therefore we handle it in a seperate module.
*          We need to hook NtCreateSection, check if it's a DOS or Win16
*          application, fake return value so that NtVdm64CreateProcess
*          gets called (so that we can get hold of the required parameters),
*          then form a call to our custom CreateProcessInternalW function, that
*          basically mimics the real XP version of the function, but includes
*          handling for DOS-Applications/NTVDM
* Changes: 01.06.2020  - Created
*/

#include <ntstatus.h>
#define WIN32_NO_STATUS
#include "ldntvdm.h"
#include "xpwrap.h"
#include "ntrtl.h"
#include "ntmmapi.h"
#include "ntpsapi.h"
#include "iathook.h"
#include "injector32.h"
#include "detour.h"
#include "basevdm.h"
#include "symcache.h"
#include "symeng.h"
#include "csrsswrap.h"

#ifndef _WIN64
#define BUILD_WOW6432
NTSTATUS CsrBasepCreateProcessHook(BASE_CREATEPROCESS_MSG *a);
#endif
#define ASSERT 

#ifdef TARGET_WINXP

#define MAXUSHORT   0xffff      // winnt

#define CREATE_WITH_USERPROFILE 0x0200000
#define PRIORITY_CLASS_MASK (NORMAL_PRIORITY_CLASS|IDLE_PRIORITY_CLASS|HIGH_PRIORITY_CLASS|REALTIME_PRIORITY_CLASS)

extern NTSTATUS LastCreateUserProcessError;
extern HMODULE g_hInst;

typedef BASE_CREATEPROCESS_MSG *PBASE_CREATEPROCESS_MSG;
typedef BASE_CHECKVDM_MSG *PBASE_CHECKVDM_MSG;
typedef LONG KPRIORITY;

#define PROCESS_PRIORITY_CLASS_UNKNOWN      0
#define PROCESS_PRIORITY_CLASS_IDLE         1
#define PROCESS_PRIORITY_CLASS_NORMAL       2
#define PROCESS_PRIORITY_CLASS_HIGH         3
#define PROCESS_PRIORITY_CLASS_REALTIME     4
#define PROCESS_PRIORITY_CLASS_BELOW_NORMAL 5
#define PROCESS_PRIORITY_CLASS_ABOVE_NORMAL 6

// Update VDM entry indexes
#define UPDATE_VDM_UNDO_CREATION    0
#define UPDATE_VDM_PROCESS_HANDLE   1
#define UPDATE_VDM_HOOKED_CTRLC     2

#define FOREGROUND_BASE_PRIORITY  9
#define NORMAL_BASE_PRIORITY      8

typedef struct _PROCESS_PRIORITY_CLASS {
	BOOLEAN Foreground;
	UCHAR PriorityClass;
} PROCESS_PRIORITY_CLASS, *PPROCESS_PRIORITY_CLASS;

typedef enum _xpPROCESSINFOCLASS {
	xpProcessBasicInformation,
	ProcessQuotaLimits,
	ProcessIoCounters,
	ProcessVmCounters,
	ProcessTimes,
	ProcessBasePriority,
	ProcessRaisePriority,
	xpProcessDebugPort,
	ProcessExceptionPort,
	ProcessAccessToken,
	ProcessLdtInformation,
	ProcessLdtSize,
	ProcessDefaultHardErrorMode,
	ProcessIoPortHandlers,          // Note: this is kernel mode only
	ProcessPooledUsageAndLimits,
	ProcessWorkingSetWatch,
	ProcessUserModeIOPL,
	ProcessEnableAlignmentFaultFixup,
	ProcessPriorityClass,
	ProcessWx86Information,
	ProcessHandleCount,
	ProcessAffinityMask,
	ProcessPriorityBoost,
	MaxProcessInfoClass
} xpPROCESSINFOCLASS;

typedef enum _BASE_CONTEXT_TYPE {
	BaseContextTypeProcess,
	BaseContextTypeThread,
	BaseContextTypeFiber
} BASE_CONTEXT_TYPE, *PBASE_CONTEXT_TYPE;

typedef
BOOL
(*fpNtVdm64CreateProcess)(
	BOOL fPrefixMappedApplicationName,
	LPCWSTR lpApplicationName,
	LPCWSTR lpCommandLine,
	LPSECURITY_ATTRIBUTES lpProcessAttributes,
	LPSECURITY_ATTRIBUTES lpThreadAttributes,
	BOOL bInheritHandles,
	DWORD dwCreationFlags,
	LPVOID lpEnvironment,
	LPCWSTR lpCurrentDirectory,
	LPSTARTUPINFOW lpStartupInfo,
	LPPROCESS_INFORMATION lpProcessInformation
	);
fpNtVdm64CreateProcess NtVdm64CreateProcess;
fpNtVdm64CreateProcess NtVdm64CreateProcessReal = NULL;

typedef 
NTSTATUS
(NTAPI
	*fpNtCreateSection)(
		OUT PHANDLE SectionHandle,
		IN ACCESS_MASK DesiredAccess,
		IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
		IN PLARGE_INTEGER MaximumSize OPTIONAL,
		IN ULONG SectionPageProtection,
		IN ULONG AllocationAttributes,
		IN HANDLE FileHandle OPTIONAL
		);
fpNtCreateSection NtCreateSectionReal;


#if defined(BUILD_WOW6432)
typedef
NTSTATUS
(WINAPI *fpCsrBasepCreateProcess)(
	PBASE_CREATEPROCESS_MSG a);
fpCsrBasepCreateProcess CsrBasepCreateProcess = NULL, CsrBasepCreateProcessReal = NULL;
#endif

typedef
NTSTATUS
(WINAPI *fpBaseCreateStack)(
	HANDLE Process,
	ULONG StackSize,
	ULONG MaximumStackSize,
	PINITIAL_TEB InitialTeb
	);
fpBaseCreateStack BaseCreateStack = NULL;

typedef
POBJECT_ATTRIBUTES
(WINAPI *fpBaseFormatObjectAttributes)(
	POBJECT_ATTRIBUTES ObjectAttributes,
	PSECURITY_ATTRIBUTES SecurityAttributes,
	PUNICODE_STRING ObjectName
	);
fpBaseFormatObjectAttributes BaseFormatObjectAttributes = NULL;

typedef 
NTSTATUS
(WINAPI *fpBasepIsProcessAllowed)(LPCWSTR lpApplicationName);
fpBasepIsProcessAllowed BasepIsProcessAllowed = NULL;

typedef
BOOLEAN
(WINAPI
	*fpBasePushProcessParameters)(IN ULONG ParameterFlags,
		IN HANDLE ProcessHandle,
		IN PPEB RemotePeb,
		IN LPCWSTR ApplicationPathName,
		IN LPWSTR lpCurrentDirectory,
		IN LPWSTR lpCommandLine,
		IN LPVOID lpEnvironment,
		IN LPSTARTUPINFOW StartupInfo,
		IN DWORD CreationFlags,
		IN BOOL InheritHandles,
		IN ULONG ImageSubsystem,
		IN PVOID AppCompatData,
		IN ULONG AppCompatDataSize);
fpBasePushProcessParameters BasePushProcessParameters = NULL;

typedef
PVOID
(*fpBasepIsRealtimeAllowed)(
	BOOLEAN LeaveEnabled
	);
fpBasepIsRealtimeAllowed BasepIsRealtimeAllowed = NULL;

typedef VOID
(WINAPI *fpBaseInitializeContext)(
	OUT PCONTEXT Context,
	IN PVOID Parameter OPTIONAL,
	IN PVOID InitialPc OPTIONAL,
	IN PVOID InitialSp OPTIONAL,
	IN BASE_CONTEXT_TYPE ContextType
	);
fpBaseInitializeContext BaseInitializeContext = NULL;

typedef
BOOL
(WINAPI *fpBuildSubSysCommandLine)(
	LPWSTR  SubSysName,
	LPCWSTR lpApplicationName,
	LPCWSTR lpCommandLine,
	PUNICODE_STRING SubSysCommandLine
	);
fpBuildSubSysCommandLine BuildSubSysCommandLine = NULL;

typedef
VOID
(WINAPI *fpStuffStdHandle)(
	IN HANDLE ProcessHandle,
	IN HANDLE StandardHandle,
	IN PHANDLE Address);
fpStuffStdHandle StuffStdHandle = NULL;

typedef
VOID
(WINAPI *fpBasepFreeAppCompatData)(
	IN PVOID AppCompatData,
	IN PVOID AppCompatSxsData
	);
fpBasepFreeAppCompatData BasepFreeAppCompatData = NULL;

typedef
LPWSTR
(WINAPI *fpBaseComputeProcessExePath)(
	IN LPWSTR FullPath
	);
fpBaseComputeProcessExePath BaseComputeProcessExePath = NULL;

typedef
NTSTATUS
(WINAPI *fpBasepReplaceProcessThreadTokens)(IN HANDLE TokenHandle,
	IN HANDLE ProcessHandle,
	IN HANDLE ThreadHandle);
fpBasepReplaceProcessThreadTokens BasepReplaceProcessThreadTokens = NULL;

typedef
NTSTATUS
(WINAPI *fpBasepCheckBadapp)(IN HANDLE FileHandle,
	IN PWCHAR ApplicationName,
	IN PWCHAR Environment,
	IN USHORT ExeType,
	IN PVOID* SdbQueryAppCompatData,
	IN PULONG SdbQueryAppCompatDataSize,
	IN PVOID* SxsData,
	IN PULONG SxsDataSize,
	OUT PULONG FusionFlags);
fpBasepCheckBadapp BasepCheckBadapp = NULL;

typedef
BOOLEAN
(WINAPI *fpBasepIsImageVersionOk)(
	IN ULONG ImageMajorVersion,
	IN ULONG ImageMinorVersion);
fpBasepIsImageVersionOk BasepIsImageVersionOk = NULL;

typedef
NTSTATUS
(WINAPI *fpBasepCheckWinSaferRestrictions)(
	IN HANDLE UserToken,
	IN LPWSTR ApplicationName,
	IN HANDLE FileHandle,
	OUT PBOOLEAN InJob,
	OUT PHANDLE NewToken,
	OUT PHANDLE JobHandle
	);
fpBasepCheckWinSaferRestrictions BasepCheckWinSaferRestrictions = NULL;

NTSYSAPI HANDLE NTAPI 	DbgUiGetThreadDebugObject(VOID);
NTSYSAPI NTSTATUS NTAPI 	DbgUiConnectToDbg(VOID);

#define DPRINT TRACE
#define DPRINT1 TRACE
#define CMD_STRING   L"cmd /c "
#define IsConsoleHandle(h) (((ULONG_PTR)(h) & 0x10000003) == 0x3)
#define AddToHandle(x,y)  (x) = (HANDLE)((ULONG_PTR)(x) | (y));
#define RemoveFromHandle(x,y)  (x) = (HANDLE)((ULONG_PTR)(x) & ~(y));
#define RtlGetProcessHeap GetProcessHeap

//
// Flags for NtCreateProcessEx
//
#define PROCESS_CREATE_FLAGS_BREAKAWAY              0x00000001
#define PROCESS_CREATE_FLAGS_NO_DEBUG_INHERIT       0x00000002
#define PROCESS_CREATE_FLAGS_INHERIT_HANDLES        0x00000004
#define PROCESS_CREATE_FLAGS_OVERRIDE_ADDRESS_SPACE 0x00000008
#define PROCESS_CREATE_FLAGS_LARGE_PAGES            0x00000010
#define PROCESS_CREATE_FLAGS_ALL_LARGE_PAGE_FLAGS   PROCESS_CREATE_FLAGS_LARGE_PAGES
#define PROCESS_CREATE_FLAGS_LEGAL_MASK             (PROCESS_CREATE_FLAGS_BREAKAWAY | \
                                                      PROCESS_CREATE_FLAGS_NO_DEBUG_INHERIT | \
                                                      PROCESS_CREATE_FLAGS_INHERIT_HANDLES | \
                                                      PROCESS_CREATE_FLAGS_OVERRIDE_ADDRESS_SPACE | \
                                                      PROCESS_CREATE_FLAGS_ALL_LARGE_PAGE_FLAGS)

// Privileges
#define SE_MIN_WELL_KNOWN_PRIVILEGE (2L)
#define SE_CREATE_TOKEN_PRIVILEGE (2L)
#define SE_ASSIGNPRIMARYTOKEN_PRIVILEGE (3L)
#define SE_LOCK_MEMORY_PRIVILEGE (4L)
#define SE_INCREASE_QUOTA_PRIVILEGE (5L)

typedef struct _KSYSTEM_TIME
{
	ULONG LowPart;
	LONG High1Time;
	LONG High2Time;
} KSYSTEM_TIME, *PKSYSTEM_TIME;

typedef struct _NLS_USER_INFO
{
	WCHAR iCountry[80];
	WCHAR sCountry[80];
	WCHAR sList[80];
	WCHAR iMeasure[80];
	WCHAR iPaperSize[80];
	WCHAR sDecimal[80];
	WCHAR sThousand[80];
	WCHAR sGrouping[80];
	WCHAR iDigits[80];
	WCHAR iLZero[80];
	WCHAR iNegNumber[80];
	WCHAR sNativeDigits[80];
	WCHAR iDigitSubstitution[80];
	WCHAR sCurrency[80];
	WCHAR sMonDecSep[80];
	WCHAR sMonThouSep[80];
	WCHAR sMonGrouping[80];
	WCHAR iCurrDigits[80];
	WCHAR iCurrency[80];
	WCHAR iNegCurr[80];
	WCHAR sPosSign[80];
	WCHAR sNegSign[80];
	WCHAR sTimeFormat[80];
	WCHAR s1159[80];
	WCHAR s2359[80];
	WCHAR sShortDate[80];
	WCHAR sYearMonth[80];
	WCHAR sLongDate[80];
	WCHAR iCalType[80];
	WCHAR iFirstDay[80];
	WCHAR iFirstWeek[80];
	WCHAR sLocale[80];
	WCHAR sLocaleName[85];
	LCID UserLocaleId;
	LUID InteractiveUserLuid;
	CHAR InteractiveUserSid[SECURITY_MAX_SID_SIZE];
	ULONG ulCacheUpdateCount;
} NLS_USER_INFO, *PNLS_USER_INFO;

typedef struct _BASE_STATIC_SERVER_DATA
{
	UNICODE_STRING WindowsDirectory;
	UNICODE_STRING WindowsSystemDirectory;
	UNICODE_STRING NamedObjectDirectory;
	USHORT WindowsMajorVersion;
	USHORT WindowsMinorVersion;
	USHORT BuildNumber;
	USHORT CSDNumber;
	USHORT RCNumber;
	WCHAR CSDVersion[128];
	SYSTEM_BASIC_INFORMATION SysInfo;
	SYSTEM_TIMEOFDAY_INFORMATION TimeOfDay;
	PVOID IniFileMapping;
	NLS_USER_INFO NlsUserInfo;
	BOOLEAN DefaultSeparateVDM;
	BOOLEAN IsWowTaskReady;
	UNICODE_STRING WindowsSys32x86Directory;
	BOOLEAN fTermsrvAppInstallMode;
	TIME_ZONE_INFORMATION tziTermsrvClientTimeZone;
	KSYSTEM_TIME ktTermsrvClientBias;
	ULONG TermsrvClientTimeZoneId;
	BOOLEAN LUIDDeviceMapsEnabled;
	ULONG TermsrvClientTimeZoneChangeNum;
} BASE_STATIC_SERVER_DATA, *PBASE_STATIC_SERVER_DATA;
PBASE_STATIC_SERVER_DATA BaseStaticServerData;


BOOL
WINAPI
xpCreateProcessW(
	LPCWSTR lpApplicationName,
	LPWSTR lpCommandLine,
	LPSECURITY_ATTRIBUTES lpProcessAttributes,
	LPSECURITY_ATTRIBUTES lpThreadAttributes,
	BOOL bInheritHandles,
	DWORD dwCreationFlags,
	LPVOID lpEnvironment,
	LPCWSTR lpCurrentDirectory,
	LPSTARTUPINFOW lpStartupInfo,
	LPPROCESS_INFORMATION lpProcessInformation
	);

BOOL
WINAPI
NtVdm64CreateProcessHook(
	BOOL fPrefixMappedApplicationName,
	LPCWSTR lpApplicationName,
	LPCWSTR lpCommandLine,
	LPSECURITY_ATTRIBUTES lpProcessAttributes,
	LPSECURITY_ATTRIBUTES lpThreadAttributes,
	BOOL bInheritHandles,
	DWORD dwCreationFlags,
	LPVOID lpEnvironment,
	LPCWSTR lpCurrentDirectory,
	LPSTARTUPINFOW lpStartupInfo,
	LPPROCESS_INFORMATION lpProcessInformation
	);

NTSYSAPI
NTSTATUS
NTAPI
RtlAdjustPrivilege(
	ULONG Privilege,
	BOOLEAN Enable,
	BOOLEAN Client,
	PBOOLEAN WasEnabled
	);

typedef struct _OBJECT_NAME_INFORMATION {
	UNICODE_STRING Name;
} OBJECT_NAME_INFORMATION, *POBJECT_NAME_INFORMATION;

struct FILEHANDLE_NAME_INFO {
	OBJECT_NAME_INFORMATION ObjectNameInfo;
	WCHAR FileName[MAX_PATH];
};

static BOOL ResolveLocalSymbols(void);

// -------------------------------------------------------

NTSTATUS GetFileNameFromFileHandle(HANDLE FileHandle, struct FILEHANDLE_NAME_INFO *s)
{
	HANDLE SectionHandle;
	DWORD ReturnLength;
	NTSTATUS Status;
	PVOID BaseAddress = NULL;
	LARGE_INTEGER SectionOffset = { 0 };
	ULONG ViewSize = 1;

	Status = NtCreateSection(&SectionHandle,
		STANDARD_RIGHTS_REQUIRED | SECTION_QUERY | SECTION_MAP_READ,
		NULL,
		NULL,
		PAGE_READONLY,
		SEC_COMMIT,
		FileHandle);
	if (!NT_SUCCESS(Status))
	{
		TRACE("GetFileNameFromFileHandle, Section error: %08X\n", Status);
		return Status;
	}


	Status = NtMapViewOfSection(SectionHandle, 
		NtCurrentProcess(), 
		&BaseAddress, 
		0, 
		0, 
		&SectionOffset,
		&ViewSize, 
		ViewShare, 
		0, 
		PAGE_READONLY);

	if (!NT_SUCCESS(Status))
	{
		TRACE("GetFileNameFromFileHandle, Map error: %08X\n", Status);
		NtClose(SectionHandle);
		return Status;
	}

	Status = NtQueryVirtualMemory(NtCurrentProcess(), 
		BaseAddress, 
		MemoryMappedFilenameInformation,
		&s->ObjectNameInfo, 
		sizeof(struct FILEHANDLE_NAME_INFO), 
		&ReturnLength);

	NtUnmapViewOfSection(NtCurrentProcess(), BaseAddress);
	NtClose(SectionHandle);

	if (!NT_SUCCESS(Status)) 
	{ 
		TRACE("GetFileNameFromFileHandle, Query mem error: %08X\n", Status); 
	}

	return Status;
}


NTSTATUS
NTAPI
NtCreateSectionHook(
	OUT PHANDLE SectionHandle,
	IN ACCESS_MASK DesiredAccess,
	IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
	IN PLARGE_INTEGER MaximumSize OPTIONAL,
	IN ULONG SectionPageProtection,
	IN ULONG AllocationAttributes,
	IN HANDLE FileHandle OPTIONAL
	)
{
	static BOOL bRecursive = FALSE;
	NTSTATUS Status;
	struct FILEHANDLE_NAME_INFO s;

	Status = NtCreateSectionReal(
		SectionHandle, 
		DesiredAccess,
		ObjectAttributes,
		MaximumSize, 
		SectionPageProtection, 
		AllocationAttributes, 
		FileHandle);

	switch (Status)
	{
		case STATUS_INVALID_IMAGE_NOT_MZ:
		case STATUS_INVALID_IMAGE_NE_FORMAT:
		case STATUS_INVALID_IMAGE_PROTECT:
		case STATUS_INVALID_IMAGE_WIN_16:
			// Resolve missing symbols
			if (!bRecursive)
			{
				bRecursive = TRUE;
#ifdef USE_SYMCACHE
				UpdateSymbolCache();
#endif
				if (!NtVdm64CreateProcessReal)
				{
					if (ResolveLocalSymbols())
					{
						NtVdm64CreateProcessReal = (fpNtVdm64CreateProcess)Hook_Inline(NtVdm64CreateProcess, NtVdm64CreateProcessHook);
					}
				}
				bRecursive = FALSE;
			}

			/* We need to find out, wether this is a DOS-application or not
			 * If no DOS-application (i.e. .cmd, .bat, ..), we don't modify
			 * the return value, as this should be handled by CreateProcess
			 * Unfortunately, we need the filename to check
			 */
			if (Status == STATUS_INVALID_IMAGE_NOT_MZ &&
				BaseIsDosApplication &&
				NT_SUCCESS(GetFileNameFromFileHandle(FileHandle, &s)) &&
				!BaseIsDosApplication(&s.ObjectNameInfo.Name, Status))
			{
				TRACE("This is no DOS application, ldntvdm skipping\n");
				return Status;
			}


			// Trigger NtVdm64CreateProcess
			TRACE("DOS/WOW32 Application redirection.\n");
			LastCreateUserProcessError = Status;
			return STATUS_INVALID_IMAGE_WIN_16;
	}
	return Status;
}


static BOOL ResolveLocalSymbols(void)
{
	BOOL bRet = TRUE;

	if (!BuildSubSysCommandLine)
	{
		// Load the private loader functions by using DbgHelp and Symbol server
		DWORD64 dwBase = 0, dwAddress;
		char szKernel32[MAX_PATH];
		int i;
		HMODULE hKrnl32 = GetModuleHandle(_T("kernel32.dll"));
		struct symmap {
			LPCSTR lpName;
			PULONG_PTR lpDest;
		} syms[] = {
			{ "NtVdm64CreateProcess", (PULONG_PTR)&NtVdm64CreateProcess },
			{ "BaseIsDosApplication", (PULONG_PTR)&BaseIsDosApplication },
			{ "BaseCreateStack", (PULONG_PTR)&BaseCreateStack },
			{ "BaseFormatObjectAttributes", (PULONG_PTR)&BaseFormatObjectAttributes },
			{ "BasepIsProcessAllowed", (PULONG_PTR)&BasepIsProcessAllowed },
			{ "BasePushProcessParameters", (PULONG_PTR)&BasePushProcessParameters },
			{ "BasepIsRealtimeAllowed", (PULONG_PTR)&BasepIsRealtimeAllowed },
			{ "BaseInitializeContext", (PULONG_PTR)&BaseInitializeContext },
			{ "BaseStaticServerData", (PULONG_PTR)&BaseStaticServerData },
			{ "StuffStdHandle", (PULONG_PTR)&StuffStdHandle },
			{ "BasepFreeAppCompatData", (PULONG_PTR)&BasepFreeAppCompatData },
			//{ "BaseComputeProcessExePath", (PULONG_PTR)&BaseComputeProcessExePath },
			{ "BasepReplaceProcessThreadTokens", (PULONG_PTR)&BasepReplaceProcessThreadTokens},
			{ "BasepCheckBadapp", (PULONG_PTR)&BasepCheckBadapp },
			//{ "BasepIsImageVersionOk", (PULONG_PTR)&BasepIsImageVersionOk },
			{ "BasepCheckWinSaferRestrictions", (PULONG_PTR)&BasepCheckWinSaferRestrictions },
#if defined(BUILD_WOW6432)
			{ "CsrBasepCreateProcess", (PULONG_PTR)&CsrBasepCreateProcess },
#endif
			{ "BuildSubSysCommandLine", (PULONG_PTR)&BuildSubSysCommandLine }
		};

		GetSystemDirectoryA(szKernel32, sizeof(szKernel32) / sizeof(szKernel32[0]));
		strcat(szKernel32, "\\kernel32.dll");
		if (SymEng_LoadModule(szKernel32, &dwBase) == 0)
		{
			for (i = 0; i < sizeof(syms) / sizeof(syms[0]); i++)
			{
				if ((dwAddress = SymEng_GetAddr(dwBase, syms[i].lpName)))
					*syms[i].lpDest = (DWORD64)hKrnl32 + dwAddress;
				else 
				{
					bRet = FALSE;
					break;
				}
			}
			SymEng_UnloadModule(dwBase);
		}
		else
		{
			OutputDebugStringA("NTVDM: Symbol engine loading failed");
			return FALSE;
		}
	}
	return bRet;
}

BOOL WINAPI
NtVdm64CreateProcessHook(
	BOOL fPrefixMappedApplicationName,
	LPCWSTR lpApplicationName,
	LPCWSTR lpCommandLine,
	LPSECURITY_ATTRIBUTES lpProcessAttributes,
	LPSECURITY_ATTRIBUTES lpThreadAttributes,
	BOOL bInheritHandles,
	DWORD dwCreationFlags,
	LPVOID lpEnvironment,
	LPCWSTR lpCurrentDirectory,
	LPSTARTUPINFOW lpStartupInfo,
	LPPROCESS_INFORMATION lpProcessInformation
	)
{
	if (LastCreateUserProcessError == STATUS_INVALID_IMAGE_PROTECT ||
		LastCreateUserProcessError == STATUS_INVALID_IMAGE_NOT_MZ ||
		LastCreateUserProcessError == STATUS_INVALID_IMAGE_WIN_16 ||
		LastCreateUserProcessError == STATUS_INVALID_IMAGE_NE_FORMAT)
	{
		// This looks like a job for me...
		WCHAR *pszCommandLine;

		TRACE("calling xpCreateProcessW\n");
		// Restore lpCommandLine
		if (fPrefixMappedApplicationName)
		{
			pszCommandLine = _alloca((wcslen(lpApplicationName) + wcslen(lpCommandLine) + 1) * sizeof(WCHAR));
			wcscpy(pszCommandLine, lpApplicationName);
			wcscat(pszCommandLine, lpCommandLine);
		}
		else pszCommandLine = lpCommandLine;

		return xpCreateProcessW(
			(LPCWSTR)lpApplicationName,
			(LPWSTR)pszCommandLine,
			lpProcessAttributes,
			lpThreadAttributes,
			bInheritHandles,
			dwCreationFlags,
			lpEnvironment,
			lpCurrentDirectory,
			lpStartupInfo,
			lpProcessInformation
			);
	}

	return NtVdm64CreateProcessReal(
		fPrefixMappedApplicationName,
		lpApplicationName,
		lpCommandLine,
		lpProcessAttributes,
		lpThreadAttributes,
		bInheritHandles,
		dwCreationFlags,
		lpEnvironment,
		lpCurrentDirectory,
		lpStartupInfo,
		lpProcessInformation
		);
}

// Windows Server 2003 is missing check for BasepIsFirstVDM..?
typedef BOOL (APIENTRY *fpGetNextVDMCommand)(LPVOID lpVDMInfo);
fpGetNextVDMCommand GetNextVDMCommandReal = NULL;

BOOL
APIENTRY
GetNextVDMCommandHook(LPVOID lpVDMInfo)
{
	NTSTATUS Status;
	BASE_API_MSG m;
	BASE_IS_FIRST_VDM_MSG *d = (BASE_IS_FIRST_VDM_MSG*)&m.u.IsFirstVDM;

	// Special case to query the first VDM In the system.
	if (lpVDMInfo == NULL) {
		Status = CsrClientCallServer(
			(struct _CSR_API_MESSAGE*)&m,
			NULL,
			CSR_MAKE_API_NUMBER(BASESRV_SERVERDLL_INDEX,
				BasepIsFirstVDM
				),
			sizeof(*d)
			);

		if (NT_SUCCESS(Status)) {
			return(d->FirstVDM);
		}
		else {
			BaseSetLastNTError(Status);
			return FALSE;
		}
	}
	return GetNextVDMCommandReal(lpVDMInfo);
}

#ifdef _WIN64
NTSTATUS HookCSRSS()
{
	PSYSTEM_PROCESS_INFORMATION ProcessInfo;
	PUCHAR  pucLargeBuffer = 0;
	ULONG   LargeBufferSize = 32 * 1024;
	ULONG   RegionSize;
	NTSTATUS status = STATUS_INFO_LENGTH_MISMATCH;
	ULONG TotalOffset;

	while (status == STATUS_INFO_LENGTH_MISMATCH)
	{
		status = NtAllocateVirtualMemory(NtCurrentProcess(), &pucLargeBuffer, 0, &LargeBufferSize, MEM_COMMIT, PAGE_READWRITE);
		if (!NT_SUCCESS(status))
		{
			TRACE("Out of memory? %08X\n", status);
			return status;
		}
		RegionSize = 0;
		status = NtQuerySystemInformation(SystemProcessInformation, pucLargeBuffer, LargeBufferSize, &TotalOffset);
		if (NT_SUCCESS(status)) break;
		else if (status == STATUS_INFO_LENGTH_MISMATCH)
		{
			LargeBufferSize += 8192;
			NtFreeVirtualMemory(NtCurrentProcess(), &pucLargeBuffer, &RegionSize, MEM_RELEASE);
			pucLargeBuffer = NULL;
		}
		else {
			if (pucLargeBuffer) NtFreeVirtualMemory(NtCurrentProcess(), &pucLargeBuffer, &RegionSize, MEM_RELEASE);
			TRACE("Process enumaration failed %08X\n", status);
			return status;
		}
	}

	TotalOffset = 0;
	ProcessInfo = (PSYSTEM_PROCESS_INFORMATION)pucLargeBuffer;
	while (TRUE)
	{
		if (((PxpSYSTEM_PROCESS_INFORMATION)ProcessInfo)->ImageName.Length > 8 &&
			__wcsicmp(((PxpSYSTEM_PROCESS_INFORMATION)ProcessInfo)->ImageName.Buffer, L"csrss.exe") == 0)
		{
			HANDLE hProcess;

			TRACE("injecting into PID %d...", ProcessInfo->UniqueProcessId);
			if (hProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION, FALSE, (DWORD)ProcessInfo->UniqueProcessId))
			{
				if (!(injectLdrLoadDLL(hProcess, NULL, LDNTVDM_NAME, METHOD_CREATETHREAD))) 
				{
					TRACE("failed (gle=%d)\n", GetLastError());
					status = STATUS_NO_MORE_ENTRIES;
				} else { TRACE("success\n"); }
				CloseHandle(hProcess);
			}
			else { TRACE("Cannot open Process: %d\n", GetLastError()); }
			
			break;
		}
		if (ProcessInfo->NextEntryOffset == 0)
		{
			status = STATUS_NO_MORE_ENTRIES;
			break;
		}
		TotalOffset += ProcessInfo->NextEntryOffset;
		ProcessInfo = (PSYSTEM_PROCESS_INFORMATION)&pucLargeBuffer[TotalOffset];
	}
	if (pucLargeBuffer) NtFreeVirtualMemory(NtCurrentProcess(), &pucLargeBuffer, &RegionSize, MEM_RELEASE);
	return status;
}
#endif // _WIN64

BOOL
WINAPI
CreateProcessInternalW(IN HANDLE hUserToken,
	IN LPCWSTR lpApplicationName,
	IN LPWSTR lpCommandLine,
	IN LPSECURITY_ATTRIBUTES lpProcessAttributes,
	IN LPSECURITY_ATTRIBUTES lpThreadAttributes,
	IN BOOL bInheritHandles,
	IN DWORD dwCreationFlags,
	IN LPVOID lpEnvironment,
	IN LPCWSTR lpCurrentDirectory,
	IN LPSTARTUPINFOW lpStartupInfo,
	IN LPPROCESS_INFORMATION lpProcessInformation,
	OUT PHANDLE hNewToken)
{
	//
	// Core variables used for creating the initial process and thread
	//
	SECURITY_ATTRIBUTES LocalThreadAttributes, LocalProcessAttributes;
	OBJECT_ATTRIBUTES LocalObjectAttributes;
	POBJECT_ATTRIBUTES ObjectAttributes;
	SECTION_IMAGE_INFORMATION ImageInformation;
	IO_STATUS_BLOCK IoStatusBlock;
	CLIENT_ID ClientId;
	ULONG NoWindow, RegionSize, StackSize, ErrorCode, Flags;
	USHORT ImageMachine;
	ULONG ParameterFlags, PrivilegeValue, HardErrorMode, ErrorResponse;
	ULONG_PTR ErrorParameters[2];
	BOOLEAN InJob, SaferNeeded, UseLargePages, HavePrivilege;
	BOOLEAN QuerySection, SkipSaferAndAppCompat;
	CONTEXT Context;
	BASE_API_MSG CsrMsg[2];
	PBASE_CREATEPROCESS_MSG CreateProcessMsg;
	PVOID BaseAddress, PrivilegeState, RealTimePrivilegeState;
	HANDLE DebugHandle, TokenHandle, JobHandle, KeyHandle, ThreadHandle;
	HANDLE FileHandle, SectionHandle, ProcessHandle;
	ULONG ResumeCount;
	PROCESS_PRIORITY_CLASS PriorityClass;
	NTSTATUS Status, Status1, ImageDbgStatus;
	PPEB Peb, RemotePeb;
	PTEB Teb;
	INITIAL_TEB InitialTeb;
	PVOID TibValue;
	PIMAGE_NT_HEADERS NtHeaders;
	STARTUPINFOW StartupInfo;
	xpPRTL_USER_PROCESS_PARAMETERS ProcessParameters;
	UNICODE_STRING DebuggerString;
	BOOL Result;
	//
	// Variables used for command-line and argument parsing
	//
	PCHAR pcScan;
	SIZE_T n;
	WCHAR SaveChar;
	ULONG Length, CurdirLength, CmdQuoteLength;
	ULONG CmdLineLength, ResultSize;
	PWCHAR QuotedCmdLine, AnsiCmdCommand, ExtBuffer, CurrentDirectory;
	PWCHAR NullBuffer, ScanString, NameBuffer, lpProcessExePath, DebuggerCmdLine;
	ANSI_STRING AnsiEnv;
	UNICODE_STRING UnicodeEnv, PathName;
	BOOLEAN SearchRetry, QuotesNeeded, CmdLineIsAppName, HasQuotes;

	//
	// Variables used for Fusion/SxS (Side-by-Side Assemblies)
	//
	RTL_PATH_TYPE SxsPathType, PathType;
#if _SXS_SUPPORT_ENABLED_
	PRTL_BUFFER ByteBuffer;
	PRTL_UNICODE_STRING_BUFFER ThisBuffer, Buffer, SxsStaticBuffers[5];
	PRTL_UNICODE_STRING_BUFFER* BufferHead, SxsStringBuffer;
	RTL_UNICODE_STRING_BUFFER SxsWin32ManifestPath, SxsNtManifestPath;
	RTL_UNICODE_STRING_BUFFER SxsWin32PolicyPath, SxsNtPolicyPath;
	RTL_UNICODE_STRING_BUFFER SxsWin32AssemblyDirectory;
	BASE_MSG_SXS_HANDLES MappedHandles, Handles, FileHandles;
	PVOID CapturedStrings[3];
	SXS_WIN32_NT_PATH_PAIR ExePathPair, ManifestPathPair, PolicyPathPair;
	SXS_OVERRIDE_MANIFEST OverrideMannifest;
	UNICODE_STRING FreeString, SxsNtExePath;
	PWCHAR SxsConglomeratedBuffer, StaticBuffer;
	ULONG ConglomeratedBufferSizeBytes, StaticBufferSize, i;
#endif
	PCSR_CAPTURE_HEADER CaptureBuffer = NULL;
	ULONG FusionFlags;

	//
	// Variables used for path conversion (and partially Fusion/SxS)
	//
	PWCHAR FilePart, PathBuffer, FreeBuffer;
	BOOLEAN TranslationStatus;
	RTL_RELATIVE_NAME_U SxsWin32RelativePath;
	UNICODE_STRING PathBufferString, SxsWin32ExePath;

	//
	// Variables used by Application Compatibility (and partially Fusion/SxS)
	//
	PVOID AppCompatSxsData, AppCompatData;
	ULONG AppCompatSxsDataSize, AppCompatDataSize;
	//
	// Variables used by VDM (Virtual Dos Machine) and WOW32 (16-bit Support)
	//
	ULONG BinarySubType, VdmBinaryType, VdmTask, VdmReserve;
	ULONG VDMCreationState;
	BOOLEAN UseVdmReserve;
	HANDLE VdmWaitObject;
	ANSI_STRING VdmAnsiEnv;
	UNICODE_STRING VdmString, VdmUnicodeEnv;
	BOOLEAN IsWowApp;
	PBASE_CHECKVDM_MSG CheckVdmMsg;

	/* Zero out the initial core variables and handles */
	QuerySection = FALSE;
	InJob = FALSE;
	SkipSaferAndAppCompat = TRUE; // HACK for making .bat/.cmd launch working again.
	ParameterFlags = 0;
	Flags = 0;
	DebugHandle = NULL;
	JobHandle = NULL;
	TokenHandle = NULL;
	FileHandle = NULL;
	SectionHandle = NULL;
	ProcessHandle = NULL;
	ThreadHandle = NULL;
	BaseAddress = (PVOID)1;

	/* Zero out initial SxS and Application Compatibility state */
	AppCompatData = NULL;
	AppCompatDataSize = 0;
	AppCompatSxsData = NULL;
	AppCompatSxsDataSize = 0;
#if _SXS_SUPPORT_ENABLED_
	SxsConglomeratedBuffer = NULL;
#endif
	FusionFlags = 0;

	/* Zero out initial parsing variables -- others are initialized later */
	DebuggerCmdLine = NULL;
	PathBuffer = NULL;
	lpProcessExePath = NULL;
	NullBuffer = 0;
	FreeBuffer = NULL;
	NameBuffer = NULL;
	CurrentDirectory = NULL;
	FilePart = NULL;
	DebuggerString.Buffer = NULL;
	HasQuotes = FALSE;
	QuotedCmdLine = NULL;

	/* Zero out initial VDM state */
	VdmAnsiEnv.Buffer = NULL;
	VdmUnicodeEnv.Buffer = NULL;
	VdmString.Buffer = NULL;
	VdmTask = 0;
	VDMCreationState = 0;
	VdmBinaryType = 0;
	VdmReserve = 0;
	VdmWaitObject = NULL;
	UseVdmReserve = FALSE;
	IsWowApp = FALSE;

	/* Set message structures */
	CreateProcessMsg = (PBASE_CREATEPROCESS_MSG)&CsrMsg[0].u.CreateProcess;
	CheckVdmMsg = (PBASE_CHECKVDM_MSG)&CsrMsg[1].u.CheckVDM;

	/* Clear the more complex structures by zeroing out their entire memory */
	RtlZeroMemory(&Context, sizeof(Context));
#if _SXS_SUPPORT_ENABLED_
	RtlZeroMemory(&FileHandles, sizeof(FileHandles));
	RtlZeroMemory(&MappedHandles, sizeof(MappedHandles));
	RtlZeroMemory(&Handles, sizeof(Handles));
#endif
	RtlZeroMemory(&CreateProcessMsg->Sxs, sizeof(CreateProcessMsg->Sxs));
	RtlZeroMemory(&LocalProcessAttributes, sizeof(LocalProcessAttributes));
	RtlZeroMemory(&LocalThreadAttributes, sizeof(LocalThreadAttributes));

	/* Zero out output arguments as well */
	RtlZeroMemory(lpProcessInformation, sizeof(*lpProcessInformation));
	if (hNewToken) *hNewToken = NULL;

	/* Capture the special window flag */
	NoWindow = dwCreationFlags & CREATE_NO_WINDOW;
	dwCreationFlags &= ~CREATE_NO_WINDOW;

#if _SXS_SUPPORT_ENABLED_
	/* Setup the SxS static string arrays and buffers */
	SxsStaticBuffers[0] = &SxsWin32ManifestPath;
	SxsStaticBuffers[1] = &SxsWin32PolicyPath;
	SxsStaticBuffers[2] = &SxsWin32AssemblyDirectory;
	SxsStaticBuffers[3] = &SxsNtManifestPath;
	SxsStaticBuffers[4] = &SxsNtPolicyPath;
	ExePathPair.Win32 = &SxsWin32ExePath;
	ExePathPair.Nt = &SxsNtExePath;
	ManifestPathPair.Win32 = &SxsWin32ManifestPath.String;
	ManifestPathPair.Nt = &SxsNtManifestPath.String;
	PolicyPathPair.Win32 = &SxsWin32PolicyPath.String;
	PolicyPathPair.Nt = &SxsNtPolicyPath.String;
#endif

	DPRINT("CreateProcessInternalW: '%S' '%S' %lx\n", lpApplicationName, lpCommandLine, dwCreationFlags);

	/* Finally, set our TEB and PEB */
	Teb = NtCurrentTeb();
	Peb = NtCurrentPeb();

	/* This combination is illegal (see MSDN) */
	if ((dwCreationFlags & (DETACHED_PROCESS | CREATE_NEW_CONSOLE)) ==
		(DETACHED_PROCESS | CREATE_NEW_CONSOLE))
	{
		DPRINT1("Invalid flag combo used\n");
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	/* Convert the priority class */
	if (dwCreationFlags & IDLE_PRIORITY_CLASS)
	{
		PriorityClass.PriorityClass = PROCESS_PRIORITY_CLASS_IDLE;
	}
	else if (dwCreationFlags & BELOW_NORMAL_PRIORITY_CLASS)
	{
		PriorityClass.PriorityClass = PROCESS_PRIORITY_CLASS_BELOW_NORMAL;
	}
	else if (dwCreationFlags & NORMAL_PRIORITY_CLASS)
	{
		PriorityClass.PriorityClass = PROCESS_PRIORITY_CLASS_NORMAL;
	}
	else if (dwCreationFlags & ABOVE_NORMAL_PRIORITY_CLASS)
	{
		PriorityClass.PriorityClass = PROCESS_PRIORITY_CLASS_ABOVE_NORMAL;
	}
	else if (dwCreationFlags & HIGH_PRIORITY_CLASS)
	{
		PriorityClass.PriorityClass = PROCESS_PRIORITY_CLASS_HIGH;
	}
	else if (dwCreationFlags & REALTIME_PRIORITY_CLASS)
	{
		PriorityClass.PriorityClass = PROCESS_PRIORITY_CLASS_HIGH;
		PriorityClass.PriorityClass += (BasepIsRealtimeAllowed(FALSE) != NULL);
	}
	else
	{
		PriorityClass.PriorityClass = PROCESS_PRIORITY_CLASS_UNKNOWN;
	}

	/* Done with the priority masks, so get rid of them */
	PriorityClass.Foreground = FALSE;
	dwCreationFlags &= ~(NORMAL_PRIORITY_CLASS |
		IDLE_PRIORITY_CLASS |
		HIGH_PRIORITY_CLASS |
		REALTIME_PRIORITY_CLASS |
		BELOW_NORMAL_PRIORITY_CLASS |
		ABOVE_NORMAL_PRIORITY_CLASS);

	/* You cannot request both a shared and a separate WoW VDM */
	if ((dwCreationFlags & CREATE_SEPARATE_WOW_VDM) &&
		(dwCreationFlags & CREATE_SHARED_WOW_VDM))
	{
		/* Fail such nonsensical attempts */
		DPRINT1("Invalid WOW flags\n");
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}
	else if (!(dwCreationFlags & CREATE_SHARED_WOW_VDM) &&
		(BaseStaticServerData->DefaultSeparateVDM))
	{
		/* A shared WoW VDM was not requested but system enforces separation */
		dwCreationFlags |= CREATE_SEPARATE_WOW_VDM;
	}

	/* If a shared WoW VDM is used, make sure the process isn't in a job */
	if (!(dwCreationFlags & CREATE_SEPARATE_WOW_VDM) &&
		(NtIsProcessInJob(NtCurrentProcess(), NULL)))
	{
		/* Remove the shared flag and add the separate flag */
		dwCreationFlags = (dwCreationFlags &~CREATE_SHARED_WOW_VDM) |
			CREATE_SEPARATE_WOW_VDM;
	}

	/* Convert the environment */
	if ((lpEnvironment) && !(dwCreationFlags & CREATE_UNICODE_ENVIRONMENT))
	{
		/* Scan the environment to calculate its Unicode size */
		AnsiEnv.Buffer = pcScan = (PCHAR)lpEnvironment;
		while ((*pcScan) || (*(pcScan + 1))) ++pcScan;

		/* Create our ANSI String */
		AnsiEnv.Length = pcScan - (PCHAR)lpEnvironment + sizeof(ANSI_NULL);
		AnsiEnv.MaximumLength = AnsiEnv.Length + sizeof(ANSI_NULL);

		/* Allocate memory for the Unicode Environment */
		UnicodeEnv.Buffer = NULL;
		RegionSize = AnsiEnv.MaximumLength * sizeof(WCHAR);
		Status = NtAllocateVirtualMemory(NtCurrentProcess(),
			(PVOID)&UnicodeEnv.Buffer,
			0,
			&RegionSize,
			MEM_COMMIT,
			PAGE_READWRITE);
		if (!NT_SUCCESS(Status))
		{
			/* Fail */
			BaseSetLastNTError(Status);
			return FALSE;
		}

		/* Use the allocated size and convert */
		UnicodeEnv.MaximumLength = (USHORT)RegionSize;
		Status = RtlAnsiStringToUnicodeString(&UnicodeEnv, &AnsiEnv, FALSE);
		if (!NT_SUCCESS(Status))
		{
			/* Fail */
			NtFreeVirtualMemory(NtCurrentProcess(),
				(PVOID)&UnicodeEnv.Buffer,
				&RegionSize,
				MEM_RELEASE);
			BaseSetLastNTError(Status);
			return FALSE;
		}

		/* Now set the Unicode environment as the environment string pointer */
		lpEnvironment = UnicodeEnv.Buffer;
	}

	/* Make a copy of the caller's startup info since we'll modify it */
	StartupInfo = *lpStartupInfo;

	/* Check if private data is being sent on the same channel as std handles */
	if ((StartupInfo.dwFlags & STARTF_USESTDHANDLES) &&
		(StartupInfo.dwFlags & (STARTF_USEHOTKEY | STARTF_HASSHELLDATA)))
	{
		/* Cannot use the std handles since we have monitor/hotkey values */
		StartupInfo.dwFlags &= ~STARTF_USESTDHANDLES;
	}

	/* If there's a debugger, or we have to launch cmd.exe, we go back here */
AppNameRetry:
	/* New iteration -- free any existing name buffer */
	if (NameBuffer)
	{
		RtlFreeHeap(RtlGetProcessHeap(), 0, NameBuffer);
		NameBuffer = NULL;
	}

	/* New iteration -- free any existing free buffer */
	if (FreeBuffer)
	{
		RtlFreeHeap(RtlGetProcessHeap(), 0, FreeBuffer);
		FreeBuffer = NULL;
	}

	/* New iteration -- close any existing file handle */
	if (FileHandle)
	{
		NtClose(FileHandle);
		FileHandle = NULL;
	}

	/* Set the initial parsing state. This code can loop -- don't move this! */
	ErrorCode = 0;
	SearchRetry = TRUE;
	QuotesNeeded = FALSE;
	CmdLineIsAppName = FALSE;

	/* First check if we don't have an application name */
	if (!lpApplicationName)
	{
		/* This should be the first time we attempt creating one */
		ASSERT(NameBuffer == NULL);

		/* Allocate a buffer to hold it */
		NameBuffer = RtlAllocateHeap(RtlGetProcessHeap(),
			0,
			MAX_PATH * sizeof(WCHAR));
		if (!NameBuffer)
		{
			SetLastError(ERROR_NOT_ENOUGH_MEMORY);
			Result = FALSE;
			goto Quickie;
		}

		/* Initialize the application name and our parsing parameters */
		lpApplicationName = NullBuffer = ScanString = lpCommandLine;

		/* Check for an initial quote*/
		if (*lpCommandLine == L'\"')
		{
			/* We found a quote, keep searching for another one */
			SearchRetry = FALSE;
			ScanString++;
			lpApplicationName = ScanString;
			while (*ScanString)
			{
				/* Have we found the terminating quote? */
				if (*ScanString == L'\"')
				{
					/* We're done, get out of here */
					NullBuffer = ScanString;
					HasQuotes = TRUE;
					break;
				}

				/* Keep searching for the quote */
				ScanString++;
				NullBuffer = ScanString;
			}
		}
		else
		{
		StartScan:
			/* We simply make the application name be the command line*/
			lpApplicationName = lpCommandLine;
			while (*ScanString)
			{
				/* Check if it starts with a space or tab */
				if ((*ScanString == L' ') || (*ScanString == L'\t'))
				{
					/* Break out of the search loop */
					NullBuffer = ScanString;
					break;
				}

				/* Keep searching for a space or tab */
				ScanString++;
				NullBuffer = ScanString;
			}
		}

		/* We have found the end of the application name, terminate it */
		SaveChar = *NullBuffer;
		*NullBuffer = UNICODE_NULL;

#if 0 // Not present in kerne32.dll 64bit?
		/* New iteration -- free any existing saved path */
		if (lpProcessExePath)
		{
			RtlFreeHeap(RtlGetProcessHeap(), 0, lpProcessExePath);
			lpProcessExePath = NULL;
		}

		/* Now compute the final EXE path based on the name */
		lpProcessExePath = BaseComputeProcessExePath((LPWSTR)lpApplicationName);
		DPRINT("Search Path: %S\n", lpProcessExePath);
		if (!lpProcessExePath)
		{
			SetLastError(ERROR_NOT_ENOUGH_MEMORY);
			Result = FALSE;
			goto Quickie;
		}
#endif

		/* And search for the executable in the search path */
		Length = SearchPathW(lpProcessExePath,
			lpApplicationName,
			L".exe",
			MAX_PATH,
			NameBuffer,
			NULL);

		/* Did we find it? */
		if ((Length) && (Length < MAX_PATH))
		{
			/* Get file attributes */
			CurdirLength = GetFileAttributesW(NameBuffer);
			if ((CurdirLength != 0xFFFFFFFF) &&
				(CurdirLength & FILE_ATTRIBUTE_DIRECTORY))
			{
				/* This was a directory, fail later on */
				Length = 0;
			}
			else
			{
				/* It's a file! */
				Length++;
			}
		}

		DPRINT("Length: %lu Buffer: %S\n", Length, NameBuffer);

		/* Check if there was a failure in SearchPathW */
		if ((Length) && (Length < MAX_PATH))
		{
			/* Everything looks good, restore the name */
			*NullBuffer = SaveChar;
			lpApplicationName = NameBuffer;
		}
		else
		{
			/* Check if this was a relative path, which would explain it */
			PathType = RtlDetermineDosPathNameType_U(lpApplicationName);
			if (PathType != RtlPathTypeRelative)
			{
				/* This should fail, and give us a detailed LastError */
				FileHandle = CreateFileW(lpApplicationName,
					GENERIC_READ,
					FILE_SHARE_READ |
					FILE_SHARE_WRITE,
					NULL,
					OPEN_EXISTING,
					FILE_ATTRIBUTE_NORMAL,
					NULL);
				if (FileHandle != INVALID_HANDLE_VALUE)
				{
					/* It worked? Return a generic error */
					CloseHandle(FileHandle);
					FileHandle = NULL;
					BaseSetLastNTError(STATUS_OBJECT_NAME_NOT_FOUND);
				}
			}
			else
			{
				/* Path was absolute, which means it doesn't exist */
				BaseSetLastNTError(STATUS_OBJECT_NAME_NOT_FOUND);
			}

			/* Did we already fail once? */
			if (ErrorCode)
			{
				/* Set the error code */
				SetLastError(ErrorCode);
			}
			else
			{
				/* Not yet, cache it */
				ErrorCode = GetLastError();
			}

			/* Put back the command line */
			*NullBuffer = SaveChar;
			lpApplicationName = NameBuffer;

			/* It's possible there's whitespace in the directory name */
			if (!(*ScanString) || !(SearchRetry))
			{
				/* Not the case, give up completely */
				Result = FALSE;
				goto Quickie;
			}

			/* There are spaces, so keep trying the next possibility */
			ScanString++;
			NullBuffer = ScanString;

			/* We will have to add a quote, since there is a space */
			QuotesNeeded = TRUE;
			HasQuotes = TRUE;
			goto StartScan;
		}
	}
	else if (!(lpCommandLine) || !(*lpCommandLine))
	{
		/* We don't have a command line, so just use the application name */
		CmdLineIsAppName = TRUE;
		lpCommandLine = (LPWSTR)lpApplicationName;
	}

	/* Convert the application name to its NT path */
	TranslationStatus = RtlDosPathNameToRelativeNtPathName_U(lpApplicationName,
		&PathName,
		NULL,
		&SxsWin32RelativePath);
	if (!TranslationStatus)
	{
		/* Path must be invaild somehow, bail out */
		DPRINT1("Path translation for SxS failed\n");
		SetLastError(ERROR_PATH_NOT_FOUND);
		Result = FALSE;
		goto Quickie;
	}

	/* Setup the buffer that needs to be freed at the end */
	ASSERT(FreeBuffer == NULL);
	FreeBuffer = PathName.Buffer;

	/* Check what kind of path the application is, for SxS (Fusion) purposes */
	RtlInitUnicodeString(&SxsWin32ExePath, lpApplicationName);
	SxsPathType = RtlDetermineDosPathNameType_U(lpApplicationName);
	if ((SxsPathType != RtlPathTypeDriveAbsolute) &&
		(SxsPathType != RtlPathTypeLocalDevice) &&
		(SxsPathType != RtlPathTypeRootLocalDevice) &&
		(SxsPathType != RtlPathTypeUncAbsolute))
	{
		/* Relative-type path, get the full path */
		RtlInitEmptyUnicodeString(&PathBufferString, NULL, 0);
		Status = RtlGetFullPathName_UstrEx(&SxsWin32ExePath,
			NULL,
			&PathBufferString,
			NULL,
			NULL,
			NULL,
			&SxsPathType,
			NULL);
		if (!NT_SUCCESS(Status))
		{
			/* Fail the rest of the create */
			RtlReleaseRelativeName(&SxsWin32RelativePath);
			BaseSetLastNTError(Status);
			Result = FALSE;
			goto Quickie;
		}

		/* Use this full path as the SxS path */
		SxsWin32ExePath = PathBufferString;
		PathBuffer = PathBufferString.Buffer;
		PathBufferString.Buffer = NULL;
		DPRINT("SxS Path: %S\n", PathBuffer);
	}

	/* Also set the .EXE path based on the path name */
#if _SXS_SUPPORT_ENABLED_
	SxsNtExePath = PathName;
#endif
	if (SxsWin32RelativePath.RelativeName.Length)
	{
		/* If it's relative, capture the relative name */
		PathName = SxsWin32RelativePath.RelativeName;
	}
	else
	{
		/* Otherwise, it's absolute, make sure no relative dir is used */
		SxsWin32RelativePath.ContainingDirectory = NULL;
	}

	/* Now use the path name, and the root path, to try opening the app */
	DPRINT("Path: %wZ. Dir: %p\n", &PathName, SxsWin32RelativePath.ContainingDirectory);
	InitializeObjectAttributes(&LocalObjectAttributes,
		&PathName,
		OBJ_CASE_INSENSITIVE,
		SxsWin32RelativePath.ContainingDirectory,
		NULL);
	Status = NtOpenFile(&FileHandle,
		SYNCHRONIZE |
		FILE_READ_ATTRIBUTES |
		FILE_READ_DATA |
		FILE_EXECUTE,
		&LocalObjectAttributes,
		&IoStatusBlock,
		FILE_SHARE_READ | FILE_SHARE_DELETE,
		FILE_SYNCHRONOUS_IO_NONALERT |
		FILE_NON_DIRECTORY_FILE);
	if (!NT_SUCCESS(Status))
	{
		/* Try to open the app just for execute purposes instead */
		Status = NtOpenFile(&FileHandle,
			SYNCHRONIZE | FILE_EXECUTE,
			&LocalObjectAttributes,
			&IoStatusBlock,
			FILE_SHARE_READ | FILE_SHARE_DELETE,
			FILE_SYNCHRONOUS_IO_NONALERT |
			FILE_NON_DIRECTORY_FILE);
	}

	/* Cleanup in preparation for failure or success */
	RtlReleaseRelativeName(&SxsWin32RelativePath);
	if (!NT_SUCCESS(Status))
	{
		/* Failure path, try to understand why */
		DPRINT1("Open file failed: %lx\n", Status);
		if (RtlIsDosDeviceName_U((PWSTR)lpApplicationName))
		{
			/* If a device is being executed, return this special error code */
			SetLastError(ERROR_BAD_DEVICE);
			Result = FALSE;
			goto Quickie;
		}
		else
		{
			/* Otherwise return the converted NT error code */
			BaseSetLastNTError(Status);
			Result = FALSE;
			goto Quickie;
		}
	}

	/* Did the caller specify a desktop? */
	if (!StartupInfo.lpDesktop)
	{
		/* Use the one from the current process */
		StartupInfo.lpDesktop = (LPWSTR)((xpPRTL_USER_PROCESS_PARAMETERS)NtCurrentPeb()->
			ProcessParameters)->DesktopInfo.Buffer;
	}

	/* Create a section for this file */
	Status = NtCreateSection(&SectionHandle,
		SECTION_ALL_ACCESS,
		NULL,
		NULL,
		PAGE_EXECUTE,
		SEC_IMAGE,
		FileHandle);
	DPRINT("Section status: %lx\n", Status);
	if (NT_SUCCESS(Status))
	{
#if 0 /* No SharedUserData */
		/* Are we running on Windows Embedded, Datacenter, Blade or Starter? */
		if (SharedUserData->SuiteMask & (VER_SUITE_EMBEDDEDNT |
			VER_SUITE_DATACENTER |
			VER_SUITE_PERSONAL |
			VER_SUITE_BLADE))
		{
			/* These SKUs do not allow running certain applications */
			Status = BasepCheckWebBladeHashes(FileHandle);
			if (Status == STATUS_ACCESS_DENIED)
			{
				/* And this is one of them! */
				DPRINT1("Invalid Blade hashes!\n");
				SetLastError(ERROR_ACCESS_DISABLED_WEBBLADE);
				Result = FALSE;
				goto Quickie;
			}

			/* Did we get some other failure? */
			if (!NT_SUCCESS(Status))
			{
				/* If we couldn't check the hashes, assume nefariousness */
				DPRINT1("Tampered Blade hashes!\n");
				SetLastError(ERROR_ACCESS_DISABLED_WEBBLADE_TAMPER);
				Result = FALSE;
				goto Quickie;
			}
		}
#endif

		/* Now do Winsafer, etc, checks */
		Status = BasepIsProcessAllowed((LPWSTR)lpApplicationName);
		if (!NT_SUCCESS(Status))
		{
			/* Fail if we're not allowed to launch the process */
			DPRINT1("Process not allowed to launch: %lx\n", Status);
			BaseSetLastNTError(Status);
			if (SectionHandle)
			{
				NtClose(SectionHandle);
				SectionHandle = NULL;
			}
			Result = FALSE;
			goto Quickie;
		}

		/* Is a DOS VDM being forced, but we already have a WOW32 instance ready? */
		if ((dwCreationFlags & CREATE_FORCEDOS) &&
			(BaseStaticServerData->IsWowTaskReady))
		{
			/* This request can't be satisfied, instead, a separate VDM is needed */
			dwCreationFlags &= ~(CREATE_FORCEDOS | CREATE_SHARED_WOW_VDM);
			dwCreationFlags |= CREATE_SEPARATE_WOW_VDM;

			/* Set a failure code, ask for VDM reservation */
			Status = STATUS_INVALID_IMAGE_WIN_16;
			UseVdmReserve = TRUE;

			/* Close the current handle */
			NtClose(SectionHandle);
			SectionHandle = NULL;

			/* Don't query the section later */
			QuerySection = FALSE;
		}
	}

	/* Did we already do these checks? */
	if (!SkipSaferAndAppCompat)
	{
		/* Is everything OK so far, OR do we have an non-MZ, non-DOS app? */
		if ((NT_SUCCESS(Status)) ||
			((Status == STATUS_INVALID_IMAGE_NOT_MZ) &&
				!(BaseIsDosApplication(&PathName, Status))))
		{
			/* Clear the machine type in case of failure */
			ImageMachine = 0;

			/* Clean any app compat data that may have accumulated */
			BasepFreeAppCompatData(AppCompatData, AppCompatSxsData);
			AppCompatData = NULL;
			AppCompatSxsData = NULL;

			/* Do we have a section? */
			if (SectionHandle)
			{
				/* Have we already queried it? */
				if (QuerySection)
				{
					/* Nothing to do */
					Status = STATUS_SUCCESS;
				}
				else
				{
					/* Get some information about the executable */
					Status = NtQuerySection(SectionHandle,
						SectionImageInformation,
						&ImageInformation,
						sizeof(ImageInformation),
						NULL);
				}

				/* Do we have section information now? */
				if (NT_SUCCESS(Status))
				{
					/* Don't ask for it again, save the machine type */
					QuerySection = TRUE;
					ImageMachine = ImageInformation.Machine;
				}
			}

			/* Is there a reason/Shim we shouldn't run this application? */
			Status = BasepCheckBadapp(FileHandle,
				FreeBuffer,
				lpEnvironment,
				ImageMachine,
				&AppCompatData,
				&AppCompatDataSize,
				&AppCompatSxsData,
				&AppCompatSxsDataSize,
				&FusionFlags);
			if (!NT_SUCCESS(Status))
			{
				/* This is usually the status we get back */
				DPRINT1("App compat launch failure: %lx\n", Status);
				if (Status == STATUS_ACCESS_DENIED)
				{
					/* Convert it to something more Win32-specific */
					SetLastError(ERROR_CANCELLED);
				}
				else
				{
					/* Some other error */
					BaseSetLastNTError(Status);
				}

				/* Did we have a section? */
				if (SectionHandle)
				{
					/* Clean it up */
					NtClose(SectionHandle);
					SectionHandle = NULL;
				}

				/* Fail the call */
				Result = FALSE;
				goto Quickie;
			}
		}
	}

	//ASSERT((dwFusionFlags & ~SXS_APPCOMPACT_FLAG_APP_RUNNING_SAFEMODE) == 0);

	/* Have we already done, and do we need to do, SRP (WinSafer) checks? */
	if (!(SkipSaferAndAppCompat) &&
		~(dwCreationFlags & CREATE_PRESERVE_CODE_AUTHZ_LEVEL))
	{
		/* Assume yes */
		SaferNeeded = TRUE;
		switch (Status)
		{
		case STATUS_INVALID_IMAGE_NE_FORMAT:
		case STATUS_INVALID_IMAGE_PROTECT:
		case STATUS_INVALID_IMAGE_WIN_16:
		case STATUS_FILE_IS_OFFLINE:
			/* For all DOS, 16-bit, OS/2 images, we do*/
			break;

		case STATUS_INVALID_IMAGE_NOT_MZ:
			/* For invalid files, we don't, unless it's a .BAT file */
			if (BaseIsDosApplication(&PathName, Status)) break;

		default:
			/* Any other error codes we also don't */
			if (!NT_SUCCESS(Status))
			{
				SaferNeeded = FALSE;
			}

			/* But for success, we do */
			break;
		}

		/* Okay, so what did the checks above result in? */
		if (SaferNeeded)
		{
			/* We have to call into the WinSafer library and actually check */
			Status = BasepCheckWinSaferRestrictions(hUserToken,
				(LPWSTR)lpApplicationName,
				FileHandle,
				&InJob,
				&TokenHandle,
				&JobHandle);
			if (Status == 0xFFFFFFFF)
			{
				/* Back in 2003, they didn't have an NTSTATUS for this... */
				DPRINT1("WinSafer blocking process launch\n");
				SetLastError(ERROR_ACCESS_DISABLED_BY_POLICY);
				Result = FALSE;
				goto Quickie;
			}

			/* Other status codes are not-Safer related, just convert them */
			if (!NT_SUCCESS(Status))
			{
				DPRINT1("Error checking WinSafer: %lx\n", Status);
				BaseSetLastNTError(Status);
				Result = FALSE;
				goto Quickie;
			}
		}
	}

	/* The last step is to figure out why the section object was not created */
	switch (Status)
	{
	case STATUS_INVALID_IMAGE_WIN_16:
	{
		/* 16-bit binary. Should we use WOW or does the caller force VDM? */
		if (!(dwCreationFlags & CREATE_FORCEDOS))
		{
			/* Remember that we're launching WOW */
			IsWowApp = TRUE;

			/* Create the VDM environment, it's valid for WOW too */
			Result = BaseCreateVDMEnvironment(lpEnvironment,
				&VdmAnsiEnv,
				&VdmUnicodeEnv);
			if (!Result)
			{
				DPRINT1("VDM environment for WOW app failed\n");
				goto Quickie;
			}

			/* We're going to try this twice, so do a loop */
			while (TRUE)
			{
				/* Pick which kind of WOW mode we want to run in */
				VdmBinaryType = (dwCreationFlags &
					CREATE_SEPARATE_WOW_VDM) ?
					BINARY_TYPE_SEPWOW : BINARY_TYPE_WIN16;

				/* Get all the VDM settings and current status */
				Status = BaseCheckVDM(VdmBinaryType,
					lpApplicationName,
					lpCommandLine,
					lpCurrentDirectory,
					&VdmAnsiEnv,
					&CsrMsg[1],
					&VdmTask,
					dwCreationFlags,
					&StartupInfo,
					hUserToken);

				/* If it worked, no need to try again */
				if (NT_SUCCESS(Status)) break;

				/* Check if it's disallowed or if it's our second time */
				BaseSetLastNTError(Status);
				if ((Status == STATUS_VDM_DISALLOWED) ||
					(VdmBinaryType == BINARY_TYPE_SEPWOW) ||
					(GetLastError() == ERROR_ACCESS_DENIED))
				{
					/* Fail the call -- we won't try again */
					DPRINT1("VDM message failure for WOW: %lx\n", Status);
					Result = FALSE;
					goto Quickie;
				}

				/* Try one more time, but with a separate WOW instance */
				dwCreationFlags |= CREATE_SEPARATE_WOW_VDM;
			}

			/* Check which VDM state we're currently in */
			switch (CheckVdmMsg->VDMState)
			{
			case VDM_NOT_PRESENT:
				/* VDM is not fully loaded, so not that much to undo */
				VDMCreationState = VDM_PARTIALLY_CREATED;

				/* Reset VDM reserve if needed */
				if (UseVdmReserve) VdmReserve = 1;

				/* Get the required parameters and names for launch */
				Result = BaseGetVdmConfigInfo(lpCommandLine,
					VdmTask,
					VdmBinaryType,
					&VdmString,
					&VdmReserve);
				if (!Result)
				{
					DPRINT1("VDM Configuration failed for WOW\n");
					BaseSetLastNTError(Status);
					goto Quickie;
				}

				/* Update the command-line with the VDM one instead */
				lpCommandLine = VdmString.Buffer;
				lpApplicationName = NULL;

				/* We don't want a console, detachment, nor a window */
				dwCreationFlags |= CREATE_NO_WINDOW;
				dwCreationFlags &= ~(CREATE_NEW_CONSOLE | DETACHED_PROCESS);

				/* Force feedback on */
				StartupInfo.dwFlags |= STARTF_FORCEONFEEDBACK;
				break;


			case VDM_PRESENT_AND_READY:
				/* VDM is ready, so we have to undo everything */
				VDMCreationState = VDM_BEING_REUSED;

				/* Check if CSRSS wants us to wait on VDM */
				VdmWaitObject = CheckVdmMsg->WaitObjectForParent;
				break;

			case VDM_PRESENT_NOT_READY:
				/* Something is wrong with VDM, we'll fail the call */
				DPRINT1("VDM is not ready for WOW\n");
				SetLastError(ERROR_NOT_READY);
				Result = FALSE;
				goto Quickie;

			default:
				break;
			}

			/* Since to get NULL, we allocate from 0x1, account for this */
			VdmReserve--;

			/* This implies VDM is ready, so skip everything else */
			if (VdmWaitObject) goto VdmShortCircuit;

			/* Don't inherit handles since we're doing VDM now */
			bInheritHandles = FALSE;

			/* Had the user passed in environment? If so, destroy it */
			if ((lpEnvironment) &&
				!(dwCreationFlags & CREATE_UNICODE_ENVIRONMENT))
			{
				RtlDestroyEnvironment(lpEnvironment);
			}

			/* We've already done all these checks, don't do them again */
			SkipSaferAndAppCompat = TRUE;
			goto AppNameRetry;
		}

		// There is no break here on purpose, so FORCEDOS drops down!
	}

	case STATUS_INVALID_IMAGE_PROTECT:
	case STATUS_INVALID_IMAGE_NOT_MZ:
	case STATUS_INVALID_IMAGE_NE_FORMAT:
	{
		/* We're launching an executable application */
		BinarySubType = BINARY_TYPE_DOS_EXE;

		/* We can drop here from other "cases" above too, so check */
		if ((Status == STATUS_INVALID_IMAGE_PROTECT) ||
			(Status == STATUS_INVALID_IMAGE_NE_FORMAT) ||
			(BinarySubType = BaseIsDosApplication(&PathName, Status)))
		{
			/* We're launching a DOS application */
			VdmBinaryType = BINARY_TYPE_DOS;

			/* Based on the caller environment, create a VDM one */
			Result = BaseCreateVDMEnvironment(lpEnvironment,
				&VdmAnsiEnv,
				&VdmUnicodeEnv);
			if (!Result)
			{
				DPRINT1("VDM environment for DOS failed\n");
				goto Quickie;
			}

			/* Check the current state of the VDM subsystem */
			Status = BaseCheckVDM(VdmBinaryType | BinarySubType,
				lpApplicationName,
				lpCommandLine,
				lpCurrentDirectory,
				&VdmAnsiEnv,
				&CsrMsg[1],
				&VdmTask,
				dwCreationFlags,
				&StartupInfo,
				NULL);
			TRACE("BaseCheckVDM = %08X\n", Status);
			if (!NT_SUCCESS(Status))
			{
				/* Failed to inquire about VDM, fail the call */
				DPRINT1("VDM message failure for DOS: %lx\n", Status);
				BaseSetLastNTError(Status);
				Result = FALSE;
				goto Quickie;
			};

			/* Handle possible VDM states */

			switch (CheckVdmMsg->VDMState)
			{
			case VDM_NOT_PRESENT:
				/* If VDM is not loaded, we'll do a partial undo */
				VDMCreationState = VDM_PARTIALLY_CREATED;

				/* A VDM process can't also be detached, so fail */
				if (dwCreationFlags & DETACHED_PROCESS)
				{
					DPRINT1("Detached process but no VDM, not allowed\n");
					SetLastError(ERROR_ACCESS_DENIED);
					return FALSE;
				}

				/* Get the required parameters and names for launch */
				Result = BaseGetVdmConfigInfo(lpCommandLine,
					VdmTask,
					VdmBinaryType,
					&VdmString,
					&VdmReserve);
				if (!Result)
				{
					DPRINT1("VDM Configuration failed for DOS\n");
					BaseSetLastNTError(Status);
					goto Quickie;
				}

				/* Update the command-line to launch VDM instead */
				lpCommandLine = VdmString.Buffer;
				lpApplicationName = NULL;
				break;

			case VDM_PRESENT_AND_READY:
				/* VDM is ready, so we have to undo everything */
				VDMCreationState = VDM_BEING_REUSED;

				/* Check if CSRSS wants us to wait on VDM */
				VdmWaitObject = CheckVdmMsg->WaitObjectForParent;
				break;

			case VDM_PRESENT_NOT_READY:
				/* Something is wrong with VDM, we'll fail the call */
				DPRINT1("VDM is not ready for DOS\n");
				SetLastError(ERROR_NOT_READY);
				Result = FALSE;
				goto Quickie;

			default:
				TRACE("BUGBUGBUG: VDMState = %d\n", CheckVdmMsg->VDMState);
				break;
			}

			/* Since to get NULL, we allocate from 0x1, account for this */
			VdmReserve--;

			/* This implies VDM is ready, so skip everything else */
			if (VdmWaitObject) goto VdmShortCircuit;

			/* Don't inherit handles since we're doing VDM now */
			bInheritHandles = FALSE;

			/* Had the user passed in environment? If so, destroy it */
			if ((lpEnvironment) &&
				!(dwCreationFlags & CREATE_UNICODE_ENVIRONMENT))
			{
				RtlDestroyEnvironment(lpEnvironment);
			}

			/* Use our VDM Unicode environment instead */
			lpEnvironment = VdmUnicodeEnv.Buffer;
		}
		else
		{
			/* It's a batch file, get the extension */
			ExtBuffer = &PathName.Buffer[PathName.Length / sizeof(WCHAR) - 4];

			/* Make sure the extensions are correct */
			if ((PathName.Length < (4 * sizeof(WCHAR))) ||
				((__wcsnicmp(ExtBuffer, L".bat", 4)) &&
					(__wcsnicmp(ExtBuffer, L".cmd", 4))))
			{
				DPRINT1("'%wZ': Invalid EXE, and not a batch or script file\n", &PathName);
				SetLastError(ERROR_BAD_EXE_FORMAT);
				Result = FALSE;
				goto Quickie;
			}

			/* Check if we need to account for quotes around the path */
			CmdQuoteLength = CmdLineIsAppName || HasQuotes;
			if (!CmdLineIsAppName)
			{
				if (HasQuotes) CmdQuoteLength++;
			}
			else
			{
				CmdQuoteLength++;
			}

			/* Calculate the length of the command line */
			CmdLineLength = wcslen(lpCommandLine);
			CmdLineLength += wcslen(CMD_STRING);
			CmdLineLength += CmdQuoteLength + sizeof(ANSI_NULL);
			CmdLineLength *= sizeof(WCHAR);

			/* Allocate space for the new command line */
			AnsiCmdCommand = RtlAllocateHeap(RtlGetProcessHeap(),
				0,
				CmdLineLength);
			if (!AnsiCmdCommand)
			{
				BaseSetLastNTError(STATUS_NO_MEMORY);
				Result = FALSE;
				goto Quickie;
			}

			/* Build it */
			wcscpy(AnsiCmdCommand, CMD_STRING);
			if ((CmdLineIsAppName) || (HasQuotes))
			{
				wcscat(AnsiCmdCommand, L"\"");
			}
			wcscat(AnsiCmdCommand, lpCommandLine);
			if ((CmdLineIsAppName) || (HasQuotes))
			{
				wcscat(AnsiCmdCommand, L"\"");
			}

			/* Create it as a Unicode String */
			RtlInitUnicodeString(&DebuggerString, AnsiCmdCommand);

			/* Set the command line to this */
			lpCommandLine = DebuggerString.Buffer;
			lpApplicationName = NULL;
			DPRINT1("Retrying with: %S\n", lpCommandLine);
		}

		/* We've already done all these checks, don't do them again */
		SkipSaferAndAppCompat = TRUE;
		goto AppNameRetry;
	}

	case STATUS_INVALID_IMAGE_WIN_64:
	{
		/* 64-bit binaries are not allowed to run on 32-bit ReactOS */
		DPRINT1("64-bit binary, failing\n");
		SetLastError(ERROR_EXE_MACHINE_TYPE_MISMATCH);
		Result = FALSE;
		goto Quickie;
	}

	case STATUS_FILE_IS_OFFLINE:
	{
		/* Set the correct last error for this */
		DPRINT1("File is offline, failing\n");
		SetLastError(ERROR_FILE_OFFLINE);
		break;
	}

	default:
	{
		/* Any other error, convert it to a generic Win32 error */
		if (!NT_SUCCESS(Status))
		{
			DPRINT1("Failed to create section: %lx\n", Status);
			SetLastError(ERROR_BAD_EXE_FORMAT);
			Result = FALSE;
			goto Quickie;
		}

		/* Otherwise, this must be success */
		ASSERT(Status == STATUS_SUCCESS);
		break;
	}
	}

	/* Is this not a WOW application, but a WOW32 VDM was requested for it? */
	if (!(IsWowApp) && (dwCreationFlags & CREATE_SEPARATE_WOW_VDM))
	{
		/* Ignore the nonsensical request */
		dwCreationFlags &= ~CREATE_SEPARATE_WOW_VDM;
	}

	/* Did we already check information for the section? */
	if (!QuerySection)
	{
		/* Get some information about the executable */
		Status = NtQuerySection(SectionHandle,
			SectionImageInformation,
			&ImageInformation,
			sizeof(ImageInformation),
			NULL);
		if (!NT_SUCCESS(Status))
		{
			/* We failed, bail out */
			DPRINT1("Section query failed\n");
			BaseSetLastNTError(Status);
			Result = FALSE;
			goto Quickie;
		}

		/* Don't check this later */
		QuerySection = TRUE;
	}

	/* Check if this was linked as a DLL */
	if (ImageInformation.ImageCharacteristics & IMAGE_FILE_DLL)
	{
		/* These aren't valid images to try to execute! */
		DPRINT1("Trying to launch a DLL, failing\n");
		SetLastError(ERROR_BAD_EXE_FORMAT);
		Result = FALSE;
		goto Quickie;
	}

	/* Don't let callers pass in this flag -- we'll only get it from IFRO */
	Flags &= ~2;

	/* Clear the IFEO-missing flag, before we know for sure... */
	ParameterFlags &= ~2;

#if 0 /* NTVDM is not being debugged */
	/* If the process is being debugged, only read IFEO if the PEB says so */
	if (!(dwCreationFlags & (DEBUG_PROCESS | DEBUG_ONLY_THIS_PROCESS)) ||
		(NtCurrentPeb()->ReadImageFileExecOptions))
	{
		/* Let's do this! Attempt to open IFEO */
		Status1 = LdrOpenImageFileOptionsKey(&PathName, 0, &KeyHandle);
		if (!NT_SUCCESS(Status1))
		{
			/* We failed, set the flag so we store this in the parameters */
			if (Status1 == STATUS_OBJECT_NAME_NOT_FOUND) ParameterFlags |= 2;
		}
		else
		{
			/* Was this our first time going through this path? */
			if (!DebuggerCmdLine)
			{
				/* Allocate a buffer for the debugger path */
				DebuggerCmdLine = RtlAllocateHeap(RtlGetProcessHeap(),
					0,
					MAX_PATH * sizeof(WCHAR));
				if (!DebuggerCmdLine)
				{
					/* Close IFEO on failure */
					Status1 = NtClose(KeyHandle);
					ASSERT(NT_SUCCESS(Status1));

					/* Fail the call */
					SetLastError(ERROR_NOT_ENOUGH_MEMORY);
					Result = FALSE;
					goto Quickie;
				}
			}

			/* Now query for the debugger */
			Status1 = LdrQueryImageFileKeyOption(KeyHandle,
				L"Debugger",
				REG_SZ,
				DebuggerCmdLine,
				MAX_PATH * sizeof(WCHAR),
				&ResultSize);
			if (!(NT_SUCCESS(Status1)) ||
				(ResultSize < sizeof(WCHAR)) ||
				(DebuggerCmdLine[0] == UNICODE_NULL))
			{
				/* If it's not there, or too small, or invalid, ignore it */
				RtlFreeHeap(RtlGetProcessHeap(), 0, DebuggerCmdLine);
				DebuggerCmdLine = NULL;
			}

			/* Also query if we should map with large pages */
			Status1 = LdrQueryImageFileKeyOption(KeyHandle,
				L"UseLargePages",
				REG_DWORD,
				&UseLargePages,
				sizeof(UseLargePages),
				NULL);
			if ((NT_SUCCESS(Status1)) && (UseLargePages))
			{
				/* Do it! This is the only way this flag can be set */
				Flags |= PROCESS_CREATE_FLAGS_LARGE_PAGES;
			}

			/* We're done with IFEO, can close it now */
			Status1 = NtClose(KeyHandle);
			ASSERT(NT_SUCCESS(Status1));
		}
	}

	/* Make sure the image was compiled for this processor */
	if ((ImageInformation.Machine < SharedUserData->ImageNumberLow) ||
		(ImageInformation.Machine > SharedUserData->ImageNumberHigh))
	{
		/* It was not -- raise a hard error */
		ErrorResponse = ResponseOk;
		ErrorParameters[0] = (ULONG_PTR)&PathName;
		NtRaiseHardError(STATUS_IMAGE_MACHINE_TYPE_MISMATCH_EXE,
			1,
			1,
			ErrorParameters,
			OptionOk,
			&ErrorResponse);
		if (Peb->ImageSubsystemMajorVersion <= 3)
		{
			/* If it's really old, return this error */
			SetLastError(ERROR_BAD_EXE_FORMAT);
		}
		else
		{
			/* Otherwise, return a more modern error */
			SetLastError(ERROR_EXE_MACHINE_TYPE_MISMATCH);
		}

		/* Go to the failure path */
		DPRINT1("Invalid image architecture: %lx\n", ImageInformation.Machine);
		Result = FALSE;
		goto Quickie;
	}
#endif

	/* Check if this isn't a Windows image */
	if ((ImageInformation.SubSystemType != IMAGE_SUBSYSTEM_WINDOWS_GUI) &&
		(ImageInformation.SubSystemType != IMAGE_SUBSYSTEM_WINDOWS_CUI))
	{
		/* Get rid of section-related information since we'll retry */
		NtClose(SectionHandle);
		SectionHandle = NULL;
		QuerySection = FALSE;

		/* The only other non-Windows image type we support here is POSIX */
		if (ImageInformation.SubSystemType != IMAGE_SUBSYSTEM_POSIX_CUI)
		{
			/* Bail out if it's something else */
			SetLastError(ERROR_CHILD_NOT_COMPLETE);
			Result = FALSE;
			goto Quickie;
		}

		/* Now build the command-line to have posix launch this image */
		Result = BuildSubSysCommandLine(L"POSIX /P ",
			lpApplicationName,
			lpCommandLine,
			&DebuggerString);
		if (!Result)
		{
			/* Bail out if that failed */
			DPRINT1("Subsystem command line failed\n");
			goto Quickie;
		}

		/* And re-try launching the process, with the new command-line now */
		lpCommandLine = DebuggerString.Buffer;
		lpApplicationName = NULL;

		/* We've already done all these checks, don't do them again */
		SkipSaferAndAppCompat = TRUE;
		DPRINT1("Retrying with: %S\n", lpCommandLine);
		goto AppNameRetry;
	}

#if 0 // Not present on x64 kernel32.dll?
	/* Was this image built for a version of Windows whose images we can run? */
	Result = BasepIsImageVersionOk(ImageInformation.SubSystemMajorVersion,
		ImageInformation.SubSystemMinorVersion);
	if (!Result)
	{
		/* It was not, bail out */
		DPRINT1("Invalid subsystem version: %hu.%hu\n",
			ImageInformation.SubSystemMajorVersion,
			ImageInformation.SubSystemMinorVersion);
		SetLastError(ERROR_BAD_EXE_FORMAT);
		goto Quickie;
	}
#endif

	/* Check if there is a debugger associated with the application */
	if (DebuggerCmdLine)
	{
		/* Get the length of the command line */
		n = wcslen(lpCommandLine);
		if (!n)
		{
			/* There's no command line, use the application name instead */
			lpCommandLine = (LPWSTR)lpApplicationName;
			n = wcslen(lpCommandLine);
		}

		/* Protect against overflow */
		if (n > UNICODE_STRING_MAX_CHARS)
		{
			BaseSetLastNTError(STATUS_NAME_TOO_LONG);
			Result = FALSE;
			goto Quickie;
		}

		/* Now add the length of the debugger command-line */
		n += wcslen(DebuggerCmdLine);

		/* Again make sure we don't overflow */
		if (n > UNICODE_STRING_MAX_CHARS)
		{
			BaseSetLastNTError(STATUS_NAME_TOO_LONG);
			Result = FALSE;
			goto Quickie;
		}

		/* Account for the quotes and space between the two */
		n += sizeof("\" \"") - sizeof(ANSI_NULL);

		/* Convert to bytes, and make sure we don't overflow */
		n *= sizeof(WCHAR);
		if (n > UNICODE_STRING_MAX_BYTES)
		{
			BaseSetLastNTError(STATUS_NAME_TOO_LONG);
			Result = FALSE;
			goto Quickie;
		}

		/* Allocate space for the string */
		DebuggerString.Buffer = RtlAllocateHeap(RtlGetProcessHeap(), 0, n);
		if (!DebuggerString.Buffer)
		{
			SetLastError(ERROR_NOT_ENOUGH_MEMORY);
			Result = FALSE;
			goto Quickie;
		}

		/* Set the length */
		RtlInitEmptyUnicodeString(&DebuggerString,
			DebuggerString.Buffer,
			(USHORT)n);

		/* Now perform the command line creation */
		ImageDbgStatus = RtlAppendUnicodeToString(&DebuggerString,
			DebuggerCmdLine);
		ASSERT(NT_SUCCESS(ImageDbgStatus));
		ImageDbgStatus = RtlAppendUnicodeToString(&DebuggerString, L" ");
		ASSERT(NT_SUCCESS(ImageDbgStatus));
		ImageDbgStatus = RtlAppendUnicodeToString(&DebuggerString, lpCommandLine);
		ASSERT(NT_SUCCESS(ImageDbgStatus));

		/* Make sure it all looks nice */
		TRACE("BASE: Calling debugger with '%wZ'\n", &DebuggerString);

		/* Update the command line and application name */
		lpCommandLine = DebuggerString.Buffer;
		lpApplicationName = NULL;

		/* Close all temporary state */
		NtClose(SectionHandle);
		SectionHandle = NULL;
		QuerySection = FALSE;

		/* Free all temporary memory */
		RtlFreeHeap(RtlGetProcessHeap(), 0, NameBuffer);
		NameBuffer = NULL;
		RtlFreeHeap(RtlGetProcessHeap(), 0, FreeBuffer);
		FreeBuffer = NULL;
		RtlFreeHeap(RtlGetProcessHeap(), 0, DebuggerCmdLine);
		DebuggerCmdLine = NULL;
		DPRINT1("Retrying with: %S\n", lpCommandLine);
		goto AppNameRetry;
	}

	/* Initialize the process object attributes */
	ObjectAttributes = BaseFormatObjectAttributes(&LocalObjectAttributes,
		lpProcessAttributes,
		NULL);
	if ((hUserToken) && (lpProcessAttributes))
	{
		/* Auggment them with information from the user */

		LocalProcessAttributes = *lpProcessAttributes;
		LocalProcessAttributes.lpSecurityDescriptor = NULL;
		ObjectAttributes = BaseFormatObjectAttributes(&LocalObjectAttributes,
			&LocalProcessAttributes,
			NULL);
	}

	/* Check if we're going to be debugged */
	if (dwCreationFlags & DEBUG_PROCESS)
	{
		/* Set process flag */
		Flags |= PROCESS_CREATE_FLAGS_BREAKAWAY;
	}

	/* Check if we're going to be debugged */
	if (dwCreationFlags & (DEBUG_PROCESS | DEBUG_ONLY_THIS_PROCESS))
	{
		/* Connect to DbgUi */
		Status = DbgUiConnectToDbg();
		if (!NT_SUCCESS(Status))
		{
			DPRINT1("Failed to connect to DbgUI!\n");
			BaseSetLastNTError(Status);
			Result = FALSE;
			goto Quickie;
		}

		/* Get the debug object */
		DebugHandle = DbgUiGetThreadDebugObject();

		/* Check if only this process will be debugged */
		if (dwCreationFlags & DEBUG_ONLY_THIS_PROCESS)
		{
			/* Set process flag */
			Flags |= PROCESS_CREATE_FLAGS_NO_DEBUG_INHERIT;
		}
	}

	/* Set inherit flag */
	if (bInheritHandles) Flags |= PROCESS_CREATE_FLAGS_INHERIT_HANDLES;

	/* Check if the process should be created with large pages */
	HavePrivilege = FALSE;
	PrivilegeState = NULL;
	if (Flags & PROCESS_CREATE_FLAGS_LARGE_PAGES)
	{
		/* Acquire the required privilege so that the kernel won't fail the call */
		PrivilegeValue = SE_LOCK_MEMORY_PRIVILEGE;
		Status = RtlAcquirePrivilege(&PrivilegeValue, 1, 0, &PrivilegeState);
		if (NT_SUCCESS(Status))
		{
			/* Remember to release it later */
			HavePrivilege = TRUE;
		}
	}

	/* Save the current TIB value since kernel overwrites it to store PEB */
	TibValue = ((struct _NT_TIB *)Teb)->ArbitraryUserPointer;

	/* Tell the kernel to create the process */
	Status = NtCreateProcessEx(&ProcessHandle,
		PROCESS_ALL_ACCESS,
		ObjectAttributes,
		NtCurrentProcess(),
		Flags,
		SectionHandle,
		DebugHandle,
		NULL,
		InJob);

#if 0
	/* Load the PEB address from the hacky location where the kernel stores it */
	RemotePeb = ((struct _NT_TIB *)Teb)->ArbitraryUserPointer;
#else	// I'm not confident about _NT_TIB layout
	//
	// Determine the location of the
	// processes PEB.
	//
	PROCESS_BASIC_INFORMATION ProcessInfo;

	Status = NtQueryInformationProcess(
		ProcessHandle,
		ProcessBasicInformation,
		&ProcessInfo,
		sizeof(ProcessInfo),
		NULL
		);
	if (!NT_SUCCESS(Status)) {
		BaseSetLastNTError(Status);
		goto Quickie;
	}

	RemotePeb = ProcessInfo.PebBaseAddress;
#endif

	/* And restore the old TIB value */
	((struct _NT_TIB *)Teb)->ArbitraryUserPointer = TibValue;

	/* Release the large page privilege if we had acquired it */
	if (HavePrivilege) RtlReleasePrivilege(PrivilegeState);

	/* And now check if the kernel failed to create the process */
	if (!NT_SUCCESS(Status))
	{
		/* Go to failure path */
		DPRINT1("Failed to create process: %lx\n", Status);
		BaseSetLastNTError(Status);
		Result = FALSE;
		goto Quickie;
	}

	/* Check if there is a priority class to set */
	if (PriorityClass.PriorityClass)
	{
		/* Reset current privilege state */
		RealTimePrivilegeState = NULL;

		/* Is realtime priority being requested? */
		if (PriorityClass.PriorityClass == PROCESS_PRIORITY_CLASS_REALTIME)
		{
			/* Check if the caller has real-time access, and enable it if so */
			RealTimePrivilegeState = BasepIsRealtimeAllowed(TRUE);
		}

		/* Set the new priority class and release the privilege */
		Status = NtSetInformationProcess(ProcessHandle,
			ProcessPriorityClass,
			&PriorityClass,
			sizeof(PROCESS_PRIORITY_CLASS));
		if (RealTimePrivilegeState) RtlReleasePrivilege(RealTimePrivilegeState);

		/* Check if we failed to set the priority class */
		if (!NT_SUCCESS(Status))
		{
			/* Bail out on failure */
			DPRINT1("Failed to set priority class: %lx\n", Status);
			BaseSetLastNTError(Status);
			Result = FALSE;
			goto Quickie;
		}
	}

	/* Check if the caller wants the default error mode */
	if (dwCreationFlags & CREATE_DEFAULT_ERROR_MODE)
	{
		/* Set Error Mode to only fail on critical errors */
		HardErrorMode = SEM_FAILCRITICALERRORS;
		NtSetInformationProcess(ProcessHandle,
			ProcessDefaultHardErrorMode,
			&HardErrorMode,
			sizeof(ULONG));
	}

	/* Check if this was a VDM binary */
	if (VdmBinaryType)
	{
		/* Update VDM by telling it the process has now been created */
		VdmWaitObject = ProcessHandle;
		Result = BaseUpdateVDMEntry(UPDATE_VDM_PROCESS_HANDLE,
			&VdmWaitObject,
			VdmTask,
			VdmBinaryType);

		if (!Result)
		{
			/* Bail out on failure */
			DPRINT1("Failed to update VDM with wait object\n");
			VdmWaitObject = NULL;
			goto Quickie;
		}

		/* At this point, a failure means VDM has to undo all the state */
		VDMCreationState |= VDM_FULLY_CREATED;
	}

	/* Check if VDM needed reserved low-memory */
	if (VdmReserve)
	{
		/* Reserve the requested allocation */
		Status = NtAllocateVirtualMemory(ProcessHandle,
			&BaseAddress,
			0,
			&VdmReserve,
			MEM_RESERVE,
			PAGE_EXECUTE_READWRITE);
		if (!NT_SUCCESS(Status))
		{
			/* Bail out on failure */
			DPRINT1("Failed to reserved memory for VDM: %lx\n", Status);
			BaseSetLastNTError(Status);
			Result = FALSE;
			goto Quickie;
		}
	}

	/* Check if we've already queried information on the section */
	if (!QuerySection)
	{
		/* We haven't, so get some information about the executable */
		Status = NtQuerySection(SectionHandle,
			SectionImageInformation,
			&ImageInformation,
			sizeof(ImageInformation),
			NULL);
		if (!NT_SUCCESS(Status))
		{
			/* Bail out on failure */
			DPRINT1("Failed to query section: %lx\n", Status);
			BaseSetLastNTError(Status);
			Result = FALSE;
			goto Quickie;
		}

		/* If we encounter a restart, don't re-query this information again */
		QuerySection = TRUE;
	}

	/* Do we need to apply SxS to this image? */
	if (!(ImageInformation.DllCharacteristics & IMAGE_DLLCHARACTERISTICS_NO_ISOLATION))
	{
		/* Too bad, we don't support this yet */
		DPRINT1("Image should receive SxS Fusion Isolation, FusionFlags = %d\n", FusionFlags);
	}

	/* There's some SxS flag that we need to set if fusion flags have 1 set */
	if (FusionFlags & 1) CreateProcessMsg->Sxs.Flags |= 0x10;

	/* Check if we have a current directory */
	if (lpCurrentDirectory)
	{
		/* Allocate a buffer so we can keep a Unicode copy */
		DPRINT1("Current directory: %S\n", lpCurrentDirectory);
		CurrentDirectory = RtlAllocateHeap(RtlGetProcessHeap(),
			0,
			(MAX_PATH * sizeof(WCHAR)) +
			sizeof(UNICODE_NULL));
		if (!CurrentDirectory)
		{
			/* Bail out if this failed */
			BaseSetLastNTError(STATUS_NO_MEMORY);
			Result = FALSE;
			goto Quickie;
		}

		/* Get the length in Unicode */
		Length = GetFullPathNameW(lpCurrentDirectory,
			MAX_PATH,
			CurrentDirectory,
			&FilePart);
		if (Length > MAX_PATH)
		{
			/* The directory is too long, so bail out */
			SetLastError(ERROR_DIRECTORY);
			Result = FALSE;
			goto Quickie;
		}

		/* Make sure the directory is actually valid */
		CurdirLength = GetFileAttributesW(CurrentDirectory);
		if ((CurdirLength == 0xffffffff) ||
			!(CurdirLength & FILE_ATTRIBUTE_DIRECTORY))
		{
			/* It isn't, so bail out */
			DPRINT1("Current directory is invalid\n");
			SetLastError(ERROR_DIRECTORY);
			Result = FALSE;
			goto Quickie;
		}
	}

	/* Insert quotes if needed */
	if ((QuotesNeeded) || (CmdLineIsAppName))
	{
		/* Allocate our buffer, plus enough space for quotes and a NULL */
		QuotedCmdLine = RtlAllocateHeap(RtlGetProcessHeap(),
			0,
			(wcslen(lpCommandLine) * sizeof(WCHAR)) +
			(2 * sizeof(L'\"') + sizeof(UNICODE_NULL)));
		if (QuotedCmdLine)
		{
			/* Copy the first quote */
			wcscpy(QuotedCmdLine, L"\"");

			/* Save the current null-character */
			if (QuotesNeeded)
			{
				SaveChar = *NullBuffer;
				*NullBuffer = UNICODE_NULL;
			}

			/* Copy the command line and the final quote */
			wcscat(QuotedCmdLine, lpCommandLine);
			wcscat(QuotedCmdLine, L"\"");

			/* Copy the null-char back */
			if (QuotesNeeded)
			{
				*NullBuffer = SaveChar;
				wcscat(QuotedCmdLine, NullBuffer);
			}
		}
		else
		{
			/* We can't put quotes around the thing, so try it anyway */
			if (QuotesNeeded) QuotesNeeded = FALSE;
			if (CmdLineIsAppName) CmdLineIsAppName = FALSE;
		}
	}

	/* Use isolation if needed */
	if (CreateProcessMsg->Sxs.Flags & 1) ParameterFlags |= 1;

	/* Set the new command-line if needed */
	if ((QuotesNeeded) || (CmdLineIsAppName)) lpCommandLine = QuotedCmdLine;

	/* Call the helper function in charge of RTL_USER_PROCESS_PARAMETERS */
	Result = BasePushProcessParameters(ParameterFlags,
		ProcessHandle,
		RemotePeb,
		lpApplicationName,
		CurrentDirectory,
		lpCommandLine,
		lpEnvironment,
		&StartupInfo,
		dwCreationFlags | NoWindow,
		bInheritHandles,
		IsWowApp ? IMAGE_SUBSYSTEM_WINDOWS_GUI : 0,
		AppCompatData,
		AppCompatDataSize);
	if (!Result)
	{
		/* The remote process would have an undefined state, so fail the call */
		DPRINT1("BasePushProcessParameters failed\n");
		goto Quickie;
	}

	/* Free the VDM command line string as it's no longer needed */
	RtlFreeUnicodeString(&VdmString);
	VdmString.Buffer = NULL;

	/* Non-VDM console applications usually inherit handles unless specified */
	if (!(VdmBinaryType) &&
		!(bInheritHandles) &&
		!(StartupInfo.dwFlags & STARTF_USESTDHANDLES) &&
		!(dwCreationFlags & (CREATE_NO_WINDOW |
			CREATE_NEW_CONSOLE |
			DETACHED_PROCESS)) &&
		(ImageInformation.SubSystemType == IMAGE_SUBSYSTEM_WINDOWS_CUI))
	{
		/* Get the remote parameters */
		Status = NtReadVirtualMemory(ProcessHandle,
			&RemotePeb->ProcessParameters,
			&ProcessParameters,
			sizeof(PRTL_USER_PROCESS_PARAMETERS),
			NULL);
		if (NT_SUCCESS(Status))
		{
			xpPRTL_USER_PROCESS_PARAMETERS PebProcessParameters = ((xpPRTL_USER_PROCESS_PARAMETERS)Peb->ProcessParameters);

			/* Duplicate standard input unless it's a console handle */
			if (!IsConsoleHandle(PebProcessParameters->StandardInput))
			{
				StuffStdHandle(ProcessHandle,
					PebProcessParameters->StandardInput,
					&ProcessParameters->StandardInput);
			}

			/* Duplicate standard output unless it's a console handle */
			if (!IsConsoleHandle(PebProcessParameters->StandardOutput))
			{
				StuffStdHandle(ProcessHandle,
					PebProcessParameters->StandardOutput,
					&ProcessParameters->StandardOutput);
			}

			/* Duplicate standard error unless it's a console handle */
			if (!IsConsoleHandle(PebProcessParameters->StandardError))
			{
				StuffStdHandle(ProcessHandle,
					PebProcessParameters->StandardError,
					&ProcessParameters->StandardError);
			}
		}
	}

	/* Create the Thread's Stack */
	StackSize = max(256 * 1024, ImageInformation.MaximumStackSize);
	Status = BaseCreateStack(ProcessHandle,
		ImageInformation.CommittedStackSize,
		StackSize,
		&InitialTeb);
	if (!NT_SUCCESS(Status))
	{
		DPRINT1("Creating the thread stack failed: %lx\n", Status);
		BaseSetLastNTError(Status);
		Result = FALSE;
		goto Quickie;
	}

	/* Create the Thread's Context */
	BaseInitializeContext(&Context,
		Peb,
		ImageInformation.TransferAddress,
		InitialTeb.StackBase,
		0);

	/* Convert the thread attributes */
	ObjectAttributes = BaseFormatObjectAttributes(&LocalObjectAttributes,
		lpThreadAttributes,
		NULL);
	if ((hUserToken) && (lpThreadAttributes))
	{
		/* If the caller specified a user token, zero the security descriptor */
		LocalThreadAttributes = *lpThreadAttributes;
		LocalThreadAttributes.lpSecurityDescriptor = NULL;
		ObjectAttributes = BaseFormatObjectAttributes(&LocalObjectAttributes,
			&LocalThreadAttributes,
			NULL);
	}

	/* Create the Kernel Thread Object */
	Status = NtCreateThread(&ThreadHandle,
		THREAD_ALL_ACCESS,
		ObjectAttributes,
		ProcessHandle,
		&ClientId,
		&Context,
		&InitialTeb,
		TRUE);
	if (!NT_SUCCESS(Status))
	{
		/* A process is not allowed to exist without a main thread, so fail */
		DPRINT1("Creating the main thread failed: %lx\n", Status);
		BaseSetLastNTError(Status);
		Result = FALSE;
		goto Quickie;
	}

	/* Begin filling out the CSRSS message, first with our IDs and handles */
	CreateProcessMsg->ProcessHandle = ProcessHandle;
	CreateProcessMsg->ThreadHandle = ThreadHandle;
	CreateProcessMsg->ClientId = ClientId;

	/* Write the remote PEB address and clear it locally, we no longer use it */
	CreateProcessMsg->PebAddressNative = (ULONG64)RemotePeb;
	CreateProcessMsg->PebAddressWow64 = (ULONG)RemotePeb;
	RemotePeb = NULL;

	/* Now check what kind of architecture this image was made for */
	switch (ImageInformation.Machine)
	{
		/* IA32, IA64 and AMD64 are supported in Server 2003 */
	case IMAGE_FILE_MACHINE_I386:
		CreateProcessMsg->ProcessorArchitecture = PROCESSOR_ARCHITECTURE_INTEL;
		break;
	case IMAGE_FILE_MACHINE_IA64:
		CreateProcessMsg->ProcessorArchitecture = PROCESSOR_ARCHITECTURE_IA64;
		break;
	case IMAGE_FILE_MACHINE_AMD64:
		CreateProcessMsg->ProcessorArchitecture = PROCESSOR_ARCHITECTURE_AMD64;
		break;

		/* Anything else results in image unknown -- but no failure */
	default:
		TRACE("kernel32: No mapping for ImageInformation.Machine == %04x\n",
			ImageInformation.Machine);
		CreateProcessMsg->ProcessorArchitecture = PROCESSOR_ARCHITECTURE_UNKNOWN;
		break;
	}

	/* Write the input creation flags except any debugger-related flags */
	CreateProcessMsg->CreationFlags = dwCreationFlags &
		~(DEBUG_PROCESS | DEBUG_ONLY_THIS_PROCESS);

	/* CSRSS needs to know if this is a GUI app or not */
	if ((ImageInformation.SubSystemType == IMAGE_SUBSYSTEM_WINDOWS_GUI) ||
		(IsWowApp))
	{
		/*
		* For GUI apps we turn on the 2nd bit. This allow CSRSS server dlls
		* (basesrv in particular) to know whether or not this is a GUI or a
		* TUI application.
		*/
		AddToHandle(CreateProcessMsg->ProcessHandle, 2);

		/* Also check if the parent is also a GUI process */
		NtHeaders = RtlImageNtHeader(GetModuleHandle(NULL));
		if ((NtHeaders) &&
			(NtHeaders->OptionalHeader.Subsystem == IMAGE_SUBSYSTEM_WINDOWS_GUI))
		{
			/* Let it know that it should display the hourglass mouse cursor */
			AddToHandle(CreateProcessMsg->ProcessHandle, 1);
		}
	}

	/* For all apps, if this flag is on, the hourglass mouse cursor is shown */
	if (StartupInfo.dwFlags & STARTF_FORCEONFEEDBACK)
	{
		AddToHandle(CreateProcessMsg->ProcessHandle, 1);
	}

	/* Likewise, the opposite holds as well */
	if (StartupInfo.dwFlags & STARTF_FORCEOFFFEEDBACK)
	{
		RemoveFromHandle(CreateProcessMsg->ProcessHandle, 1);
	}

	/* Also store which kind of VDM app (if any) this is */
	CreateProcessMsg->VdmBinaryType = VdmBinaryType;

	/* And if it really is a VDM app... */
	if (VdmBinaryType)
	{
		/* Store the task ID and VDM console handle */
		CreateProcessMsg->hVDM = VdmTask ? 0 : ((xpPRTL_USER_PROCESS_PARAMETERS)Peb->ProcessParameters)->ConsoleHandle;
		CreateProcessMsg->VdmTask = VdmTask;
	}
	else if (VdmReserve)
	{
		/* Extended VDM, set a flag */
		CreateProcessMsg->VdmBinaryType = BINARY_TYPE_WOW_EX;
	}

	/* We are finally ready to call CSRSS to tell it about our new process! */
#if defined(BUILD_WOW6432)
	CsrMsg[0].ReturnValue = CsrBasepCreateProcess(CreateProcessMsg);
#else
	/* Check if there's side-by-side assembly data associated with the process */
	if (CreateProcessMsg->Sxs.Flags)
	{
		PUNICODE_STRING CapturedStrings[3] = {
			&CreateProcessMsg->Sxs.ManifestStream.FileName,
			&CreateProcessMsg->Sxs.PolicyStream.FileName,
			&CreateProcessMsg->Sxs.AssemblyName
		};
		Status = CsrCaptureMessageMultiUnicodeStringsInPlace(&CaptureBuffer, 3, CapturedStrings);
		if (!NT_SUCCESS(Status))
		{
			BaseSetLastNTError(Status);
			Result = FALSE;
			goto Quickie;
		}
	}

	myCsrClientCallServer((PCSR_API_MSG)&CsrMsg[0],
		NULL,
		CSR_MAKE_API_NUMBER(BASESRV_SERVERDLL_INDEX,
			BasepCreateProcess
			),
		sizeof(*CreateProcessMsg)
		);

	if (CaptureBuffer)
	{
		CsrFreeCaptureBuffer(CaptureBuffer);
		CaptureBuffer = NULL;
	}
#endif

	/* Check if CSRSS failed to accept ownership of the new Windows process */
	if (!NT_SUCCESS(CsrMsg[0].ReturnValue))
	{
		/* Terminate the process and enter failure path with the CSRSS status */
#if defined(BUILD_WOW6432)
		DPRINT1("x86 Failed to tell csrss about new process: %08X\n", CsrMsg[0].ReturnValue);
#else
		DPRINT1("x64 Failed to tell csrss about new process: %08X\n", CsrMsg[0].ReturnValue);
#endif
		BaseSetLastNTError(CsrMsg[0].ReturnValue);
		NtTerminateProcess(ProcessHandle, CsrMsg[0].ReturnValue);
		Result = FALSE;
		goto Quickie;
	}

	/* Check if we have a token due to Authz/Safer, not passed by the user */
	if ((TokenHandle) && !(hUserToken))
	{
		/* Replace the process and/or thread token with the one from Safer */
		Status = BasepReplaceProcessThreadTokens(TokenHandle,
			ProcessHandle,
			ThreadHandle);
		if (!NT_SUCCESS(Status))
		{
			/* If this failed, kill the process and enter the failure path */
			DPRINT1("Failed to update process token: %lx\n", Status);
			NtTerminateProcess(ProcessHandle, Status);
			BaseSetLastNTError(Status);
			Result = FALSE;
			goto Quickie;
		}
	}

	/* Check if a job was associated with this process */
	if (JobHandle)
	{
		/* Bind the process and job together now */
		Status = NtAssignProcessToJobObject(JobHandle, ProcessHandle);
		if (!NT_SUCCESS(Status))
		{
			/* Kill the process and enter the failure path if binding failed */
			DPRINT1("Failed to assign process to job: %lx\n", Status);
			NtTerminateProcess(ProcessHandle, STATUS_ACCESS_DENIED);
			BaseSetLastNTError(Status);
			Result = FALSE;
			goto Quickie;
		}
	}

	/* Finally, resume the thread to actually get the process started */
	if (!(dwCreationFlags & CREATE_SUSPENDED))
	{
		NtResumeThread(ThreadHandle, &ResumeCount);
	}

VdmShortCircuit:
	/* We made it this far, meaning we have a fully created process and thread */
	Result = TRUE;

	/* Anyone doing a VDM undo should now undo everything, since we are done */
	if (VDMCreationState) VDMCreationState |= VDM_CREATION_SUCCESSFUL;

	/* Having a VDM wait object implies this must be a VDM process */
	if (VdmWaitObject)
	{
		/* Check if it's a 16-bit separate WOW process */
		if (VdmBinaryType == BINARY_TYPE_SEPWOW)
		{
			/* OR-in the special flag to indicate this, and return to caller */
			AddToHandle(VdmWaitObject, 2);
			lpProcessInformation->hProcess = VdmWaitObject;

			/* Check if this was a re-used VDM */
			if (VDMCreationState & VDM_BEING_REUSED)
			{
				/* No Client ID should be returned in this case */
				ClientId.UniqueProcess = 0;
				ClientId.UniqueThread = 0;
			}
		}
		else
		{
			/* OR-in the special flag to indicate this is not a separate VDM */
			AddToHandle(VdmWaitObject, 1);

			/* Return handle to the caller */
			lpProcessInformation->hProcess = VdmWaitObject;
		}
		TRACE("Returning VdmWaitObject %08X as process handle\n", VdmWaitObject);

		/* Close the original process handle, since it's not needed for VDM */
		if (ProcessHandle) NtClose(ProcessHandle);
	}
	else
	{
		/* This is a regular process, so return the real process handle */
		lpProcessInformation->hProcess = ProcessHandle;
	}

	/* Return the rest of the process information based on what we have so far */
	lpProcessInformation->hThread = ThreadHandle;
	lpProcessInformation->dwProcessId = (DWORD)HandleToUlong(ClientId.UniqueProcess);
	lpProcessInformation->dwThreadId = (DWORD)HandleToUlong(ClientId.UniqueThread);

	/* NULL these out here so we know to treat this as a success scenario */
	ProcessHandle = NULL;
	ThreadHandle = NULL;

Quickie:
	/* Free the debugger command line if one was allocated */
	if (DebuggerCmdLine) RtlFreeHeap(RtlGetProcessHeap(), 0, DebuggerCmdLine);

	/* Check if an SxS full path as queried */
	if (PathBuffer)
	{
		/* Reinitialize the executable path */
		RtlInitEmptyUnicodeString(&SxsWin32ExePath, NULL, 0);
		SxsWin32ExePath.Length = 0;

		/* Free the path buffer */
		RtlFreeHeap(RtlGetProcessHeap(), 0, PathBuffer);
	}

#if _SXS_SUPPORT_ENABLED_
	/* Check if this was a non-VDM process */
	if (!VdmBinaryType)
	{
		/* Then it must've had SxS data, so close the handles used for it */
		BasepSxsCloseHandles(&Handles);
		BasepSxsCloseHandles(&FileHandles);

		/* Check if we built SxS byte buffers for this create process request */
		if (SxsConglomeratedBuffer)
		{
			/* Loop all of them */
			for (i = 0; i < 5; i++)
			{
				/* Check if this one was allocated */
				ThisBuffer = SxsStaticBuffers[i];
				if (ThisBuffer)
				{
					/* Get the underlying RTL_BUFFER structure */
					ByteBuffer = &ThisBuffer->ByteBuffer;
					if ((ThisBuffer != (PVOID)-8) && (ByteBuffer->Buffer))
					{
						/* Check if it was dynamic */
						if (ByteBuffer->Buffer != ByteBuffer->StaticBuffer)
						{
							/* Free it from the heap */
							FreeString.Buffer = (PWCHAR)ByteBuffer->Buffer;
							RtlFreeUnicodeString(&FreeString);
						}

						/* Reset the buffer to its static data */
						ByteBuffer->Buffer = ByteBuffer->StaticBuffer;
						ByteBuffer->Size = ByteBuffer->StaticSize;
					}

					/* Reset the string to the static buffer */
					RtlInitEmptyUnicodeString(&ThisBuffer->String,
						(PWCHAR)ByteBuffer->StaticBuffer,
						ByteBuffer->StaticSize);
					if (ThisBuffer->String.Buffer)
					{
						/* Also NULL-terminate it */
						*ThisBuffer->String.Buffer = UNICODE_NULL;
					}
				}
			}
		}
	}
#endif
	/* Check if an environment was passed in */
	if ((lpEnvironment) && !(dwCreationFlags & CREATE_UNICODE_ENVIRONMENT))
	{
		/* Destroy it */
		RtlDestroyEnvironment(lpEnvironment);

		/* If this was the VDM environment too, clear that as well */
		if (VdmUnicodeEnv.Buffer == lpEnvironment) VdmUnicodeEnv.Buffer = NULL;
		lpEnvironment = NULL;
	}

	/* Unconditionally free all the name parsing buffers we always allocate */
	RtlFreeHeap(RtlGetProcessHeap(), 0, QuotedCmdLine);
	RtlFreeHeap(RtlGetProcessHeap(), 0, NameBuffer);
	RtlFreeHeap(RtlGetProcessHeap(), 0, CurrentDirectory);
	RtlFreeHeap(RtlGetProcessHeap(), 0, FreeBuffer);

	/* Close open file/section handles */
	if (FileHandle) NtClose(FileHandle);
	if (SectionHandle) NtClose(SectionHandle);

	/* If we have a thread handle, this was a failure path */
	if (ThreadHandle)
	{
		/* So kill the process and close the thread handle */
		NtTerminateProcess(ProcessHandle, 0);
		NtClose(ThreadHandle);
	}

	/* If we have a process handle, this was a failure path, so close it */
	if (ProcessHandle) NtClose(ProcessHandle);

	/* Thread/process handles, if any, are now processed. Now close this one. */
	if (JobHandle) NtClose(JobHandle);

	/* Check if we had created a token */
	if (TokenHandle)
	{
		/* And if the user asked for one */
		if (hUserToken)
		{
			/* Then return it */
			*hNewToken = TokenHandle;
		}
		else
		{
			/* User didn't want it, so we used it temporarily -- close it */
			NtClose(TokenHandle);
		}
	}

	/* Free any temporary app compatibility data, it's no longer needed */
	BasepFreeAppCompatData(AppCompatData, AppCompatSxsData);

	/* Free a few strings. The API takes care of these possibly being NULL */
	RtlFreeUnicodeString(&VdmString);
	RtlFreeUnicodeString(&DebuggerString);

	/* Check if we had built any sort of VDM environment */
	if ((VdmAnsiEnv.Buffer) || (VdmUnicodeEnv.Buffer))
	{
		/* Free it */
		BaseDestroyVDMEnvironment(&VdmAnsiEnv, &VdmUnicodeEnv);
	}

	/* Check if this was any kind of VDM application that we ended up creating */
	if ((VDMCreationState) && (!(VDMCreationState & VDM_CREATION_SUCCESSFUL)))
	{
		/* Send an undo */
		BaseUpdateVDMEntry(UPDATE_VDM_UNDO_CREATION,
			(PHANDLE)&VdmTask,
			VDMCreationState,
			VdmBinaryType);

		/* And close whatever VDM handle we were using for notifications */
		if (VdmWaitObject) NtClose(VdmWaitObject);
	}

	/* Check if we ended up here with an allocated search path, and free it */
	if (lpProcessExePath) RtlFreeHeap(RtlGetProcessHeap(), 0, lpProcessExePath);

	/* Finally, return the API's result */
	return Result;
}

/*
* @implemented
*/
BOOL
WINAPI
xpCreateProcessW(LPCWSTR lpApplicationName,
	LPWSTR lpCommandLine,
	LPSECURITY_ATTRIBUTES lpProcessAttributes,
	LPSECURITY_ATTRIBUTES lpThreadAttributes,
	BOOL bInheritHandles,
	DWORD dwCreationFlags,
	LPVOID lpEnvironment,
	LPCWSTR lpCurrentDirectory,
	LPSTARTUPINFOW lpStartupInfo,
	LPPROCESS_INFORMATION lpProcessInformation)
{
	/* Call the internal (but exported) version */
	return CreateProcessInternalW(NULL,
		lpApplicationName,
		lpCommandLine,
		lpProcessAttributes,
		lpThreadAttributes,
		bInheritHandles,
		dwCreationFlags,
		lpEnvironment,
		lpCurrentDirectory,
		lpStartupInfo,
		lpProcessInformation,
		NULL);
}


BOOL XpCreateProcHandler_Install(HMODULE hKrnl32)
{
	HANDLE hMutex;

	Hook_IAT_x64_IAT((LPBYTE)hKrnl32, "ntdll.dll", "NtCreateSection", NtCreateSectionHook, (PULONG_PTR)&NtCreateSectionReal);
	Hook_IAT_x64_IAT((LPBYTE)GetModuleHandle(NULL), "KERNEL32.DLL", "GetNextVDMCommand", GetNextVDMCommandHook, (PULONG_PTR)&GetNextVDMCommandReal);
#ifdef _WIN64
	// We create a global mutex to only inject into csrss.exe once
	if (hMutex = CreateMutex(NULL, TRUE, L"ldntvdm invaded csrss") && GetLastError() != ERROR_ALREADY_EXISTS)
	{
		BOOLEAN bRes;
		NTSTATUS Status;

		// SeDebugPrivilege
		TRACE("ldntvdm trying to invade csrss...");

		if (NT_SUCCESS(Status = RtlAdjustPrivilege(20, TRUE, FALSE, &bRes)))
		{
			if (NT_SUCCESS(HookCSRSS()))
				return TRUE;
		}
		else { TRACE("Failed to acquire SeDebugPrivilege: %08X\n", Status); }
		CloseHandle(hMutex);
	}
#endif // _WIN64
	return TRUE;
}


#endif // TARGET_WINXP
