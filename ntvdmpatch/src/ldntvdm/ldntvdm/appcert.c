/* Just in case we need to be loaded as appcert DLL. 
 * Currently this is not used */

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "Winternl.h"
#include <ntstatus.h>


#define APPCERT_IMAGE_OK_TO_RUN   0x00000001L
#define APPCERT_CREATION_ALLOWED  0x00000002L
#define APPCERT_CREATION_DENIED   0x00000003L

__declspec(dllexport) NTSTATUS NTAPI CreateProcessNotify(
	LPCWSTR lpApplicationName,
	ULONG uNotifyReason
	)
{
	switch (uNotifyReason)
	{
	case APPCERT_IMAGE_OK_TO_RUN:
		OutputDebugStringA("APPCERT_IMAGE_OK_TO_RUN");
		return STATUS_SUCCESS;

	case APPCERT_CREATION_ALLOWED:
		OutputDebugStringA("APPCERT_CREATION_ALLOWED");
		return STATUS_SUCCESS;

	case APPCERT_CREATION_DENIED:
		OutputDebugStringA("APPCERT_CREATION_DENIED");
		return STATUS_SUCCESS;

	default:
		OutputDebugStringA("APPCERT_UNKNOWN");
		return STATUS_SUCCESS;
	}
}
