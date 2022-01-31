/* Project: ldntvdm
 * Module : injector32
 * Author : leecher@dose.0wnz.at, Metasploit Framework
 * Descr. : This module implements functions for injecting DLLs into 32bit
 *          processes.
 * Changes: 01.04.2016  - Created
 */

#include "ldntvdm.h"
#include "injector.h"
#include "injector32.h"
#include "detour.h"
#include "reg.h"
#include "symcache.h"
#include "symeng.h"
#include <stddef.h>

#define NtCurrentThread() ( (HANDLE)(LONG_PTR) -2 )   // ntddk wdm ntifs

#include "injector_pl64.c"
#include "injector_pl32.c"
#ifdef _WIN64
#define ThreadProc ThreadProcx64
#define APCProc APCProcx64
#define PostProcProc PostProcProcx64
#define LdrpInitializeProcessProc LdrpInitializeProcessProcx64
#else
#define ThreadProc ThreadProcx86
#define APCProc APCProcx86
#define PostProcProc PostProcProcx86
#define LdrpInitializeProcessProc LdrpInitializeProcessProcx86
#endif

#ifdef METHOD_APC

BYTE apc_stub_x86[] =
"\xFC\x8B\x74\x24\x04\x55\x89\xE5\xE8\x89\x00\x00\x00\x60\x89\xE5"
"\x31\xD2\x64\x8B\x52\x30\x8B\x52\x0C\x8B\x52\x14\x8B\x72\x28\x0F"
"\xB7\x4A\x26\x31\xFF\x31\xC0\xAC\x3C\x61\x7C\x02\x2C\x20\xC1\xCF"
"\x0D\x01\xC7\xE2\xF0\x52\x57\x8B\x52\x10\x8B\x42\x3C\x01\xD0\x8B"
"\x40\x78\x85\xC0\x74\x4A\x01\xD0\x50\x8B\x48\x18\x8B\x58\x20\x01"
"\xD3\xE3\x3C\x49\x8B\x34\x8B\x01\xD6\x31\xFF\x31\xC0\xAC\xC1\xCF"
"\x0D\x01\xC7\x38\xE0\x75\xF4\x03\x7D\xF8\x3B\x7D\x24\x75\xE2\x58"
"\x8B\x58\x24\x01\xD3\x66\x8B\x0C\x4B\x8B\x58\x1C\x01\xD3\x8B\x04"
"\x8B\x01\xD0\x89\x44\x24\x24\x5B\x5B\x61\x59\x5A\x51\xFF\xE0\x58"
"\x5F\x5A\x8B\x12\xEB\x86\x5B\x80\x7E\x10\x00\x75\x3B\xC6\x46\x10"
"\x01\x68\xA6\x95\xBD\x9D\xFF\xD3\x3C\x06\x7C\x1A\x31\xC9\x64\x8B"
"\x41\x18\x39\x88\xA8\x01\x00\x00\x75\x0C\x8D\x93\xCF\x00\x00\x00"
"\x89\x90\xA8\x01\x00\x00\x31\xC9\x51\x51\xFF\x76\x08\xFF\x36\x51"
"\x51\x68\x38\x68\x0D\x16\xFF\xD3\xC9\xC2\x0C\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00";
int apc_stub_x86_size = sizeof(apc_stub_x86);

typedef NTSTATUS(NTAPI * NTQUEUEAPCTHREAD)(HANDLE hThreadHandle, LPVOID lpApcRoutine, LPVOID lpApcRoutineContext, LPVOID lpApcStatusBlock, LPVOID lpApcReserved);

