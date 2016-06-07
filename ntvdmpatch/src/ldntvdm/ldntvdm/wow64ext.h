#ifndef STATUS_SUCCESS
#   define STATUS_SUCCESS 0
#endif

ULONG __stdcall BaseSetLastNTError(NTSTATUS NtStatus);
ULONGLONG getNTDLL64();
ULONGLONG __cdecl X64Call(ULONGLONG func, int argC, ...);
ULONGLONG  GetModuleHandle64(wchar_t* lpModuleName);
ULONGLONG  GetProcAddress64(ULONGLONG hModule, char* funcName);
ULONGLONG  VirtualAllocEx64(HANDLE hProcess, ULONGLONG lpAddress, DWORD dwSize, DWORD flAllocationType, DWORD flProtect);
BOOL  VirtualFreeEx64(HANDLE hProcess, ULONGLONG lpAddress, DWORD dwSize, DWORD dwFreeType);
BOOL  VirtualProtectEx64(HANDLE hProcess, ULONGLONG lpAddress, DWORD dwSize, DWORD flNewProtect, DWORD* lpflOldProtect);
BOOL  ReadProcessMemory64(HANDLE hProcess, ULONGLONG lpBaseAddress, LPVOID lpBuffer, DWORD nSize, DWORD *lpNumberOfBytesRead);
BOOL WriteProcessMemory64(HANDLE hProcess, ULONGLONG lpBaseAddress, LPVOID lpBuffer, DWORD nSize, DWORD *lpNumberOfBytesWritten);
ULONGLONG LoadLibrary64(const wchar_t* path);
