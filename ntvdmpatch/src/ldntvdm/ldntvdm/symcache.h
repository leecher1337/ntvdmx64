BOOL UpdateSymbolCache(BOOL fUpdate);
HANDLE SymCache_GetDLLKey(HKEY hKey, LPWSTR lpDLLKey, BOOL fUpdate);
DWORD SymCache_GetProcAddress(HKEY hKey, LPWSTR lpKeyName);
