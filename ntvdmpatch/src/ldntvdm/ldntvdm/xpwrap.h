#include "basemsg64.h"

#define NtCurrentProcess() ( (HANDLE)(LONG_PTR) -1 )

typedef LONG KPRIORITY;

#define ThreadBasicInformation 0

typedef struct _THREAD_BASIC_INFORMATION {
	NTSTATUS ExitStatus;
	PTEB TebBaseAddress;
	CLIENT_ID ClientId;
	ULONG_PTR AffinityMask;
	KPRIORITY Priority;
	LONG BasePriority;
} THREAD_BASIC_INFORMATION;
typedef THREAD_BASIC_INFORMATION *PTHREAD_BASIC_INFORMATION;

