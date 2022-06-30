/* Project: ldntvdm
 * Module : matcheng
 * Author : leecher@dose.0wnz.at
 * Descr. : Provides routines to search for a FLIRT-Signature in a file
 * Changes: 29.06.2022  - Created
 */

typedef struct
{
	HANDLE hFile;
	HANDLE hMap;
	BYTE *lpMem;
	DWORD dwSize;
} MATCHENG_INST;

DWORD MatchEng_LoadModule(char *pszFile, MATCHENG_INST *pInst);
ULONG64 MatchEng_FindSig(MATCHENG_INST *pInst, BYTE *pbSig, DWORD cbSig, DWORD step);
void MatchEng_UnloadModule(MATCHENG_INST *pInst);
