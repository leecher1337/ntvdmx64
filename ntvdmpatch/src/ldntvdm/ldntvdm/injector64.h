#pragma once
HANDLE InjectLdntvdmWow64(HANDLE hProcess);
DWORD WINAPI InjectLdntvdmWow64Thread(LPVOID lpPID);
