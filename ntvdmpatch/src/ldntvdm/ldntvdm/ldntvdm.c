/* Project: ldntvdm
 * Module : ldntvdm (main)
 * Author : leecher@dose.0wnz.at
 * Descr. : The purpose of this module is to inject into every process 
 *          (initially via AppInit_DLLs, propagation via CreateProcess
 *          hook) and patch the loader in order to fire up the NTVDM
 *          when trying to execute DOS executables. It does this by 
 *          hooking KERNEL32.DLL BasepProcessInvalidImage funtion.
 *          In 32bit version, it also has to fix all CSRSS-Calls done
 *          by kernel32-functions in order to get loader and NTVDM
 *          up and running (this normaly should be done by WOW64.dll)
 *          Additionally it fixes a bug in SetConsolePalette call.
 *          Also it fixes a missing NULL pointer initialiszation
 *          in ConHostV1.dll!CreateConsoleBitmap 
 *          (second NtMapViewOFSection doesn't have its buffer Ptr
 *           initialized) by hooking RtlAllocateHeap.
 * Changes: 01.04.2016  - Created
 */

#include <ntstatus.h>
#define WIN32_NO_STATUS
#include "ldntvdm.h"
#include <tchar.h>
#include "Winternl.h"
#include "csrsswrap.h"
#include "injector32.h"
#include "injector64.h"
#include "wow64inj.h"
#include "wow64ext.h"
#include "iathook.h"
#include "basemsg64.h"
#include "basevdm.h"
#include "symeng.h"
#include "detour.h"
#include "consbmp.h"
#include "reg.h"

#pragma comment(lib, "ntdll.lib")

#ifdef _WIN64
#define BASEP_CALL __fastcall
#else
#define BASEP_CALL WINAPI
#endif

#if defined(TARGET_WIN7) || defined(TARGET_WIN80)
#define KRNL32_CALL BASEP_CALL
#else
#define KRNL32_CALL __fastcall
#endif

#define ProcessConsoleHostProcess 49

typedef INT_PTR(BASEP_CALL *fpBasepProcessInvalidImage)(NTSTATUS Error, HANDLE TokenHandle,
	LPCWSTR dosname, LPCWSTR *lppApplicationName,
	LPCWSTR *lppCommandLine, LPCWSTR lpCurrentDirectory,
	PDWORD pdwCreationFlags, BOOL *pbInheritHandles, PUNICODE_STRING PathName, INT_PTR a10,
	LPVOID *lppEnvironment, LPSTARTUPINFOW lpStartupInfo, BASE_API_MSG *m, PULONG piTask,
	PUNICODE_STRING pVdmNameString, ANSI_STRING *pAnsiStringEnv,
	PUNICODE_STRING pUnicodeStringEnv, PDWORD pVDMCreationState, PULONG pVdmBinaryType, PDWORD pbVDMCreated,
	PHANDLE pVdmWaitHandle);

typedef BOOL(BASEP_CALL *fpCreateProcessInternalW)(HANDLE hToken,
	LPCWSTR lpApplicationName,
	LPWSTR lpCommandLine,
	LPSECURITY_ATTRIBUTES lpProcessAttributes,
	LPSECURITY_ATTRIBUTES lpThreadAttributes,
	BOOL bInheritHandles,
	DWORD dwCreationFlags,
	LPVOID lpEnvironment,
	LPCWSTR lpCurrentDirectory,
	LPSTARTUPINFOW lpStartupInfo,
	LPPROCESS_INFORMATION lpProcessInformation,
	PHANDLE hNewToken
	);

typedef ULONG(BASEP_CALL *fpBaseIsDosApplication)(
	IN PUNICODE_STRING  	PathName,
	IN NTSTATUS  	Status
	);
typedef BOOL(KRNL32_CALL *fpBaseCheckVDM)(
	IN	ULONG BinaryType,
#if !(defined(TARGET_WIN80) && !defined(_WIN64))
	IN	PCWCH lpApplicationName,
#endif
	IN	PCWCH lpCommandLine,
	IN  PCWCH lpCurrentDirectory,
	IN	ANSI_STRING *pAnsiStringEnv,
	IN	BASE_API_MSG *m,
	IN OUT PULONG iTask,
	IN	DWORD dwCreationFlags,
	LPSTARTUPINFOW lpStartupInfo,
	IN HANDLE hUserToken
	);

typedef BOOL(KRNL32_CALL *fpBaseCreateVDMEnvironment)(
	LPWSTR  lpEnvironment,
	ANSI_STRING *pAStringEnv,
	UNICODE_STRING *pUStringEnv
	);
typedef BOOL (KRNL32_CALL *fpBaseGetVdmConfigInfo)(
#ifndef TARGET_WIN80
	IN  LPCWSTR CommandLine,
#endif
	IN  ULONG  DosSeqId,
	IN  ULONG  BinaryType,
	IN  PUNICODE_STRING CmdLineString
#ifdef TARGET_WIN7
	,OUT PULONG VdmSize
#endif
	);
	
// We now have https://github.com/otya128/winevdm , no more need to try to support WOW16, which didn't work anyway
//#define WOW16_SUPPORT
#if defined(WOW16_SUPPORT) || (defined(TARGET_WIN7) && !defined(CREATEPROCESS_HOOK))
typedef NTSTATUS (NTAPI *fpNtCreateUserProcess)(
	PHANDLE ProcessHandle,
	PHANDLE ThreadHandle,
	ACCESS_MASK ProcessDesiredAccess,
	ACCESS_MASK ThreadDesiredAccess,
	POBJECT_ATTRIBUTES ProcessObjectAttributes,
	POBJECT_ATTRIBUTES ThreadObjectAttributes,
	ULONG ProcessFlags,
	ULONG ThreadFlags,
	PRTL_USER_PROCESS_PARAMETERS ProcessParameters,
	PVOID CreateInfo,
	PVOID AttributeList
	);
