/**
*
* Using parts of the WOW64Ext Library
*
* Copyright (c) 2014 ReWolf
* http://blog.rewolf.pl/
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published
* by the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*/

#include <windows.h>
#include <stddef.h>
#include "ldntvdm.h"
#include "Winternl.h"
#include "wow64int.h"
#include "wow64ext.h"
#include "ntpeb.h"

typedef struct tag_CLIENT_ID64
{
	ULONGLONG UniqueProcess;
	ULONGLONG UniqueThread;
} CLIENT_ID64;

typedef struct tag_TEB64
{
	NT_TIB64 NtTib;
	ULONGLONG EnvironmentPointer;
	CLIENT_ID64 ClientId;
	ULONGLONG ActiveRpcHandle;
	ULONGLONG ThreadLocalStoragePointer;
	ULONGLONG ProcessEnvironmentBlock;
	DWORD LastErrorValue;
	DWORD CountOfOwnedCriticalSections;
	ULONGLONG CsrClientThread;
	ULONGLONG Win32ThreadInfo;
	DWORD User32Reserved[26];
	//rest of the structure is not defined for now, as it is not needed
} TEB64;

typedef struct tag_LDR_DATA_TABLE_ENTRY64
{
	LIST_ENTRY64 InLoadOrderLinks;
	LIST_ENTRY64 InMemoryOrderLinks;
	LIST_ENTRY64 InInitializationOrderLinks;
	ULONGLONG DllBase;
	ULONGLONG EntryPoint;
	union
	{
		DWORD SizeOfImage;
		ULONGLONG dummy01;
	};
	UNICODE_STRING64 FullDllName;
	UNICODE_STRING64 BaseDllName;
	DWORD Flags;
	WORD LoadCount;
	WORD TlsIndex;
	union
	{
		LIST_ENTRY64 HashLinks;
		struct
		{
			ULONGLONG SectionPointer;
			ULONGLONG CheckSum;
		};
	};
	union
	{
		ULONGLONG LoadedImports;
		DWORD TimeDateStamp;
	};
	ULONGLONG EntryPointActivationContext;
	ULONGLONG PatchInformation;
	LIST_ENTRY64 ForwarderLinks;
	LIST_ENTRY64 ServiceTagLinks;
	LIST_ENTRY64 StaticLinks;
	ULONGLONG ContextInformation;
	ULONGLONG OriginalBase;
	LARGE_INTEGER LoadTime;
} LDR_DATA_TABLE_ENTRY64;

typedef struct tag_PEB_LDR_DATA64
{
	DWORD Length;
	DWORD Initialized;
	ULONGLONG SsHandle;
	LIST_ENTRY64 InLoadOrderModuleList;
	LIST_ENTRY64 InMemoryOrderModuleList;
	LIST_ENTRY64 InInitializationOrderModuleList;
	ULONGLONG EntryInProgress;
	DWORD ShutdownInProgress;
	ULONGLONG ShutdownThreadId;
} PEB_LDR_DATA64;


#ifndef _WIN64

