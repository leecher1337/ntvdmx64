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
#include <stdio.h>
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

#pragma comment(lib, "ntdll.lib")

//#define TRACING

#ifdef TRACING
#define TRACE(...) { char szDbgBuf[2048]; wsprintfA(szDbgBuf, __VA_ARGS__); OutputDebugStringA(szDbgBuf); }
#else
#define TRACE(...)
#endif

#ifdef _WIN64
#define BASEP_CALL __fastcall
#else
#define BASEP_CALL WINAPI
#endif

typedef INT_PTR(BASEP_CALL *fpBasepProcessInvalidImage)(NTSTATUS Error, HANDLE TokenHandle,
	LPCWSTR dosname, LPCWSTR *lppApplicationName,
	LPCWSTR *lppCommandLine, LPCWSTR lpCurrentDirectory,
	PDWORD pdwCreationFlags, BOOL *pbInheritHandles, PUNICODE_STRING PathName, INT_PTR a10,
	LPVOID *lppEnvironment, LPSTARTUPINFOW lpStartupInfo, BASE_API_MSG *m, PULONG piTask,
	PUNICODE_STRING pVdmNameString, ANSI_STRING *pAnsiStringEnv,
	PUNICODE_STRING pUnicodeStringEnv, PDWORD pVDMCreationState, PULONG pVdmBinaryType, PDWORD pbVDMCreated,
	PHANDLE pVdmWaitHandle);


typedef ULONG(BASEP_CALL *fpBaseIsDosApplication)(
	IN PUNICODE_STRING  	PathName,
	IN NTSTATUS  	Status
	);
typedef BOOL(__fastcall *fpBaseCheckVDM)(
	IN	ULONG BinaryType,
	IN	PCWCH lpApplicationName,
	IN	PCWCH lpCommandLine,
	IN  PCWCH lpCurrentDirectory,
	IN	ANSI_STRING *pAnsiStringEnv,
	IN	BASE_API_MSG *m,
	IN OUT PULONG iTask,
	IN	DWORD dwCreationFlags,
	LPSTARTUPINFOW lpStartupInfo,
	IN HANDLE hUserToken
	);
typedef BOOL(__fastcall *fpBaseCreateVDMEnvironment)(
	LPWSTR  lpEnvironment,
	ANSI_STRING *pAStringEnv,
	UNICODE_STRING *pUStringEnv
	);
typedef BOOL (__fastcall *fpBaseGetVdmConfigInfo)(
	IN  LPCWSTR CommandLine,
	IN  ULONG  DosSeqId,
	IN  ULONG  BinaryType,
	IN  PUNICODE_STRING CmdLineString
	);


