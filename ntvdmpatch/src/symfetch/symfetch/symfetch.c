#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include "symeng.h"

int main(int argc, char **argv)
{
	int i, iRet = 0;
	DWORD64 dwBase;

	if (argc < 2) 
	{
		fprintf(stderr, "Usage: %s <File1> [File2...]\n", argv[0]);
		return -1;
	}

	for (i = 1; i < argc; i++)
	{
		if ((iRet = SymEng_LoadModule(argv[i], &dwBase)) < 0)
		{
			switch (iRet)
			{
			case -1: fprintf(stderr, "Cannot load dbghelp library\n"); break;
			case -2: fprintf(stderr, "Cannot initialize symbol engine\n"); break;
			case -3: fprintf(stderr, "Cannot load symbols for module\n"); break;
			}
			return iRet;
		}
		SymEng_UnloadModule(dwBase);
	}
	return iRet;
}
