#pragma once

typedef enum _RTL_PATH_TYPE {
	RtlPathTypeUnknown,
	RtlPathTypeUncAbsolute,
	RtlPathTypeDriveAbsolute,
	RtlPathTypeDriveRelative,
	RtlPathTypeRooted,
	RtlPathTypeRelative,
	RtlPathTypeLocalDevice,
	RtlPathTypeRootLocalDevice
} RTL_PATH_TYPE;


typedef struct _RTL_RELATIVE_NAME {
	STRING RelativeName;
	HANDLE ContainingDirectory;
} RTL_RELATIVE_NAME, *PRTL_RELATIVE_NAME;

typedef struct _RTLP_CURDIR_REF *PRTLP_CURDIR_REF;

typedef struct _RTL_RELATIVE_NAME_U {
	UNICODE_STRING RelativeName;
	HANDLE ContainingDirectory;
	PRTLP_CURDIR_REF CurDirRef;
} RTL_RELATIVE_NAME_U, *PRTL_RELATIVE_NAME_U;


NTSYSAPI ULONG NTAPI RtlGetCurrentDirectory_U(ULONG nBufferLength, PWSTR lpBuffer);
NTSYSAPI PVOID NTAPI RtlAllocateHeap(IN PVOID HeapHandle, IN ULONG Flags, IN ULONG Size);
NTSYSAPI NTSTATUS NTAPI NtAllocateVirtualMemory(IN HANDLE ProcessHandle, IN OUT PVOID *BaseAddress, IN ULONG_PTR ZeroBits,
	IN OUT PSIZE_T RegionSize, IN ULONG AllocationType, IN ULONG Protect);
NTSYSAPI BOOLEAN NTAPI RtlFreeHeap(IN PVOID HeapHandle, IN ULONG Flags, IN PVOID BaseAddress);
NTSYSAPI NTSTATUS NTAPI RtlCreateEnvironment(BOOLEAN CloneCurrentEnvironment, PVOID *Environment);
NTSYSAPI NTSTATUS NTAPI RtlDestroyEnvironment(PVOID Environment);
NTSYSAPI NTSTATUS NTAPI RtlExpandEnvironmentStrings_U(IN PVOID Environment OPTIONAL, IN PCUNICODE_STRING Source,
	OUT PUNICODE_STRING Destination, OUT PULONG ReturnedLength OPTIONAL);
NTSYSAPI ULONG NTAPI RtlIsDosDeviceName_U(PWSTR DosFileName);
NTSYSAPI NTSTATUS NTAPI RtlAppendUnicodeToString(PUNICODE_STRING Destination, PCWSTR Source);
NTSYSAPI VOID NTAPI RtlReleasePrivilege(PVOID StatePointer);
NTSYSAPI BOOLEAN NTAPI RtlDosPathNameToNtPathName_U(PCWSTR DosFileName, PUNICODE_STRING NtFileName, PWSTR *FilePart,PVOID Reserved);
NTSYSAPI PIMAGE_NT_HEADERS NTAPI RtlImageNtHeader(PVOID Base);
NTSYSAPI RTL_PATH_TYPE NTAPI RtlDetermineDosPathNameType_U(PCWSTR DosFileName);
NTSYSAPI NTSTATUS NTAPI RtlOpenCurrentUser(_In_ ACCESS_MASK DesiredAccess, _Out_ PHANDLE KeyHandle);
NTSYSAPI BOOLEAN NTAPI RtlCreateUnicodeString(OUT PUNICODE_STRING DestinationString, IN PCWSTR SourceString);
NTSYSAPI NTSTATUS NTAPI RtlGetFullPathName_UstrEx(PCUNICODE_STRING FileName, PUNICODE_STRING StaticString, PUNICODE_STRING DynamicString,
	PUNICODE_STRING *StringUsed, SIZE_T *FilePartPrefixCch OPTIONAL, PBOOLEAN NameInvalid, RTL_PATH_TYPE *InputPathType, OUT SIZE_T *BytesRequired OPTIONAL);
NTSYSAPI VOID NTAPI RtlReleaseRelativeName(IN PRTL_RELATIVE_NAME_U RelativeName);
NTSYSAPI BOOLEAN NTAPI RtlDosPathNameToRelativeNtPathName_U(__in PCWSTR DosFileName, __out PUNICODE_STRING NtFileName,
	__out_opt PWSTR *FilePart, __out_opt PRTL_RELATIVE_NAME_U RelativeName);
NTSYSAPI NTSTATUS NTAPI RtlAcquirePrivilege(PULONG Privilege, ULONG NumPriv, ULONG Flags, PVOID *ReturnedState);


#define RtlInitEmptyUnicodeString(_ucStr,_buf,_bufSize) \
    ((_ucStr)->Buffer = (_buf), \
     (_ucStr)->Length = 0, \
     (_ucStr)->MaximumLength = (USHORT)(_bufSize))

#define BaseSetLastNTError(status) SetLastError(RtlNtStatusToDosError(status))