fpBasepProcessInvalidImage BasepProcessInvalidImageReal;
fpBaseIsDosApplication BaseIsDosApplication = NULL;
fpBaseCheckVDM BaseCheckVDM = NULL;
fpBaseCreateVDMEnvironment BaseCreateVDMEnvironment = NULL;
fpBaseGetVdmConfigInfo BaseGetVdmConfigInfo = NULL;

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
			DWORD64 dwBase, dwAddress;
			char szKernel32[MAX_PATH];
			HMODULE hKrnl32 = GetModuleHandle(_T("kernel32.dll"));

			GetSystemDirectoryA(szKernel32, sizeof(szKernel32) / sizeof(szKernel32[0]));
			lstrcatA(szKernel32, "\\kernel32.dll");
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

			Status = BaseCheckVDM(*pVdmBinaryType | BinarySubType,
				*lppApplicationName,
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
				if (!BaseGetVdmConfigInfo(m,
					*piTask,
					*pVdmBinaryType,
					pVdmNameString
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
#endif
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
	if (Size == 0x150) /* Any better idea to find correct call? */ Flags |= HEAP_ZERO_MEMORY;
	return RtlAllocateHeapReal(HeapHandle, Flags, Size);
}

typedef HMODULE(WINAPI *LoadLibraryExWFunc)(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags);
LoadLibraryExWFunc LoadLibraryExWReal;
HMODULE WINAPI LoadLibraryExWHook(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags)
{
	HANDLE hRet = LoadLibraryExW(lpLibFileName, hFile, dwFlags);

	TRACE("LDNTVDM: LoadLibraryExWHook(%S)", lpLibFileName);
	if (hRet && lstrcmpiW(lpLibFileName, L"ConHostV1.dll") == 0)
	{
		TRACE("LDNTVDM hooks Conhost RtlAllocateHeap");
		Hook_IAT_x64_IAT((LPBYTE)hRet, "ntdll.dll", "RtlAllocateHeap", RtlAllocateHeapHook, &RtlAllocateHeapReal);
	}
	return hRet;
}

#define SUBSYS_OFFSET 0x128
#else	// _WIN32
#define SUBSYS_OFFSET 0xB4

#define ProcessConsoleHostProcess 49

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
#endif	// _WIN32

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

	bRet = CreateProcessAReal(lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, 
		bInheritHandles, dwCreationFlags | CREATE_SUSPENDED, lpEnvironment, lpCurrentDirectory, lpStartupInfo, 
		lpProcessInformation);
	if (bRet && lpProcessInformation->hThread)
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

	bRet = CreateProcessWReal(lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes,
		bInheritHandles, dwCreationFlags | CREATE_SUSPENDED, lpEnvironment, lpCurrentDirectory, lpStartupInfo,
		lpProcessInformation);

	if (bRet && lpProcessInformation->hThread)
	{
		InjectIntoCreatedThread(lpProcessInformation);
		if (!(dwCreationFlags & CREATE_SUSPENDED)) ResumeThread(lpProcessInformation->hThread);
	}
	return bRet;
}

typedef BOOL(WINAPI *fpSetConsolePalette)(IN HANDLE hConsoleOutput, IN HPALETTE hPalette, IN UINT dwUsage);
fpSetConsolePalette SetConsolePaletteReal;
BOOL WINAPI mySetConsolePalette(IN HANDLE hConsoleOutput, IN HPALETTE hPalette, IN UINT dwUsage)
{
	/* This dirty hack via clipboard causes hPalette to become public (GreSetPaletteOwner(hPalette, OBJECT_OWNER_PUBLIC))
 	 * as Microsoft seemes to have removed NtUserConsoleControl(ConsolePublicPalette, ..) call from kernel :( 
	 * For the record, ConsoleControl() is a function located in USER32.DLL that could be used, but as said,
	 * ConsolePublicPalette call isn't implemented anymore in WIN32k, another stupidity by our friends at M$...
	 */
	OpenClipboard(NULL);
	SetClipboardData(CF_PALETTE, hPalette);
	CloseClipboard();

	return SetConsolePaletteReal(hConsoleOutput, hPalette, dwUsage);
}

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
		HMODULE hKernelBase, hKrnl32;
		LPBYTE lpProcII;
		int i;

		hKrnl32 = GetModuleHandle(_T("kernel32.dll"));
		hKernelBase = (HMODULE)GetModuleHandle(_T("KernelBase.dll"));
		lpProcII = (LPBYTE)GetProcAddress(hKrnl32, "BasepProcessInvalidImage");
		BasepProcessInvalidImageReal = (fpBasepProcessInvalidImage)lpProcII;
		TRACE("LDNTVDM: BasepProcessInvalidImageReal = %08X", BasepProcessInvalidImageReal);
		BaseIsDosApplication = GetProcAddress(hKrnl32, "BaseIsDosApplication");
		TRACE("LDNTVDM: BaseIsDosApplication = %08X", BaseIsDosApplication);
		Hook_IAT_x64((LPBYTE)hKernelBase, "KERNEL32.DLL", "ext-ms-win-kernelbase-processthread-l1-1-0.dll", 
			"BasepProcessInvalidImage", BasepProcessInvalidImage);

		Hook_IAT_x64_IAT((LPBYTE)hKrnl32, "api-ms-win-core-processthreads-l1-1-2.dll", "CreateProcessA", CreateProcessAHook, &CreateProcessAReal);
		Hook_IAT_x64_IAT((LPBYTE)hKrnl32, "api-ms-win-core-processthreads-l1-1-2.dll", "CreateProcessW", CreateProcessWHook, &CreateProcessWReal);

#ifdef _WIN64
		FixNTDLL();
		{
			TCHAR szProcess[MAX_PATH], *p;

			// Fix ConhostV1.dll bug where memory isn't initialized properly
			p=szProcess + GetModuleFileName(NULL, szProcess, sizeof(szProcess) / sizeof(TCHAR));
			while (*p != '\\') p--;
			if (lstrcmpi(++p, _T("ConHost.exe")) == 0)
			{
				TRACE("LDNTVDM is running inside ConHost.exe");
				if (hKrnl32 = GetModuleHandle(_T("ConHostV1.dll")))
					Hook_IAT_x64_IAT((LPBYTE)hKrnl32, "ntdll.dll", "RtlAllocateHeap", RtlAllocateHeapHook, &RtlAllocateHeapReal);
				else
					Hook_IAT_x64_IAT((LPBYTE)GetModuleHandle(NULL), "api-ms-win-core-libraryloader-l1-2-0.dll", "LoadLibraryExW", LoadLibraryExWHook, &LoadLibraryExWReal);
				
			}
		}
#else
		HookCsrClientCallServer();
		/* SetConsolePalette bug */
		Hook_IAT_x64_IAT(GetModuleHandle(NULL), "KERNEL32.DLL", "SetConsolePalette", mySetConsolePalette, &SetConsolePaletteReal);
		Hook_IAT_x64_IAT((LPBYTE)hKrnl32, "ntdll.dll", "NtQueryInformationProcess", myNtQueryInformationProcess, NULL);
		Hook_IAT_x64_IAT((LPBYTE)hKernelBase, "ntdll.dll", "NtQueryInformationProcess", myNtQueryInformationProcess, NULL);
#endif
		break;
	}
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}