fpNtCreateUserProcess NtCreateUserProcessReal;
NTSTATUS LastCreateUserProcessError = STATUS_SUCCESS;
NTSTATUS NTAPI NtCreateUserProcessHook(
	PHANDLE ProcessHandle,
	PHANDLE ThreadHandle,
	ACCESS_MASK ProcessDesiredAccess,
	ACCESS_MASK ThreadDesiredAccess,
	POBJECT_ATTRIBUTES ProcessObjectAttributes,
	POBJECT_ATTRIBUTES ThreadObjectAttributes,
	ULONG ProcessFlags,
	ULONG ThreadFlags,
	PRTL_USER_PROCESS_PARAMETERS ProcessParameters,
	PVOID CreateInfo,
	PVOID AttributeList
	)
{
#if defined(TARGET_WIN7) && !defined(CREATEPROCESS_HOOK)
	UpdateSymbolCache();
#endif
	LastCreateUserProcessError = NtCreateUserProcessReal(ProcessHandle,
		ThreadHandle,
		ProcessDesiredAccess,
		ThreadDesiredAccess,
		ProcessObjectAttributes,
		ThreadObjectAttributes,
		ProcessFlags,
		ThreadFlags,
		ProcessParameters,
		CreateInfo,
		AttributeList);

#ifdef WOW16_SUPPORT
	return LastCreateUserProcessError==STATUS_INVALID_IMAGE_WIN_16?STATUS_INVALID_IMAGE_PROTECT:LastCreateUserProcessError;
#else
	return LastCreateUserProcessError;
#endif
}
#endif /* defined(WOW16_SUPPORT) || (defined(TARGET_WIN7) && !defined(CREATEPROCESS_HOOK)) */

#ifdef TRACING
fpsprintf sprintf;
#endif
fp_stricmp __stricmp;
fp_wcsicmp __wcsicmp;
fpstrcmp _strcmp;
fpBasepProcessInvalidImage BasepProcessInvalidImageReal = NULL;
fpBaseIsDosApplication BaseIsDosApplication = NULL;
fpBaseCheckVDM BaseCheckVDM = NULL;
fpBaseCreateVDMEnvironment BaseCreateVDMEnvironment = NULL;
fpBaseGetVdmConfigInfo BaseGetVdmConfigInfo = NULL;
fpCreateProcessInternalW CreateProcessInternalW = NULL;

BOOL __declspec(dllexport) BASEP_CALL NtVdm64CreateProcessInternalW(HANDLE hToken,
	LPCWSTR lpApplicationName,
	LPWSTR lpCommandLine,
	LPSECURITY_ATTRIBUTES lpProcessAttributes,
	LPSECURITY_ATTRIBUTES lpThreadAttributes,
	BOOL bInheritHandles,
	DWORD dwCreationFlags,
	LPVOID lpEnvironment,
	LPCWSTR lpCurrentDirectory,
	LPSTARTUPINFOW lpStartupInfo,
	LPPROCESS_INFORMATION lpProcessInformation,
	PHANDLE hNewToken
	)
{
	if (!CreateProcessInternalW)
	{
		if (!(CreateProcessInternalW = (fpCreateProcessInternalW)GetProcAddress(GetModuleHandle(_T("kernel32.dll")), "CreateProcessInternalW")))
			return FALSE;
	}
	return CreateProcessInternalW(hToken, lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles,
		dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation, hNewToken);
}


