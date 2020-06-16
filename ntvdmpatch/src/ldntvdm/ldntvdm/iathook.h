#pragma once
BOOL Hook_IAT_x64_IAT(LPBYTE hMod, char LibNameBigCaseName_SmallFormat[], char FunName[], LPVOID NewFun, PULONG_PTR OldFun);
BOOL Hook_IAT_x64(LPBYTE hMod, char *LibDelayImpName, char *FunName, LPVOID NewFun);
PIMAGE_SECTION_HEADER GetEnclosingSectionHeader(DWORD rva, PIMAGE_NT_HEADERS pNTHeader);
