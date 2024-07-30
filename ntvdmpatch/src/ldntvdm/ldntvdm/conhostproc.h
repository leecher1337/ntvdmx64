#ifdef TARGET_WINXP
#define GetConsoleHost() ((xpPRTL_USER_PROCESS_PARAMETERS)NtCurrentPeb()->ProcessParameters)->ConsoleHandle
#else
HANDLE GetConsoleHost(void);
void HookNtQueryInformationProcess(HMODULE hKernelBase, HMODULE hKrnl32);
#endif