INT_PTR BASEP_CALL BasepProcessInvalidImage(NTSTATUS Error, HANDLE TokenHandle,
	LPCWSTR dosname, LPCWSTR *lppApplicationName,
	LPCWSTR *lppCommandLine, LPCWSTR lpCurrentDirectory,
	PDWORD pdwCreationFlags, BOOL *pbInheritHandles, PUNICODE_STRING PathName, INT_PTR a10,
	LPVOID *lppEnvironment, LPSTARTUPINFOW lpStartupInfo, BASE_API_MSG *m, PULONG piTask,
	PUNICODE_STRING pVdmNameString, ANSI_STRING *pAnsiStringEnv,
	PUNICODE_STRING pUnicodeStringEnv, PDWORD pVDMCreationState, PULONG pVdmBinaryType, PDWORD pbVDMCreated,
	PHANDLE pVdmWaitHandle)
{
	INT_PTR ret;
	ULONG BinarySubType = BINARY_TYPE_DOS_EXE;

	TRACE("LDNTVDM: BasepProcessInvalidImage(%08X,'%ws');", Error, PathName->Buffer);
#ifdef WOW16_SUPPORT
	if (LastCreateUserProcessError == STATUS_INVALID_IMAGE_WIN_16 && Error == STATUS_INVALID_IMAGE_PROTECT)
	{
		TRACE("LDNTVDM: Launching Win16 application");
		Error = LastCreateUserProcessError;
	}
#endif
	if (Error == STATUS_INVALID_IMAGE_PROTECT ||
		(Error == STATUS_INVALID_IMAGE_NOT_MZ && BaseIsDosApplication &&
			(BinarySubType = BaseIsDosApplication(PathName, Error))))
	{
#ifdef WOW_HACK
		/* These flags cause kernel32.dll BasepProcessInvalidImage to launch NTVDM, even though
		* this is not a WOW program. However ntvdm needs to ignore -w switch  */
		Error = STATUS_INVALID_IMAGE_WIN_16;
		*pdwCreationFlags |= CREATE_SEPARATE_WOW_VDM;
#else
		if (!BaseCheckVDM)
		{
			// Load the private loader functions by using DbgHelp and Symbol server
			DWORD64 dwBase = 0, dwAddress;
			char szKernel32[MAX_PATH];
			HMODULE hKrnl32 = GetModuleHandle(_T("kernel32.dll"));

			GetSystemDirectoryA(szKernel32, sizeof(szKernel32) / sizeof(szKernel32[0]));
			strcat(szKernel32, "\\kernel32.dll");
			if (SymEng_LoadModule(szKernel32, &dwBase) == 0)
			{
				if ((dwAddress = SymEng_GetAddr(dwBase, "BaseCreateVDMEnvironment")) &&
					(BaseCreateVDMEnvironment = (DWORD64)hKrnl32 + dwAddress) &&
					(dwAddress = SymEng_GetAddr(dwBase, "BaseGetVdmConfigInfo")) &&
					(BaseGetVdmConfigInfo = (DWORD64)hKrnl32 + dwAddress) &&
					(dwAddress = SymEng_GetAddr(dwBase, "BaseCheckVDM")))
				{
					BaseCheckVDM = (DWORD64)hKrnl32 + dwAddress;
				}
				else
				{
					OutputDebugStringA("NTVDM: Resolving symbols failed.");
				}
				SymEng_UnloadModule(dwBase);
			}
			else
			{
				OutputDebugStringA("NTVDM: Symbol engine loading failed");
			}
			// If resolution fails, fallback to normal loader code and display error
		}
		if (BaseCheckVDM)
		{
			// Now that we have the functions, do what the loader normally does on 32bit Windows
			BASE_CHECKVDM_MSG *b = (BASE_CHECKVDM_MSG*)&m->u.CheckVDM;
			NTSTATUS Status;
#ifdef TARGET_WIN7
			ULONG VdmType;
#endif

			*pVdmBinaryType = BINARY_TYPE_DOS;
			*pVdmWaitHandle = NULL;

			if (!BaseCreateVDMEnvironment(
				*lppEnvironment,
				pAnsiStringEnv,
				pUnicodeStringEnv
				))
			{
				TRACE("LDNTVDM: BaseCreateVDMEnvironment failed");
				return FALSE;
			}

#if defined(TARGET_WIN80) && !defined(_WIN64)
			/* ARRRGH! Microsoft used compiler option for Link time optimization
			 * http://msdn.microsoft.com/en-us/library/xbf3tbeh.aspx 
			 * This means, second parameter is in ESI... *grrr*
			 */
			PCWCH ApplicationName = *lppApplicationName;
			__asm mov esi, ApplicationName
#endif
			Status = BaseCheckVDM(
				*pVdmBinaryType | BinarySubType,
#if !(defined(TARGET_WIN80) && !defined(_WIN64))
				*lppApplicationName,
#endif
				*lppCommandLine,
				lpCurrentDirectory,
				pAnsiStringEnv,
				m,
				piTask,
				*pdwCreationFlags,
				lpStartupInfo,
				TokenHandle
				);
			if (!NT_SUCCESS(Status))
			{
				SetLastError(RtlNtStatusToDosError(Status));
				TRACE("LDNTVDM: BaseCheckVDM failed, gle=%d", GetLastError());
				return FALSE;
			}

			TRACE("VDMState=%08X", b->VDMState);
			switch (b->VDMState & VDM_STATE_MASK) {
			case VDM_NOT_PRESENT:
				*pVDMCreationState = VDM_PARTIALLY_CREATED;
				if (*pdwCreationFlags & DETACHED_PROCESS)
				{
					SetLastError(ERROR_ACCESS_DENIED);
					TRACE("LDNTVDM: VDM_NOT_PRESENT -> ERROR_ACCESS_DENIED");
					return FALSE;
				}
				if (!BaseGetVdmConfigInfo(
#ifndef TARGET_WIN80
					m,
#endif
					*piTask,
					*pVdmBinaryType,
					pVdmNameString
#ifdef TARGET_WIN7
					,&VdmType
#endif
					))
				{
					SetLastError(RtlNtStatusToDosError(Status));
					OutputDebugStringA("BaseGetVdmConfigInfo failed.");
					return FALSE;
				}

				*lppCommandLine = pVdmNameString->Buffer;
				*lppApplicationName = NULL;

				break;

			case VDM_PRESENT_NOT_READY:
				TRACE("VDM_PRESENT_NOT_READY");
				SetLastError(ERROR_NOT_READY);
				return FALSE;

			case VDM_PRESENT_AND_READY:
				*pVDMCreationState = VDM_BEING_REUSED;
				*pVdmWaitHandle = b->WaitObjectForParent;
				break;
			}
			if (!*pVdmWaitHandle)
			{
				*pbInheritHandles = 0;
				*lppEnvironment = pUnicodeStringEnv->Buffer;
			}
			TRACE("LDNTVDM: Launch DOS!");
			return TRUE;
		}
	} else if (Error == STATUS_INVALID_IMAGE_WIN_16) {
		//*pdwCreationFlags |= CREATE_SEPARATE_WOW_VDM;
#endif /* WOW_HACK */
	}
	ret = BasepProcessInvalidImageReal(Error, TokenHandle, dosname, lppApplicationName, lppCommandLine, lpCurrentDirectory,
		pdwCreationFlags, pbInheritHandles, PathName, a10, lppEnvironment, lpStartupInfo, m, piTask, pVdmNameString,
		pAnsiStringEnv, pUnicodeStringEnv, pVDMCreationState, pVdmBinaryType, pbVDMCreated, pVdmWaitHandle);
#ifdef WOW_HACK
	*pdwCreationFlags &= ~CREATE_NO_WINDOW;
#endif
	TRACE("LDNTVDM: BasepProcessInvalidImage = %d", ret);

	return ret;
}


#ifdef _WIN64
BOOL FixNTDLL(void)
{
	PVOID pNtVdmControl = GetProcAddress(GetModuleHandle(_T("ntdll.dll")), "NtVdmControl");
	DWORD OldProt;

	if (VirtualProtect(pNtVdmControl, 7, PAGE_EXECUTE_READWRITE, &OldProt))
	{
		RtlMoveMemory(pNtVdmControl, "\xC6\x42\x08\x01\x33\xc0\xc3", 7);
		VirtualProtect(pNtVdmControl, 7, OldProt, &OldProt);
		return TRUE;
	}
	return FALSE;
}

typedef PVOID(WINAPI *RtlAllocateHeapFunc)(PVOID  HeapHandle, ULONG  Flags, SIZE_T Size);
RtlAllocateHeapFunc RtlAllocateHeapReal;
PVOID WINAPI RtlAllocateHeapHook(PVOID  HeapHandle, ULONG  Flags, SIZE_T Size)
{
#ifdef TARGET_WIN7
	if (Size == 0x130) /* Any better idea to find correct call? */ Flags |= HEAP_ZERO_MEMORY;
#else
	if (Size == 0x150) /* Any better idea to find correct call? */ Flags |= HEAP_ZERO_MEMORY;
#endif
	return RtlAllocateHeapReal(HeapHandle, Flags, Size);
}

