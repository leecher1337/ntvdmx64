#define WIN32_MEAN_AND_LEAN
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>

#ifndef _DEBUG
// As CRT is big and unneccessary, implement short necessary functions on our own so that
// we can reduce bloated .exe size

#include <winternl.h>
#undef RtlZeroMemory
EXTERN_C void _declspec(dllimport) WINAPI RtlZeroMemory(PVOID, SIZE_T);

#pragma comment(lib, "ntdll.lib")

NTSTATUS NTAPI RtlUnicodeStringToInteger(
	PCUNICODE_STRING String,
	ULONG            Base,
	PULONG           Value
	);
int wmain(int argc, wchar_t *argv[], wchar_t *envp[]);

void wmainCRTStartup(void)
{
	wchar_t **argv;
	int argc, ret = ERROR_INVALID_PARAMETER;

	if (argv = CommandLineToArgvW(GetCommandLineW(), &argc))
	{
		ret = wmain(argc, argv, NULL);
		LocalFree(argv);
	}
	ExitProcess(ret);
}

ULONG my_wtoi(wchar_t const* _String)
{
	UNICODE_STRING uStr;
	ULONG ret;

	RtlInitUnicodeString(&uStr, _String);
	RtlUnicodeStringToInteger(&uStr, 0, &ret);
	return ret;
}
#define _wcsicmp lstrcmpiW
#define _wtoi my_wtoi
#endif

int wmain(int argc, wchar_t *argv[], wchar_t *envp[])
{
	int ExitCode = ERROR_INVALID_PARAMETER;
	STARTUPINFOW StartupInfo;
	PROCESS_INFORMATION ProcessInformation;
	DWORD dwCreationFlags;
	LPWSTR lpCommandLine;

	RtlZeroMemory(&StartupInfo, sizeof(StartupInfo));
	StartupInfo.cb = sizeof(StartupInfo);
	RtlZeroMemory(&ProcessInformation, sizeof(ProcessInformation));
	HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);
	if ((lpCommandLine = GetCommandLineW()) && argc >= 3 && !_wcsicmp(argv[2], L"-P"))
	{
		dwCreationFlags = _wtoi(argv[1]) & (~(EXTENDED_STARTUPINFO_PRESENT | DETACHED_PROCESS | CREATE_SUSPENDED)) | CREATE_NEW_CONSOLE;
		while (*lpCommandLine && *lpCommandLine != '-') lpCommandLine++;
		if (CreateProcessW(NULL, lpCommandLine + 3, NULL, NULL, FALSE, dwCreationFlags, NULL, NULL, &StartupInfo, &ProcessInformation))
		{
			WaitForSingleObject(ProcessInformation.hProcess, INFINITE);
			GetExitCodeProcess(ProcessInformation.hProcess, &ExitCode);
			CloseHandle(ProcessInformation.hProcess);
			CloseHandle(ProcessInformation.hThread);
		}
		else
		{
			ExitCode = GetLastError();
		}
	}
	return ExitCode;
}

