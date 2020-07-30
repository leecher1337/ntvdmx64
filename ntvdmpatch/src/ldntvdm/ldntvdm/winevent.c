/*Project: ldntvdm
* Module : winevent
* Author : leecher@dose.0wnz.at
* Descr. : This module hooks NotifyWinEvent in console host process
*          to get early notification of a new process being created,
*          so that our loader can be injected.
*          On XP, UserNotifyProcessCreate may be another interesting
*          option.
* Changes: 29.05.2020  - Moved to separate module
*/

#include "ldntvdm.h"
#include "injector64.h"
#include "injector32.h"
#include "iathook.h"

#if !defined(CREATEPROCESS_HOOK) && defined(_WIN64)

typedef struct {
	HANDLE hProcess;
	HANDLE hThread;
	BOOL bIsWow64;
} INJECTOR_PARAM;

DWORD WINAPI InjectIntoCreatedThreadThread(INJECTOR_PARAM *param)
{
	/* 64 -> 32 */
	if (param->bIsWow64)
	{
		if (isProcessInitialized(param->hProcess) || !InjectDllHijackThreadX32(param->hProcess, param->hThread, LDNTVDM_NAME))
		{
			HANDLE hThread;
			
			if (!(hThread = InjectLdntvdmWow64RemoteThread(param->hProcess)))
			{
				OutputDebugStringA("Injecting into 32bit conhost parent failed");
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
	CloseHandle(param->hThread);
	CloseHandle(param->hProcess);
	HeapFree(GetProcessHeap(), 0, param);
	ExitThread(0);
	return 0;
}

#ifdef TARGET_WINXP
typedef void (WINAPI *fpNotifyWinEvent)(PVOID msg, DWORD event, LONG idObject, LONG idChild);
fpNotifyWinEvent NotifyWinEventReal = NULL;
typedef BOOL(WINAPI *fpIsWinEventHookInstalled)(DWORD);
fpIsWinEventHookInstalled IsWinEventHookInstalledReal = NULL;
//VOID WINAPI ConsoleNotifyWinEventHook(PVOID msg, DWORD event, LONG idObject, LONG idChild)
// Not hooking ConsoleNotifyWinEventHook, thus: 
BOOL WINAPI ConsoleNotifyWinEventHook(PVOID msg, DWORD event, LONG idObject, LONG idChild)
#else
typedef void (WINAPI *fpNotifyWinEvent)(DWORD event, HWND hwnd, LONG idObject, LONG idChild);
fpNotifyWinEvent NotifyWinEventReal;
void WINAPI NotifyWinEventHook(DWORD event, HWND  hwnd, LONG  idObject, LONG  idChild)
#endif
{
	if (event == EVENT_CONSOLE_START_APPLICATION)
	{
		HANDLE hProcess;
		TRACE("EVENT_CONSOLE_START_APPLICATION PID %d\n", idObject);
		// First freeze the process, don't lose time or it may be too late
		if (hProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION, FALSE, idObject))
		{
			NTSTATUS Status;
			HANDLE hThread;
			BOOL bIsWow64;
			HMODULE hModLdntvdm;

			IsWow64Process(hProcess, &bIsWow64);

			/* Check, if we are already loaded into target process */
			hModLdntvdm = bIsWow64 ? GetRemoteModuleHandle32(hProcess, LDNTVDM_NAME) : GetRemoteModuleHandle64(hProcess, LDNTVDM_NAME);
			if (!hModLdntvdm)
			{
				if (!NT_SUCCESS(Status = NtGetNextThread(hProcess, NULL, THREAD_GET_CONTEXT | THREAD_SET_CONTEXT | THREAD_SUSPEND_RESUME | THREAD_QUERY_INFORMATION | THREAD_SET_INFORMATION, 0, 0, &hThread)))
				{
					TRACE("NtGetNextThread failed: %08X\n", Status);
					CloseHandle(hProcess);
				}
				else
				{
					// Inject ourself into the new process
					INJECTOR_PARAM *param = HeapAlloc(GetProcessHeap(), 0, sizeof(INJECTOR_PARAM));
					param->hProcess = hProcess;
					param->hThread = hThread;
					param->bIsWow64 = bIsWow64;
					SuspendThread(hThread);
					TRACE("Before CreateThread\n");
					CloseHandle(CreateThread(NULL, 0, InjectIntoCreatedThreadThread, param, 0, NULL));
					TRACE("After CreateThread\n");
				}
			}
			else 
			{ 
				TRACE("Module already loaded\n"); 
				CloseHandle(hProcess);
			}
		}
	}
#ifdef TARGET_WINXP
	//if (NotifyWinEventReal) NotifyWinEventReal(msg, event, idObject, idChild);
	// Not hooking ConsoleNotifyWinEventHook, thus: 
	return IsWinEventHookInstalledReal(event);
#else
	NotifyWinEventReal(event, hwnd, idObject, idChild);
#endif
}

void WinEventHook_Install(HMODULE hModule)
{
#ifdef TARGET_WINXP
	/* This may be confusing: Hooking IsWinEventHookInstalled and pointing it to ConsoleNotifyWinEventHook ??
	 *
	 * This works thanks to fastcall calling convention:
	 * ecx = event    -> msg not used in our hook
	 * edx = event    -> OK
	 * r8d = idObject -> OK
	 * r9d = idChild  -> OK
	 */
	if (hModule = GetModuleHandle(_T("winsrv.dll")))
		Hook_IAT_x64_IAT((LPBYTE)hModule, "user32.dll", "IsWinEventHookInstalled", ConsoleNotifyWinEventHook, (PULONG_PTR)&IsWinEventHookInstalledReal);
#else
	if (hModule) Hook_IAT_x64_IAT((LPBYTE)hModule, "user32.dll", "NotifyWinEvent", NotifyWinEventHook, (PULONG_PTR)&NotifyWinEventReal);
#endif
}

#endif /* !CREATEPROCESS_HOOK && WIN64 */