ULONGLONG __cdecl X64Call(ULONGLONG func, int argC, ...)
{
	va_list args;
	reg64 _rcx, _rdx, _r8, _r9, _rax, restArgs, _argC;
	DWORD back_esp = 0;

	va_start(args, argC);
	_rcx.v = (argC > 0) ? argC--, va_arg(args, ULONGLONG) : 0;
	_rdx.v = (argC > 0) ? argC--, va_arg(args, ULONGLONG) : 0;
	_r8.v = (argC > 0) ? argC--, va_arg(args, ULONGLONG) : 0;
	_r9.v = (argC > 0) ? argC--, va_arg(args, ULONGLONG) : 0;
	_rax.v = 0;
	restArgs.v = (ULONGLONG)&va_arg(args, ULONGLONG);

	// conversion to QWORD for easier use in inline assembly
	_argC.v = (ULONGLONG)argC;

	__asm
	{
		;// keep original esp in back_esp variable
		mov    back_esp, esp

		;// align esp to 0x10, without aligned stack some syscalls may return errors !
		;// (actually, for syscalls it is sufficient to align to 8, but SSE opcodes 
		;// requires 0x10 alignment), it will be further adjusted according to the
		;// number of arguments above 4
		and    esp, 0xFFFFFFF0

			X64_Start();

		;// below code is compiled as x86 inline asm, but it is executed as x64 code
		;// that's why it need sometimes REX_W() macro, right column contains detailed
		;// transcription how it will be interpreted by CPU

		;// fill first four arguments
		REX_W mov    ecx, _rcx.dw[0];// mov     rcx, qword ptr [_rcx]
		REX_W mov    edx, _rdx.dw[0];// mov     rdx, qword ptr [_rdx]
		push   _r8.v;// push    qword ptr [_r8]
		X64_Pop(_R8); ;// pop     r8
		push   _r9.v;// push    qword ptr [_r9]
		X64_Pop(_R9); ;// pop     r9
		;//
		REX_W mov    eax, _argC.dw[0];// mov     rax, qword ptr [_argC]
		;// 
		;// final stack adjustment, according to the    ;//
		;// number of arguments above 4                 ;// 
		test   al, 1;// test    al, 1
		jnz    _no_adjust;// jnz     _no_adjust
		sub    esp, 8;// sub     rsp, 8
	_no_adjust:;//
		;// 
		push   edi;// push    rdi
		REX_W mov    edi, restArgs.dw[0];// mov     rdi, qword ptr [restArgs]
		;// 
		;// put rest of arguments on the stack          ;// 
		REX_W test   eax, eax;// test    rax, rax
		jz     _ls_e;// je      _ls_e
		REX_W lea    edi, dword ptr[edi + 8 * eax - 8];// lea     rdi, [rdi + rax*8 - 8]
		;// 
	_ls:;// 
		REX_W test   eax, eax;// test    rax, rax
		jz     _ls_e;// je      _ls_e
		push   dword ptr[edi];// push    qword ptr [rdi]
		REX_W sub    edi, 8;// sub     rdi, 8
		REX_W sub    eax, 1;// sub     rax, 1
		jmp    _ls;// jmp     _ls
	_ls_e:;// 
		;// 
		;// create stack space for spilling registers   ;// 
		REX_W sub    esp, 0x20;// sub     rsp, 20h
		;// 
		call   func;// call    qword ptr [func]
		;// 
		;// cleanup stack                               ;// 
		REX_W mov    ecx, _argC.dw[0];// mov     rcx, qword ptr [_argC]
		REX_W lea    esp, dword ptr[esp + 8 * ecx + 0x20];// lea     rsp, [rsp + rcx*8 + 20h]
		;// 
		pop    edi;// pop     rdi
		;// 
		 // set return value                             ;// 
		REX_W mov    _rax.dw[0], eax;// mov     qword ptr [_rax], rax

		X64_End();

		mov    ax, ds
			mov    ss, ax
			mov    esp, back_esp
	}
	return _rax.v;
}

