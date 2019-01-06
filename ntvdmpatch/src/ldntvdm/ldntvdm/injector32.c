/* Project: ldntvdm
 * Module : injector32
 * Author : leecher@dose.0wnz.at, Metasploit Framework
 * Descr. : This module implements functions for injecting DLLs into 32bit
 *          processes.
 * Changes: 01.04.2016  - Created
 */

#include "ldntvdm.h"
#include "basemsg64.h"
#include "injector32.h"

#ifdef _WIN64
#undef METHOD_APC
#undef METHOD_MODIFYSTARTUP
#endif


typedef NTSTATUS(NTAPI *pRtlInitUnicodeString)(PUNICODE_STRING, PCWSTR);
typedef NTSTATUS(NTAPI *pLdrLoadDll)(PWCHAR, ULONG, PUNICODE_STRING, PHANDLE);

typedef struct _THREAD_DATA
{
	pRtlInitUnicodeString fnRtlInitUnicodeString;
	pLdrLoadDll fnLdrLoadDll;
	UNICODE_STRING UnicodeString;
	WCHAR DllName[MAX_PATH];
	PWCHAR DllPath;
	ULONG Flags;
	LPTHREAD_START_ROUTINE OrigEIP;
	PVOID EIPParams;
	HANDLE ModuleHandle;
}THREAD_DATA, *PTHREAD_DATA;

EXTERN_C NTSTATUS NTAPI RtlCreateUserThread(
	HANDLE,
	PSECURITY_DESCRIPTOR,
	BOOLEAN,
	ULONG,
	PULONG,
	PULONG,
	PVOID,
	PVOID,
	PHANDLE,
	CLIENT_ID*);

static HANDLE WINAPI ThreadProc(PTHREAD_DATA data)
{
	data->fnRtlInitUnicodeString(&data->UnicodeString, data->DllName);
	data->fnLdrLoadDll(data->DllPath, data->Flags, &data->UnicodeString, &data->ModuleHandle);
	if (data->OrigEIP) return data->OrigEIP(data->EIPParams);
	return data->ModuleHandle;
}
static DWORD WINAPI dummy() { return 0; }

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

// The context used for injection via inject_via_apcthread
typedef struct _APCCONTEXT
{
	union
	{
		LPVOID lpStartAddress;
		BYTE bPadding1[8];
	} s;

	union
	{
		LPVOID lpParameter;
		BYTE bPadding2[8];
	} p;

	BYTE bExecuted;

} APCCONTEXT, *LPAPCCONTEXT;

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

#ifdef METHOD_INTERCEPTTHREAD
BOOL InjectDllHijackThread(HANDLE hProc, HANDLE hThread, char *DllName)
{
#ifdef _WIN64
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
#else
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
#endif
	//get the thread context
	CONTEXT ThreadContext;
	ThreadContext.ContextFlags = CONTEXT_FULL;
	GetThreadContext(hThread, &ThreadContext);

#ifdef _WIN64
	TRACE("Hijacked Thread currently @%X", ThreadContext.Rip);
#else
	TRACE("Hijacked Thread currently @%08X", ThreadContext.Eip);
#endif

	//allocate a remote buffer
	PBYTE InjData =
		(PBYTE)VirtualAllocEx(hProc, NULL, sizeof(code) + (strlen(DllName) + 1),
			MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
#ifdef _WIN64
	*(ULONG_PTR*)&code[17] = GetProcAddress(GetModuleHandle(_T("kernel32.dll")), "LoadLibraryA");;
	*(ULONG_PTR*)&code[27] = InjData + sizeof(code);
	*(ULONG_PTR*)&code[58] = ThreadContext.Rip;
#else
	*(ULONG_PTR*)&code[3] = GetProcAddress(GetModuleHandle(_T("kernel32.dll")), "LoadLibraryA");;
	*(ULONG_PTR*)&code[8] = InjData + sizeof(code);
	*(ULONG_PTR*)&code[17] = ThreadContext.Eip;
#endif

	//write the tmp buff + dll
	//Format: [RemoteFunction][DllName][null char]
	ULONG dwWritten;
	WriteProcessMemory(hProc, InjData, code, sizeof(code), &dwWritten);
	WriteProcessMemory(hProc, InjData + sizeof(code), DllName, (strlen(DllName) + 1), &dwWritten);

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

BOOL injectLdrLoadDLL(HANDLE hProcess, HANDLE hThread, WCHAR *szDLL, UCHAR method)
{
	THREAD_DATA data;
	PVOID pData, code;
	BOOL bRet = TRUE;
	ULONG ulSizeOfCode;
	NTSTATUS Status;
	HMODULE nt;
	CONTEXT ctx;

#ifdef METHOD_INTERCEPTTHREAD
	if (method == METHOD_INTERCEPTTHREAD)
	{
		if (!hThread)
		{
			if (!NT_SUCCESS(Status = NtGetNextThread(hProcess, NULL, THREAD_GET_CONTEXT | THREAD_SET_CONTEXT | THREAD_SUSPEND_RESUME, 0, 0, &hThread)))
			{
				TRACE("NtGetNextThread failed: %08X", Status);
				return FALSE;
			}
		}
		SuspendThread(hThread);
		bRet = InjectDllHijackThread(hProcess, hThread, "ldntvdm.dll");
		ResumeThread(hThread);
	}
#endif

#if defined(METHOD_MODIFYSTARTUP) || defined (METHOD_CREATETHREAD) || defined(METHOD_APC)

	nt = GetModuleHandleW(L"ntdll.dll");
	ulSizeOfCode = (LPBYTE)dummy - (LPBYTE)ThreadProc;

	ZeroMemory(&data, sizeof(data));
	data.fnRtlInitUnicodeString = (pRtlInitUnicodeString)GetProcAddress(nt, "RtlInitUnicodeString");
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

	if (!(pData = VirtualAllocEx(hProcess, NULL, sizeof(data), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE)) ||
		!(code = VirtualAllocEx(hProcess, NULL, ulSizeOfCode, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE)) ||
		!WriteProcessMemory(hProcess, pData, &data, sizeof(data), NULL) ||
		!WriteProcessMemory(hProcess, code, (PVOID)ThreadProc, ulSizeOfCode, NULL))
	{
		return FALSE;
	}
	switch (method)
	{
#ifdef METHOD_APC
	case METHOD_APC:
		return inject_via_apcthread(hProcess, hThread, code, pData);
#endif
#ifdef METHOD_CREATETHREAD
	case METHOD_CREATETHREAD:
		if (bRet = NT_SUCCESS(RtlCreateUserThread(hProcess, NULL, TRUE, 0, 0, 0, code, pData, &hThread, NULL)))
		{
			ResumeThread(hThread);
			WaitForSingleObject(hThread, INFINITE);
			CloseHandle(hThread);
		}
		VirtualFreeEx(hProcess, pData, 0, MEM_RELEASE);
		VirtualFreeEx(hProcess, code, 0, MEM_RELEASE);
		break;
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
