#pragma once
HANDLE InjectLdntvdmWow64RemoteThread(HANDLE hProcess);
BOOL InjectLdntvdmWow64HijackThread(HANDLE hProcess);
DWORD WINAPI InjectLdntvdmWow64Thread(LPVOID lpPID);