typedef HMODULE(WINAPI *LoadLibraryExWFunc)(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags);
LoadLibraryExWFunc LoadLibraryExWReal;
HMODULE WINAPI LoadLibraryExWHook(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags)
{
	HANDLE hRet = LoadLibraryExW(lpLibFileName, hFile, dwFlags);

	TRACE("LDNTVDM: LoadLibraryExWHook(%S)", lpLibFileName);
	if (hRet && __wcsicmp(lpLibFileName, L"ConHostV1.dll") == 0)
	{
		TRACE("LDNTVDM hooks Conhost RtlAllocateHeap");
		Hook_IAT_x64_IAT((LPBYTE)hRet, "ntdll.dll", "RtlAllocateHeap", RtlAllocateHeapHook, &RtlAllocateHeapReal);
	}
	return hRet;
}

#define myNtQueryInformationProcess NtQueryInformationProcess

#define SUBSYS_OFFSET 0x128
#else	// _WIN32
#define SUBSYS_OFFSET 0xB4

NTSTATUS
NTAPI
myNtQueryInformationProcess(
	IN HANDLE ProcessHandle,
	IN PROCESSINFOCLASS ProcessInformationClass,
	OUT PVOID ProcessInformation,
	IN ULONG ProcessInformationLength,
	OUT PULONG ReturnLength OPTIONAL
	)
{
	static ULONGLONG ntqip = 0;

	if (ProcessInformationClass == ProcessConsoleHostProcess)
	{
		ULONGLONG ret = 0, status, ProcessInformation64 = 0;

		if (0 == ntqip)
			ntqip = GetProcAddress64(getNTDLL64(), "NtQueryInformationProcess");
		if (ntqip)
		{
			status = X64Call(ntqip, 5, (ULONGLONG)-1, (ULONGLONG)ProcessConsoleHostProcess, (ULONGLONG)&ProcessInformation64, (ULONGLONG)sizeof(ProcessInformation64), (ULONGLONG)NULL);
			if (NT_SUCCESS(status))
			{
				*((ULONG*)ProcessInformation) = (ULONG)ProcessInformation64;
				if (ReturnLength) *ReturnLength = sizeof(ULONG);
			}
			return status;
		}
	}
	return NtQueryInformationProcess(ProcessHandle, ProcessInformationClass, ProcessInformation, ProcessInformationLength, ReturnLength);
}

BOOL FixNTDLL(void)
{
	PVOID pNtVdmControl = GetProcAddress(GetModuleHandle(_T("ntdll.dll")), "NtVdmControl");
	DWORD OldProt;

	if (VirtualProtect(pNtVdmControl, 13, PAGE_EXECUTE_READWRITE, &OldProt))
	{
		RtlMoveMemory(pNtVdmControl, "\x8B\x54\x24\x08\xC6\x42\x04\x01\x33\xc0\xc2\x08\x00", 13);
		VirtualProtect(pNtVdmControl, 13, OldProt, &OldProt);
		return TRUE;
	}
	return FALSE;
}
#endif	// _WIN32

#ifdef CREATEPROCESS_HOOK
BOOL InjectIntoCreatedThread(LPPROCESS_INFORMATION lpProcessInformation)
{
	PROCESS_BASIC_INFORMATION basicInfo;
	ULONG ImageSubsystem;
	LPTHREAD_START_ROUTINE pLoadLibraryW;
	DWORD result;
	BOOL bIsWow64 = FALSE, bRet = FALSE;

	if (!NT_SUCCESS(NtQueryInformationProcess(lpProcessInformation->hProcess, ProcessBasicInformation, &basicInfo, sizeof(basicInfo), NULL))) return FALSE;
	ReadProcessMemory(lpProcessInformation->hProcess, (PVOID)((ULONG_PTR)basicInfo.PebBaseAddress + SUBSYS_OFFSET), &ImageSubsystem, sizeof(ImageSubsystem), &result);
	if (ImageSubsystem == IMAGE_SUBSYSTEM_WINDOWS_CUI)
	{
		// Commandline application, inject DLL 
		IsWow64Process(lpProcessInformation->hProcess, &bIsWow64);
#ifdef _WIN64
		/* 64 -> 32 */
		/* To inject  into WOW64 process, unfortunately we have to wait until process becomes ready, otherwise
		Ldr is NULL and we cannot find LoadLibraryW entry point in target process
		*/
		if (bIsWow64) CreateThread(NULL, 0, InjectLdntvdmWow64Thread, basicInfo.UniqueProcessId, 0, NULL);
		/*
		if (pLoadLibraryW = (LPTHREAD_START_ROUTINE)GetLoadLibraryAddressX32(lpProcessInformation->hProcess))
		{
			PBYTE *pLibRemote;
			if (pLibRemote = VirtualAllocEx(lpProcessInformation->hProcess, NULL, sizeof(LDNTVDM_NAME), MEM_COMMIT, PAGE_READWRITE))
			{
				HANDLE hThread;

				WriteProcessMemory(lpProcessInformation->hProcess, pLibRemote, (void*)LDNTVDM_NAME, sizeof(LDNTVDM_NAME), NULL);
				bRet = (hThread = CreateRemoteThread(lpProcessInformation->hProcess, NULL, 0, pLoadLibraryW, pLibRemote, 0, NULL))!=0;
				if (hThread) WaitForSingleObject(hThread, INFINITE);
				VirtualFreeEx(lpProcessInformation->hProcess, pLibRemote, 0, MEM_RELEASE);
			}

		}
		*/
#else
		if (!bIsWow64)
		{
			WCHAR szDLL[256];

			/* 32 -> 64 */
			GetSystemDirectoryW(szDLL, sizeof(szDLL) / sizeof(WCHAR));
			wcscat(szDLL, L"\\"LDNTVDM_NAME);
			if (!(bRet = inject_dll_x64(lpProcessInformation->hProcess, szDLL)))
			{
				OutputDebugStringA("Injecting 64bit DLL from 32bit failed");
			}
		}
#endif
		/* 64 -> 64, 32 -> 32 */
		else {
			if (!(bRet = injectLdrLoadDLL(lpProcessInformation->hProcess, lpProcessInformation->hThread, LDNTVDM_NAME, METHOD_CREATETHREAD))) OutputDebugStringA("Inject LdrLoadDLL failed.");
		}
	}
	return bRet;
}
#else /* CREATEPROCESS_HOOK */
#ifdef _WIN64
typedef struct {
	HANDLE hProcess;
	HANDLE hThread;
} INJECTOR_PARAM;

