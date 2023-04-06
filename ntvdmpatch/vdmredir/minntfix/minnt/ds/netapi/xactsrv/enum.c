#include "xactsrvp.h"
#undef NT_PIPE_PREFIX
#include <rpcutil.h>
#include <netrpc.h>
#include <lmsname.h>
#include <bowser.h>



NET_API_STATUS
I_BrowserServerEnumEx (
    IN  LPCWSTR      servername OPTIONAL,
    IN  LPCWSTR      transport OPTIONAL,
    IN  LPCWSTR      clientname OPTIONAL,
    IN  DWORD       level,
    OUT LPBYTE      *bufptr,
    IN  DWORD       prefmaxlen,
    OUT LPDWORD     entriesread,
    OUT LPDWORD     totalentries,
    IN  DWORD       servertype,
    IN  LPCWSTR      domain OPTIONAL,
    IN  LPCWSTR     FirstNameToReturn OPTIONAL
    )

/*++

Routine Description:

    This is the DLL entrypoint for NetWkstaSetInfo.

Arguments:

    servername - Supplies the name of server to execute this function

    level - Supplies the level of information.

    buf - Supplies a buffer which contains the information structure of fields
        to set.  The level denotes the structure in this buffer.

    parm_err - Returns the identifier to the invalid parameter in buf if this
        function returns ERROR_INVALID_PARAMETER.

    FirstNameToReturn - Supplies the name of the first domain or server entry to return.
        The caller can use this parameter to implement a resume handle of sorts by passing
        the name of the last entry returned on a previous call.  (Notice that the specified
        entry will, also, be returned on this call unless it has since been deleted.)
        Pass NULL to start with the first entry available.

Return Value:

    NET_API_STATUS - NERR_Success or reason for failure.

--*/
{
    NET_API_STATUS status;
    GENERIC_INFO_CONTAINER GenericInfoContainer;
    GENERIC_ENUM_STRUCT InfoStruct;

    GenericInfoContainer.Buffer = NULL;
    GenericInfoContainer.EntriesRead = 0;

    InfoStruct.Container = &GenericInfoContainer;
    InfoStruct.Level = level;

    NET_REMOTE_TRY_RPC

        //
        // Try RPC (local or remote) version of API.
        //

        status = I_BrowserrServerEnumEx(
                     (LPWSTR) servername,
                     (LPWSTR) transport,
                     (LPWSTR) clientname,
                     (LPSERVER_ENUM_STRUCT)&InfoStruct,
                     prefmaxlen,
                     totalentries,
                     servertype,
                     (LPWSTR) domain,
                     (LPWSTR) FirstNameToReturn );

        if (status == NERR_Success || status == ERROR_MORE_DATA) {
            *bufptr = (LPBYTE) GenericInfoContainer.Buffer;
            *entriesread = GenericInfoContainer.EntriesRead;
        }

    NET_REMOTE_RPC_FAILED("I_BrServerEnum",
            servername,
            status,
            NET_REMOTE_FLAG_NORMAL,
            SERVICE_BROWSER )

        //
        // There is no downlevel version of api.
        //
        status = ERROR_NOT_SUPPORTED;

    NET_REMOTE_END

    return status;
}


WORD
I_BrowserServerEnumForXactsrv(
    IN LPCWSTR TransportName OPTIONAL,
    IN LPCWSTR ClientName OPTIONAL,

    IN ULONG Level,
    IN USHORT ClientLevel,

    IN PVOID ClientBuffer,
    IN WORD BufferLength,
    IN DWORD PreferedMaximumLength,

    OUT LPDWORD EntriesFilled,
    OUT LPDWORD TotalEntries,

    IN DWORD ServerType,
    IN LPCWSTR Domain,
    IN LPCWSTR FirstNameToReturnArg OPTIONAL,

    OUT PWORD Converter
    )
/*++

Routine Description:

    This function is a private entrypoint for Xactsrv that bypasses RPC
    entirely.

Arguments:

    TransportName - Supplies the name of xport on which to enumerate servers

    ClientName - Supplies the name of the client that requested the data

    Level - Level of data requested.
    ClientLevel - Level requested by the client.

    ClientBuffer - Output buffer allocated to hold the buffer.
    BufferLength - Size of ClientBuffer
    PreferedMaximumLength - Prefered maximum size of Client buffer if we are
                            going to use the NT form of the buffer

    OUT LPDWORD EntriesFilled - The entries packed into ClientBuffer
    OUT LPDWORD TotalEntries - The total # of entries available.

    IN DWORD ServerType - Server type mask.
    IN LPTSTR Domain    - Domain to query

    OUT PWORD Converter - Magic constant from Xactsrv that allows the client
                            to convert the response buffer.

Return Value:

    WORD - NERR_Success or reason for failure.

--*/
{
// NB: I_BrowserrServerEnum is now in browcli.dll
	NET_API_STATUS status;
	PVOID ServerList = NULL;

// TODO: implement caching

	status = I_BrowserServerEnumEx
	(
		NULL,
		TransportName,
		ClientName,
		Level,
		ServerList,
		PreferedMaximumLength,
		EntriesFilled,
		TotalEntries,
		ServerType,
		Domain,
		FirstNameToReturnArg
	);

	if (status == NERR_Success || status == ERROR_MORE_DATA) {

                status = XsConvertServerEnumBuffer(
                              ServerList,
                              *EntriesFilled,
                              TotalEntries,
                              ClientLevel,
                              ClientBuffer,
                              BufferLength,
                              EntriesFilled,
                              Converter);

                if (ServerList != NULL) {
                    NetApiBufferFree(ServerList);
                    ServerList = NULL;
                }
	}

	return (WORD)status;
}