static void getMem64(void* dstMem, ULONGLONG srcMem, size_t sz)
{
	reg64 _src;

	_src.v = srcMem;
	if ((NULL == dstMem) || (0 == srcMem) || (0 == sz))
		return;

	__asm
	{
		X64_Start();

		;// below code is compiled as x86 inline asm, but it is executed as x64 code
		;// that's why it need sometimes REX_W() macro, right column contains detailed
		;// transcription how it will be interpreted by CPU

		push   edi;// push     rdi
		push   esi;// push     rsi
		;//
		mov    edi, dstMem;// mov      edi, dword ptr [dstMem]        ; high part of RDI is zeroed
		REX_W mov    esi, _src.dw[0];// mov      rsi, qword ptr [_src]
		mov    ecx, sz;// mov      ecx, dword ptr [sz]            ; high part of RCX is zeroed
		;//
		mov    eax, ecx;// mov      eax, ecx
		and    eax, 3;// and      eax, 3
		shr    ecx, 2;// shr      ecx, 2
		;//
		rep    movsd;// rep movs dword ptr [rdi], dword ptr [rsi]
		;//
		test   eax, eax;// test     eax, eax
		//je     _move_0;// je       _move_0
		EMIT(0x74) EMIT(0x0D)
		cmp    eax, 1;// cmp      eax, 1
		//je     _move_1;// je       _move_1
		EMIT(0x74) EMIT(0x07)
		;//
		movsw;// movs     word ptr [rdi], word ptr [rsi]
		cmp    eax, 2;// cmp      eax, 2
		//je     _move_0;// je       _move_0
		EMIT(0x74) EMIT(0x01)
		;//
	_move_1:;//
		movsb;// movs     byte ptr [rdi], byte ptr [rsi]
		;//
	_move_0:;//
		pop    esi;// pop      rsi
		pop    edi;// pop      rdi

		X64_End();
	}
}

static BOOL cmpMem64(void* dstMem, ULONGLONG srcMem, size_t sz)
{
	BOOL result = FALSE;
	reg64 _src = { srcMem };

	if ((NULL == dstMem) || (0 == srcMem) || (0 == sz))
		return FALSE;

	__asm
	{
		X64_Start();

		;// below code is compiled as x86 inline asm, but it is executed as x64 code
		;// that's why it need sometimes REX_W() macro, right column contains detailed
		;// transcription how it will be interpreted by CPU

		push   edi;// push      rdi
		push   esi;// push      rsi
		;//           
		mov    edi, dstMem;// mov       edi, dword ptr [dstMem]       ; high part of RDI is zeroed
		REX_W mov    esi, _src.dw[0];// mov       rsi, qword ptr [_src]
		mov    ecx, sz;// mov       ecx, dword ptr [sz]           ; high part of RCX is zeroed
		;//           
		mov    eax, ecx;// mov       eax, ecx
		and    eax, 3;// and       eax, 3
		shr    ecx, 2;// shr       ecx, 2
		;// 
		repe   cmpsd;// repe cmps dword ptr [rsi], dword ptr [rdi]
		jnz     _ret_false;// jnz       _ret_false
		;// 
		test   eax, eax;// test      eax, eax
		je     _move_0;// je        _move_0
		cmp    eax, 1;// cmp       eax, 1
		je     _move_1;// je        _move_1
		;// 
		cmpsw;// cmps      word ptr [rsi], word ptr [rdi]
		jnz     _ret_false;// jnz       _ret_false
		cmp    eax, 2;// cmp       eax, 2
		je     _move_0;// je        _move_0
		;// 
	_move_1:;// 
		cmpsb;// cmps      byte ptr [rsi], byte ptr [rdi]
		jnz     _ret_false;// jnz       _ret_false
		;// 
	_move_0:;// 
		mov    result, 1;// mov       byte ptr [result], 1
		;// 
	_ret_false:;// 
		pop    esi;// pop      rsi
		pop    edi;// pop      rdi

		X64_End();
	}

	return result;
}

static ULONGLONG getTEB64()
{
	reg64 reg;
	reg.v = 0;

	X64_Start();
	// R12 register should always contain pointer to TEB64 in WoW64 processes
	X64_Push(_R12);
	// below pop will pop QWORD from stack, as we're in x64 mode now
	__asm pop reg.dw[0]
		X64_End();

	return reg.v;
}

