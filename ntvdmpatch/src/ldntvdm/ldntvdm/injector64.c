#ifdef _WIN64
#include "ldntvdm.h"
#include "injector.h"
#include "injector32.h"
#include "ntmmapi.h"
#include "detour.h"
#include "ntpeb.h"
#include <stddef.h>
#include <Psapi.h>

NTSTATUS NTAPI 	RtlQueueApcWow64Thread(_In_ HANDLE ThreadHandle, PVOID ApcRoutine, _In_opt_ PVOID NormalContext, _In_opt_ PVOID SystemArgument1, _In_opt_ PVOID SystemArgument2);

#ifdef TARGET_WINXP
#include "xpwrap.h"
#define WOW64_CS32   0x23
#define WOW64_TLS_CPURESERVED               1   // per-thread data for the CPU simulator
#endif

extern BYTE ThreadProcx86[];
extern unsigned int ThreadProcx86Size;
extern BYTE APCProcx86[];
extern unsigned int APCProcx86Size;
extern BYTE PostProcProcx86[];
extern unsigned int PostProcProcx86Size;
extern BYTE LdrpInitializeProcessProcx86[];
extern unsigned int LdrpInitializeProcessProcx86Size;

#pragma pack(1)
typedef struct _LDR_DATA_TABLE_ENTRY32
{
	LIST_ENTRY32 InLoadOrderModuleList;
	LIST_ENTRY32 InMemoryOrderModuleList;
	LIST_ENTRY32 InInitializationOrderModuleList;
	ULONG DllBase;
	ULONG EntryPoint;
	ULONG SizeOfImage;
	UNICODE_STRING32 FullDllName;
	UNICODE_STRING32 BaseDllName;
	ULONG Flags;
	USHORT LoadCount;
	USHORT TlsIndex;
	union
	{
		LIST_ENTRY32 HashLinks;
		PVOID SectionPointer;
	};
	ULONG CheckSum;
	union
	{
		ULONG TimeDateStamp;
		PVOID LoadedImports;
	};
	ULONG EntryPointActivationContext;
	ULONG PatchInformation;
} LDR_DATA_TABLE_ENTRY32, *PLDR_DATA_TABLE_ENTRY32;

typedef struct _PEB_LDR_DATA32
{
	ULONG Length;
	BOOL Initialized;
	ULONG SsHandle;
	LIST_ENTRY32 InLoadOrderModuleList;
	LIST_ENTRY32 InMemoryOrderModuleList;
	LIST_ENTRY32 InInitializationOrderModuleList;
} PEB_LDR_DATA32, *PPEB_LDR_DATA32;

#pragma pack()

typedef struct
{
	HANDLE hFile;
	HANDLE hMap;
	PBYTE lpMem;
} HMODULE32;

static BOOL getWow64PEB(HANDLE hProcess, PEB32 *pParamWow64PEB)
{
	PEB32 *pWow64PEB;
	NTSTATUS Status;

	/* query the process basic information (includes the PEB address) */
	if (!NT_SUCCESS(Status = NtQueryInformationProcess(hProcess, ProcessWow64Information, &pWow64PEB,
		sizeof(pWow64PEB), NULL)) ||
		/* get the address of the PE Loader data */
		!ReadProcessMemory(hProcess, pWow64PEB, pParamWow64PEB, sizeof(PEB32), NULL))
		return FALSE;
	return TRUE;
}

static BOOL isx64ProcessInitialized(HANDLE hProcess)
{
#ifdef TARGET_WINXP
	/* On Windows XP, we get called at ConnectConsoleInternal, Hijacking would be
	* possible, but it's too early, not all imports may have been bound at the time
	* when the console client library is loaded.
	*/
	return FALSE;
#else
	PEB32 Wow64PEB;

	if (!getWow64PEB(hProcess, &Wow64PEB))
		return TRUE;	// Better lie...
	TRACE("ProcessInitializing = %d\n", Wow64PEB.ProcessInitializing);
	return !Wow64PEB.ProcessInitializing;
#endif
}

