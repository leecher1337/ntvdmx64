/* Project: ldntvdm
 * Module : ldntvdm (main)
 * Author : leecher@dose.0wnz.at
 * Descr. : The purpose of this module is to inject into every process 
 *          (initially via AppInit_DLLs, propagation via Winevent
 *          hook) and patch the loader in order to fire up the NTVDM
 *          when trying to execute DOS executables. It does this by 
 *          hooking KERNEL32.DLL BasepProcessInvalidImage function on
 *          Windows 7 and above, for Windows XP description, see
 *          xpcreateproc.c
 *          In 32bit version, it also has to fix all CSRSS-Calls done
 *          by kernel32-functions in order to get loader and NTVDM
 *          up and running (this normaly should be done by WOW64.dll)
 * Changes: 01.04.2016  - Created
 */

#include <ntstatus.h>
#define WIN32_NO_STATUS
#include "ldntvdm.h"
#include <tchar.h>
#include "Winternl.h"
#include "csrsswrap.h"
#include "wow64ext.h"
#include "iathook.h"
#include "basemsg64.h"
#include "basevdm.h"
#include "symeng.h"
#include "symcache.h"
#include "detour.h"
#include "createproc.h"
#include "consbmpbug.h"
#include "consbmpxpbug.h"
#include "conspal.h"
#include "conhostproc.h"
#include "reg.h"
#include "winevent.h"
#include "extracticon.h"
#include "xpcreateproc.h"
#include "ntregapi.h"
#include "apppatch.h"
#include "appinfo.h"
#include "injector32.h"
#include "oemcp.h"

#pragma comment(lib, "ntdll.lib")

volatile char av_vendor_info[] = "AV-vendors: This is NOT malware! This is the loader of https://github.com/leecher1337/ntvdmx64 project";

#if !defined(TARGET_WIN10) && !defined(TARGET_WIN11)
#define KRNL32_CALL BASEP_CALL
#else
#define KRNL32_CALL __fastcall
#endif

#ifdef WOW16_SUPPORT
BOOL gfHasOTVDM = FALSE;
#endif
HMODULE g_hInst;

typedef INT_PTR(BASEP_CALL *fpBasepProcessInvalidImage)(NTSTATUS Error, HANDLE TokenHandle,
	LPCWSTR dosname, LPCWSTR *lppApplicationName,
	LPCWSTR *lppCommandLine, LPCWSTR lpCurrentDirectory,
	PDWORD pdwCreationFlags, BOOL *pbInheritHandles, PUNICODE_STRING PathName, INT_PTR a10,
	LPVOID *lppEnvironment, LPSTARTUPINFOW lpStartupInfo, BASE_API_MSG *m, PULONG piTask,
	PUNICODE_STRING pVdmNameString, ANSI_STRING *pAnsiStringEnv,
	PUNICODE_STRING pUnicodeStringEnv, PDWORD pVDMCreationState, PULONG pVdmBinaryType, PDWORD pbVDMCreated,
	PHANDLE pVdmWaitHandle);

typedef NTSTATUS(KRNL32_CALL *fpBaseCheckVDM)(
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
#ifdef TARGET_WINXP
	IN  LPCWSTR CommandLine,
#else
	BASE_API_MSG *m,
#endif
#endif
	IN  ULONG  DosSeqId,
	IN  ULONG  BinaryType,
	IN  PUNICODE_STRING CmdLineString
#ifdef TARGET_WIN7
	,OUT PULONG VdmSize
#endif
	);

#define VDM_HIDE_WINDOW 	1
#define VDM_IS_ICONIC           2
#define VDM_CLIENT_RECT         3
#define VDM_CLIENT_TO_SCREEN    4
#define VDM_SCREEN_TO_CLIENT    5

#define ERR_NOT_IMPLEMENTED         0x4001

typedef BOOL (BASEP_CALL *fpVDMConsoleOperation)(
	DWORD iFunction,
	LPVOID lpData
	);
fpVDMConsoleOperation VDMConsoleOperation = NULL;

#ifdef TRACE_FILE
HANDLE g_hLog = INVALID_HANDLE_VALUE;