ULONGLONG  GetModuleHandle64(wchar_t* lpModuleName)
{
	TEB64 teb64;
	PEB64 peb64;
	PEB_LDR_DATA64 ldr;
	LDR_DATA_TABLE_ENTRY64 head;
	ULONGLONG LastEntry;
	wchar_t tempBuf[MAX_PATH];

	getMem64(&teb64, getTEB64(), sizeof(TEB64));
	getMem64(&peb64, teb64.ProcessEnvironmentBlock, sizeof(PEB64));
	getMem64(&ldr, peb64.Ldr, sizeof(PEB_LDR_DATA64));

	LastEntry = peb64.Ldr + offsetof(PEB_LDR_DATA64, InLoadOrderModuleList);
	head.InLoadOrderLinks.Flink = ldr.InLoadOrderModuleList.Flink;
	do
	{
		getMem64(&head, head.InLoadOrderLinks.Flink, sizeof(LDR_DATA_TABLE_ENTRY64));
		getMem64(tempBuf, head.BaseDllName.Buffer, min(head.BaseDllName.MaximumLength,MAX_PATH));
		if (0 == __wcsicmp(lpModuleName, tempBuf))
			return head.DllBase;
	} while (head.InLoadOrderLinks.Flink != LastEntry);

	return 0;
}

ULONGLONG getNTDLL64()
{
	static ULONGLONG ntdll64 = 0;
	if (0 != ntdll64)
		return ntdll64;

	ntdll64 = GetModuleHandle64(L"ntdll.dll");
	return ntdll64;
}

static ULONGLONG getLdrGetProcedureAddress()
{
	ULONGLONG modBase = getNTDLL64(), ret = 0;
	IMAGE_DOS_HEADER idh;
	IMAGE_NT_HEADERS64 inh;
	IMAGE_DATA_DIRECTORY *idd;
	IMAGE_EXPORT_DIRECTORY ied;
	DWORD* rvaTable, *nameTable, i, dwSize;
	WORD* ordTable;

	if (0 == modBase)
		return 0;

	getMem64(&idh, modBase, sizeof(idh));
	getMem64(&inh, modBase + idh.e_lfanew, sizeof(IMAGE_NT_HEADERS64));
	idd = &inh.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];

	if (0 == idd->VirtualAddress)
		return 0;

	getMem64(&ied, modBase + idd->VirtualAddress, sizeof(ied));

	if (rvaTable = (DWORD*)HeapAlloc(GetProcessHeap(), 0, (dwSize = sizeof(DWORD)*ied.NumberOfFunctions)))
	{
		getMem64(rvaTable, modBase + (ULONGLONG)ied.AddressOfFunctions, dwSize);

		if (ordTable = (WORD*)HeapAlloc(GetProcessHeap(), 0, (dwSize = sizeof(WORD)*ied.NumberOfFunctions)))
		{
			getMem64(ordTable, modBase + (ULONGLONG)ied.AddressOfNameOrdinals, dwSize);

			if (nameTable = (DWORD*)HeapAlloc(GetProcessHeap(), 0, (dwSize = sizeof(DWORD)*ied.NumberOfNames)))
			{
				getMem64(nameTable, modBase + (ULONGLONG)ied.AddressOfNames, dwSize);

				// lazy search, there is no need to use binsearch for just one function
				for (i = 0; i < ied.NumberOfFunctions; i++)
				{
					if (cmpMem64("LdrGetProcedureAddress", modBase + (ULONGLONG)nameTable[i], sizeof("LdrGetProcedureAddress")))
					{
						ret = modBase + (ULONGLONG)rvaTable[ordTable[i]];
						break;
					}
				}
				HeapFree(GetProcessHeap(), 0, nameTable);
			}
			HeapFree(GetProcessHeap(), 0, ordTable);
		}
		HeapFree(GetProcessHeap(), 0, rvaTable);
	}
	return ret;
}

ULONG __stdcall BaseSetLastNTError(NTSTATUS NtStatus)
{

  ULONG ret = RtlNtStatusToDosError(NtStatus);
  SetLastError(ret);
  return ret;
}


ULONGLONG  GetProcAddress64(ULONGLONG hModule, char* funcName)
{
	static ULONGLONG _LdrGetProcedureAddress = 0;
	UNICODE_STRING64 fName = { 0 };
	ULONGLONG funcRet = 0;

	if (0 == _LdrGetProcedureAddress)
	{
		_LdrGetProcedureAddress = getLdrGetProcedureAddress();
		if (0 == _LdrGetProcedureAddress)
			return 0;
	}

	fName.Buffer = (ULONGLONG)funcName;
	fName.Length = (WORD)strlen(funcName);
	fName.MaximumLength = fName.Length + 1;
	X64Call(_LdrGetProcedureAddress, 4, (ULONGLONG)hModule, (ULONGLONG)&fName, (ULONGLONG)0, (ULONGLONG)&funcRet);
	return funcRet;
}