DWORD WINAPI InjectIntoCreatedThreadThread(INJECTOR_PARAM *param)
{
	BOOL bIsWow64;
	
	IsWow64Process(param->hProcess, &bIsWow64);
	/* 64 -> 32 */
	if (bIsWow64)
	{
		if (isProcessInitialized(param->hProcess) || !InjectDllHijackThreadX32(param->hProcess, param->hThread, LDNTVDM_NAME))
		{
			HANDLE hThread;
			
			if (!(hThread = InjectLdntvdmWow64RemoteThread(param->hProcess)))
			{
				OutputDebugStringA("Injecting int 32bit conhost parent failed");
				ResumeThread(param->hThread);
			}
			else
			{
				int prio = GetThreadPriority(param->hThread);
				SetThreadPriority(param->hThread, THREAD_PRIORITY_LOWEST);
				SetThreadPriority(hThread, THREAD_PRIORITY_TIME_CRITICAL);
				ResumeThread(param->hThread);
				WaitForSingleObject(hThread, 1000);
				SetThreadPriority(param->hThread, prio);
			}
		} else ResumeThread(param->hThread);
	}
	/* 64 -> 64, 32 -> 32 */
	else
	{
		if (!(injectLdrLoadDLL(param->hProcess, param->hThread, LDNTVDM_NAME, METHOD_INTERCEPTTHREAD))) OutputDebugStringA("Inject LdrLoadDLL failed.");
		ResumeThread(param->hThread);
	}
	CloseHandle(param->hProcess);
	HeapFree(GetProcessHeap(), 0, param);
}

typedef void (WINAPI *fpNotifyWinEvent)(DWORD event, HWND hwnd, LONG idObject, LONG idChild);
fpNotifyWinEvent NotifyWinEventReal;
void WINAPI NotifyWinEventHook(DWORD event, HWND  hwnd, LONG  idObject, LONG  idChild)
{
	if (event == EVENT_CONSOLE_START_APPLICATION)
	{
		HANDLE hProcess;
		TRACE("EVENT_CONSOLE_START_APPLICATION PID %d", idObject);
		// First freeze the process, don't lose time or it may be too late
		if (hProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION, FALSE, idObject))
		{
			NTSTATUS Status;
			HANDLE hThread;

			if (!NT_SUCCESS(Status = NtGetNextThread(hProcess, NULL, THREAD_GET_CONTEXT | THREAD_SET_CONTEXT | THREAD_SUSPEND_RESUME | THREAD_QUERY_INFORMATION | THREAD_SET_INFORMATION, 0, 0, &hThread)))
			{
				TRACE("NtGetNextThread failed: %08X", Status);
				CloseHandle(hProcess);
			}
			else
			{
				// Inject ourself into the new process
				INJECTOR_PARAM *param = HeapAlloc(GetProcessHeap(), 0, sizeof(INJECTOR_PARAM));
				param->hProcess = hProcess;
				param->hThread = hThread;
				SuspendThread(hThread);
				TRACE("Before CreatEThread");
				CreateThread(NULL, 0, InjectIntoCreatedThreadThread, param, 0, NULL);
				TRACE("After CreatEThread");
			}
		}
	}
	NotifyWinEventReal(event, hwnd, idObject, idChild);
}
#endif /* _WIN64 */
#endif /* CREATEPROCESS_HOOK*/

#ifdef TARGET_WIN7
#ifdef _WIN64
#define REGKEY_BasepProcessInvalidImage L"BasepProcessInvalidImage64"
#define REGKEY_BaseIsDosApplication L"BaseIsDosApplication64"
#else
#define REGKEY_BasepProcessInvalidImage L"BasepProcessInvalidImage32"
#define REGKEY_BaseIsDosApplication L"BaseIsDosApplication32"
#endif
typedef struct {
	char *pszFunction;
	LPWSTR lpKeyName;
} REGKEY_PAIR;
static BOOL UpdateSymsForModule(HKEY hKey, char *pszDLL, LPWSTR lpDLLKey, REGKEY_PAIR *keys)
{
	char szKernel32[MAX_PATH];
	DWORD dwAddress, i;
	DWORD64 dwBase = 0;
	FILETIME tm = { 0 };
	HANDLE hFile;

	sprintf(szKernel32 + GetSystemDirectoryA(szKernel32, sizeof(szKernel32) / sizeof(szKernel32[0])), "\\%s", pszDLL);
	hFile = CreateFileA(szKernel32, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		ULONGLONG tmr;

		if (GetFileTime(hFile, NULL, NULL, &tm) &&
			NT_SUCCESS(REG_QueryQWORD(hKey, lpDLLKey, &tmr)) && *((PULONGLONG)&tm) == tmr)
		{
			// So far, it seems unchanged, now check if exports are present
			for (i = 0; keys[i].lpKeyName; i++)
			{
				if (!NT_SUCCESS(REG_QueryDWORD(hKey, keys[i].lpKeyName, &dwAddress)) || !dwAddress) break;
			}
			if (!keys[i].lpKeyName) return TRUE; // No update needed, everything fine
		}
		CloseHandle(hFile);
	}

	// Update needed
	if (SymEng_LoadModule(szKernel32, &dwBase) == 0 || GetLastError() == 0x1E7)
	{
		if (!dwBase) dwBase = GetModuleHandleA(pszDLL);
		TRACE("UpdateSymbolCache() loading %s symbols", pszDLL);
		for (i = 0; keys[i].lpKeyName; i++)
		{
			if (dwAddress = SymEng_GetAddr(dwBase, keys[i].pszFunction))
				REG_SetDWORD(hKey, keys[i].lpKeyName, dwAddress);
		}
		SymEng_UnloadModule(dwBase);
		if (tm.dwLowDateTime)
		{
			REG_SetQWORD(hKey, lpDLLKey, *((PULONGLONG)&tm));
		}
		return TRUE;
	}
	return FALSE;
}