void Trace(char *pszLine)
{
	if (g_hLog != INVALID_HANDLE_VALUE)
	{
		DWORD dwWritten;
		SYSTEMTIME st;
		char szDateBuf[64];

		GetSystemTime(&st);
		WriteFile(g_hLog, szDateBuf,
			sprintf(szDateBuf, "[%d] %02d.%02d.%02d %02d:%02d:%02d.%06d ", GetCurrentProcessId(), st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds),
			&dwWritten, NULL);
		WriteFile(g_hLog, pszLine, strlen(pszLine), &dwWritten, NULL);
	}
}
#endif
	
#if defined(WOW16_SUPPORT) || (defined(USE_SYMCACHE) && !defined(CREATEPROCESS_HOOK)) || defined(CREATEPROCESS_HOOKNTCREATE)
NTSTATUS LastCreateUserProcessError = STATUS_SUCCESS;
#ifndef TARGET_WINXP
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
#if defined (USE_SYMCACHE) && (!defined(CREATEPROCESS_HOOK) || defined(CREATEPROCESS_HOOKNTCREATE))
	UpdateSymbolCache(TRUE);
#endif
	LastCreateUserProcessError = 
		NtCreateUserProcessReal(ProcessHandle,
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
	if (!NT_SUCCESS(LastCreateUserProcessError) && ProcessParameters)
	{
		TRACE("NtCreateUserProcess(ThreadHandle=%X, CommandLine=%wZ) failed with %08X\n", ThreadHandle?*ThreadHandle:-1, &ProcessParameters->CommandLine, LastCreateUserProcessError);
	}
#ifdef CREATEPROCESS_HOOKNTCREATE
	else
	{
		PROCESS_INFORMATION pi;

		pi.hProcess = *ProcessHandle;
		pi.hThread = *ThreadHandle;
		InjectIntoCreatedThread(&pi, METHOD_HOOKLDR);
	}
#endif
#ifdef WOW16_SUPPORT
	return (!gfHasOTVDM && LastCreateUserProcessError==STATUS_INVALID_IMAGE_WIN_16)?STATUS_INVALID_IMAGE_PROTECT:LastCreateUserProcessError;
#else
	return LastCreateUserProcessError;
#endif
}
#endif /* TARGET_WINXP */
#endif /* defined(WOW16_SUPPORT) || (defined(TARGET_WIN7) && !defined(CREATEPROCESS_HOOK)) */

#ifdef TRACING
fpsprintf sprintf;
#endif
fp_stricmp __stricmp;
fp_wcsicmp __wcsicmp;
fpstrcmp _strcmp;
fpswprintf __swprintf;
#ifdef NEED_BASEVDM
fpwcsncpy _wcsncpy;
fp_wcsnicmp __wcsnicmp;
fpwcsrchr _wcsrchr;
fpstrstr _strstr;
#endif
fpBaseIsDosApplication BaseIsDosApplication = NULL;
#ifndef NEED_BASEVDM
fpBasepProcessInvalidImage BasepProcessInvalidImageReal = NULL;
fpBaseCheckVDM BaseCheckVDM = NULL;
fpBaseCreateVDMEnvironment BaseCreateVDMEnvironment = NULL;
fpBaseGetVdmConfigInfo BaseGetVdmConfigInfo = NULL;
#endif

#if 0
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
#endif /* 0 */

#ifndef TARGET_WINXP  /* XP has its own xpcreateproc.c module */
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

	TRACE("LDNTVDM: BasepProcessInvalidImage(%08X,'%ws');\n", Error, PathName->Buffer);
#ifdef WOW16_SUPPORT
	if (LastCreateUserProcessError == STATUS_INVALID_IMAGE_WIN_16 && Error == STATUS_INVALID_IMAGE_PROTECT)
	{
		TRACE("LDNTVDM: Launching Win16 application\n");
		Error = LastCreateUserProcessError;
	}
