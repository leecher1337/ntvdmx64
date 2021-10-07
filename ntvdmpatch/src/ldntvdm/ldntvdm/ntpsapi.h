#ifdef CSR_API_NUMBER
typedef CLIENT_ID *PCLIENT_ID;


typedef struct {
	ULONG NextEntryOffset;
	ULONG NumberOfThreads;
	LARGE_INTEGER SpareLi1;
	LARGE_INTEGER SpareLi2;
	LARGE_INTEGER SpareLi3;
	LARGE_INTEGER CreateTime;
	LARGE_INTEGER UserTime;
	LARGE_INTEGER KernelTime;
	UNICODE_STRING ImageName;
	KPRIORITY BasePriority;
	HANDLE UniqueProcessId;
	HANDLE InheritedFromUniqueProcessId;
	ULONG HandleCount;
	ULONG SessionId;
	ULONG_PTR PageDirectoryBase;
	SIZE_T PeakVirtualSize;
	SIZE_T VirtualSize;
	ULONG PageFaultCount;
	SIZE_T PeakWorkingSetSize;
	SIZE_T WorkingSetSize;
	SIZE_T QuotaPeakPagedPoolUsage;
	SIZE_T QuotaPagedPoolUsage;
	SIZE_T QuotaPeakNonPagedPoolUsage;
	SIZE_T QuotaNonPagedPoolUsage;
	SIZE_T PagefileUsage;
	SIZE_T PeakPagefileUsage;
	SIZE_T PrivatePageCount;
	LARGE_INTEGER ReadOperationCount;
	LARGE_INTEGER WriteOperationCount;
	LARGE_INTEGER OtherOperationCount;
	LARGE_INTEGER ReadTransferCount;
	LARGE_INTEGER WriteTransferCount;
	LARGE_INTEGER OtherTransferCount;
} xpSYSTEM_PROCESS_INFORMATION, *PxpSYSTEM_PROCESS_INFORMATION;

typedef struct {
	NTSTATUS ExitStatus;
	PPEB PebBaseAddress;
	KAFFINITY AffinityMask;
	KPRIORITY BasePriority;
	ULONG_PTR UniqueProcessId;
	ULONG_PTR InheritedFromUniqueProcessId;
} xpPROCESS_BASIC_INFORMATION, *PxpPROCESS_BASIC_INFORMATION;

typedef struct _INITIAL_TEB {
	struct {
		PVOID OldStackBase;
		PVOID OldStackLimit;
	} OldInitialTeb;
	PVOID StackBase;
	PVOID StackLimit;
	PVOID StackAllocationBase;
} INITIAL_TEB, *PINITIAL_TEB;

NTSYSCALLAPI
NTSTATUS
NTAPI
NtCreateThread (
    __out PHANDLE ThreadHandle,
    __in ACCESS_MASK DesiredAccess,
    __in_opt POBJECT_ATTRIBUTES ObjectAttributes,
    __in HANDLE ProcessHandle,
    __out PCLIENT_ID ClientId,
    __in PCONTEXT ThreadContext,
    __in PINITIAL_TEB InitialTeb,
    __in BOOLEAN CreateSuspended
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtOpenThread (
    __out PHANDLE ThreadHandle,
    __in ACCESS_MASK DesiredAccess,
    __in POBJECT_ATTRIBUTES ObjectAttributes,
    __in_opt PCLIENT_ID ClientId
    );
#endif

NTSYSCALLAPI
NTSTATUS
NTAPI
NtTerminateThread (
    __in_opt HANDLE ThreadHandle,
    __in NTSTATUS ExitStatus
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtOpenThread (
    __out PHANDLE ThreadHandle,
    __in ACCESS_MASK DesiredAccess,
    __in POBJECT_ATTRIBUTES ObjectAttributes,
    __in_opt PCLIENT_ID ClientId
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtResumeThread(
	__in HANDLE ThreadHandle,
	__out_opt PULONG PreviousSuspendCount
	);



NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryInformationJobObject(
	__in_opt HANDLE JobHandle,
	__in JOBOBJECTINFOCLASS JobObjectInformationClass,
	__out_bcount(JobObjectInformationLength) PVOID JobObjectInformation,
	__in ULONG JobObjectInformationLength,
	__out_opt PULONG ReturnLength
	);

NTSYSCALLAPI
NTSTATUS
NTAPI
NtIsProcessInJob(
	__in HANDLE ProcessHandle,
	__in_opt HANDLE JobHandle
	);

NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetInformationProcess(
	__in HANDLE ProcessHandle,
	__in PROCESSINFOCLASS ProcessInformationClass,
	__in_bcount(ProcessInformationLength) PVOID ProcessInformation,
	__in ULONG ProcessInformationLength
	);

NTSYSCALLAPI
NTSTATUS
NTAPI
NtCreateProcess(
	__out PHANDLE ProcessHandle,
	__in ACCESS_MASK DesiredAccess,
	__in_opt POBJECT_ATTRIBUTES ObjectAttributes,
	__in HANDLE ParentProcess,
	__in BOOLEAN InheritObjectTable,
	__in_opt HANDLE SectionHandle,
	__in_opt HANDLE DebugPort,
	__in_opt HANDLE ExceptionPort
	);

NTSYSCALLAPI
NTSTATUS
NTAPI
NtCreateProcessEx(
	__out PHANDLE ProcessHandle,
	__in ACCESS_MASK DesiredAccess,
	__in_opt POBJECT_ATTRIBUTES ObjectAttributes,
	__in HANDLE ParentProcess,
	__in ULONG Flags,
	__in_opt HANDLE SectionHandle,
	__in_opt HANDLE DebugPort,
	__in_opt HANDLE ExceptionPort,
	__in ULONG JobMemberLevel
	);


NTSYSCALLAPI
NTSTATUS
NTAPI
NtTerminateProcess(
	__in_opt HANDLE ProcessHandle,
	__in NTSTATUS ExitStatus
	);

NTSYSCALLAPI
NTSTATUS
NTAPI
NtAssignProcessToJobObject(
	__in HANDLE JobHandle,
	__in HANDLE ProcessHandle
	);