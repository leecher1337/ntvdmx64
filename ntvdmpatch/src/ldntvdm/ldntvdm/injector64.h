#pragma once
void FreeLibrary32(HANDLE hModule);
HMODULE LoadLibraryWindir32(LPWSTR lpDllName);
DWORD GetRemoteProcAddressX32(HANDLE hRemoteMod, HANDLE hLocalMod, LPSTR lpProcName);
HANDLE InjectLdntvdmWow64RemoteThread(HANDLE hProcess);
DWORD WINAPI InjectLdntvdmWow64Thread(LPVOID lpPID);
DWORD WINAPI InjectLdntvdmWow64UsingRemoteThread(HANDLE hProcess);
BOOL InjectDllHijackThreadX32(HANDLE hProc, HANDLE hThread, WCHAR *DllName);
BOOL isProcessInitialized(HANDLE hProcess);
HMODULE GetRemoteModuleHandle32(HANDLE ProcessHandle, LPWSTR lpDllName);
