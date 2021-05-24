#ifdef _WIN64
#define REGKEY_BasepProcessInvalidImage L"BasepProcessInvalidImage64"
#define REGKEY_BaseIsDosApplication L"BaseIsDosApplication64"
#else
#define REGKEY_BasepProcessInvalidImage L"BasepProcessInvalidImage32"
#define REGKEY_BaseIsDosApplication L"BaseIsDosApplication32"
#endif

BOOL UpdateSymbolCache(BOOL fUpdate);
HANDLE SymCache_GetDLLKey(HKEY hKey, LPWSTR lpDLLKey, BOOL fUpdate);
DWORD SymCache_GetProcAddress(HKEY hKey, LPWSTR lpKeyName);
