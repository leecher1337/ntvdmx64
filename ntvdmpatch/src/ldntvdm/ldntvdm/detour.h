#define Hook_Inline(hProcess, hModule, src, tgt) Hook_Inline_Func(hProcess, hModule, src, tgt, sizeof(ULONG_PTR), #tgt)
#define Hook_Inline32(hProcess, hModule, src, tgt) Hook_Inline_Func(hProcess, hModule, src, tgt, 4, #tgt)
#define Hook_Inline64(hProcess, hModule, src, tgt) Hook_Inline_Func(hProcess, hModule, src, tgt, 8, #tgt)
LPBYTE Hook_Inline_Func(HANDLE hProcess, HANDLE hModule, PVOID src, PVOID tgt, SIZE_T ptr_size, const char *pszHookName);