BOOL UpdateSymbolCache()
{
	HKEY hKey = NULL;
	DWORD64 dwBase=0;
	DWORD dwAddress;
	NTSTATUS Status;
	char szKernel32[MAX_PATH];
	static BOOL bUpdated = FALSE;

	if (bUpdated) return TRUE;
	if (NT_SUCCESS(Status = REG_OpenLDNTVDM(KEY_READ | KEY_WRITE, &hKey)))
	{
		{
			REGKEY_PAIR Keys[] = {
				{"BasepProcessInvalidImage", REGKEY_BasepProcessInvalidImage},
				{"BaseIsDosApplication", REGKEY_BaseIsDosApplication},
				{NULL, NULL}
			};
			bUpdated = UpdateSymsForModule(hKey, "kernel32.dll", L"kernel32.dll", Keys);
		}

		{
			REGKEY_PAIR Keys[] = {
				{"dwConBaseTag", L"dwConBaseTag"},
				{"FindProcessInList", L"FindProcessInList"},
				{"CreateConsoleBitmap",  L"CreateConsoleBitmap"},
				{ NULL, NULL }
			};
			bUpdated &= UpdateSymsForModule(hKey, "conhost.exe", L"conhost.exe", Keys);
		}
		REG_CloseKey(hKey);
	}
	else
	{
		TRACE("RegCreateKeyEx failed: %08X", Status);
	}

	return bUpdated;
}
#endif /* TARGET_WIN7 */

#ifdef CREATEPROCESS_HOOK
typedef BOOL(WINAPI *CreateProcessAFunc)(LPCSTR lpApplicationName, LPSTR lpCommandLine, 
	LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes, 
	BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCSTR lpCurrentDirectory, 
	LPSTARTUPINFOA lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation);
CreateProcessAFunc CreateProcessAReal;
BOOL WINAPI CreateProcessAHook(LPCSTR lpApplicationName, LPSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes, 
	LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, 
	LPCSTR lpCurrentDirectory, LPSTARTUPINFOA lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation)
{
	BOOL bRet;
	DWORD dwDebugged = (dwCreationFlags&DEBUG_PROCESS) ?  0 : CREATE_SUSPENDED;

#ifdef TARGET_WIN7
	UpdateSymbolCache();
#endif
	bRet = CreateProcessAReal(lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, 
		bInheritHandles, dwCreationFlags | dwDebugged, lpEnvironment, lpCurrentDirectory, lpStartupInfo,
		lpProcessInformation);
	if (dwDebugged && bRet && lpProcessInformation->hThread)
	{
		InjectIntoCreatedThread(lpProcessInformation);
		if (!(dwCreationFlags & CREATE_SUSPENDED)) ResumeThread(lpProcessInformation->hThread);
	}
	return bRet;
}

typedef BOOL(WINAPI *CreateProcessWFunc)(LPCWSTR lpApplicationName, LPWSTR lpCommandLine, 
	LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles, 
	DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory, LPSTARTUPINFOW lpStartupInfo, 
	LPPROCESS_INFORMATION lpProcessInformation);
CreateProcessWFunc CreateProcessWReal;
BOOL WINAPI CreateProcessWHook(LPCWSTR lpApplicationName, LPWSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes,
	LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment,
	LPCWSTR lpCurrentDirectory, LPSTARTUPINFOW lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation)
{
	BOOL bRet;
	DWORD dwDebugged = (dwCreationFlags&DEBUG_PROCESS) ? 0 : CREATE_SUSPENDED;

#ifdef TARGET_WIN7
	UpdateSymbolCache();
#endif
	bRet = CreateProcessWReal(lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes,
		bInheritHandles, dwCreationFlags | dwDebugged, lpEnvironment, lpCurrentDirectory, lpStartupInfo,
		lpProcessInformation);

	if (dwDebugged && bRet && lpProcessInformation->hThread)
	{
		InjectIntoCreatedThread(lpProcessInformation);
		if (!(dwCreationFlags & CREATE_SUSPENDED)) ResumeThread(lpProcessInformation->hThread);
	}
	return bRet;
}
#endif /* CREATEPROCESS_HOOK */

typedef BOOL(WINAPI *fpSetConsolePalette)(IN HANDLE hConsoleOutput, IN HPALETTE hPalette, IN UINT dwUsage);
fpSetConsolePalette SetConsolePaletteReal;
typedef BOOL (WINAPI *fpOpenClipboard)(IN HWND hWndNewOwner);
fpOpenClipboard pOpenClipboard = NULL;
typedef HANDLE (WINAPI *fpSetClipboardData)(IN UINT uFormat, IN HANDLE hMem);
fpSetClipboardData pSetClipboardData = NULL;
typedef BOOL (WINAPI *fpCloseClipboard)(VOID);
fpCloseClipboard pCloseClipboard = NULL;

BOOL WINAPI mySetConsolePalette(IN HANDLE hConsoleOutput, IN HPALETTE hPalette, IN UINT dwUsage)
{
	/* This dirty hack via clipboard causes hPalette to become public (GreSetPaletteOwner(hPalette, OBJECT_OWNER_PUBLIC))
 	 * as Microsoft seemes to have removed NtUserConsoleControl(ConsolePublicPalette, ..) call from kernel :( 
	 * For the record, ConsoleControl() is a function located in USER32.DLL that could be used, but as said,
	 * ConsolePublicPalette call isn't implemented anymore in WIN32k, another stupidity by our friends at M$...
	 */
	BOOL bRet;

	// Avoid USER32.DLL dependency of loader by manually loading functions
	if (!pOpenClipboard)
	{
		HMODULE hUser32 = GetModuleHandle(_T("user32.dll"));
		if (!hUser32 && !(hUser32 = LoadLibrary(_T("user32.dll"))))
		{
			TRACE("Loadlibrary user32.dll fail");
			return SetConsolePaletteReal(hConsoleOutput, hPalette, dwUsage);
		}
		pOpenClipboard = GetProcAddress(hUser32, "OpenClipboard");
		pSetClipboardData = GetProcAddress(hUser32, "SetClipboardData");
		pCloseClipboard = GetProcAddress(hUser32, "CloseClipboard");
	}
	pOpenClipboard(NULL);
	pSetClipboardData(CF_PALETTE, hPalette);
	pCloseClipboard();

	bRet = SetConsolePaletteReal(hConsoleOutput, hPalette, dwUsage);
	//TRACE("SetConsolePalette = %d", bRet);
	return bRet;
}

TCHAR *GetProcessName(void)
{
	static TCHAR szProcess[MAX_PATH], *p;

	p = szProcess + GetModuleFileName(NULL, szProcess, sizeof(szProcess) / sizeof(TCHAR));
	while (*p != '\\') p--;
	return ++p;
}

