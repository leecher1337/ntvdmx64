#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <windows.h>
#include "conapi.h"
#include "nt_w10.h"

typedef 
BOOL
(*fpRegisterConsoleVDMw7)(
    IN BOOL bRegister,
    IN HANDLE hStartHardwareEvent,
    IN HANDLE hEndHardwareEvent,
    IN LPWSTR lpStateSectionName,
    IN DWORD dwStateSectionNameLength,
    OUT LPDWORD lpStateLength,
    OUT PVOID *lpState,
    COORD VDMBufferSize OPTIONAL,
    OUT PVOID *lpVDMBuffer
    );


// On Windows 7 and above, BufferSection Parameters got removed
w7WrapRegisterConsoleVDM(
    IN BOOL bRegister,
    IN HANDLE hStartHardwareEvent,
    IN HANDLE hEndHardwareEvent,
    IN LPWSTR lpStateSectionName,
    IN DWORD dwStateSectionNameLength,
    OUT LPDWORD lpStateLength,
    OUT PVOID *lpState,
    IN LPWSTR lpVDMBufferSectionName,
    IN DWORD dwVDMBufferSectionNameLength,
    COORD VDMBufferSize OPTIONAL,
    OUT PVOID *lpVDMBuffer
    )
{
	return 
        ((fpRegisterConsoleVDMw7)RegisterConsoleVDM)(
                             bRegister,
                             hStartHardwareEvent,
                             hEndHardwareEvent,
                             lpStateSectionName,
                             dwStateSectionNameLength,
                             lpStateLength,
                             lpState,
                             VDMBufferSize,
                             lpVDMBuffer
                           );
}
