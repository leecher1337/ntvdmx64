#pragma once
HANDLE InjectLdntvdmWow64RemoteThread(HANDLE hProcess);
DWORD WINAPI InjectLdntvdmWow64Thread(LPVOID lpPID);
DWORD WINAPI InjectLdntvdmWow64UsingRemoteThread(HANDLE hProcess);
BOOL InjectDllHijackThreadX32(HANDLE hProc, HANDLE hThread, WCHAR *DllName);
BOOL isProcessInitialized(HANDLE hProcess);