#if !defined(TARGET_WIN7) && !defined(TARGET_WIN80)
HANDLE GetConsoleHost(void)
{
	static ULONG_PTR hConHost = NULL;

	if (!hConHost)
	{
		if (myNtQueryInformationProcess(-1, ProcessConsoleHostProcess, &hConHost, sizeof(hConHost), NULL) != STATUS_SUCCESS)
			return -1;
		if (hConHost & 1) hConHost &= ~1; else hConHost = 0;
	}
	return hConHost;
}

#ifdef _WIN64
NTSTATUS NTAPI CsrClientCallServer(BASE_API_MSG *m, PCSR_CAPTURE_HEADER CaptureHeader, CSR_API_NUMBER ApiNumber, ULONG ArgLength);
#define myCsrClientCallServer CsrClientCallServer
#else
NTSTATUS NTAPI myCsrClientCallServer(BASE_API_MSG *m, PCSR_CAPTURE_HEADER CaptureHeader, CSR_API_NUMBER ApiNumber, ULONG ArgLength);
#endif

VOID APIENTRY myCmdBatNotification(IN ULONG fBeginEnd)
{
	BASE_API_MSG m;
	BASE_BAT_NOTIFICATION_MSG *a = (BASE_BAT_NOTIFICATION_MSG*)&m.u.BatNotification;

	a->ConsoleHandle = GetConsoleHost();
	if (a->ConsoleHandle == (HANDLE)-1)
		return;

	a->fBeginEnd = fBeginEnd;
	myCsrClientCallServer((struct _CSR_API_MESSAGE *)&m, NULL, CSR_MAKE_API_NUMBER(BASESRV_SERVERDLL_INDEX, 
		BasepBatNotification),	sizeof(*a));
}
#endif /* Win10 only */


#ifndef _WIN64
void NTAPI HookSetConsolePaletteAPC(ULONG_PTR Parameter)
{
	static int iTries = 0;
	if (Hook_IAT_x64_IAT(GetModuleHandle(NULL), "KERNEL32.DLL", "SetConsolePalette", mySetConsolePalette, &SetConsolePaletteReal) == -3)
	{
		HANDLE hThread;
		if (iTries >= 10) return;

		// Queue APC to main thread and try again
		if (hThread = OpenThread(THREAD_SET_CONTEXT, FALSE, *((DWORD*)(__readfsdword(0x18) + 36))))
		{
			if (!QueueUserAPC(HookSetConsolePaletteAPC, hThread, NULL))
			{
				TRACE("QueueUserAPC failed gle=%d", GetLastError());
			}
			CloseHandle(hThread);
		}
		else { TRACE("OpenThread error %d", GetLastError()); }
		iTries++;
	}
}
#endif /* WIN32 */