void FreeLibrary32(HANDLE hModule)
{
	HMODULE32 *hMod = (HMODULE32*)hModule;

	UnmapViewOfFile(hMod->lpMem);
	CloseHandle(hMod->hMap);
	CloseHandle(hMod->hFile);
	HeapFree(GetProcessHeap(), 0, hMod);
}

HANDLE LoadLibrary32(WCHAR *pszFile)
{
	HMODULE32 *hMod = HeapAlloc(GetProcessHeap(), 0, sizeof(HMODULE32));

	hMod->hFile = CreateFile(pszFile, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

	if (hMod->hFile != INVALID_HANDLE_VALUE)
	{
		if (hMod->hMap = CreateFileMapping(hMod->hFile, NULL, PAGE_READONLY, 0, 0, NULL))
		{
			if (hMod->lpMem = MapViewOfFile(hMod->hMap, FILE_MAP_READ, 0, 0, 0))
			{
				return hMod;
			}
			CloseHandle(hMod->hMap);
		}
		CloseHandle(hMod->hFile);
	}
	HeapFree(GetProcessHeap(), 0, hMod);
	return NULL;
}

static DWORD GetProcAddress32(HANDLE hModule, char *lpProcName)
{
	PBYTE pBase = ((HMODULE32*)hModule)->lpMem;
	PIMAGE_DOS_HEADER DosHeader = (PIMAGE_DOS_HEADER)pBase;
	PIMAGE_NT_HEADERS32 NtHeaders = (PIMAGE_NT_HEADERS32)(pBase + DosHeader->e_lfanew);
	PIMAGE_EXPORT_DIRECTORY pExportDir = NULL;
	PIMAGE_SECTION_HEADER SectHdr = (PIMAGE_SECTION_HEADER)((PBYTE)NtHeaders + sizeof(IMAGE_NT_HEADERS32));
	DWORD EATAddr = NtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
	PDWORD pNamesArray, pAddressArray;
	PWORD pNameOrdinalsArray;
	DWORD i, j;

	for (i = 0; i < NtHeaders->FileHeader.NumberOfSections; i++)
	{
		if (EATAddr > SectHdr[i].VirtualAddress && EATAddr <= SectHdr[i].VirtualAddress + SectHdr[i].Misc.VirtualSize)
		{
			pExportDir = (PIMAGE_EXPORT_DIRECTORY)(EATAddr - SectHdr[i].VirtualAddress + SectHdr[i].PointerToRawData + pBase);
			pNamesArray = (PDWORD)(pExportDir->AddressOfNames - SectHdr[i].VirtualAddress + SectHdr[i].PointerToRawData + pBase);
			pNameOrdinalsArray = (PWORD)(pExportDir->AddressOfNameOrdinals - SectHdr[i].VirtualAddress + SectHdr[i].PointerToRawData + pBase);
			pAddressArray = (PDWORD)(pExportDir->AddressOfFunctions - SectHdr[i].VirtualAddress + SectHdr[i].PointerToRawData + pBase);
			break;
		}
	}
	if (pExportDir)
	{
		// Search for name of function in the NamesArray   
		for (j = 0; j < pExportDir->NumberOfNames; j++)
		{
			char *pszProc = (char *)(pBase + pNamesArray[j] - SectHdr[i].VirtualAddress + SectHdr[i].PointerToRawData);

			if (!_strcmp(pszProc, lpProcName))
				return pAddressArray[pNameOrdinalsArray[j]];
		}
	}
	return 0;
}

HMODULE GetRemoteModuleHandle32FromMemList(HANDLE ProcessHandle, LPWSTR lpDllName)
{
	MEMORY_BASIC_INFORMATION memInfo;
	size_t currentFrom = 0, len, fnlen;
	WCHAR fileNameBuffer[MAX_PATH], moduleName[MAX_PATH];

	wcscpy(moduleName, L"Syswow64\\");
	wcscat(moduleName, lpDllName);
	len = wcslen(moduleName);
	while (VirtualQueryEx(ProcessHandle, (void*)currentFrom, &memInfo, sizeof(memInfo)))
	{
		if (memInfo.Type == MEM_IMAGE)
		{
			if ((fnlen = GetMappedFileNameW(ProcessHandle, memInfo.BaseAddress, fileNameBuffer, sizeof(fileNameBuffer)/sizeof(fileNameBuffer[0]))) > 0)
			{
				if (fnlen >= len && !__wcsicmp(fileNameBuffer + fnlen - len, moduleName))
					return memInfo.BaseAddress;
				//TRACE("Mapped: %S @%X\n", fileNameBuffer, memInfo.BaseAddress);
			}
		}
		currentFrom += memInfo.RegionSize;
	}
	return NULL;
}

HMODULE GetRemoteModuleHandle32(HANDLE ProcessHandle, LPWSTR lpDllName)
{
	PEB32 *pWow64PEB, Wow64PEB;
	NTSTATUS Status;
	PEB_LDR_DATA32 *pMod, *pStart, LoaderData;
	LDR_DATA_TABLE_ENTRY32 ldrMod;
	WCHAR dllName[MAX_PATH + 1];

	/* query the process basic information (includes the PEB address) */
	if (!NT_SUCCESS(Status = NtQueryInformationProcess(ProcessHandle, ProcessWow64Information, &pWow64PEB,
		sizeof(pWow64PEB), NULL)))
	{
		TRACE("NtQueryInformationProcess failed: %08X\n", Status);
		return NULL;
	}

	if (!pWow64PEB)
	{
		TRACE("Not a WOW64 proces?!\n");
		return NULL;
	}
		/* get the address of the PE Loader data */
	if (!ReadProcessMemory(ProcessHandle, (LPCVOID)pWow64PEB, &Wow64PEB, sizeof(Wow64PEB), NULL))
	{
		TRACE("Reading PEB @%X failed: %08X\n", pWow64PEB, GetLastError());
		return NULL;
	}

	if (!Wow64PEB.Ldr)
	{
		/* This may be the case, if i.e. process isn't initialized yet (which is the case when process is
		 * started suspended). We have to find it in memory section list there instead.
		 */
		return GetRemoteModuleHandle32FromMemList(ProcessHandle, lpDllName);
	}

	if (!ReadProcessMemory(ProcessHandle, (LPCVOID)Wow64PEB.Ldr, &LoaderData, sizeof(LoaderData), NULL))
	{
		TRACE("Reading LoaderData @%X failed: %08X\n", Wow64PEB.Ldr, GetLastError());
		return NULL;
	}

	/* head of the module list: the last element in the list will point to this */
	pStart = pMod = (PEB_LDR_DATA32*)LoaderData.InLoadOrderModuleList.Flink;
	do
	{
		if (!ReadProcessMemory(ProcessHandle, pMod, &ldrMod, sizeof(ldrMod), NULL))
		{
			TRACE("Reading process list entry failed: %08X\n", GetLastError());
			return GetRemoteModuleHandle32FromMemList(ProcessHandle, lpDllName);
		}
		if (ldrMod.DllBase &&
			ReadProcessMemory(ProcessHandle, (LPCVOID)ldrMod.BaseDllName.Buffer, dllName, min(ldrMod.BaseDllName.Length, sizeof(dllName) / sizeof(WCHAR)), NULL))
		{
			dllName[ldrMod.BaseDllName.Length / sizeof(WCHAR)] = 0;
			if (!__wcsicmp(dllName, lpDllName))
				return (HANDLE)ldrMod.DllBase;
		}
		pMod = (PEB_LDR_DATA32*)ldrMod.InLoadOrderModuleList.Flink;
	} while (pMod != pStart);
	return NULL;
}

HANDLE LoadLibraryWindir32(LPWSTR lpDllName)
{
	WCHAR dllName[MAX_PATH + 1];

	GetSystemWow64Directory(dllName, sizeof(dllName) / sizeof(WCHAR));
	wcscat(dllName, L"\\");
	wcscat(dllName, lpDllName);
	return LoadLibrary32(dllName);
}

DWORD GetRemoteProcAddressX32(HANDLE hRemoteMod, HANDLE hLocalMod, LPSTR lpProcName)
{
	DWORD dwRet;

	dwRet = GetProcAddress32(hLocalMod, lpProcName);
	return dwRet ? dwRet + (DWORD)hRemoteMod : 0;
}

DWORD GetLoadLibraryAddressX32(HANDLE ProcessHandle)
{
	HANDLE hRemoteMod = GetRemoteModuleHandle32(ProcessHandle, L"kernel32.dll");
	HANDLE hLibKernel32;
	DWORD dwRet = 0;

	if (hRemoteMod && (hLibKernel32 = LoadLibraryWindir32(L"kernel32.dll")))
	{
		dwRet = GetRemoteProcAddressX32(hRemoteMod, hLibKernel32, "LoadLibraryW");
		FreeLibrary32(hLibKernel32);
	}
	return dwRet;
}

#ifdef TARGET_WINXP
BOOL xpWow64GetThreadContext(PWOW64_CONTEXT *ppRemoteCtx, HANDLE hProc, HANDLE hThread, WOW64_CONTEXT *ThreadContext)
{
	// No Wow64GetThreadContext on Windows XP, so read it from TLS:
	// http://www.nynaeve.net/?p=191
	CONTEXT                  Ctx64;
	SIZE_T                   Transferred = 0;

	Ctx64.ContextFlags = CONTEXT_CONTROL;
	GetThreadContext(hThread, &Ctx64);
	if (Ctx64.SegCs != WOW64_CS32)
	{
		THREAD_BASIC_INFORMATION ThreadInfo;

		if (!NT_SUCCESS(NtQueryInformationThread(hThread, ThreadBasicInformation, &ThreadInfo, sizeof(THREAD_BASIC_INFORMATION), 0)) ||
			!ReadProcessMemory(hProc, &((PTEB)ThreadInfo.TebBaseAddress)->TlsSlots[WOW64_TLS_CPURESERVED], ppRemoteCtx, sizeof(PWOW64_CONTEXT), &Transferred) ||
			Transferred != sizeof(PWOW64_CONTEXT))
		{
			TRACE("Reading WOW64 thread context ptr failed: %d, Bytes: %d, Needed: %d\n", GetLastError(), Transferred, sizeof(PWOW64_CONTEXT));
			return FALSE;
		}
		if (!ReadProcessMemory(hProc, (PUCHAR)(*ppRemoteCtx)+sizeof(ULONG), ThreadContext, sizeof(WOW64_CONTEXT), &Transferred) ||
			Transferred != sizeof(WOW64_CONTEXT))
		{
			TRACE("Reading WOW64 thread context @%08X failed: %d, Bytes: %d, Needed: %d, TebBaseAddress: %08X, Ptr: %08X\n", 
				(PUCHAR)(*ppRemoteCtx)+sizeof(ULONG), GetLastError(), Transferred, sizeof(WOW64_CONTEXT), 
				ThreadInfo.TebBaseAddress, &((PTEB)ThreadInfo.TebBaseAddress)->TlsSlots[WOW64_TLS_CPURESERVED]);
			return FALSE;
		}
	}
	else
	{
		//ThreadContext.Eip = (ULONG)Ctx64.Rip;

		// When thread is in 64bit code/syscall, that may go wrong...
		TRACE("Reading WOW64 thread context failed: In Syscall\n");
		return FALSE;
	}
	return TRUE;
}


BOOL xpWow64SetThreadContext(PWOW64_CONTEXT RemoteCtx, HANDLE hProc, WOW64_CONTEXT *ThreadContext)
{
	SIZE_T                   Transferred = 0;

	if (RemoteCtx)
	{
		if (WriteProcessMemory(hProc, (PUCHAR)(RemoteCtx)+sizeof(ULONG), ThreadContext, sizeof(WOW64_CONTEXT), &Transferred) &&
			Transferred == sizeof(ThreadContext))
		{
			TRACE("Success.\n");
			return TRUE;
		}
		TRACE("Writing memory failed: %d\n", GetLastError());
		return FALSE;
	}
	TRACE("Cannot write back x64 Context: RemoteCtx=%08X, Transferred=%08X", RemoteCtx, Transferred);
	return FALSE;
}
#endif  // TARGET_WINXP

#ifdef METHOD_APC
extern BYTE *apc_stub_x86;
extern int apc_stub_x86_size;

BOOL inject_via_apcthread_wow64(HANDLE hProcess, HANDLE hThread, LPVOID lpStartAddress, LPVOID lpParameter)
{
	LPVOID lpApcStub;
	LPVOID lpRemoteApcStub, lpRemoteApcContext;
	APCCONTEXT ctx = { 0 };
	DWORD dwApcStubLength = 0;

	ctx.s.lpStartAddress = lpStartAddress;
	ctx.p.lpParameter = lpParameter;
	ctx.bExecuted = FALSE;

	// Get the architecture specific apc inject stub...
	lpApcStub = apc_stub_x86;
	dwApcStubLength = apc_stub_x86_size;

	if (!(lpRemoteApcStub = VirtualAllocEx(hProcess, NULL, dwApcStubLength + sizeof(APCCONTEXT), MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE)))
		return FALSE;
	lpRemoteApcContext = ((BYTE *)lpRemoteApcStub + dwApcStubLength);
	if (!WriteProcessMemory(hProcess, lpRemoteApcStub, lpApcStub, dwApcStubLength, NULL) ||
		!WriteProcessMemory(hProcess, lpRemoteApcContext, (LPCVOID)&ctx, sizeof(APCCONTEXT), NULL))
		return FALSE;

	return RtlQueueApcWow64Thread(hThread, (PVOID)lpRemoteApcStub, (ULONG_PTR)lpRemoteApcContext, NULL, NULL);
}
#endif

BOOL InjectDllHijackThreadX32(HANDLE hProc, HANDLE hThread, WCHAR *DllName)
{
	//get the thread context
	WOW64_CONTEXT ThreadContext;
	DWORD InjData, lpLoadLibraryW, dwSize;
	BOOL fGotThreadCtx = FALSE;
#ifdef TARGET_WINXP
	PWOW64_CONTEXT           RemoteCtx = NULL;

	fGotThreadCtx = xpWow64GetThreadContext(&RemoteCtx, hProc, hThread, &ThreadContext);
#else  // TARGET_WINXP
	ThreadContext.ContextFlags = CONTEXT_CONTROL;
	fGotThreadCtx = Wow64GetThreadContext(hThread, &ThreadContext);
#endif // TARGET_WINXP
	TRACE("InjectDllHijackThreadX32(%08X, %08X, %S)...", hProc, hThread, DllName);


	if (!(lpLoadLibraryW = GetLoadLibraryAddressX32(hProc)) ||
		!(InjData = InjectWriteLoadLibraryShellcodex86(hProc, lpLoadLibraryW, ThreadContext.Eip, DllName, &dwSize)))
		return FALSE;

	if (!fGotThreadCtx || !isx64ProcessInitialized(hProc))
	{
		/* process has not yet been initialized... As hijacking threads is a bit dangerous anyway,
		* we back out by queuing an APC instead. NtTestAlert() call in loader should call us just
		* in time in this case... */
		NTSTATUS Status;

		Status = RtlQueueApcWow64Thread(hThread, (PVOID)lpLoadLibraryW, (PVOID)((ULONG_PTR)InjData + dwSize), NULL, NULL);
		if (NT_SUCCESS(Status))
		{
			TRACE("Queued loader injection via APC @%08X\n", lpLoadLibraryW);
			return TRUE;
		}
		else
		{
			TRACE("QueueUserAPC failed: %d\n", Status);
		}
	}
	else
	{
		// FIXME: Too dangerous?
		//return FALSE;
	}

	//set the EIP
	ThreadContext.Eip = (ULONG)InjData;
#ifdef TARGET_WINXP
	if (!xpWow64SetThreadContext(RemoteCtx, hProc, &ThreadContext)) return FALSE;
#else
	if (!Wow64SetThreadContext(hThread, &ThreadContext)) return FALSE;
#endif
	return TRUE;
}

HANDLE InjectLdntvdmWow64RemoteThread(HANDLE hProcess)
{
	LPTHREAD_START_ROUTINE pLoadLibraryW;

	TRACE("InjectLdntvdmWow64RemoteThread(%08X)...", hProcess);
	if (pLoadLibraryW = (LPTHREAD_START_ROUTINE)GetLoadLibraryAddressX32(hProcess))
	{
		PBYTE *pLibRemote;

		if (pLibRemote = VirtualAllocEx(hProcess, NULL, sizeof(LDNTVDM_NAME), MEM_COMMIT, PAGE_READWRITE))
		{
			HANDLE hThread;

			WriteProcessMemory(hProcess, pLibRemote, (void*)LDNTVDM_NAME, sizeof(LDNTVDM_NAME), NULL);
			hThread = CreateRemoteThread(hProcess, NULL, 0, pLoadLibraryW, pLibRemote, 0, NULL);
			if (hThread) {TRACE("hThread = %08X\n", hThread);}  else { TRACE("CreateRemoteThread failed with\n", GetLastError()); }
			return hThread;
		}
		else { TRACE("VirtualAlloc failed\n"); }
	}
	else { TRACE("GetLoadLibraryAddressX32 failed\n"); }
	return NULL;
}

DWORD WINAPI InjectLdntvdmWow64UsingRemoteThread(HANDLE hProcess)
{
	NTSTATUS Status;
	HANDLE hThread;

	if (NT_SUCCESS((Status = NtGetNextThread(hProcess, NULL, THREAD_GET_CONTEXT | THREAD_SET_CONTEXT | THREAD_SUSPEND_RESUME, 0, 0, &hThread))))
	{
		HANDLE hNewThread;

		SuspendThread(hThread);
		if (hNewThread = InjectLdntvdmWow64RemoteThread(hProcess))
		{
			int i;

			for (i = 0; i < 20; i++)
			{
				if (WaitForSingleObject(hNewThread, 50) == WAIT_OBJECT_0) break;
				InjectDllHijackThreadX32(hProcess, hThread, LDNTVDM_NAME);
				ResumeThread(hThread);
				break;
			}
			CloseHandle(hNewThread);
		}
		ResumeThread(hThread);
		CloseHandle(hThread);
		return TRUE;
	}
	else { TRACE("NtGetThread returned Status %08X\n", Status); }
	return FALSE;
}

DWORD WINAPI InjectLdntvdmWow64Thread(LPVOID lpPID)
{
	HANDLE hProcess;
	int i;
	HANDLE hThread;

	for (i = 0; i < 20; i++)
	{
		if (hProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION, FALSE, (DWORD)lpPID))
		{
			hThread = InjectLdntvdmWow64RemoteThread(hProcess);
			CloseHandle(hProcess);
			if (hThread)
			{
				CloseHandle(hThread);
				break;
			}
		}
		else break;
		Sleep(250);
	}
	ExitThread(0);
}