BOOL inject_via_apcthread(HANDLE hProcess, HANDLE hThread, LPVOID lpStartAddress, LPVOID lpParameter)
{
	LPVOID lpApcStub;
	LPVOID lpRemoteApcStub, lpRemoteApcContext;
	APCCONTEXT ctx = { 0 };
	DWORD dwApcStubLength = 0;
	NTQUEUEAPCTHREAD pNtQueueApcThread = (NTQUEUEAPCTHREAD)GetProcAddress(GetModuleHandle(_T("ntdll.dll")), "NtQueueApcThread");

	ctx.s.lpStartAddress = lpStartAddress;
	ctx.p.lpParameter = lpParameter;
	ctx.bExecuted = FALSE;

	// Get the architecture specific apc inject stub...
	lpApcStub = &apc_stub_x86;
	dwApcStubLength = sizeof(apc_stub_x86);

	if (!pNtQueueApcThread ||
		!(lpRemoteApcStub = VirtualAllocEx(hProcess, NULL, dwApcStubLength + sizeof(APCCONTEXT), MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE)))
		return FALSE;
	lpRemoteApcContext = ((BYTE *)lpRemoteApcStub + dwApcStubLength);
	if (!WriteProcessMemory(hProcess, lpRemoteApcStub, lpApcStub, dwApcStubLength, NULL) ||
		!WriteProcessMemory(hProcess, lpRemoteApcContext, (LPCVOID)&ctx, sizeof(APCCONTEXT), NULL))
		return FALSE;

	return pNtQueueApcThread(hThread, lpRemoteApcStub, lpRemoteApcContext, 0, 0) == ERROR_SUCCESS;
}
#endif

#ifdef _WIN64
#undef METHOD_APC
#undef METHOD_MODIFYSTARTUP
#endif