BOOL WINAPI _DllMainCRTStartup(
	HANDLE  hDllHandle,
	DWORD   dwReason,
	LPVOID  lpreserved
	)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
	{
		STARTUPINFO si = { 0 };
		HMODULE hKernelBase, hKrnl32, hNTDLL = GetModuleHandle(_T("ntdll.dll"));
		LPBYTE lpProcII = NULL;
		int i;

		hKrnl32 = GetModuleHandle(_T("kernel32.dll"));
		hKernelBase = (HMODULE)GetModuleHandle(_T("KernelBase.dll"));
		__stricmp = (fp_stricmp)GetProcAddress(hNTDLL, "_stricmp");
		__wcsicmp = (fp_wcsicmp)GetProcAddress(hNTDLL, "_wcsicmp");
		_strcmp = (fpstrcmp)GetProcAddress(hNTDLL, "strcmp");
#ifdef TRACING
		sprintf = (fpsprintf)GetProcAddress(hNTDLL, "sprintf");
#endif

#ifdef TARGET_WIN7
		{
/*
			// Windows 7, all the stuff is internal in kernel32.dll :(
			// But we cannot use the symbol loader in the module loading routine
			DWORD64 dwBase=0, dwAddress;
			char szKernel32[MAX_PATH];
			GetSystemDirectoryA(szKernel32, sizeof(szKernel32) / sizeof(szKernel32[0]));
			strcat(szKernel32, "\\kernel32.dll");
			if (SymEng_LoadModule(szKernel32, &dwBase) == 0)
			{
				if (dwAddress = SymEng_GetAddr(dwBase, "BasepProcessInvalidImage"))
				{
					lpProcII = (DWORD64)hKrnl32 + dwAddress;
					BasepProcessInvalidImageReal = (fpBasepProcessInvalidImage)Hook_Inline(lpProcII, BasepProcessInvalidImage, 10);
				}
				if (dwAddress = SymEng_GetAddr(dwBase, "BaseIsDosApplication"))
					BaseIsDosApplication = (DWORD64)hKrnl32 + dwAddress;
			}
*/
			NTSTATUS Status;
			DWORD dwAddress;
			HKEY hKey;

			if (NT_SUCCESS(Status = REG_OpenLDNTVDM(KEY_READ, &hKey)))
			{
				if (NT_SUCCESS(Status = REG_QueryDWORD(hKey, REGKEY_BasepProcessInvalidImage, &dwAddress)))
				{
					lpProcII = (DWORD64)hKrnl32 + dwAddress;
					BasepProcessInvalidImageReal = (fpBasepProcessInvalidImage)Hook_Inline(lpProcII, BasepProcessInvalidImage);
				}
				else
				{
					TRACE("RegQueryValueEx 1 failed: %08X", Status);
				}
				if (NT_SUCCESS(Status = REG_QueryDWORD(hKey, REGKEY_BaseIsDosApplication, &dwAddress)))
					BaseIsDosApplication = (DWORD64)hKrnl32 + dwAddress;
				else
				{
					TRACE("RegQueryValueEx 2 failed: %08X", Status);
				}
				REG_CloseKey(hKey);
			}
			else
			{
				TRACE("RegOpenKey failed: %08X", Status);
			}


#ifdef CREATEPROCESS_HOOK
			CreateProcessAReal = Hook_Inline(CreateProcessA, CreateProcessAHook);
			CreateProcessWReal = Hook_Inline(CreateProcessW, CreateProcessWHook);
#endif
		}
#if defined(WOW16_SUPPORT) || (defined(TARGET_WIN7) && !defined(CREATEPROCESS_HOOK))
		Hook_IAT_x64_IAT((LPBYTE)hKrnl32, "ntdll.dll", "NtCreateUserProcess", NtCreateUserProcessHook, &NtCreateUserProcessReal);
#endif	// WOW16_SUPPORT

#else	// TARGET_WIN7
		// Windows 10
		lpProcII = (LPBYTE)GetProcAddress(hKrnl32, "BasepProcessInvalidImage");
		BasepProcessInvalidImageReal = (fpBasepProcessInvalidImage)lpProcII;
		BaseIsDosApplication = GetProcAddress(hKrnl32, "BaseIsDosApplication");
		Hook_IAT_x64((LPBYTE)hKernelBase, "ext-ms-win-kernelbase-processthread-l1-1-0.dll",
			"BasepProcessInvalidImage", BasepProcessInvalidImage);
#ifndef TARGET_WIN80
		// These idiots recently replaced CmdBatNotification function with a nullstub?!?
		if (__wcsicmp(GetProcessName(), _T("cmd.exe")) == 0)
			Hook_IAT_x64((LPBYTE)GetModuleHandle(NULL), "ext-ms-win-cmd-util-l1-1-0.dll", "CmdBatNotificationStub", myCmdBatNotification, NULL);
#endif
#ifdef CREATEPROCESS_HOOK
		/* Newer Windows Versions use l1-1-0 instead of l1-1-2 */
		if (Hook_IAT_x64_IAT((LPBYTE)hKrnl32, "api-ms-win-core-processthreads-l1-1-2.dll", "CreateProcessA", CreateProcessAHook, &CreateProcessAReal)<0)
			Hook_IAT_x64_IAT((LPBYTE)hKrnl32, "api-ms-win-core-processthreads-l1-1-0.dll", "CreateProcessA", CreateProcessAHook, &CreateProcessAReal);
		if (Hook_IAT_x64_IAT((LPBYTE)hKrnl32, "api-ms-win-core-processthreads-l1-1-2.dll", "CreateProcessW", CreateProcessWHook, &CreateProcessWReal)<0)
			Hook_IAT_x64_IAT((LPBYTE)hKrnl32, "api-ms-win-core-processthreads-l1-1-0.dll", "CreateProcessW", CreateProcessWHook, &CreateProcessWReal);
#endif
#ifdef WOW16_SUPPORT
		Hook_IAT_x64_IAT((LPBYTE)hKernelBase, "ntdll.dll", "NtCreateUserProcess", NtCreateUserProcessHook, &NtCreateUserProcessReal);
#endif	// WOW16_SUPPORT
#endif /* TARGET_WIN7 */
		TRACE("LDNTVDM: BasepProcessInvalidImageReal = %08X", BasepProcessInvalidImageReal);
		TRACE("LDNTVDM: BaseIsDosApplication = %08X", BaseIsDosApplication);

#ifdef _WIN64
		{
			BOOL bIsWindows8 = FALSE;
			// Fix ConhostV1.dll bug where memory isn't initialized properly
			if (__wcsicmp(GetProcessName(), _T("ConHost.exe")) == 0)
			{
				TRACE("LDNTVDM is running inside ConHost.exe");

				FixNTDLL();
#ifdef TARGET_WIN7
				ConsBmp_Install();
				Hook_IAT_x64_IAT((LPBYTE)GetModuleHandle(NULL), "ntdll.dll", "RtlAllocateHeap", RtlAllocateHeapHook, &RtlAllocateHeapReal);
#else
				if (hKrnl32 = GetModuleHandle(_T("ConHostV1.dll")))
					Hook_IAT_x64_IAT((LPBYTE)hKrnl32, "ntdll.dll", "RtlAllocateHeap", RtlAllocateHeapHook, &RtlAllocateHeapReal);
				else
				{
					char szFile[MAX_PATH];

					GetSystemDirectoryA(szFile, sizeof(szFile) / sizeof(szFile[0]));
					strcat(szFile, "\\ConhostV1.dll");
					if (bIsWindows8 = GetFileAttributesA(szFile) == 0xFFFFFFFF) // Windows 8
						Hook_IAT_x64_IAT((LPBYTE)GetModuleHandle(NULL), "ntdll.dll", "RtlAllocateHeap", RtlAllocateHeapHook, &RtlAllocateHeapReal); 
					else
						Hook_IAT_x64_IAT((LPBYTE)GetModuleHandle(NULL), "api-ms-win-core-libraryloader-l1-2-0.dll", "LoadLibraryExW", LoadLibraryExWHook, &LoadLibraryExWReal);
				}
#endif /* TARGET_WIN7 */
#ifndef CREATEPROCESS_HOOK
				// We want notification when new console process gets started so that we can inject
#ifdef TARGET_WIN7
				Hook_IAT_x64_IAT((LPBYTE)GetModuleHandle(NULL), "user32.dll", "NotifyWinEvent", NotifyWinEventHook, &NotifyWinEventReal);
#else
				if (bIsWindows8) Hook_IAT_x64_IAT((LPBYTE)GetModuleHandle(NULL), "user32.dll", "NotifyWinEvent", NotifyWinEventHook, &NotifyWinEventReal); else
				if (hKrnl32) Hook_IAT_x64_IAT((LPBYTE)hKrnl32, "user32.dll", "NotifyWinEvent", NotifyWinEventHook, &NotifyWinEventReal);
#endif
#endif /* CREATEPROCESS_HOOK */
			}
		}
#else /* WIN64 */
#ifndef WOW16_SUPPORT
		// Only fix NTDLL in ntvdm.exe where it is needed, don't interfere with other applications like ovdm, until we natively support Win16
		if (__wcsicmp(GetProcessName(), _T("ntvdm.exe")) == 0)
#endif /* WOW16_SUPPORT */
		FixNTDLL();
		HookCsrClientCallServer();
		/* SetConsolePalette bug */
		HookSetConsolePaletteAPC(NULL);
		Hook_IAT_x64_IAT((LPBYTE)hKrnl32, "ntdll.dll", "NtQueryInformationProcess", myNtQueryInformationProcess, NULL);
		Hook_IAT_x64_IAT((LPBYTE)hKernelBase, "ntdll.dll", "NtQueryInformationProcess", myNtQueryInformationProcess, NULL);
#endif /* WIN64 */
		HookExtractIcon();
		TRACE("ldntvdm Init done");
		break;
	}
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}