BOOL injectLdrLoadDLLWow64(HANDLE hProcess, HANDLE hThread, WCHAR *szDLL, UCHAR method)
{
	THREAD_DATA32 data;
	PEB32 *pWow64PEB;
	PVOID pData, code;
	PBYTE pProc;
	BOOL bRet = TRUE;
	ULONG ulSizeOfCode;
	NTSTATUS Status;
	HMODULE ntRemote;
	HANDLE ntLocal;
	WOW64_CONTEXT ctx, *RemoteCtx;
	static ULONG fnLdrpInitializeProcess = NULL;

#ifdef METHOD_INTERCEPTTHREAD
	if (method == METHOD_INTERCEPTTHREAD)
	{
		bRet = InjectLdntvdmWow64RemoteThread(hProcess);
		if (bRet) return TRUE;
		method = METHOD_CREATETHREAD;
	}
#endif

#if defined(METHOD_MODIFYSTARTUP) || defined (METHOD_CREATETHREAD) || defined(METHOD_APC) || defined(METHOD_POSTPROCESSINIT) || defined(METHOD_HOOKLDR)


	if (!(ntRemote = GetRemoteModuleHandle32(hProcess, L"ntdll.dll")))
	{
		TRACE("GetRemoteModuleHandle32 failed.\n");
		return FALSE;
	}
	if (!(ntLocal = LoadLibraryWindir32(L"ntdll.dll")))
	{
		TRACE("LoadLibraryWindir32 failed.\n");
		return FALSE;
	}

	ZeroMemory(&data, sizeof(data));
	data.fnLdrLoadDll = (pLdrLoadDll)GetRemoteProcAddressX32(ntRemote, ntLocal, "LdrLoadDll");
	lstrcpyW(data.DllName, szDLL);

#ifdef METHOD_MODIFYSTARTUP
	switch (method)
	{
	case METHOD_MODIFYSTARTUP:
#ifdef TARGET_WINXP
		bRet = xpWow64GetThreadContext(&RemoteCtx, hProcess, hThread, &ctx);
#else  // TARGET_WINXP
		ctx.ContextFlags = CONTEXT_INTEGER;
		bRet = Wow64GetThreadContext(hThread, &ctx);
#endif // TARGET_WINXP
		if (bRet)
		{
			data.OrigEIP = ctx.Eax;
			data.EIPParams = ctx.Ebx;
		}
		else
		{
			OutputDebugStringA("GetThreadContext failed");
		}
		break;
	}
#endif

	switch (method)
	{
#ifdef METHOD_CREATETHREAD
	case METHOD_CREATETHREAD:
		// TODO: data hat 64bit layout, angleichen!!
		data.fnNtTerminateThread = GetRemoteProcAddressX32(ntRemote, ntLocal, "NtTerminateThread");
		pProc = ThreadProcx86;
		ulSizeOfCode = ThreadProcx86Size;
		break;
#endif
#ifdef METHOD_HOOKLDR
	case METHOD_HOOKLDR:
	{
		pProc = LdrpInitializeProcessProcx86;
		ulSizeOfCode = LdrpInitializeProcessProcx86Size;
		if (!fnLdrpInitializeProcess)
		{
			DWORD64 dwAddress;
#ifdef USE_SYMCACHE
			NTSTATUS Status;
			HKEY hKey;

			if (NT_SUCCESS(Status = REG_OpenLDNTVDMWOW64(KEY_READ | KEY_WRITE, &hKey)))
			{
				if (SymCache_GetDLLKeyWOW64(hKey, L"ntdll.dll", FALSE) &&
					(dwAddress = (DWORD64)SymCache_GetProcAddress(hKey, L"LdrpInitializeProcess")))
				{
					dwAddress += (DWORD64)ntRemote;
					fnLdrpInitializeProcess = dwAddress;
				}
				REG_CloseKey(hKey);
			}
			else
			{
				TRACE("Cannot open WOW64 ldntvdm node: %08X\n", Status);
			}
#else
			DWORD64 dwBase;
			char szPath[MAX_PATH];

			strcpy(szPath + GetWindowsDirectoryA(szPath, MAX_PATH), "\\SysWOW64\\ntdll.dll");
			if (SymEng_LoadModule(szPath, &dwBase) == 0)
			{
				if ((dwAddress = SymEng_GetAddr(dwBase, "LdrpInitializeProcess")))
				{
					dwAddress += (DWORD64)ntRemote;
					fnLdrpInitializeProcess = dwAddress;
				}
				else
				{
					OutputDebugStringA("NTVDM: Resolving symbols failed.");
				}
				SymEng_UnloadModule(dwBase);
			}
#endif
		}
		if (!fnLdrpInitializeProcess)
		{
			TRACE("Cannot resolve LdrpInitializeProcess\n");
			return FALSE;
		}
		break;
	}
#endif
#ifdef METHOD_POSTPROCESSINIT
	case METHOD_POSTPROCESSINIT:
	{
		DWORD result;
		// Get remote PEB
		if (!NT_SUCCESS(Status = NtQueryInformationProcess(hProcess, ProcessWow64Information, &pWow64PEB, sizeof(pWow64PEB), NULL)))
		{
			TRACE("NtQueryInformationProcess failed with %08X\n", Status);
			return FALSE;
		}
		pProc = (PVOID)PostProcProcx86;
		ulSizeOfCode = PostProcProcx86Size;
		ReadProcessMemory(hProcess, (PVOID)((ULONG_PTR)pWow64PEB + offsetof(PEB32, PostProcessInitRoutine)), (PVOID)&data.fnOrigPostProcessInit, sizeof(data.fnOrigPostProcessInit), &result);
		break;
	}
#endif
	default:
		pProc = APCProcx86;
		ulSizeOfCode = APCProcx86Size;
		break;
	}


	if (!(code = VirtualAllocEx(hProcess, NULL, ulSizeOfCode + sizeof(data), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE)))
	{
		TRACE("Cannot allocate code page: %08X", GetLastError());
		return FALSE;
	}
	pData = (LPBYTE)code + ulSizeOfCode;
#ifdef METHOD_POSTPROCESSINIT
	if (method == METHOD_POSTPROCESSINIT) data.EIPParams = pData;
#endif
#ifdef METHOD_HOOKLDR
	if (method == METHOD_HOOKLDR)
	{
		if (!(data.fnLdrpInitializeProcess = Hook_Inline32(hProcess, ntRemote, fnLdrpInitializeProcess, code)))
		{
			TRACE("Hook_Inline32 failed.\n");
			return FALSE;
		}
		data.EIPParams = pData;
	}
#endif
	data.UnicodeString.Length = (USHORT)wcslen(data.DllName) * sizeof(WCHAR);
	data.UnicodeString.MaximumLength = sizeof(data.DllName);
	data.UnicodeString.Buffer = (PWSTR)((PBYTE)pData + offsetof(THREAD_DATA32, DllName));
	if (!WriteProcessMemory(hProcess, pData, &data, sizeof(data), NULL))
	{
		TRACE("Cannot write pData @%x: %08X", pData, GetLastError());
		return FALSE;
	}
	if (!WriteProcessMemory(hProcess, code, (PVOID)pProc, ulSizeOfCode, NULL))
	{
		TRACE("Cannot write code 1 @%x (%d bytes): %08X\n", code, ulSizeOfCode, GetLastError());
		return FALSE;
	}
	switch (method)
	{
#ifdef METHOD_APC
	case METHOD_APC:
		return inject_via_apcthread_wow64(hProcess, hThread, code, pData);
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
			if (*((PDWORD)&((PBYTE)pProc)[i]) == ((DWORD)1 << (sizeof(DWORD) * 8 - 1)))
			{
				DWORD pDataPtr = (DWORD)((PBYTE)pData + offsetof(THREAD_DATA32, EIPParams));
				if (!WriteProcessMemory(hProcess, (LPVOID)((DWORD)code + i), (PVOID)&pDataPtr, sizeof(pDataPtr), NULL))
				{
					TRACE("Cannot write code @%x: %08X\n", (DWORD)code + i, GetLastError());
					return FALSE;
				}
				break;
			}
		if (i == ulSizeOfCode) return FALSE;

		if (method == METHOD_POSTPROCESSINIT)
		{
			// Modify PostProcessInitRoutine to point to our routine
			if (!WriteProcessMemory(hProcess, (PVOID)((DWORD)pWow64PEB + offsetof(PEB32, PostProcessInitRoutine)), (PVOID)&code, sizeof(code), NULL))
			{
				TRACE("Cannot write patch: %08X\n", GetLastError());
				return FALSE;
			}
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
#ifdef TARGET_WINXP
		bRet = xpWow64SetThreadContext(RemoteCtx, hProcess, &ctx);
#else
		ctx.ContextFlags = CONTEXT_INTEGER;
		bRet = Wow64SetThreadContext(hThread, &ctx);
#endif
		if (!bRet)
			OutputDebugStringA("SetThreadContext failed.");
		break;
	}
#endif
	}
#endif

	FreeLibrary32(ntLocal);

	return bRet;
}


#endif
