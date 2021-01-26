#pragma once

PVOID NTAPI CsrAllocateCaptureBuffer(ULONG ArgumentCount, ULONG BufferSize);
VOID NTAPI CsrFreeCaptureBuffer(struct _CSR_CAPTURE_BUFFER *CaptureBuffer);
ULONG NTAPI CsrAllocateMessagePointer(struct _CSR_CAPTURE_BUFFER *CaptureBuffer, ULONG MessageLength, PVOID *CaptureData);
NTSTATUS NTAPI CsrClientCallServer(struct _CSR_API_MESSAGE *Request, struct _CSR_CAPTURE_BUFFER *CaptureBuffer OPTIONAL,
	ULONG ApiNumber, ULONG RequestLength);
NTSYSAPI VOID NTAPI CsrCaptureMessageBuffer(struct _CSR_CAPTURE_BUFFER *CaptureBuffer, PVOID Buffer, ULONG Length, PVOID *CapturedBuffer);
NTSTATUS NTAPI CsrCaptureMessageMultiUnicodeStringsInPlace(struct _CSR_CAPTURE_BUFFER *CaptureBuffer, IN ULONG StringsCount, IN PUNICODE_STRING *MessageStrings);


#ifdef _WIN64
#define myCsrClientCallServer CsrClientCallServer
#define myCsrAllocateMessagePointer CsrAllocateMessagePointer
#else
NTSTATUS NTAPI myCsrClientCallServer(struct _CSR_API_MESSAGE *Request, struct _CSR_CAPTURE_BUFFER *CaptureBuffer OPTIONAL,
	ULONG ApiNumber, ULONG RequestLength);
ULONG NTAPI myCsrAllocateMessagePointer(struct _CSR_CAPTURE_BUFFER *CaptureBuffer, ULONG MessageLength, PVOID *CaptureData);
#endif
void HookCsrClientCallServer();