PBYTE InjectWriteLoadLibraryShellcodex64(HANDLE hProc, DWORD64 lpLoadLibraryW, DWORD64 Rip, WCHAR *DllName, DWORD *pdwSize)
{
	BYTE code[] = {
		// pushfq
		0x9c,
		// sub rsp, 28h
		0x48, 0x83, 0xec, 0x28,
		// mov [rsp + 18], rax
		0x48, 0x89, 0x44, 0x24, 0x18,
		// mov [rsp + 10h], rcx
		0x48, 0x89, 0x4c, 0x24, 0x10,
		// mov rax, 22222222222222222h
		0x48, 0xb8, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22,
		// mov rcx, 11111111111111111h
		0x48, 0xb9, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
		// call rax
		0xff, 0xd0,
		// mov rcx, [rsp + 10h]
		0x48, 0x8b, 0x4c, 0x24, 0x10,
		// mov rax, [rsp + 18h]
		0x48, 0x8b, 0x44, 0x24, 0x18,
		// add rsp, 28h
		0x48, 0x83, 0xc4, 0x28,
		// popfq
		0x9d,
		// jmp [rip+0]
		0xFF, 0x25, 0x00, 0x00, 0x00, 0x00,
		// Target address:
		0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33,
	};

	//allocate a remote buffer
	PBYTE InjData =
		(PBYTE)VirtualAllocEx(hProc, NULL, sizeof(code) + (wcslen(DllName) + 1)*sizeof(WCHAR),
			MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	*(PDWORD64)&code[3] = lpLoadLibraryW;
	*(PDWORD64)&code[8] = (DWORD64)(InjData + sizeof(code));
	*(PDWORD64)&code[17] = Rip;

	//write the tmp buff + dll
	//Format: [RemoteFunction][DllName][null char]
	SIZE_T dwWritten;
	if (!WriteProcessMemory(hProc, InjData, code, sizeof(code), &dwWritten) ||
		!WriteProcessMemory(hProc, InjData + sizeof(code), DllName, (wcslen(DllName) + 1)*sizeof(WCHAR), &dwWritten))
	{
		TRACE("Writing memory failed: %d\n", GetLastError());
		return NULL;
	}
	*pdwSize = sizeof(code);
	return InjData;
}

PBYTE  InjectWriteLoadLibraryShellcodex86(HANDLE hProc, DWORD lpLoadLibraryW, DWORD Eip, WCHAR *DllName, DWORD *pdwSize)
{
	BYTE code[] = {
		0x60,	// PUSHAD
		0x9C,	// PUSHFD
		0xb8, 0xAA, 0xAA, 0xAA, 0xAA,	// MOV EAX, AAAAAAAA
		0x68, 0xBB, 0xBB, 0xBB, 0xBB,	// PUSH BBBBBBBB
		0xff, 0xd0,	// CALL EAX
		0x9D,	// POPFD
		0x61,	// POPAD
		0x68, 0xCC, 0xCC, 0xCC, 0xCC,	// PUSH CCCCCCCC
		0xc3	// RET
	};

	//allocate a remote buffer
	PBYTE InjData =
		(PBYTE)VirtualAllocEx(hProc, NULL, sizeof(code) + (wcslen(DllName) + 1)*sizeof(WCHAR),
			MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	*(DWORD*)&code[3] = lpLoadLibraryW;
	*(DWORD*)&code[8] = (DWORD)(InjData + sizeof(code));
	*(DWORD*)&code[17] = Eip;

	//write the tmp buff + dll
	//Format: [RemoteFunction][DllName][null char]
	SIZE_T dwWritten;
	if (!WriteProcessMemory(hProc, InjData, code, sizeof(code), &dwWritten) ||
		!WriteProcessMemory(hProc, InjData + sizeof(code), DllName, (wcslen(DllName) + 1)*sizeof(WCHAR), &dwWritten))
	{
		TRACE("Writing memory failed: %d\n", GetLastError());
		return NULL;
	}
	*pdwSize = sizeof(code);
	return InjData;
}

#ifdef METHOD_INTERCEPTTHREAD
BOOL InjectDllHijackThread(HANDLE hProc, HANDLE hThread, WCHAR *DllName)
{
	ULONG_PTR InjData;
	DWORD dwSize;
	//get the thread context
	CONTEXT ThreadContext;
	ULONG_PTR lpLoadLibraryW;
	ThreadContext.ContextFlags = CONTEXT_CONTROL;
	GetThreadContext(hThread, &ThreadContext);

	lpLoadLibraryW = (ULONG_PTR)GetProcAddress(GetModuleHandle(_T("kernel32.dll")), "LoadLibraryW");
	
#ifdef _WIN64
	TRACE("Hijacked Thread currently @%016llX, Flags: %016llX\n", ThreadContext.Rip, ThreadContext.ContextFlags);
	if (!(InjData = InjectWriteLoadLibraryShellcodex64(hProc, lpLoadLibraryW, ThreadContext.Rip, DllName, &dwSize)))
		return FALSE;
#else
	TRACE("Hijacked Thread currently @%08X, Flags: %08X\n", ThreadContext.Eip, ThreadContext.ContextFlags);
	if (!(InjData = InjectWriteLoadLibraryShellcodex86(hProc, lpLoadLibraryW, ThreadContext.Eip, DllName, &dwSize)))
		return FALSE;
#endif

	if (!isProcessInitialized(hProc))
	{
		/* process has not yet been initialized... As hijacking threads is a bit dangerous anyway,
		 * we back out by queuing an APC instead. NtTestAlert() call in loader should call us just
		 * in time in this case... */
		if (QueueUserAPC((PAPCFUNC)lpLoadLibraryW, hThread, (ULONG_PTR)InjData + dwSize))
		{
			TRACE("Queued loader injection via APC @%08X\n", lpLoadLibraryW);
			return TRUE;
		}
		else
		{
			TRACE("QueueUserAPC failed: %d\n", GetLastError());
		}
	}
	else
	{
		// FIXME: Too dangerous?
		TRACE("Process already initialized.\n");
		return FALSE;
	}

	//set the EIP
#ifdef _WIN64
	ThreadContext.Rip = (DWORD64)InjData;
#else
	ThreadContext.Eip = (ULONG)InjData;
#endif
	SetThreadContext(hThread, &ThreadContext);
	return TRUE;
}

#endif

BOOL isProcessInitialized(HANDLE hProcess)
{
#ifdef TARGET_WINXP
	/* On Windows XP, we get called at ConnectConsoleInternal, Hijacking would be
	 * possible, but it's too early, not all imports may have been bound at the time
	 * when the console client library is loaded.
	 */
	return FALSE;
#endif
#if defined(USE_LDRINITSTATE) && !defined(TARGET_WIN7) /* Win 7 doesn't have it */
	static DWORD *pLdrInitState = NULL;
	DWORD LdrInitState = 3, dwRead;

	/* We could also check ((PROCESS_FLAGS)Peb.Reserved6).ProcessInitializing;, if we want to avoid internal symbols (CrossProcessFlags)  */
	if (!pLdrInitState)
	{
		// Load the private loader functions by using DbgHelp and Symbol server
		DWORD64 dwBase = 0, dwAddress;
		char szNTDLL[MAX_PATH];
		HMODULE hNTDLL = GetModuleHandle(_T("ntdll.dll"));

		GetSystemDirectoryA(szNTDLL, sizeof(szNTDLL) / sizeof(szNTDLL[0]));
		strcat(szNTDLL, "\\ntdll.dll");
		if (SymEng_LoadModule(szNTDLL, &dwBase) == 0)
		{
			if ((dwAddress = SymEng_GetAddr(dwBase, "LdrInitState")))
			{
				pLdrInitState = (DWORD64)hNTDLL + dwAddress;
			}
			SymEng_UnloadModule(dwBase);
		}
	}
	if (!ReadProcessMemory(hProcess, pLdrInitState, &LdrInitState, sizeof(LdrInitState), &dwRead) || dwRead != sizeof(LdrInitState))
		return TRUE; /* Better assume yes, if we cannot read it */
	TRACE("LdrInitState = %d\n", LdrInitState);
	return LdrInitState >= 3;
#else
	PROCESS_BASIC_INFORMATION basicInfo;
	ULONG Flags;
	NTSTATUS Status;
	SIZE_T dwRead;

	if (!NT_SUCCESS(Status = NtQueryInformationProcess(hProcess, ProcessBasicInformation, &basicInfo, sizeof(basicInfo), NULL)))
	{
		TRACE("Failed to Query Process basic info: %08X\n", Status);
		return TRUE;
	}
	if (!ReadProcessMemory(hProcess, &(basicInfo.PebBaseAddress->Reserved6), &Flags, sizeof(Flags), &dwRead))
	{
		TRACE("Failed to read process memory: %08X\n", GetLastError());
		return TRUE;
	}
	if (dwRead == sizeof(Flags) && (Flags & 2) /* =ProcessInitializing */) 
	{
		TRACE("Process is in state Initializing (Flags=%08X)\n", Flags);
		return FALSE;
	}
	TRACE("Flags = %08X\n", Flags);
	return TRUE;
#endif
}

BOOL injectLdrLoadDLL(HANDLE hProcess, HANDLE hThread, WCHAR *szDLL, UCHAR method)
{
	PROCESS_BASIC_INFORMATION basicInfo;
	THREAD_DATA data;
	PVOID pData, code, pProc;
	BOOL bRet = TRUE;
	ULONG ulSizeOfCode;
	NTSTATUS Status;
	HMODULE nt;
	CONTEXT ctx;
	static pLdrpInitializeProcess fnLdrpInitializeProcess = NULL;

#ifdef METHOD_INTERCEPTTHREAD
	if (method == METHOD_INTERCEPTTHREAD)
	{
		if (!hThread)
		{
			if (!NT_SUCCESS(Status = NtGetNextThread(hProcess, NULL, THREAD_GET_CONTEXT | THREAD_SET_CONTEXT | THREAD_SUSPEND_RESUME, 0, 0, &hThread)))
			{
				TRACE("NtGetNextThread failed: %08X\n", Status);
				return FALSE;
			}
		}
		bRet = InjectDllHijackThread(hProcess, hThread, L"ldntvdm.dll");
		if (bRet) return TRUE;
		method = METHOD_CREATETHREAD;
	}
#endif

#if defined(METHOD_MODIFYSTARTUP) || defined (METHOD_CREATETHREAD) || defined(METHOD_APC) || defined(METHOD_POSTPROCESSINIT) || defined(METHOD_HOOKLDR)

	nt = GetModuleHandleW(L"ntdll.dll");

	ZeroMemory(&data, sizeof(data));
	data.fnLdrLoadDll = (pLdrLoadDll)GetProcAddress(nt, "LdrLoadDll");
	lstrcpyW(data.DllName, szDLL);

#ifdef METHOD_MODIFYSTARTUP
	switch (method)
	{
	case METHOD_MODIFYSTARTUP:
		ctx.ContextFlags = CONTEXT_INTEGER;
		if (GetThreadContext(hThread, &ctx))
		{
			data.OrigEIP = ctx.Eax;
			data.EIPParams = ctx.Ebx;
		}
		else
		{
			OutputDebugStringA("GetThreadContext failed");
			return FALSE;
		}
		break;
	}
#endif

	switch (method)
	{
#ifdef METHOD_CREATETHREAD
	case METHOD_CREATETHREAD:
		data.fnNtTerminateThread = (pNtTerminateThread)GetProcAddress(nt, "NtTerminateThread");
		pProc = ThreadProc;
		ulSizeOfCode = sizeof(ThreadProc) / sizeof(ThreadProc[0]);
		break;
#endif
#ifdef METHOD_HOOKLDR
	case METHOD_HOOKLDR:
	{
		pProc = LdrpInitializeProcessProc;
		ulSizeOfCode = sizeof(LdrpInitializeProcessProc)/sizeof(LdrpInitializeProcessProc[0]);
		if (!fnLdrpInitializeProcess)
		{
			DWORD64 dwAddress;
#ifdef USE_SYMCACHE
			NTSTATUS Status;
			HKEY hKey;

			if (NT_SUCCESS(Status = REG_OpenLDNTVDM(KEY_READ | KEY_WRITE, &hKey)))
			{
				if (SymCache_GetDLLKey(hKey, L"ntdll.dll", FALSE) &&
					(dwAddress = (DWORD64)SymCache_GetProcAddress(hKey, L"LdrpInitializeProcess")))
				{
					dwAddress += (DWORD64)nt;
					fnLdrpInitializeProcess = (pLdrpInitializeProcess)dwAddress;
				}
				REG_CloseKey(hKey);
			}
#else
			DWORD64 dwBase;
			char szNTDLL[MAX_PATH];

			GetSystemDirectoryA(szNTDLL, sizeof(szNTDLL) / sizeof(szNTDLL[0]));
			strcat(szNTDLL, "\\ntdll.dll");
			if (SymEng_LoadModule(szNTDLL, &dwBase) == 0)
			{
				if ((dwAddress = SymEng_GetAddr(dwBase, "LdrpInitializeProcess")))
				{
					dwAddress += (DWORD64)nt;
					fnLdrpInitializeProcess = (pLdrpInitializeProcess)dwAddress;
				}
				else
				{
					OutputDebugStringA("NTVDM: Resolving symbols failed.");
				}
				SymEng_UnloadModule(dwBase);
			}
#endif

		}
		if (!fnLdrpInitializeProcess) return FALSE;
		break;
	}
#endif
#ifdef METHOD_POSTPROCESSINIT
	case METHOD_POSTPROCESSINIT:
	{
		DWORD result;
		// Get remote PEB
		if (!NT_SUCCESS(NtQueryInformationProcess(hProcess, ProcessBasicInformation, &basicInfo, sizeof(basicInfo), NULL))) return FALSE;
		pProc = (PVOID)PostProcProc;
		ulSizeOfCode = sizeof(PostProcProc) / sizeof(PostProcProc[0]);
		ReadProcessMemory(hProcess, (PVOID)((ULONG_PTR)basicInfo.PebBaseAddress + offsetof(PEB, PostProcessInitRoutine)), (PVOID)&data.fnOrigPostProcessInit, sizeof(data.fnOrigPostProcessInit), &result);
		break;
	}
#endif
	default:
		pProc = APCProc;
		ulSizeOfCode = sizeof(APCProc) / sizeof(APCProc[0]);
		break;
	}

	
	if (!(code = VirtualAllocEx(hProcess, NULL, ulSizeOfCode + sizeof(data), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE)))
		return FALSE;
	pData = (LPBYTE)code + ulSizeOfCode;
#ifdef METHOD_POSTPROCESSINIT
	if (method == METHOD_POSTPROCESSINIT) data.EIPParams = pData;
#endif
#ifdef METHOD_HOOKLDR
	if (method == METHOD_HOOKLDR)
	{
		if (!(data.fnLdrpInitializeProcess = (pLdrpInitializeProcess)Hook_Inline(hProcess, nt, fnLdrpInitializeProcess, code)))
			return FALSE;
		data.EIPParams = pData;
	}
#endif
	data.UnicodeString.Length = (USHORT)wcslen(data.DllName) * sizeof(WCHAR);
	data.UnicodeString.MaximumLength = sizeof(data.DllName);
	data.UnicodeString.Buffer = (PWSTR)((PBYTE)pData + offsetof(THREAD_DATA, DllName));
	if (!WriteProcessMemory(hProcess, pData, &data, sizeof(data), NULL) ||
		!WriteProcessMemory(hProcess, code, (PVOID)pProc, ulSizeOfCode, NULL))
	{
		return FALSE;
	}
	switch (method)
	{
#ifdef METHOD_APC
	case METHOD_APC:
		return inject_via_apcthread(hProcess, hThread, code, pData);
#endif
#if defined(METHOD_POSTPROCESSINIT) || defined(METHOD_HOOKLDR)
#ifdef METHOD_POSTPROCESSINIT
	case METHOD_POSTPROCESSINIT:
#endif
#ifdef METHOD_HOOKLDR
	case METHOD_HOOKLDR:
#endif
	{
		ULONG i;
	
		// Write pointer to pData into shellcode
		for (i = 0; i < ulSizeOfCode; i++)
			if (*((PULONG_PTR)&((PBYTE)pProc)[i]) == ((ULONG_PTR)1 << (sizeof(ULONG_PTR) * 8 - 1)))
			{
				PVOID pDataPtr = (PBYTE)pData + offsetof(THREAD_DATA, EIPParams);
				if (!WriteProcessMemory(hProcess, (LPVOID)((ULONG_PTR)code + i), (PVOID)&pDataPtr, sizeof(pDataPtr), NULL)) return FALSE;
				break;
			}
		if (i == ulSizeOfCode) return FALSE;

		if (method == METHOD_POSTPROCESSINIT)
		{
			// Modify PostProcessInitRoutine to point to our routine
			if (!WriteProcessMemory(hProcess, (PVOID)((ULONG_PTR)basicInfo.PebBaseAddress + offsetof(PEB, PostProcessInitRoutine)), (PVOID)&code, sizeof(code), NULL)) return FALSE;
		}

		// Now caller needs to resume the main thread and cross fingers..
		return TRUE;
	}
#endif
#ifdef METHOD_CREATETHREAD
	case METHOD_CREATETHREAD:
	{
		HANDLE hNewThread;
		CLIENT_ID cid;

		if (bRet = (NT_SUCCESS(Status = RtlCreateUserThread(hProcess, NULL, FALSE, 0, 0, 0, code, pData, &hNewThread, &cid))))
		{
			TRACE("Created injection thread h=%08x, tid=%d\n", hNewThread, cid.UniqueThread);
			WaitForSingleObject(hNewThread, INFINITE);
			CloseHandle(hNewThread);
		} 
		TRACE("RtlCreateUserThread Status = %08X\n", Status);
		VirtualFreeEx(hProcess, pData, 0, MEM_RELEASE);
		VirtualFreeEx(hProcess, code, 0, MEM_RELEASE);
		break;
	}
#endif
#ifdef METHOD_MODIFYSTARTUP
	case METHOD_MODIFYSTARTUP:
	{
		//Entry point is stored in register eax (ctx + 0xB0)
		ctx.Eax = code;
		ctx.Ebx = pData;
		ctx.ContextFlags = CONTEXT_INTEGER;
		if (!SetThreadContext(hThread, &ctx))
		{
			OutputDebugStringA("SetThreadContext failed.");
			return FALSE;
		}
		break;
	}
#endif
	}
#endif

	return bRet;
}