ULONGLONG  VirtualAllocEx64(HANDLE hProcess, ULONGLONG lpAddress, DWORD dwSize, DWORD flAllocationType, DWORD flProtect)
{
	static ULONGLONG ntavm = 0;
	ULONGLONG ret, tmpAddr = lpAddress, tmpSize = dwSize;

	if (0 == ntavm)
	{
		ntavm = GetProcAddress64(getNTDLL64(), "NtAllocateVirtualMemory");
		if (0 == ntavm)
			return 0;
	}

	ret = X64Call(ntavm, 6, (ULONGLONG)hProcess, (ULONGLONG)&tmpAddr, (ULONGLONG)0, (ULONGLONG)&tmpSize, (ULONGLONG)flAllocationType, (ULONGLONG)flProtect);
	if (STATUS_SUCCESS != ret)
	{
		BaseSetLastNTError((NTSTATUS)ret);
		return FALSE;
	}
	else
		return tmpAddr;
}

BOOL  VirtualFreeEx64(HANDLE hProcess, ULONGLONG lpAddress, DWORD dwSize, DWORD dwFreeType)
{
	static ULONGLONG ntfvm = 0;
	ULONGLONG ret, tmpAddr = lpAddress, tmpSize = dwSize;

	if (0 == ntfvm)
	{
		ntfvm = GetProcAddress64(getNTDLL64(), "NtFreeVirtualMemory");
		if (0 == ntfvm)
			return 0;
	}

	ret = X64Call(ntfvm, 4, (ULONGLONG)hProcess, (ULONGLONG)&tmpAddr, (ULONGLONG)&tmpSize, (ULONGLONG)dwFreeType);
	if (STATUS_SUCCESS != ret)
	{
		BaseSetLastNTError((NTSTATUS)ret);
		return FALSE;
	}
	else
		return TRUE;
}

BOOL  VirtualProtectEx64(HANDLE hProcess, ULONGLONG lpAddress, DWORD dwSize, DWORD flNewProtect, DWORD* lpflOldProtect)
{
	static ULONGLONG ntpvm = 0;
	ULONGLONG ret, tmpAddr = lpAddress, tmpSize = dwSize;

	if (0 == ntpvm)
	{
		ntpvm = GetProcAddress64(getNTDLL64(), "NtProtectVirtualMemory");
		if (0 == ntpvm)
			return 0;
	}

	ret = X64Call(ntpvm, 5, (ULONGLONG)hProcess, (ULONGLONG)&tmpAddr, (ULONGLONG)&tmpSize, (ULONGLONG)flNewProtect, (ULONGLONG)lpflOldProtect);
	if (STATUS_SUCCESS != ret)
	{
		BaseSetLastNTError((NTSTATUS)ret);
		return FALSE;
	}
	else
		return TRUE;
}

BOOL  ReadProcessMemory64(HANDLE hProcess, ULONGLONG lpBaseAddress, LPVOID lpBuffer, DWORD nSize, DWORD *lpNumberOfBytesRead)
{
	static ULONGLONG nrvm = 0;
	ULONGLONG ret, numOfBytes = lpNumberOfBytesRead ? *lpNumberOfBytesRead : 0;

	if (0 == nrvm)
	{
		nrvm = GetProcAddress64(getNTDLL64(), "NtReadVirtualMemory");
		if (0 == nrvm)
			return 0;
	}
	ret = X64Call(nrvm, 5, (ULONGLONG)hProcess, lpBaseAddress, (ULONGLONG)lpBuffer, (ULONGLONG)nSize, (ULONGLONG)&numOfBytes);
	if (STATUS_SUCCESS != ret)
	{
		BaseSetLastNTError((NTSTATUS)ret);
		return FALSE;
	}
	else
	{
		if (lpNumberOfBytesRead)
			*lpNumberOfBytesRead = (DWORD)numOfBytes;
		return TRUE;
	}
}