#endif
#ifdef NEED_APPINFO
	// It's in here, because the cache may resolve the symbol after already launching the
	// service DLL and user shouldn't need to restart the service once Symbol got resolved.
	AppInfo_InstallHook();
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
#ifndef NEED_BASEVDM
		if (!BaseCheckVDM)
		{
			// Load the private loader functions by using DbgHelp and Symbol server
			DWORD64 dwBase = 0, dwAddress;
			HMODULE hKrnl32 = GetModuleHandle(_T("kernel32.dll"));
#ifdef USE_SYMCACHE
			NTSTATUS Status;
			HKEY hKey;

#ifdef USE_SYMCACHE
			UpdateSymbolCache(TRUE);
#endif
			if (NT_SUCCESS(Status = REG_OpenLDNTVDM(KEY_READ | KEY_WRITE, &hKey)))
			{
				if (SymCache_GetDLLKey(hKey, L"kernel32.dll", TRUE))
				{
					if ((dwAddress = (DWORD64)SymCache_GetProcAddress(hKey, L"BaseCreateVDMEnvironment")) &&
						(BaseCreateVDMEnvironment = (fpBaseCreateVDMEnvironment)((DWORD64)hKrnl32 + dwAddress)) &&
						(dwAddress = (DWORD64)SymCache_GetProcAddress(hKey, L"BaseGetVdmConfigInfo")) &&
						(BaseGetVdmConfigInfo = (fpBaseGetVdmConfigInfo)((DWORD64)hKrnl32 + dwAddress)) &&
						(dwAddress = (DWORD64)SymCache_GetProcAddress(hKey, L"BaseCheckVDM")))
					{
						BaseCheckVDM = (fpBaseCheckVDM)((DWORD64)hKrnl32 + dwAddress);
					}
					else
					{
						OutputDebugStringA("NTVDM: Resolving symbols failed.");
					}
				}
				REG_CloseKey(hKey);
			}
#else	// USE_SYMCACHE
			char szKernel32[MAX_PATH];

			GetSystemDirectoryA(szKernel32, sizeof(szKernel32) / sizeof(szKernel32[0]));
			strcat(szKernel32, "\\kernel32.dll");
			if (SymEng_LoadModule(szKernel32, &dwBase) == 0)
			{
				if ((dwAddress = SymEng_GetAddr(dwBase, "BaseCreateVDMEnvironment")) &&
					(BaseCreateVDMEnvironment = (fpBaseCreateVDMEnvironment)((DWORD64)hKrnl32 + dwAddress)) &&
					(dwAddress = SymEng_GetAddr(dwBase, "BaseGetVdmConfigInfo")) &&
					(BaseGetVdmConfigInfo = (fpBaseGetVdmConfigInfo)((DWORD64)hKrnl32 + dwAddress)) &&
					(dwAddress = SymEng_GetAddr(dwBase, "BaseCheckVDM")))
				{
					BaseCheckVDM = (fpBaseCheckVDM)((DWORD64)hKrnl32 + dwAddress);
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
#endif	// USE_SYMCACHE
			// If resolution fails, fallback to normal loader code and display error
		}
		if (BaseCheckVDM)
#endif /*NEED_BASEVDM */
		{
			// Now that we have the functions, do what the loader normally does on 32bit Windows
			BASE_CHECKVDM_MSG *b = (BASE_CHECKVDM_MSG*)&m->u.CheckVDM;
			NTSTATUS Status;
#if defined(TARGET_WIN7) || defined(TARGET_WINXP)
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
				TRACE("LDNTVDM: BaseCreateVDMEnvironment failed\n");
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

#ifndef TARGET_WINXP
			if (GetConsoleHost() != (HANDLE)-1)
			{
				BOOL bIcon;

				if (VDMConsoleOperation && !VDMConsoleOperation(VDM_IS_ICONIC, &bIcon) && GetLastError() == ERR_NOT_IMPLEMENTED)
				{
					// We are running on a V2 console, so enforce creating a new 
					// window for NTVDM (which runs on V1 console). It's better than error, at least
					*pdwCreationFlags |= CREATE_NEW_CONSOLE;
					TRACE("LDNTVDM: Running on a V2 console, thus opening DOS application in new V1 console window.\n")
				}
			}
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
				TRACE("LDNTVDM: BaseCheckVDM failed, gle=%d\n", GetLastError());
				return FALSE;
			}

			TRACE("VDMState=%08X\n", b->VDMState);
			switch (b->VDMState & VDM_STATE_MASK) {
			case VDM_NOT_PRESENT:
				*pVDMCreationState = VDM_PARTIALLY_CREATED;
				if (*pdwCreationFlags & DETACHED_PROCESS)
				{
					SetLastError(ERROR_ACCESS_DENIED);
					TRACE("LDNTVDM: VDM_NOT_PRESENT -> ERROR_ACCESS_DENIED\n");
					return FALSE;
				}
				if (!BaseGetVdmConfigInfo(
#ifndef TARGET_WIN80
#ifdef TARGET_WINXP
					*lppCommandLine,
#else
					m,
#endif
#endif
					*piTask,
					*pVdmBinaryType,
					pVdmNameString
#if defined(TARGET_WIN7) || defined(TARGET_WINXP)
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
				TRACE("VDM_PRESENT_NOT_READY\n");
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
			TRACE("LDNTVDM: Launch DOS!\n");
			return TRUE;
		}
	} else if (Error == STATUS_INVALID_IMAGE_WIN_16) {
		//*pdwCreationFlags |= CREATE_SEPARATE_WOW_VDM;
#endif /* WOW_HACK */
	}
	ret = BasepProcessInvalidImageReal(Error, TokenHandle, dosname, lppApplicationName, lppCommandLine, lpCurrentDirectory,
		pdwCreationFlags, pbInheritHandles, PathName, a10, lppEnvironment, lpStartupInfo, m, piTask, pVdmNameString,
		pAnsiStringEnv, pUnicodeStringEnv, pVDMCreationState, pVdmBinaryType, pbVDMCreated, pVdmWaitHandle);
#if defined(TARGET_WIN7) && !defined(_WIN64)
	/* On Windows 7 32bit, this parameter seems to be used to set PROC_THREAD_ATTRIBUTE_MITIGATION_POLICY 
	 * to 1 and this in turn causes a weird Virtual memory allocation call in PspSetupUserProcessAddressSpace 
	 * to fail. So I suppose that this parameter means "VdmReserve" there, nevertheless the logic is weird,
	 * as CreateProcessInternalW sets low DWORD to 1 (which would make sense in terms of Mitigation policy),
	 * and hiword to the value specified here. 
	 * In kernel, it takes the low word (thus, 1 !) as base address and the high word as size.
	 * It seems we get 0xFFFFFF here in pbVDMCreated, which would make sense (even though we don't need it),
	 * in terms of VdmReserve, but the LOWORD is still 1 and therefore the address is unaligned making 
	 * NtCreateUserProcess fail with STATUS_INVALID_PARAMETER_2 (which gets originally issued by 
	 * ZwAllocateVirtualMemory call in PspSetupUserProcessAddressSpace). 
	 * So my guess is, that it simply is a kernel bug, given the fact that the problematic code got removed 
	 * in later Windows versions. 
	 */
	*pbVDMCreated = 0;
#endif
#ifdef WOW_HACK
	*pdwCreationFlags &= ~CREATE_NO_WINDOW;
#endif
	TRACE("LDNTVDM: BasepProcessInvalidImage = %d\n", ret);

	return ret;
}
#endif /* !TARGET_WINXP  */


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

#else	// _WIN32

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

WCHAR *GetProcessName(void)
{
	static WCHAR szProcess[MAX_PATH], *p;

	p = szProcess + GetModuleFileNameW(NULL, szProcess, sizeof(szProcess) / sizeof(WCHAR));
	while (*p != '\\') p--;
	return ++p;
}

VOID APIENTRY myCmdBatNotification(IN ULONG fBeginEnd)
{
	BASE_API_MSG m;
	BASE_BAT_NOTIFICATION_MSG *a = (BASE_BAT_NOTIFICATION_MSG*)&m.u.BatNotification;

	a->ConsoleHandle = GetConsoleHost();
	TRACE("CmdBatNotification(%d), hConhost = %X\n", fBeginEnd, a->ConsoleHandle);

	if (a->ConsoleHandle == (HANDLE)-1)
		return;

	a->fBeginEnd = fBeginEnd;
	myCsrClientCallServer(&m, NULL, CSR_MAKE_API_NUMBER(BASESRV_SERVERDLL_INDEX, 
		BasepBatNotification),	sizeof(*a));
}

#ifndef TARGET_WINXP

UNICODE_STRING g_ComponentsKey = {
	28 * sizeof(WCHAR),
	29 * sizeof(WCHAR),
	L"\\Registry\\Machine\\COMPONENTS"
};

BOOL IsSystemUpdating(void)
{
	BOOL bRet = FALSE;
	OBJECT_ATTRIBUTES objAttr;
	HANDLE hKey;

	InitializeObjectAttributes(&objAttr, &g_ComponentsKey, OBJ_CASE_INSENSITIVE, NULL, NULL);
	if (NT_SUCCESS(NtOpenKey(&hKey, KEY_READ, &objAttr)))
	{
		DWORD dwVal;

		if (NT_SUCCESS(REG_QueryDWORD(hKey, L"ExecutionState", &dwVal)))
		{
			TRACE("Windows updates are pending: ExecutionState = %d\n", dwVal);
			bRet = dwVal>1;
		}
		NtClose(hKey);
	}

	return bRet;
}
#endif /* TARGET_WINXP */

BOOL IsProcessBlacklisted(WCHAR *pszProcess)
{
	WCHAR szBlacklist[1024], *p;

	if (!GetPrivateProfileSectionW(L"blacklist", szBlacklist, sizeof(szBlacklist)/sizeof(WCHAR), L"ldntvdm.ini"))
		return FALSE;
	for (p = szBlacklist; *p; p += wcslen(p) + 1)
	{
		if (__wcsicmp(pszProcess, p) == 0)
		{
			TRACE("Process %S is blacklisted, not injecting.\n", pszProcess);
			return TRUE;
		}
	}
	return FALSE;
}

#if defined(USE_SYMCACHE) && defined(TARGET_WIN7)
void EnsureWin7Symbols(HMODULE hKrnl32)
{
	if (!BasepProcessInvalidImageReal || !BaseIsDosApplication)
	{
		// Windows 7, all the stuff is internal in kernel32.dll :(
		// But we cannot use the symbol loader in the module loading routine
		NTSTATUS Status;
		DWORD dwAddress;
		HKEY hKey;
		LPBYTE lpProcII;

		if (NT_SUCCESS(Status = REG_OpenLDNTVDM(KEY_READ, &hKey)))
		{
#ifndef NEED_BASEVDM
			if (dwAddress = SymCache_GetProcAddress(hKey, L"BasepProcessInvalidImage"))
			{
				lpProcII = (LPBYTE)((DWORD64)hKrnl32 + dwAddress);
				BasepProcessInvalidImageReal = (fpBasepProcessInvalidImage)Hook_Inline(GetCurrentProcess(), hKrnl32, lpProcII, BasepProcessInvalidImage);
			}
#endif /* !NEED_BASEVDM */
			if (dwAddress = SymCache_GetProcAddress(hKey, L"BaseIsDosApplication"))
				BaseIsDosApplication = (fpBaseIsDosApplication)((DWORD64)hKrnl32 + dwAddress);
			REG_CloseKey(hKey);
		}
		else
		{
			TRACE("RegOpenKey failed: %08X\n", Status);
		}
	}
}
#endif



#ifdef CRYPT_LDR
BOOL WINAPI real_DllMainCRTStartup(
#else
BOOL WINAPI _DllMainCRTStartup(
#endif /* !CRYPT_LDR */
	HANDLE  hDllHandle,
	DWORD   dwReason,
	LPVOID  lpreserved
	)
{
	BOOL bRet = TRUE;

	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
	{
		STARTUPINFO si = { 0 };
#ifndef TARGET_WINXP
		HMODULE hKernelBase;
#endif
		HMODULE hKrnl32, hNTDLL = GetModuleHandle(_T("ntdll.dll"));
		LPBYTE lpProcII = NULL;
		WCHAR *pszProcess;

		DisableThreadLibraryCalls(hDllHandle);
		// Prevent from unloadiung DLL
		GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_PIN, (LPCWSTR)&_DllMainCRTStartup, &hKrnl32);
		g_hInst = hDllHandle;
		hKrnl32 = GetModuleHandle(_T("kernel32.dll"));
#ifndef TARGET_WINXP
		hKernelBase = (HMODULE)GetModuleHandle(_T("KernelBase.dll"));
#endif
		__stricmp = (fp_stricmp)GetProcAddress(hNTDLL, "_stricmp");
		__wcsicmp = (fp_wcsicmp)GetProcAddress(hNTDLL, "_wcsicmp");
		_strcmp = (fpstrcmp)GetProcAddress(hNTDLL, "strcmp");
		__swprintf = (fpswprintf)GetProcAddress(hNTDLL, "swprintf");
#ifdef TRACING
		sprintf = (fpsprintf)GetProcAddress(hNTDLL, "sprintf");
#ifdef TRACE_FILE
		{
			char szTmpFile[MAX_PATH];
			sprintf(szTmpFile + GetWindowsDirectoryA(szTmpFile, sizeof(szTmpFile)), "\\Temp\\ldntvdm.%d.log", GetCurrentProcessId());
			g_hLog = CreateFileA(szTmpFile, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_NEW,
				0, NULL);
		}
#endif // TRACE_FILE
#endif // TRACING
#ifdef NEED_BASEVDM
		_wcsncpy = (fpwcsncpy)GetProcAddress(hNTDLL, "wcsncpy");
		__wcsnicmp = (fp_wcsnicmp)GetProcAddress(hNTDLL, "_wcsnicmp");
		_wcsrchr = (fpwcsrchr)GetProcAddress(hNTDLL, "wcsrchr");
		 _strstr = (fpstrstr)GetProcAddress(hNTDLL, "strstr");
#endif

		 pszProcess = GetProcessName();
		 TRACE("LDNTVDM is running inside %S\n", pszProcess)
#if defined(TARGET_WINXP) && defined(_WIN64)
		 if (__wcsicmp(pszProcess, _T("csrss.exe")) == 0)
		 {
			 FixNTDLL();
#ifndef CREATEPROCESS_HOOK
			 // We want notification when new console process gets started so that we can inject
			 WinEventHook_Install(NULL);
#endif /* CREATEPROCESS_HOOK */
			 
			 // Nothing more needed inside CSRSS, we are done.
			 break;
		 }
#endif /* TARGET_WINXP && _WIN64*/

		 if (IsProcessBlacklisted(pszProcess))
		 {
			 bRet = FALSE;
			 break;
		 }
#ifndef TARGET_WINXP
		 if (IsSystemUpdating())
		 {
			 bRet = FALSE;
			 break;
		 }
		 VDMConsoleOperation = (fpVDMConsoleOperation)GetProcAddress(hKrnl32, "VDMConsoleOperation");
#endif // !TARGET_WINXP
		 AppPatch_Check(pszProcess);


#ifdef WOW16_SUPPORT
		gfHasOTVDM = REG_CheckForOTVDM();
#endif
#if defined(USE_SYMCACHE) && defined(TARGET_WIN7)
		EnsureWin7Symbols(hKrnl32);
#else	// USE_SYMCACHE && TARGET_WIN7
#ifndef TARGET_WINXP /* XP has its own xpcreateproc.c module */
		// Windows 10
#ifndef NEED_BASEVDM
		lpProcII = (LPBYTE)GetProcAddress(hKrnl32, "BasepProcessInvalidImage");
		BasepProcessInvalidImageReal = (fpBasepProcessInvalidImage)lpProcII;
#endif /* !NEED_BASEVDM */
		BaseIsDosApplication = (fpBaseIsDosApplication)GetProcAddress(hKrnl32, "BaseIsDosApplication");
		Hook_IAT_x64((LPBYTE)hKernelBase, "ext-ms-win-kernelbase-processthread-l1-1-0.dll",
			"BasepProcessInvalidImage", BasepProcessInvalidImage);
#else // !TARGET_WINXP
		XpCreateProcHandler_Install(hKrnl32);
#endif // TARGET_WINXP
#endif // !(USE_SYMCACHE && TARGET_WIN7)
#ifndef NEED_BASEVDM
		TRACE("LDNTVDM: BasepProcessInvalidImageReal = %08X\n", BasepProcessInvalidImageReal);
#endif // !NEED_BASEVDM
#ifndef TARGET_WINXP
		TRACE("LDNTVDM: BaseIsDosApplication = %08X\n", BaseIsDosApplication);
#endif // !TARGET_WINXP


/* WOW 16bit always needs the hook so that it can trigger STATUS_INVALID_IMAGE_WIN_16 
 * Systems with Symbol caching need the hook to refresh the cache, unless they are using
 * the CreateProcess hook which does the refresh alrady and therefore don't need the hook
 * On WOW 16bit, the Hook isn't necessary, if OTVDM is enabled and we shouldn't handle 16bit apps
 *
 * WOW16  SYMCACHE  CREATEPROC  OTVDM  HOOK?
 * -----------------------------------------
 * Y      Y         Y           Y      N	Symbol update will be done in CreateProcess; WOW16 support not needed: OTVDM
 * Y      Y         Y           N      Y	Symbol update will be done in CreateProcess, but WOW16 support needed
 * Y      Y         N           Y      Y	Symbol update needed
 * Y      Y         N           N      Y	Symbol update needed
 * Y      N         Y           Y      N	WOW16 support not needed: OTVDM
 * Y      N         Y           N      Y	WOW16 support needed
 * Y      N         N           Y      N	WOW16 support not needed: OTVDM
 * Y      N         N           N      Y	WOW16 support needed
 * N      Y         Y           Y      N	Symbol update will be done in CreateProcess
 * N      Y         Y           N      N	Symbol update will be done in CreateProcess
 * N      Y         N           Y      Y	Symbol update needed
 * N      Y         N           N      Y	Symbol update needed
 * N      N         Y           Y      N	Symbol update will be done in CreateProcess; WOW16 support not needed
 * N      N         Y           N      N	Symbol update will be done in CreateProcess; WOW16 support not needed
 * N      N         N           Y      N	WOW16 support not needed
 * N      N         N           N      N	WOW16 support not needed
 */
#if !defined (TARGET_WINXP) && (defined(WOW16_SUPPORT) || (defined(USE_SYMCACHE) && !defined(CREATEPROCESS_HOOK)) || defined(CREATEPROCESS_HOOKNTCREATE))
#if !defined(USE_SYMCACHE) || (defined(CREATEPROCESS_HOOK) && !defined(CREATEPROCESS_HOOKNTCREATE))
		if (!gfHasOTVDM)
#endif // !defined(USE_SYMCACHE) || defined(CREATEPROCESS_HOOK)
			if (Hook_IAT_x64_IAT((LPBYTE)hKernelBase, "ntdll.dll", "NtCreateUserProcess", NtCreateUserProcessHook, (PULONG_PTR)&NtCreateUserProcessReal) == -2)
				Hook_IAT_x64_IAT((LPBYTE)hKrnl32, "ntdll.dll", "NtCreateUserProcess", NtCreateUserProcessHook, (PULONG_PTR)&NtCreateUserProcessReal); // Earlier Win7 -> It's in kernel32.dll
#endif // defined(WOW16_SUPPORT) || (defined(USE_SYMCACHE) && !defined(CREATEPROCESS_HOOK))

/* Use CreateProcess Hook? */
#if defined(CREATEPROCESS_HOOK) && !defined(CREATEPROCESS_HOOKNTCREATE)
		CreateProcessHook_Install(hKrnl32);
#endif

/* Fix CmdBatNotification in cmd.exe */
		if (__wcsicmp(pszProcess, _T("cmd.exe")) == 0)
		{
#if defined(TARGET_WIN7) || defined(TARGET_WINXP)
			Hook_IAT_x64_IAT((LPBYTE)GetModuleHandle(NULL), "kernel32.dll", "CmdBatNotification", myCmdBatNotification, NULL);
#else
			// Unfortunately, this gets delay-loaed by cmd.exe later, therefore place an inline-hook
			//Hook_IAT_x64((LPBYTE)GetModuleHandle(NULL), "ext-ms-win-cmd-util-l1-1-0.dll", "CmdBatNotificationStub", myCmdBatNotification);
			HANDLE hModCmdUtil = GetModuleHandleA("ext-ms-win-cmd-util-l1-1-0.dll");
			if (!hModCmdUtil) hModCmdUtil = LoadLibraryA("ext-ms-win-cmd-util-l1-1-0.dll");
			if (hModCmdUtil)
				Hook_Inline(GetCurrentProcess(), hModCmdUtil, (PVOID)GetProcAddress(hModCmdUtil, "CmdBatNotificationStub"), myCmdBatNotification);
#endif
		}


#ifdef _WIN64
#ifndef TARGET_WINXP /* !TARGET_WINXP */
		if (__wcsicmp(pszProcess, _T("ConHost.exe")) == 0)
		{
			BOOL fNoConhostDll;
			HMODULE hModConhost = NULL;

			TRACE("LDNTVDM is running inside ConHost.exe\n");

			FixNTDLL();
			// Fix ConhostV1.dll bug where memory isn't initialized properly
			fNoConhostDll = ConsBmpBug_Install(&hModConhost);

#ifdef TARGET_WIN11
			// Windows 11 bug: OEM NLS Table not mapped
			if (OEMCP_FixNLSTable())
			{
				//SuspendThread(GetCurrentThread());
				OEMCP_CallInitializeCustomCP();
			}
#endif

#ifndef CREATEPROCESS_HOOK
			// We want notification when new console process gets started so that we can inject
			WinEventHook_Install(fNoConhostDll ? NULL : hModConhost);
#endif /* CREATEPROCESS_HOOK */
		}
#endif /* !TARGET_WINXP */
#else /* !WIN64 */
		// Only fix NTDLL in ntvdm.exe where it is needed, don't interfere with other applications like ovdm, until we natively support Win16
		if (__wcsicmp(pszProcess, _T("ntvdm.exe")) == 0)
			FixNTDLL();
		HookCsrClientCallServer();
#ifdef TARGET_WINXP
		// XP whCreateConsoleScreenBuffer is buggy
		ConsBmpXpBug_Install();
#else
		HookSetConsolePaletteAPC(0);
		HookNtQueryInformationProcess(hKernelBase, hKrnl32);
#endif /* !TARGET_WINXP */
#endif /* !WIN64 */

/* Extract 16bit icon was disabled on Win7 and above */
#ifdef EXTRACTICON_HOOK
		HookExtractIcon();
#endif

/* If we cannot propagate via AppInit-DLLs, we have to inject into child processes manually */
#ifdef NO_APPINIT_DLL
		InjectIntoChildren(GetCurrentProcess());
#endif

#ifdef NEED_APPINFO
		AppInfo_InstallHook();
#endif


		TRACE("ldntvdm Init done (https://github.com/leecher1337/ntvdmx64)\n");
		break;
	}
	case DLL_PROCESS_DETACH:
#ifdef TRACE_FILE
		if (g_hLog != INVALID_HANDLE_VALUE)
		{
			CloseHandle(g_hLog);
			g_hLog = INVALID_HANDLE_VALUE;
		}
#endif
		break;
	}

	return bRet;
}

#ifdef CRYPT_LDR
// .stub SECTION
#pragma section(".stub", execute, read, write)
#pragma code_seg(".stub")

#define CODE_BASE_ADDRESS	0x15151515 // dumpbin file pointer to raw data (do not change, this will be patched by cryptor)
#define CODE_SIZE			0x14141414 // dumpbin size of Virtual memory (do not change, this will be patched by cryptor)

BOOL decryptCodeSection()
{
	__asm {
		mov esi, CODE_BASE_ADDRESS
		mov ecx, CODE_SIZE
		mov edi, esi
		mov bx, 55h
dec_loop:
		lodsb
		xor al, bl
		stosb
		inc bl
		loop dec_loop
	}
	return TRUE;
}

BOOL WINAPI _DllMainCRTStartup(
	HANDLE  hDllHandle,
	DWORD   dwReason,
	LPVOID  lpreserved
	)
{
	static BOOL bInit = FALSE;
	if (!bInit) bInit = decryptCodeSection();
	return real_DllMainCRTStartup(hDllHandle, dwReason, lpreserved);
}
#endif /* CRYPT_LDR */