BOOL WriteProcessMemory64(HANDLE hProcess, ULONGLONG lpBaseAddress, LPVOID lpBuffer, DWORD nSize, DWORD *lpNumberOfBytesWritten)
{
	static ULONGLONG nrvm = 0;
	ULONGLONG ret, numOfBytes = lpNumberOfBytesWritten ? *lpNumberOfBytesWritten : 0;

	if (0 == nrvm)
	{
		nrvm = GetProcAddress64(getNTDLL64(), "NtWriteVirtualMemory");
		if (0 == nrvm)
			return 0;
	}
	ret = X64Call(nrvm, 5, (ULONGLONG)hProcess, lpBaseAddress, (ULONGLONG)lpBuffer, (ULONGLONG)nSize, (ULONGLONG)&numOfBytes);
	if (STATUS_SUCCESS != ret)
	{
		BaseSetLastNTError((NTSTATUS)ret);
		return FALSE;
	}
	else
	{
		if (lpNumberOfBytesWritten)
			*lpNumberOfBytesWritten = (DWORD)numOfBytes;
		return TRUE;
	}
}

ULONGLONG LoadLibrary64(const wchar_t* path)
{
	UNICODE_STRING64 upath = { 0 };
	DWORD64 hModule = 0;
	DWORD64 pfnLdrLoad = (DWORD64)GetProcAddress64(getNTDLL64(), "LdrLoadDll");

	upath.Length = (WORD)wcslen(path) * sizeof(wchar_t);
	upath.MaximumLength = (WORD)upath.Length;
	upath.Buffer = (DWORD64)path;

	X64Call(pfnLdrLoad, 4, (DWORD64)NULL, (DWORD64)0, (DWORD64)&upath, (DWORD64)&hModule);

	return hModule;
};
#endif

ULONGLONG  GetRemoteModuleHandle64(HANDLE ProcessHandle, LPWSTR lpDllName)
{
	PROCESS_BASIC_INFORMATION BasicInformation;
	NTSTATUS Status;
	PEB_LDR_DATA64 *pMod, *pStart, LoaderData;
	LDR_DATA_TABLE_ENTRY64 ldrMod;
	WCHAR dllName[MAX_PATH + 1];

	/* query the process basic information (includes the PEB address) */
	if (!NT_SUCCESS(Status = NtQueryInformationProcess(ProcessHandle, ProcessBasicInformation, &BasicInformation,
		sizeof(BasicInformation), NULL)) ||
		/* get the address of the PE Loader data */
		!ReadProcessMemory(ProcessHandle, (LPCVOID)&(BasicInformation.PebBaseAddress->Ldr), &LoaderData, sizeof(LoaderData), NULL))
		return 0;

	/* head of the module list: the last element in the list will point to this */
	pStart = pMod = (PEB_LDR_DATA64*)LoaderData.InLoadOrderModuleList.Flink;
	do
	{
		if (!ReadProcessMemory(ProcessHandle, pMod, &ldrMod, sizeof(ldrMod), NULL)) break;
		if (ldrMod.DllBase &&
			ReadProcessMemory(ProcessHandle, (LPCVOID)ldrMod.BaseDllName.Buffer, dllName, min(ldrMod.BaseDllName.Length, sizeof(dllName) / sizeof(WCHAR)), NULL))
		{
			dllName[ldrMod.BaseDllName.Length / sizeof(WCHAR)] = 0;
			if (!__wcsicmp(dllName, lpDllName))
				return (ULONGLONG)ldrMod.DllBase;
		}
		pMod = (PEB_LDR_DATA64*)ldrMod.InLoadOrderLinks.Flink;
	} while (pMod != pStart);
	return 0;
}
