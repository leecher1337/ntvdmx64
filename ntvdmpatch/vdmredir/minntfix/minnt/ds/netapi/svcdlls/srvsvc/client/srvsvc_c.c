

/* this ALWAYS GENERATED file contains the RPC client stubs */


 /* File created by MIDL compiler version 6.00.0361 */
/* at Sun Apr 02 10:36:50 2023
 */
/* Compiler settings for srvsvc.idl:
    Oi, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, oldnames
    error checks: allocation ref 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#if !defined(_M_IA64) && !defined(_M_AMD64)


#pragma warning( disable: 4049 )  /* more than 64k source lines */
#if _MSC_VER >= 1200
#pragma warning(push)
#endif

#pragma warning( disable: 4211 )  /* redefine extent to static */
#pragma warning( disable: 4232 )  /* dllimport identity*/
#include <string.h>

#include "srvsvc.h"

#define TYPE_FORMAT_STRING_SIZE   3783                              
#define PROC_FORMAT_STRING_SIZE   1777                              
#define TRANSMIT_AS_TABLE_SIZE    0            
#define WIRE_MARSHAL_TABLE_SIZE   0            

typedef struct _MIDL_TYPE_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ TYPE_FORMAT_STRING_SIZE ];
    } MIDL_TYPE_FORMAT_STRING;

typedef struct _MIDL_PROC_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ PROC_FORMAT_STRING_SIZE ];
    } MIDL_PROC_FORMAT_STRING;


static RPC_SYNTAX_IDENTIFIER  _RpcTransferSyntax = 
{{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}};


extern const MIDL_TYPE_FORMAT_STRING __MIDL_TypeFormatString;
extern const MIDL_PROC_FORMAT_STRING __MIDL_ProcFormatString;

#define GENERIC_BINDING_TABLE_SIZE   1            


/* Standard interface: srvsvc, ver. 3.0,
   GUID={0x4B324FC8,0x1670,0x01D3,{0x12,0x78,0x5A,0x47,0xBF,0x6E,0xE1,0x88}} */

handle_t srvsvc_bhandle;


static const RPC_CLIENT_INTERFACE srvsvc___RpcClientInterface =
    {
    sizeof(RPC_CLIENT_INTERFACE),
    {{0x4B324FC8,0x1670,0x01D3,{0x12,0x78,0x5A,0x47,0xBF,0x6E,0xE1,0x88}},{3,0}},
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    0,
    0,
    0,
    0,
    0x00000000
    };
RPC_IF_HANDLE srvsvc_ClientIfHandle = (RPC_IF_HANDLE)& srvsvc___RpcClientInterface;

extern const MIDL_STUB_DESC srvsvc_StubDesc;

static RPC_BINDING_HANDLE srvsvc__MIDL_AutoBindHandle;


DWORD NetrCharDevEnum( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [out][in] */ LPCHARDEV_ENUM_STRUCT InfoStruct,
    /* [in] */ DWORD PreferedMaximumLength,
    /* [out] */ LPDWORD TotalEntries,
    /* [unique][out][in] */ LPDWORD ResumeHandle)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[0],
                 ( unsigned char * )&ServerName);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetrCharDevGetInfo( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [string][in] */ wchar_t *DevName,
    /* [in] */ DWORD Level,
    /* [switch_is][out] */ LPCHARDEV_INFO InfoStruct)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[36],
                 ( unsigned char * )&ServerName);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetrCharDevControl( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [string][in] */ wchar_t *DevName,
    /* [in] */ DWORD Opcode)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[68],
                 ( unsigned char * )&ServerName);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetrCharDevQEnum( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [unique][string][in] */ wchar_t *UserName,
    /* [out][in] */ LPCHARDEVQ_ENUM_STRUCT InfoStruct,
    /* [in] */ DWORD PreferedMaximumLength,
    /* [out] */ LPDWORD TotalEntries,
    /* [unique][out][in] */ LPDWORD ResumeHandle)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[96],
                 ( unsigned char * )&ServerName);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetrCharDevQGetInfo( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [string][in] */ wchar_t *QueueName,
    /* [string][in] */ wchar_t *UserName,
    /* [in] */ DWORD Level,
    /* [switch_is][out] */ LPCHARDEVQ_INFO InfoStruct)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[136],
                 ( unsigned char * )&ServerName);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetrCharDevQSetInfo( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [string][in] */ wchar_t *QueueName,
    /* [in] */ DWORD Level,
    /* [switch_is][in] */ LPCHARDEVQ_INFO CharDevQInfo,
    /* [unique][out][in] */ LPDWORD ParmErr)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[172],
                 ( unsigned char * )&ServerName);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetrCharDevQPurge( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [string][in] */ wchar_t *QueueName)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[208],
                 ( unsigned char * )&ServerName);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetrCharDevQPurgeSelf( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [string][in] */ wchar_t *QueueName,
    /* [string][in] */ wchar_t *ComputerName)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[234],
                 ( unsigned char * )&ServerName);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetrConnectionEnum( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [unique][string][in] */ wchar_t *Qualifier,
    /* [out][in] */ LPCONNECT_ENUM_STRUCT InfoStruct,
    /* [in] */ DWORD PreferedMaximumLength,
    /* [out] */ LPDWORD TotalEntries,
    /* [unique][out][in] */ LPDWORD ResumeHandle)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[264],
                 ( unsigned char * )&ServerName);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetrFileEnum( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [unique][string][in] */ wchar_t *BasePath,
    /* [unique][string][in] */ wchar_t *UserName,
    /* [out][in] */ PFILE_ENUM_STRUCT InfoStruct,
    /* [in] */ DWORD PreferedMaximumLength,
    /* [out] */ LPDWORD TotalEntries,
    /* [unique][out][in] */ LPDWORD ResumeHandle)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[304],
                 ( unsigned char * )&ServerName);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetrFileGetInfo( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [in] */ DWORD FileId,
    /* [in] */ DWORD Level,
    /* [switch_is][out] */ LPFILE_INFO InfoStruct)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[348],
                 ( unsigned char * )&ServerName);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetrFileClose( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [in] */ DWORD FileId)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[378],
                 ( unsigned char * )&ServerName);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetrSessionEnum( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [unique][string][in] */ wchar_t *ClientName,
    /* [unique][string][in] */ wchar_t *UserName,
    /* [out][in] */ PSESSION_ENUM_STRUCT InfoStruct,
    /* [in] */ DWORD PreferedMaximumLength,
    /* [out] */ LPDWORD TotalEntries,
    /* [unique][out][in] */ LPDWORD ResumeHandle)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[402],
                 ( unsigned char * )&ServerName);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetrSessionDel( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [unique][string][in] */ wchar_t *ClientName,
    /* [unique][string][in] */ wchar_t *UserName)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[446],
                 ( unsigned char * )&ServerName);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetrShareAdd( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [in] */ DWORD Level,
    /* [switch_is][in] */ LPSHARE_INFO InfoStruct,
    /* [unique][out][in] */ LPDWORD ParmErr)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[476],
                 ( unsigned char * )&ServerName);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetrShareEnum( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [out][in] */ LPSHARE_ENUM_STRUCT InfoStruct,
    /* [in] */ DWORD PreferedMaximumLength,
    /* [out] */ LPDWORD TotalEntries,
    /* [unique][out][in] */ LPDWORD ResumeHandle)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[508],
                 ( unsigned char * )&ServerName);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetrShareGetInfo( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [string][in] */ wchar_t *NetName,
    /* [in] */ DWORD Level,
    /* [switch_is][out] */ LPSHARE_INFO InfoStruct)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[544],
                 ( unsigned char * )&ServerName);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetrShareSetInfo( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [string][in] */ wchar_t *NetName,
    /* [in] */ DWORD Level,
    /* [switch_is][in] */ LPSHARE_INFO ShareInfo,
    /* [unique][out][in] */ LPDWORD ParmErr)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[576],
                 ( unsigned char * )&ServerName);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetrShareDel( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [string][in] */ wchar_t *NetName,
    /* [in] */ DWORD Reserved)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[612],
                 ( unsigned char * )&ServerName);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetrShareDelSticky( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [string][in] */ wchar_t *NetName,
    /* [in] */ DWORD Reserved)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[640],
                 ( unsigned char * )&ServerName);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetrShareCheck( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [string][in] */ wchar_t *Device,
    /* [out] */ LPDWORD Type)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[668],
                 ( unsigned char * )&ServerName);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetrServerGetInfo( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [in] */ DWORD Level,
    /* [switch_is][out] */ LPSERVER_INFO InfoStruct)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[698],
                 ( unsigned char * )&ServerName);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetrServerSetInfo( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [in] */ DWORD Level,
    /* [switch_is][in] */ LPSERVER_INFO ServerInfo,
    /* [unique][out][in] */ LPDWORD ParmErr)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[726],
                 ( unsigned char * )&ServerName);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetrServerDiskEnum( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [in] */ DWORD Level,
    /* [out][in] */ DISK_ENUM_CONTAINER *DiskInfoStruct,
    /* [in] */ DWORD PreferredMaximumLength,
    /* [out] */ LPDWORD TotalEntries,
    /* [unique][out][in] */ LPDWORD ResumeHandle)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[758],
                 ( unsigned char * )&ServerName);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetrServerStatisticsGet( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [unique][string][in] */ wchar_t *Service,
    /* [in] */ DWORD Level,
    /* [in] */ DWORD Options,
    /* [out] */ LPSTAT_SERVER_0 *InfoStruct)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[796],
                 ( unsigned char * )&ServerName);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetrServerTransportAdd( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [in] */ DWORD Level,
    /* [in] */ LPSERVER_TRANSPORT_INFO_0 Buffer)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[830],
                 ( unsigned char * )&ServerName);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetrServerTransportEnum( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [out][in] */ LPSERVER_XPORT_ENUM_STRUCT InfoStruct,
    /* [in] */ DWORD PreferedMaximumLength,
    /* [out] */ LPDWORD TotalEntries,
    /* [unique][out][in] */ LPDWORD ResumeHandle)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[858],
                 ( unsigned char * )&ServerName);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetrServerTransportDel( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [in] */ DWORD Level,
    /* [in] */ LPSERVER_TRANSPORT_INFO_0 Buffer)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[894],
                 ( unsigned char * )&ServerName);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetrRemoteTOD( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [out] */ LPTIME_OF_DAY_INFO *BufferPtr)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[922],
                 ( unsigned char * )&ServerName);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD I_NetrServerSetServiceBits( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [unique][string][in] */ wchar_t *TransportName,
    /* [in] */ DWORD ServiceBits,
    /* [in] */ DWORD UpdateImmediately)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[948],
                 ( unsigned char * )&ServerName);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetprPathType( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [string][in] */ wchar_t *PathName,
    /* [out] */ LPDWORD PathType,
    /* [in] */ DWORD Flags)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[978],
                 ( unsigned char * )&ServerName);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetprPathCanonicalize( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [string][in] */ wchar_t *PathName,
    /* [size_is][out] */ LPBYTE Outbuf,
    /* [range][in] */ DWORD OutbufLen,
    /* [string][in] */ wchar_t *Prefix,
    /* [out][in] */ LPDWORD PathType,
    /* [in] */ DWORD Flags)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1010],
                 ( unsigned char * )&ServerName);
    return ( DWORD  )_RetVal.Simple;
    
}


LONG NetprPathCompare( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [string][in] */ wchar_t *PathName1,
    /* [string][in] */ wchar_t *PathName2,
    /* [in] */ DWORD PathType,
    /* [in] */ DWORD Flags)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1052],
                 ( unsigned char * )&ServerName);
    return ( LONG  )_RetVal.Simple;
    
}


DWORD NetprNameValidate( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [string][in] */ wchar_t *Name,
    /* [in] */ DWORD NameType,
    /* [in] */ DWORD Flags)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1086],
                 ( unsigned char * )&ServerName);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetprNameCanonicalize( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [string][in] */ wchar_t *Name,
    /* [size_is][out] */ wchar_t *Outbuf,
    /* [range][in] */ DWORD OutbufLen,
    /* [in] */ DWORD NameType,
    /* [in] */ DWORD Flags)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1116],
                 ( unsigned char * )&ServerName);
    return ( DWORD  )_RetVal.Simple;
    
}


LONG NetprNameCompare( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [string][in] */ wchar_t *Name1,
    /* [string][in] */ wchar_t *Name2,
    /* [in] */ DWORD NameType,
    /* [in] */ DWORD Flags)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1152],
                 ( unsigned char * )&ServerName);
    return ( LONG  )_RetVal.Simple;
    
}


DWORD NetrShareEnumSticky( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [out][in] */ LPSHARE_ENUM_STRUCT InfoStruct,
    /* [in] */ DWORD PreferedMaximumLength,
    /* [out] */ LPDWORD TotalEntries,
    /* [unique][out][in] */ LPDWORD ResumeHandle)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1186],
                 ( unsigned char * )&ServerName);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetrShareDelStart( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [string][in] */ wchar_t *NetName,
    /* [in] */ DWORD Reserved,
    /* [out] */ PSHARE_DEL_HANDLE ContextHandle)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1222],
                 ( unsigned char * )&ServerName);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetrShareDelCommit( 
    /* [out][in] */ PSHARE_DEL_HANDLE ContextHandle)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1254],
                 ( unsigned char * )&ContextHandle);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetrpGetFileSecurity( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [unique][string][in] */ LPWSTR ShareName,
    /* [string][in] */ LPWSTR lpFileName,
    /* [in] */ SECURITY_INFORMATION RequestedInformation,
    /* [out] */ PADT_SECURITY_DESCRIPTOR *SecurityDescriptor)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1276],
                 ( unsigned char * )&ServerName);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetrpSetFileSecurity( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [unique][string][in] */ LPWSTR ShareName,
    /* [string][in] */ LPWSTR lpFileName,
    /* [in] */ SECURITY_INFORMATION SecurityInformation,
    /* [in] */ PADT_SECURITY_DESCRIPTOR SecurityDescriptor)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1312],
                 ( unsigned char * )&ServerName);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetrServerTransportAddEx( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [in] */ DWORD Level,
    /* [switch_is][in] */ LPTRANSPORT_INFO Buffer)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1348],
                 ( unsigned char * )&ServerName);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD I_NetrServerSetServiceBitsEx( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [unique][string][in] */ LPWSTR EmulatedServerName,
    /* [unique][string][in] */ wchar_t *TransportName,
    /* [in] */ DWORD ServiceBitsOfInterest,
    /* [in] */ DWORD ServiceBits,
    /* [in] */ DWORD UpdateImmediately)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1376],
                 ( unsigned char * )&ServerName);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD __stdcall NetrDfsGetVersion( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [out] */ LPDWORD Version)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1412],
                 ( unsigned char * )&ServerName);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD __stdcall NetrDfsCreateLocalPartition( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [string][in] */ LPWSTR ShareName,
    /* [in] */ LPGUID EntryUid,
    /* [string][in] */ LPWSTR EntryPrefix,
    /* [string][in] */ LPWSTR ShortName,
    /* [in] */ LPNET_DFS_ENTRY_ID_CONTAINER RelationInfo,
    /* [in] */ BOOL Force)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1438],
                 ( unsigned char * )&ServerName);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD __stdcall NetrDfsDeleteLocalPartition( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [in] */ LPGUID Uid,
    /* [string][in] */ LPWSTR Prefix)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1482],
                 ( unsigned char * )&ServerName);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD __stdcall NetrDfsSetLocalVolumeState( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [in] */ LPGUID Uid,
    /* [string][in] */ LPWSTR Prefix,
    /* [in] */ ULONG State)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1512],
                 ( unsigned char * )&ServerName);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD __stdcall NetrDfsSetServerInfo( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [in] */ LPGUID Uid,
    /* [string][in] */ LPWSTR Prefix)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1544],
                 ( unsigned char * )&ServerName);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD __stdcall NetrDfsCreateExitPoint( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [in] */ LPGUID Uid,
    /* [string][in] */ LPWSTR Prefix,
    /* [in] */ ULONG Type,
    /* [range][in] */ DWORD ShortPrefixLen,
    /* [size_is][out] */ LPWSTR ShortPrefix)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1574],
                 ( unsigned char * )&ServerName);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD __stdcall NetrDfsDeleteExitPoint( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [in] */ LPGUID Uid,
    /* [string][in] */ LPWSTR Prefix,
    /* [in] */ ULONG Type)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1612],
                 ( unsigned char * )&ServerName);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD __stdcall NetrDfsModifyPrefix( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [in] */ LPGUID Uid,
    /* [string][in] */ LPWSTR Prefix)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1644],
                 ( unsigned char * )&ServerName);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD __stdcall NetrDfsFixLocalVolume( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [string][in] */ LPWSTR VolumeName,
    /* [in] */ ULONG EntryType,
    /* [in] */ ULONG ServiceType,
    /* [string][in] */ LPWSTR StgId,
    /* [in] */ LPGUID EntryUid,
    /* [string][in] */ LPWSTR EntryPrefix,
    /* [in] */ LPNET_DFS_ENTRY_ID_CONTAINER RelationInfo,
    /* [in] */ ULONG CreateDisposition)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1674],
                 ( unsigned char * )&ServerName);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD __stdcall NetrDfsManagerReportSiteInfo( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [unique][out][in] */ LPDFS_SITELIST_INFO *ppSiteInfo)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1722],
                 ( unsigned char * )&ServerName);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetrServerTransportDelEx( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [in] */ DWORD Level,
    /* [switch_is][in] */ LPTRANSPORT_INFO Buffer)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1748],
                 ( unsigned char * )&ServerName);
    return ( DWORD  )_RetVal.Simple;
    
}

extern const GENERIC_BINDING_ROUTINE_PAIR BindingRoutines[ GENERIC_BINDING_TABLE_SIZE ];

#if !defined(__RPC_WIN32__)
#error  Invalid build platform for this stub.
#endif

static const MIDL_PROC_FORMAT_STRING __MIDL_ProcFormatString =
    {
        0,
        {
			0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/*  2 */	NdrFcLong( 0x0 ),	/* 0 */
/*  6 */	NdrFcShort( 0x0 ),	/* 0 */
/*  8 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 10 */	0x31,		/* FC_BIND_GENERIC */
			0x4,		/* 4 */
/* 12 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 14 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 16 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 18 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 20 */	
			0x50,		/* FC_IN_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 22 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */
/* 24 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 26 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 28 */	NdrFcShort( 0xde ),	/* Type Offset=222 */
/* 30 */	
			0x50,		/* FC_IN_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 32 */	NdrFcShort( 0xe2 ),	/* Type Offset=226 */
/* 34 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 36 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 38 */	NdrFcLong( 0x0 ),	/* 0 */
/* 42 */	NdrFcShort( 0x1 ),	/* 1 */
/* 44 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 46 */	0x31,		/* FC_BIND_GENERIC */
			0x4,		/* 4 */
/* 48 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 50 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 52 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 54 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 56 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 58 */	NdrFcShort( 0xe6 ),	/* Type Offset=230 */
/* 60 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 62 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 64 */	NdrFcShort( 0xea ),	/* Type Offset=234 */
/* 66 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 68 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 70 */	NdrFcLong( 0x0 ),	/* 0 */
/* 74 */	NdrFcShort( 0x2 ),	/* 2 */
/* 76 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 78 */	0x31,		/* FC_BIND_GENERIC */
			0x4,		/* 4 */
/* 80 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 82 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 84 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 86 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 88 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 90 */	NdrFcShort( 0xe6 ),	/* Type Offset=230 */
/* 92 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 94 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 96 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 98 */	NdrFcLong( 0x0 ),	/* 0 */
/* 102 */	NdrFcShort( 0x3 ),	/* 3 */
/* 104 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 106 */	0x31,		/* FC_BIND_GENERIC */
			0x4,		/* 4 */
/* 108 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 110 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 112 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 114 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 116 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 118 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 120 */	
			0x50,		/* FC_IN_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 122 */	NdrFcShort( 0x110 ),	/* Type Offset=272 */
/* 124 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 126 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 128 */	NdrFcShort( 0xde ),	/* Type Offset=222 */
/* 130 */	
			0x50,		/* FC_IN_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 132 */	NdrFcShort( 0xe2 ),	/* Type Offset=226 */
/* 134 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 136 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 138 */	NdrFcLong( 0x0 ),	/* 0 */
/* 142 */	NdrFcShort( 0x4 ),	/* 4 */
/* 144 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 146 */	0x31,		/* FC_BIND_GENERIC */
			0x4,		/* 4 */
/* 148 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 150 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 152 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 154 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 156 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 158 */	NdrFcShort( 0xe6 ),	/* Type Offset=230 */
/* 160 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 162 */	NdrFcShort( 0xe6 ),	/* Type Offset=230 */
/* 164 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 166 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 168 */	NdrFcShort( 0x19e ),	/* Type Offset=414 */
/* 170 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 172 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 174 */	NdrFcLong( 0x0 ),	/* 0 */
/* 178 */	NdrFcShort( 0x5 ),	/* 5 */
/* 180 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 182 */	0x31,		/* FC_BIND_GENERIC */
			0x4,		/* 4 */
/* 184 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 186 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 188 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 190 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 192 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 194 */	NdrFcShort( 0xe6 ),	/* Type Offset=230 */
/* 196 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 198 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 200 */	NdrFcShort( 0x1da ),	/* Type Offset=474 */
/* 202 */	
			0x50,		/* FC_IN_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 204 */	NdrFcShort( 0xe2 ),	/* Type Offset=226 */
/* 206 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 208 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 210 */	NdrFcLong( 0x0 ),	/* 0 */
/* 214 */	NdrFcShort( 0x6 ),	/* 6 */
/* 216 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 218 */	0x31,		/* FC_BIND_GENERIC */
			0x4,		/* 4 */
/* 220 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 222 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 224 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 226 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 228 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 230 */	NdrFcShort( 0xe6 ),	/* Type Offset=230 */
/* 232 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 234 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 236 */	NdrFcLong( 0x0 ),	/* 0 */
/* 240 */	NdrFcShort( 0x7 ),	/* 7 */
/* 242 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 244 */	0x31,		/* FC_BIND_GENERIC */
			0x4,		/* 4 */
/* 246 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 248 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 250 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 252 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 254 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 256 */	NdrFcShort( 0xe6 ),	/* Type Offset=230 */
/* 258 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 260 */	NdrFcShort( 0xe6 ),	/* Type Offset=230 */
/* 262 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 264 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 266 */	NdrFcLong( 0x0 ),	/* 0 */
/* 270 */	NdrFcShort( 0x8 ),	/* 8 */
/* 272 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 274 */	0x31,		/* FC_BIND_GENERIC */
			0x4,		/* 4 */
/* 276 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 278 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 280 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 282 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 284 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 286 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 288 */	
			0x50,		/* FC_IN_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 290 */	NdrFcShort( 0x1e6 ),	/* Type Offset=486 */
/* 292 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 294 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 296 */	NdrFcShort( 0xde ),	/* Type Offset=222 */
/* 298 */	
			0x50,		/* FC_IN_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 300 */	NdrFcShort( 0xe2 ),	/* Type Offset=226 */
/* 302 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 304 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 306 */	NdrFcLong( 0x0 ),	/* 0 */
/* 310 */	NdrFcShort( 0x9 ),	/* 9 */
/* 312 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 314 */	0x31,		/* FC_BIND_GENERIC */
			0x4,		/* 4 */
/* 316 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 318 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 320 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 322 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 324 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 326 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 328 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 330 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 332 */	
			0x50,		/* FC_IN_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 334 */	NdrFcShort( 0x29c ),	/* Type Offset=668 */
/* 336 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 338 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 340 */	NdrFcShort( 0xde ),	/* Type Offset=222 */
/* 342 */	
			0x50,		/* FC_IN_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 344 */	NdrFcShort( 0xe2 ),	/* Type Offset=226 */
/* 346 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 348 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 350 */	NdrFcLong( 0x0 ),	/* 0 */
/* 354 */	NdrFcShort( 0xa ),	/* 10 */
/* 356 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 358 */	0x31,		/* FC_BIND_GENERIC */
			0x4,		/* 4 */
/* 360 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 362 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 364 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 366 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 368 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 370 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 372 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 374 */	NdrFcShort( 0x32a ),	/* Type Offset=810 */
/* 376 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 378 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 380 */	NdrFcLong( 0x0 ),	/* 0 */
/* 384 */	NdrFcShort( 0xb ),	/* 11 */
/* 386 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 388 */	0x31,		/* FC_BIND_GENERIC */
			0x4,		/* 4 */
/* 390 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 392 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 394 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 396 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 398 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 400 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 402 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 404 */	NdrFcLong( 0x0 ),	/* 0 */
/* 408 */	NdrFcShort( 0xc ),	/* 12 */
/* 410 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 412 */	0x31,		/* FC_BIND_GENERIC */
			0x4,		/* 4 */
/* 414 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 416 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 418 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 420 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 422 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 424 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 426 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 428 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 430 */	
			0x50,		/* FC_IN_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 432 */	NdrFcShort( 0x350 ),	/* Type Offset=848 */
/* 434 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 436 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 438 */	NdrFcShort( 0xde ),	/* Type Offset=222 */
/* 440 */	
			0x50,		/* FC_IN_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 442 */	NdrFcShort( 0xe2 ),	/* Type Offset=226 */
/* 444 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 446 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 448 */	NdrFcLong( 0x0 ),	/* 0 */
/* 452 */	NdrFcShort( 0xd ),	/* 13 */
/* 454 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 456 */	0x31,		/* FC_BIND_GENERIC */
			0x4,		/* 4 */
/* 458 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 460 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 462 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 464 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 466 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 468 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 470 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 472 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 474 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 476 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 478 */	NdrFcLong( 0x0 ),	/* 0 */
/* 482 */	NdrFcShort( 0xe ),	/* 14 */
/* 484 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 486 */	0x31,		/* FC_BIND_GENERIC */
			0x4,		/* 4 */
/* 488 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 490 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 492 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 494 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 496 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 498 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 500 */	NdrFcShort( 0x54e ),	/* Type Offset=1358 */
/* 502 */	
			0x50,		/* FC_IN_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 504 */	NdrFcShort( 0xe2 ),	/* Type Offset=226 */
/* 506 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 508 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 510 */	NdrFcLong( 0x0 ),	/* 0 */
/* 514 */	NdrFcShort( 0xf ),	/* 15 */
/* 516 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 518 */	0x31,		/* FC_BIND_GENERIC */
			0x4,		/* 4 */
/* 520 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 522 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 524 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 526 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 528 */	
			0x50,		/* FC_IN_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 530 */	NdrFcShort( 0x66a ),	/* Type Offset=1642 */
/* 532 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 534 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 536 */	NdrFcShort( 0xde ),	/* Type Offset=222 */
/* 538 */	
			0x50,		/* FC_IN_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 540 */	NdrFcShort( 0xe2 ),	/* Type Offset=226 */
/* 542 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 544 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 546 */	NdrFcLong( 0x0 ),	/* 0 */
/* 550 */	NdrFcShort( 0x10 ),	/* 16 */
/* 552 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 554 */	0x31,		/* FC_BIND_GENERIC */
			0x4,		/* 4 */
/* 556 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 558 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 560 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 562 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 564 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 566 */	NdrFcShort( 0xe6 ),	/* Type Offset=230 */
/* 568 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 570 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 572 */	NdrFcShort( 0x790 ),	/* Type Offset=1936 */
/* 574 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 576 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 578 */	NdrFcLong( 0x0 ),	/* 0 */
/* 582 */	NdrFcShort( 0x11 ),	/* 17 */
/* 584 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 586 */	0x31,		/* FC_BIND_GENERIC */
			0x4,		/* 4 */
/* 588 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 590 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 592 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 594 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 596 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 598 */	NdrFcShort( 0xe6 ),	/* Type Offset=230 */
/* 600 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 602 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 604 */	NdrFcShort( 0x79c ),	/* Type Offset=1948 */
/* 606 */	
			0x50,		/* FC_IN_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 608 */	NdrFcShort( 0xe2 ),	/* Type Offset=226 */
/* 610 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 612 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 614 */	NdrFcLong( 0x0 ),	/* 0 */
/* 618 */	NdrFcShort( 0x12 ),	/* 18 */
/* 620 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 622 */	0x31,		/* FC_BIND_GENERIC */
			0x4,		/* 4 */
/* 624 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 626 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 628 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 630 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 632 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 634 */	NdrFcShort( 0xe6 ),	/* Type Offset=230 */
/* 636 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 638 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 640 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 642 */	NdrFcLong( 0x0 ),	/* 0 */
/* 646 */	NdrFcShort( 0x13 ),	/* 19 */
/* 648 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 650 */	0x31,		/* FC_BIND_GENERIC */
			0x4,		/* 4 */
/* 652 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 654 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 656 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 658 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 660 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 662 */	NdrFcShort( 0xe6 ),	/* Type Offset=230 */
/* 664 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 666 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 668 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 670 */	NdrFcLong( 0x0 ),	/* 0 */
/* 674 */	NdrFcShort( 0x14 ),	/* 20 */
/* 676 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 678 */	0x31,		/* FC_BIND_GENERIC */
			0x4,		/* 4 */
/* 680 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 682 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 684 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 686 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 688 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 690 */	NdrFcShort( 0xe6 ),	/* Type Offset=230 */
/* 692 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 694 */	NdrFcShort( 0xde ),	/* Type Offset=222 */
/* 696 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 698 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 700 */	NdrFcLong( 0x0 ),	/* 0 */
/* 704 */	NdrFcShort( 0x15 ),	/* 21 */
/* 706 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 708 */	0x31,		/* FC_BIND_GENERIC */
			0x4,		/* 4 */
/* 710 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 712 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 714 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 716 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 718 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 720 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 722 */	NdrFcShort( 0x7a8 ),	/* Type Offset=1960 */
/* 724 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 726 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 728 */	NdrFcLong( 0x0 ),	/* 0 */
/* 732 */	NdrFcShort( 0x16 ),	/* 22 */
/* 734 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 736 */	0x31,		/* FC_BIND_GENERIC */
			0x4,		/* 4 */
/* 738 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 740 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 742 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 744 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 746 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 748 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 750 */	NdrFcShort( 0xadc ),	/* Type Offset=2780 */
/* 752 */	
			0x50,		/* FC_IN_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 754 */	NdrFcShort( 0xe2 ),	/* Type Offset=226 */
/* 756 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 758 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 760 */	NdrFcLong( 0x0 ),	/* 0 */
/* 764 */	NdrFcShort( 0x17 ),	/* 23 */
/* 766 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 768 */	0x31,		/* FC_BIND_GENERIC */
			0x4,		/* 4 */
/* 770 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 772 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 774 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 776 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 778 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 780 */	
			0x50,		/* FC_IN_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 782 */	NdrFcShort( 0xae8 ),	/* Type Offset=2792 */
/* 784 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 786 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 788 */	NdrFcShort( 0xde ),	/* Type Offset=222 */
/* 790 */	
			0x50,		/* FC_IN_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 792 */	NdrFcShort( 0xe2 ),	/* Type Offset=226 */
/* 794 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 796 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 798 */	NdrFcLong( 0x0 ),	/* 0 */
/* 802 */	NdrFcShort( 0x18 ),	/* 24 */
/* 804 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 806 */	0x31,		/* FC_BIND_GENERIC */
			0x4,		/* 4 */
/* 808 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 810 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 812 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 814 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 816 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 818 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 820 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 822 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 824 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 826 */	NdrFcShort( 0xb24 ),	/* Type Offset=2852 */
/* 828 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 830 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 832 */	NdrFcLong( 0x0 ),	/* 0 */
/* 836 */	NdrFcShort( 0x19 ),	/* 25 */
/* 838 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 840 */	0x31,		/* FC_BIND_GENERIC */
			0x4,		/* 4 */
/* 842 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 844 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 846 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 848 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 850 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 852 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 854 */	NdrFcShort( 0xb42 ),	/* Type Offset=2882 */
/* 856 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 858 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 860 */	NdrFcLong( 0x0 ),	/* 0 */
/* 864 */	NdrFcShort( 0x1a ),	/* 26 */
/* 866 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 868 */	0x31,		/* FC_BIND_GENERIC */
			0x4,		/* 4 */
/* 870 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 872 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 874 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 876 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 878 */	
			0x50,		/* FC_IN_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 880 */	NdrFcShort( 0xb7c ),	/* Type Offset=2940 */
/* 882 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 884 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 886 */	NdrFcShort( 0xde ),	/* Type Offset=222 */
/* 888 */	
			0x50,		/* FC_IN_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 890 */	NdrFcShort( 0xe2 ),	/* Type Offset=226 */
/* 892 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 894 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 896 */	NdrFcLong( 0x0 ),	/* 0 */
/* 900 */	NdrFcShort( 0x1b ),	/* 27 */
/* 902 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 904 */	0x31,		/* FC_BIND_GENERIC */
			0x4,		/* 4 */
/* 906 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 908 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 910 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 912 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 914 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 916 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 918 */	NdrFcShort( 0xb42 ),	/* Type Offset=2882 */
/* 920 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 922 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 924 */	NdrFcLong( 0x0 ),	/* 0 */
/* 928 */	NdrFcShort( 0x1c ),	/* 28 */
/* 930 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 932 */	0x31,		/* FC_BIND_GENERIC */
			0x4,		/* 4 */
/* 934 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 936 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 938 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 940 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 942 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 944 */	NdrFcShort( 0xd9c ),	/* Type Offset=3484 */
/* 946 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 948 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 950 */	NdrFcLong( 0x0 ),	/* 0 */
/* 954 */	NdrFcShort( 0x1d ),	/* 29 */
/* 956 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 958 */	0x31,		/* FC_BIND_GENERIC */
			0x4,		/* 4 */
/* 960 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 962 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 964 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 966 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 968 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 970 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 972 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 974 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 976 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 978 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 980 */	NdrFcLong( 0x0 ),	/* 0 */
/* 984 */	NdrFcShort( 0x1e ),	/* 30 */
/* 986 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 988 */	0x31,		/* FC_BIND_GENERIC */
			0x4,		/* 4 */
/* 990 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 992 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 994 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 996 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 998 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1000 */	NdrFcShort( 0xe6 ),	/* Type Offset=230 */
/* 1002 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1004 */	NdrFcShort( 0xde ),	/* Type Offset=222 */
/* 1006 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1008 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1010 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1012 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1016 */	NdrFcShort( 0x1f ),	/* 31 */
/* 1018 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 1020 */	0x31,		/* FC_BIND_GENERIC */
			0x4,		/* 4 */
/* 1022 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1024 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 1026 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1028 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 1030 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1032 */	NdrFcShort( 0xe6 ),	/* Type Offset=230 */
/* 1034 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1036 */	NdrFcShort( 0xdb6 ),	/* Type Offset=3510 */
/* 1038 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1040 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1042 */	NdrFcShort( 0xe6 ),	/* Type Offset=230 */
/* 1044 */	
			0x50,		/* FC_IN_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1046 */	NdrFcShort( 0xde ),	/* Type Offset=222 */
/* 1048 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1050 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1052 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1054 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1058 */	NdrFcShort( 0x20 ),	/* 32 */
/* 1060 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 1062 */	0x31,		/* FC_BIND_GENERIC */
			0x4,		/* 4 */
/* 1064 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1066 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 1068 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1070 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 1072 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1074 */	NdrFcShort( 0xe6 ),	/* Type Offset=230 */
/* 1076 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1078 */	NdrFcShort( 0xe6 ),	/* Type Offset=230 */
/* 1080 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1082 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1084 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1086 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1088 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1092 */	NdrFcShort( 0x21 ),	/* 33 */
/* 1094 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1096 */	0x31,		/* FC_BIND_GENERIC */
			0x4,		/* 4 */
/* 1098 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1100 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 1102 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1104 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 1106 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1108 */	NdrFcShort( 0xe6 ),	/* Type Offset=230 */
/* 1110 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1112 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1114 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1116 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1118 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1122 */	NdrFcShort( 0x22 ),	/* 34 */
/* 1124 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 1126 */	0x31,		/* FC_BIND_GENERIC */
			0x4,		/* 4 */
/* 1128 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1130 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 1132 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1134 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 1136 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1138 */	NdrFcShort( 0xe6 ),	/* Type Offset=230 */
/* 1140 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1142 */	NdrFcShort( 0xdc4 ),	/* Type Offset=3524 */
/* 1144 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1146 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1148 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1150 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1152 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1154 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1158 */	NdrFcShort( 0x23 ),	/* 35 */
/* 1160 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 1162 */	0x31,		/* FC_BIND_GENERIC */
			0x4,		/* 4 */
/* 1164 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1166 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 1168 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1170 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 1172 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1174 */	NdrFcShort( 0xe6 ),	/* Type Offset=230 */
/* 1176 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1178 */	NdrFcShort( 0xe6 ),	/* Type Offset=230 */
/* 1180 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1182 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1184 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1186 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1188 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1192 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1194 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 1196 */	0x31,		/* FC_BIND_GENERIC */
			0x4,		/* 4 */
/* 1198 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1200 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 1202 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1204 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 1206 */	
			0x50,		/* FC_IN_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1208 */	NdrFcShort( 0x66a ),	/* Type Offset=1642 */
/* 1210 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1212 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1214 */	NdrFcShort( 0xde ),	/* Type Offset=222 */
/* 1216 */	
			0x50,		/* FC_IN_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1218 */	NdrFcShort( 0xe2 ),	/* Type Offset=226 */
/* 1220 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1222 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1224 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1228 */	NdrFcShort( 0x25 ),	/* 37 */
/* 1230 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1232 */	0x31,		/* FC_BIND_GENERIC */
			0x4,		/* 4 */
/* 1234 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1236 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 1238 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1240 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 1242 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1244 */	NdrFcShort( 0xe6 ),	/* Type Offset=230 */
/* 1246 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1248 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1250 */	NdrFcShort( 0xdd2 ),	/* Type Offset=3538 */
/* 1252 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1254 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1256 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1260 */	NdrFcShort( 0x26 ),	/* 38 */
/* 1262 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1264 */	0x30,		/* FC_BIND_CONTEXT */
			0xe8,		/* Ctxt flags:  via ptr, in, out, strict, */
/* 1266 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1268 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1270 */	
			0x50,		/* FC_IN_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1272 */	NdrFcShort( 0xdda ),	/* Type Offset=3546 */
/* 1274 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1276 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1278 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1282 */	NdrFcShort( 0x27 ),	/* 39 */
/* 1284 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 1286 */	0x31,		/* FC_BIND_GENERIC */
			0x4,		/* 4 */
/* 1288 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1290 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 1292 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1294 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 1296 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1298 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 1300 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1302 */	NdrFcShort( 0xe6 ),	/* Type Offset=230 */
/* 1304 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1306 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1308 */	NdrFcShort( 0xde2 ),	/* Type Offset=3554 */
/* 1310 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1312 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1314 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1318 */	NdrFcShort( 0x28 ),	/* 40 */
/* 1320 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 1322 */	0x31,		/* FC_BIND_GENERIC */
			0x4,		/* 4 */
/* 1324 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1326 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 1328 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1330 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 1332 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1334 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 1336 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1338 */	NdrFcShort( 0xe6 ),	/* Type Offset=230 */
/* 1340 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1342 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1344 */	NdrFcShort( 0xde6 ),	/* Type Offset=3558 */
/* 1346 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1348 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1350 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1354 */	NdrFcShort( 0x29 ),	/* 41 */
/* 1356 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1358 */	0x31,		/* FC_BIND_GENERIC */
			0x4,		/* 4 */
/* 1360 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1362 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 1364 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1366 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 1368 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1370 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1372 */	NdrFcShort( 0xdea ),	/* Type Offset=3562 */
/* 1374 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1376 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1378 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1382 */	NdrFcShort( 0x2a ),	/* 42 */
/* 1384 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 1386 */	0x31,		/* FC_BIND_GENERIC */
			0x4,		/* 4 */
/* 1388 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1390 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 1392 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1394 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 1396 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1398 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 1400 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1402 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 1404 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1406 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1408 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1410 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1412 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1414 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1418 */	NdrFcShort( 0x2b ),	/* 43 */
/* 1420 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1422 */	0x31,		/* FC_BIND_GENERIC */
			0x4,		/* 4 */
/* 1424 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1426 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 1428 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1430 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 1432 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1434 */	NdrFcShort( 0xde ),	/* Type Offset=222 */
/* 1436 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1438 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1440 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1444 */	NdrFcShort( 0x2c ),	/* 44 */
/* 1446 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 1448 */	0x31,		/* FC_BIND_GENERIC */
			0x4,		/* 4 */
/* 1450 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1452 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 1454 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1456 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 1458 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1460 */	NdrFcShort( 0xe6 ),	/* Type Offset=230 */
/* 1462 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1464 */	NdrFcShort( 0xe14 ),	/* Type Offset=3604 */
/* 1466 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1468 */	NdrFcShort( 0xe6 ),	/* Type Offset=230 */
/* 1470 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1472 */	NdrFcShort( 0xe6 ),	/* Type Offset=230 */
/* 1474 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1476 */	NdrFcShort( 0xe2a ),	/* Type Offset=3626 */
/* 1478 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1480 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1482 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1484 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1488 */	NdrFcShort( 0x2d ),	/* 45 */
/* 1490 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1492 */	0x31,		/* FC_BIND_GENERIC */
			0x4,		/* 4 */
/* 1494 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1496 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 1498 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1500 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 1502 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1504 */	NdrFcShort( 0xe14 ),	/* Type Offset=3604 */
/* 1506 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1508 */	NdrFcShort( 0xe6 ),	/* Type Offset=230 */
/* 1510 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1512 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1514 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1518 */	NdrFcShort( 0x2e ),	/* 46 */
/* 1520 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1522 */	0x31,		/* FC_BIND_GENERIC */
			0x4,		/* 4 */
/* 1524 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1526 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 1528 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1530 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 1532 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1534 */	NdrFcShort( 0xe14 ),	/* Type Offset=3604 */
/* 1536 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1538 */	NdrFcShort( 0xe6 ),	/* Type Offset=230 */
/* 1540 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1542 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1544 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1546 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1550 */	NdrFcShort( 0x2f ),	/* 47 */
/* 1552 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1554 */	0x31,		/* FC_BIND_GENERIC */
			0x4,		/* 4 */
/* 1556 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1558 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 1560 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1562 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 1564 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1566 */	NdrFcShort( 0xe14 ),	/* Type Offset=3604 */
/* 1568 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1570 */	NdrFcShort( 0xe6 ),	/* Type Offset=230 */
/* 1572 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1574 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1576 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1580 */	NdrFcShort( 0x30 ),	/* 48 */
/* 1582 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 1584 */	0x31,		/* FC_BIND_GENERIC */
			0x4,		/* 4 */
/* 1586 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1588 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 1590 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1592 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 1594 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1596 */	NdrFcShort( 0xe14 ),	/* Type Offset=3604 */
/* 1598 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1600 */	NdrFcShort( 0xe6 ),	/* Type Offset=230 */
/* 1602 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1604 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1606 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1608 */	NdrFcShort( 0xe7a ),	/* Type Offset=3706 */
/* 1610 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1612 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1614 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1618 */	NdrFcShort( 0x31 ),	/* 49 */
/* 1620 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1622 */	0x31,		/* FC_BIND_GENERIC */
			0x4,		/* 4 */
/* 1624 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1626 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 1628 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1630 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 1632 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1634 */	NdrFcShort( 0xe14 ),	/* Type Offset=3604 */
/* 1636 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1638 */	NdrFcShort( 0xe6 ),	/* Type Offset=230 */
/* 1640 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1642 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1644 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1646 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1650 */	NdrFcShort( 0x32 ),	/* 50 */
/* 1652 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1654 */	0x31,		/* FC_BIND_GENERIC */
			0x4,		/* 4 */
/* 1656 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1658 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 1660 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1662 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 1664 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1666 */	NdrFcShort( 0xe14 ),	/* Type Offset=3604 */
/* 1668 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1670 */	NdrFcShort( 0xe6 ),	/* Type Offset=230 */
/* 1672 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1674 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1676 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1680 */	NdrFcShort( 0x33 ),	/* 51 */
/* 1682 */	NdrFcShort( 0x28 ),	/* x86 Stack size/offset = 40 */
/* 1684 */	0x31,		/* FC_BIND_GENERIC */
			0x4,		/* 4 */
/* 1686 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1688 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 1690 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1692 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 1694 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1696 */	NdrFcShort( 0xe6 ),	/* Type Offset=230 */
/* 1698 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1700 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1702 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1704 */	NdrFcShort( 0xe6 ),	/* Type Offset=230 */
/* 1706 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1708 */	NdrFcShort( 0xe14 ),	/* Type Offset=3604 */
/* 1710 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1712 */	NdrFcShort( 0xe6 ),	/* Type Offset=230 */
/* 1714 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1716 */	NdrFcShort( 0xe2a ),	/* Type Offset=3626 */
/* 1718 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1720 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1722 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1724 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1728 */	NdrFcShort( 0x34 ),	/* 52 */
/* 1730 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1732 */	0x31,		/* FC_BIND_GENERIC */
			0x4,		/* 4 */
/* 1734 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1736 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 1738 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1740 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 1742 */	
			0x50,		/* FC_IN_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1744 */	NdrFcShort( 0xe88 ),	/* Type Offset=3720 */
/* 1746 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1748 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1750 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1754 */	NdrFcShort( 0x35 ),	/* 53 */
/* 1756 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1758 */	0x31,		/* FC_BIND_GENERIC */
			0x4,		/* 4 */
/* 1760 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1762 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 1764 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1766 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 1768 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1770 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1772 */	NdrFcShort( 0xeba ),	/* Type Offset=3770 */
/* 1774 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */

			0x0
        }
    };

static const MIDL_TYPE_FORMAT_STRING __MIDL_TypeFormatString =
    {
        0,
        {
			NdrFcShort( 0x0 ),	/* 0 */
/*  2 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/*  4 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/*  6 */	
			0x11, 0x0,	/* FC_RP */
/*  8 */	NdrFcShort( 0xc8 ),	/* Offset= 200 (208) */
/* 10 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0x9,		/* FC_ULONG */
/* 12 */	0x9,		/* Corr desc: FC_ULONG */
			0x0,		/*  */
/* 14 */	NdrFcShort( 0xfffc ),	/* -4 */
/* 16 */	NdrFcShort( 0x2 ),	/* Offset= 2 (18) */
/* 18 */	NdrFcShort( 0x4 ),	/* 4 */
/* 20 */	NdrFcShort( 0x3002 ),	/* 12290 */
/* 22 */	NdrFcLong( 0x0 ),	/* 0 */
/* 26 */	NdrFcShort( 0xa ),	/* Offset= 10 (36) */
/* 28 */	NdrFcLong( 0x1 ),	/* 1 */
/* 32 */	NdrFcShort( 0x50 ),	/* Offset= 80 (112) */
/* 34 */	NdrFcShort( 0x0 ),	/* Offset= 0 (34) */
/* 36 */	
			0x12, 0x0,	/* FC_UP */
/* 38 */	NdrFcShort( 0x36 ),	/* Offset= 54 (92) */
/* 40 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 42 */	NdrFcShort( 0x4 ),	/* 4 */
/* 44 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 46 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 48 */	NdrFcShort( 0x0 ),	/* 0 */
/* 50 */	NdrFcShort( 0x0 ),	/* 0 */
/* 52 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 54 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 56 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 58 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 60 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 62 */	NdrFcShort( 0x4 ),	/* 4 */
/* 64 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 66 */	NdrFcShort( 0x0 ),	/* 0 */
/* 68 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 70 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 72 */	NdrFcShort( 0x4 ),	/* 4 */
/* 74 */	NdrFcShort( 0x0 ),	/* 0 */
/* 76 */	NdrFcShort( 0x1 ),	/* 1 */
/* 78 */	NdrFcShort( 0x0 ),	/* 0 */
/* 80 */	NdrFcShort( 0x0 ),	/* 0 */
/* 82 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 84 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 86 */	
			0x5b,		/* FC_END */

			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 88 */	0x0,		/* 0 */
			NdrFcShort( 0xffcf ),	/* Offset= -49 (40) */
			0x5b,		/* FC_END */
/* 92 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 94 */	NdrFcShort( 0x8 ),	/* 8 */
/* 96 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 98 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 100 */	NdrFcShort( 0x4 ),	/* 4 */
/* 102 */	NdrFcShort( 0x4 ),	/* 4 */
/* 104 */	0x12, 0x1,	/* FC_UP [all_nodes] */
/* 106 */	NdrFcShort( 0xffd2 ),	/* Offset= -46 (60) */
/* 108 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 110 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 112 */	
			0x12, 0x0,	/* FC_UP */
/* 114 */	NdrFcShort( 0x4a ),	/* Offset= 74 (188) */
/* 116 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 118 */	NdrFcShort( 0x10 ),	/* 16 */
/* 120 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 122 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 124 */	NdrFcShort( 0x0 ),	/* 0 */
/* 126 */	NdrFcShort( 0x0 ),	/* 0 */
/* 128 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 130 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 132 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 134 */	NdrFcShort( 0x8 ),	/* 8 */
/* 136 */	NdrFcShort( 0x8 ),	/* 8 */
/* 138 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 140 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 142 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 144 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 146 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 148 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 150 */	NdrFcShort( 0x10 ),	/* 16 */
/* 152 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 154 */	NdrFcShort( 0x0 ),	/* 0 */
/* 156 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 158 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 160 */	NdrFcShort( 0x10 ),	/* 16 */
/* 162 */	NdrFcShort( 0x0 ),	/* 0 */
/* 164 */	NdrFcShort( 0x2 ),	/* 2 */
/* 166 */	NdrFcShort( 0x0 ),	/* 0 */
/* 168 */	NdrFcShort( 0x0 ),	/* 0 */
/* 170 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 172 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 174 */	NdrFcShort( 0x8 ),	/* 8 */
/* 176 */	NdrFcShort( 0x8 ),	/* 8 */
/* 178 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 180 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 182 */	
			0x5b,		/* FC_END */

			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 184 */	0x0,		/* 0 */
			NdrFcShort( 0xffbb ),	/* Offset= -69 (116) */
			0x5b,		/* FC_END */
/* 188 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 190 */	NdrFcShort( 0x8 ),	/* 8 */
/* 192 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 194 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 196 */	NdrFcShort( 0x4 ),	/* 4 */
/* 198 */	NdrFcShort( 0x4 ),	/* 4 */
/* 200 */	0x12, 0x1,	/* FC_UP [all_nodes] */
/* 202 */	NdrFcShort( 0xffca ),	/* Offset= -54 (148) */
/* 204 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 206 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 208 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 210 */	NdrFcShort( 0x8 ),	/* 8 */
/* 212 */	NdrFcShort( 0x0 ),	/* 0 */
/* 214 */	NdrFcShort( 0x0 ),	/* Offset= 0 (214) */
/* 216 */	0x8,		/* FC_LONG */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 218 */	0x0,		/* 0 */
			NdrFcShort( 0xff2f ),	/* Offset= -209 (10) */
			0x5b,		/* FC_END */
/* 222 */	
			0x11, 0x8,	/* FC_RP [simple_pointer] */
/* 224 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 226 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 228 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 230 */	
			0x11, 0x8,	/* FC_RP [simple_pointer] */
/* 232 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 234 */	
			0x11, 0x0,	/* FC_RP */
/* 236 */	NdrFcShort( 0x2 ),	/* Offset= 2 (238) */
/* 238 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0x9,		/* FC_ULONG */
/* 240 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 242 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 244 */	NdrFcShort( 0x2 ),	/* Offset= 2 (246) */
/* 246 */	NdrFcShort( 0x4 ),	/* 4 */
/* 248 */	NdrFcShort( 0x3002 ),	/* 12290 */
/* 250 */	NdrFcLong( 0x0 ),	/* 0 */
/* 254 */	NdrFcShort( 0xa ),	/* Offset= 10 (264) */
/* 256 */	NdrFcLong( 0x1 ),	/* 1 */
/* 260 */	NdrFcShort( 0x8 ),	/* Offset= 8 (268) */
/* 262 */	NdrFcShort( 0x0 ),	/* Offset= 0 (262) */
/* 264 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 266 */	NdrFcShort( 0xff1e ),	/* Offset= -226 (40) */
/* 268 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 270 */	NdrFcShort( 0xff66 ),	/* Offset= -154 (116) */
/* 272 */	
			0x11, 0x0,	/* FC_RP */
/* 274 */	NdrFcShort( 0x7e ),	/* Offset= 126 (400) */
/* 276 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0x9,		/* FC_ULONG */
/* 278 */	0x9,		/* Corr desc: FC_ULONG */
			0x0,		/*  */
/* 280 */	NdrFcShort( 0xfffc ),	/* -4 */
/* 282 */	NdrFcShort( 0x2 ),	/* Offset= 2 (284) */
/* 284 */	NdrFcShort( 0x4 ),	/* 4 */
/* 286 */	NdrFcShort( 0x3002 ),	/* 12290 */
/* 288 */	NdrFcLong( 0x0 ),	/* 0 */
/* 292 */	NdrFcShort( 0xff00 ),	/* Offset= -256 (36) */
/* 294 */	NdrFcLong( 0x1 ),	/* 1 */
/* 298 */	NdrFcShort( 0x4 ),	/* Offset= 4 (302) */
/* 300 */	NdrFcShort( 0x0 ),	/* Offset= 0 (300) */
/* 302 */	
			0x12, 0x0,	/* FC_UP */
/* 304 */	NdrFcShort( 0x4c ),	/* Offset= 76 (380) */
/* 306 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 308 */	NdrFcShort( 0x14 ),	/* 20 */
/* 310 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 312 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 314 */	NdrFcShort( 0x0 ),	/* 0 */
/* 316 */	NdrFcShort( 0x0 ),	/* 0 */
/* 318 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 320 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 322 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 324 */	NdrFcShort( 0x8 ),	/* 8 */
/* 326 */	NdrFcShort( 0x8 ),	/* 8 */
/* 328 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 330 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 332 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 334 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 336 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 338 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 340 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 342 */	NdrFcShort( 0x14 ),	/* 20 */
/* 344 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 346 */	NdrFcShort( 0x0 ),	/* 0 */
/* 348 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 350 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 352 */	NdrFcShort( 0x14 ),	/* 20 */
/* 354 */	NdrFcShort( 0x0 ),	/* 0 */
/* 356 */	NdrFcShort( 0x2 ),	/* 2 */
/* 358 */	NdrFcShort( 0x0 ),	/* 0 */
/* 360 */	NdrFcShort( 0x0 ),	/* 0 */
/* 362 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 364 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 366 */	NdrFcShort( 0x8 ),	/* 8 */
/* 368 */	NdrFcShort( 0x8 ),	/* 8 */
/* 370 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 372 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 374 */	
			0x5b,		/* FC_END */

			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 376 */	0x0,		/* 0 */
			NdrFcShort( 0xffb9 ),	/* Offset= -71 (306) */
			0x5b,		/* FC_END */
/* 380 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 382 */	NdrFcShort( 0x8 ),	/* 8 */
/* 384 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 386 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 388 */	NdrFcShort( 0x4 ),	/* 4 */
/* 390 */	NdrFcShort( 0x4 ),	/* 4 */
/* 392 */	0x12, 0x1,	/* FC_UP [all_nodes] */
/* 394 */	NdrFcShort( 0xffca ),	/* Offset= -54 (340) */
/* 396 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 398 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 400 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 402 */	NdrFcShort( 0x8 ),	/* 8 */
/* 404 */	NdrFcShort( 0x0 ),	/* 0 */
/* 406 */	NdrFcShort( 0x0 ),	/* Offset= 0 (406) */
/* 408 */	0x8,		/* FC_LONG */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 410 */	0x0,		/* 0 */
			NdrFcShort( 0xff79 ),	/* Offset= -135 (276) */
			0x5b,		/* FC_END */
/* 414 */	
			0x11, 0x0,	/* FC_RP */
/* 416 */	NdrFcShort( 0x2 ),	/* Offset= 2 (418) */
/* 418 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0x9,		/* FC_ULONG */
/* 420 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 422 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 424 */	NdrFcShort( 0x2 ),	/* Offset= 2 (426) */
/* 426 */	NdrFcShort( 0x4 ),	/* 4 */
/* 428 */	NdrFcShort( 0x3004 ),	/* 12292 */
/* 430 */	NdrFcLong( 0x0 ),	/* 0 */
/* 434 */	NdrFcShort( 0xff56 ),	/* Offset= -170 (264) */
/* 436 */	NdrFcLong( 0x1 ),	/* 1 */
/* 440 */	NdrFcShort( 0x10 ),	/* Offset= 16 (456) */
/* 442 */	NdrFcLong( 0x3ea ),	/* 1002 */
/* 446 */	NdrFcShort( 0xe ),	/* Offset= 14 (460) */
/* 448 */	NdrFcLong( 0x3eb ),	/* 1003 */
/* 452 */	NdrFcShort( 0x12 ),	/* Offset= 18 (470) */
/* 454 */	NdrFcShort( 0x0 ),	/* Offset= 0 (454) */
/* 456 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 458 */	NdrFcShort( 0xff68 ),	/* Offset= -152 (306) */
/* 460 */	
			0x12, 0x0,	/* FC_UP */
/* 462 */	NdrFcShort( 0x2 ),	/* Offset= 2 (464) */
/* 464 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 466 */	NdrFcShort( 0x4 ),	/* 4 */
/* 468 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 470 */	
			0x12, 0x0,	/* FC_UP */
/* 472 */	NdrFcShort( 0xfe50 ),	/* Offset= -432 (40) */
/* 474 */	
			0x11, 0x0,	/* FC_RP */
/* 476 */	NdrFcShort( 0x2 ),	/* Offset= 2 (478) */
/* 478 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0x9,		/* FC_ULONG */
/* 480 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 482 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 484 */	NdrFcShort( 0xffc6 ),	/* Offset= -58 (426) */
/* 486 */	
			0x11, 0x0,	/* FC_RP */
/* 488 */	NdrFcShort( 0xa6 ),	/* Offset= 166 (654) */
/* 490 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0x9,		/* FC_ULONG */
/* 492 */	0x9,		/* Corr desc: FC_ULONG */
			0x0,		/*  */
/* 494 */	NdrFcShort( 0xfffc ),	/* -4 */
/* 496 */	NdrFcShort( 0x2 ),	/* Offset= 2 (498) */
/* 498 */	NdrFcShort( 0x4 ),	/* 4 */
/* 500 */	NdrFcShort( 0x3002 ),	/* 12290 */
/* 502 */	NdrFcLong( 0x0 ),	/* 0 */
/* 506 */	NdrFcShort( 0xa ),	/* Offset= 10 (516) */
/* 508 */	NdrFcLong( 0x1 ),	/* 1 */
/* 512 */	NdrFcShort( 0x2a ),	/* Offset= 42 (554) */
/* 514 */	NdrFcShort( 0x0 ),	/* Offset= 0 (514) */
/* 516 */	
			0x12, 0x0,	/* FC_UP */
/* 518 */	NdrFcShort( 0x10 ),	/* Offset= 16 (534) */
/* 520 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 522 */	NdrFcShort( 0x4 ),	/* 4 */
/* 524 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 526 */	NdrFcShort( 0x0 ),	/* 0 */
/* 528 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 530 */	NdrFcShort( 0xffbe ),	/* Offset= -66 (464) */
/* 532 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 534 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 536 */	NdrFcShort( 0x8 ),	/* 8 */
/* 538 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 540 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 542 */	NdrFcShort( 0x4 ),	/* 4 */
/* 544 */	NdrFcShort( 0x4 ),	/* 4 */
/* 546 */	0x12, 0x1,	/* FC_UP [all_nodes] */
/* 548 */	NdrFcShort( 0xffe4 ),	/* Offset= -28 (520) */
/* 550 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 552 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 554 */	
			0x12, 0x0,	/* FC_UP */
/* 556 */	NdrFcShort( 0x4e ),	/* Offset= 78 (634) */
/* 558 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 560 */	NdrFcShort( 0x1c ),	/* 28 */
/* 562 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 564 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 566 */	NdrFcShort( 0x14 ),	/* 20 */
/* 568 */	NdrFcShort( 0x14 ),	/* 20 */
/* 570 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 572 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 574 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 576 */	NdrFcShort( 0x18 ),	/* 24 */
/* 578 */	NdrFcShort( 0x18 ),	/* 24 */
/* 580 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 582 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 584 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 586 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 588 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 590 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 592 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 594 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 596 */	NdrFcShort( 0x1c ),	/* 28 */
/* 598 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 600 */	NdrFcShort( 0x0 ),	/* 0 */
/* 602 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 604 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 606 */	NdrFcShort( 0x1c ),	/* 28 */
/* 608 */	NdrFcShort( 0x0 ),	/* 0 */
/* 610 */	NdrFcShort( 0x2 ),	/* 2 */
/* 612 */	NdrFcShort( 0x14 ),	/* 20 */
/* 614 */	NdrFcShort( 0x14 ),	/* 20 */
/* 616 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 618 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 620 */	NdrFcShort( 0x18 ),	/* 24 */
/* 622 */	NdrFcShort( 0x18 ),	/* 24 */
/* 624 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 626 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 628 */	
			0x5b,		/* FC_END */

			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 630 */	0x0,		/* 0 */
			NdrFcShort( 0xffb7 ),	/* Offset= -73 (558) */
			0x5b,		/* FC_END */
/* 634 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 636 */	NdrFcShort( 0x8 ),	/* 8 */
/* 638 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 640 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 642 */	NdrFcShort( 0x4 ),	/* 4 */
/* 644 */	NdrFcShort( 0x4 ),	/* 4 */
/* 646 */	0x12, 0x1,	/* FC_UP [all_nodes] */
/* 648 */	NdrFcShort( 0xffca ),	/* Offset= -54 (594) */
/* 650 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 652 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 654 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 656 */	NdrFcShort( 0x8 ),	/* 8 */
/* 658 */	NdrFcShort( 0x0 ),	/* 0 */
/* 660 */	NdrFcShort( 0x0 ),	/* Offset= 0 (660) */
/* 662 */	0x8,		/* FC_LONG */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 664 */	0x0,		/* 0 */
			NdrFcShort( 0xff51 ),	/* Offset= -175 (490) */
			0x5b,		/* FC_END */
/* 668 */	
			0x11, 0x0,	/* FC_RP */
/* 670 */	NdrFcShort( 0x7e ),	/* Offset= 126 (796) */
/* 672 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0x9,		/* FC_ULONG */
/* 674 */	0x9,		/* Corr desc: FC_ULONG */
			0x0,		/*  */
/* 676 */	NdrFcShort( 0xfffc ),	/* -4 */
/* 678 */	NdrFcShort( 0x2 ),	/* Offset= 2 (680) */
/* 680 */	NdrFcShort( 0x4 ),	/* 4 */
/* 682 */	NdrFcShort( 0x3002 ),	/* 12290 */
/* 684 */	NdrFcLong( 0x2 ),	/* 2 */
/* 688 */	NdrFcShort( 0xff54 ),	/* Offset= -172 (516) */
/* 690 */	NdrFcLong( 0x3 ),	/* 3 */
/* 694 */	NdrFcShort( 0x4 ),	/* Offset= 4 (698) */
/* 696 */	NdrFcShort( 0x0 ),	/* Offset= 0 (696) */
/* 698 */	
			0x12, 0x0,	/* FC_UP */
/* 700 */	NdrFcShort( 0x4c ),	/* Offset= 76 (776) */
/* 702 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 704 */	NdrFcShort( 0x14 ),	/* 20 */
/* 706 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 708 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 710 */	NdrFcShort( 0xc ),	/* 12 */
/* 712 */	NdrFcShort( 0xc ),	/* 12 */
/* 714 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 716 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 718 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 720 */	NdrFcShort( 0x10 ),	/* 16 */
/* 722 */	NdrFcShort( 0x10 ),	/* 16 */
/* 724 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 726 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 728 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 730 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 732 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 734 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 736 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 738 */	NdrFcShort( 0x14 ),	/* 20 */
/* 740 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 742 */	NdrFcShort( 0x0 ),	/* 0 */
/* 744 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 746 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 748 */	NdrFcShort( 0x14 ),	/* 20 */
/* 750 */	NdrFcShort( 0x0 ),	/* 0 */
/* 752 */	NdrFcShort( 0x2 ),	/* 2 */
/* 754 */	NdrFcShort( 0xc ),	/* 12 */
/* 756 */	NdrFcShort( 0xc ),	/* 12 */
/* 758 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 760 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 762 */	NdrFcShort( 0x10 ),	/* 16 */
/* 764 */	NdrFcShort( 0x10 ),	/* 16 */
/* 766 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 768 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 770 */	
			0x5b,		/* FC_END */

			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 772 */	0x0,		/* 0 */
			NdrFcShort( 0xffb9 ),	/* Offset= -71 (702) */
			0x5b,		/* FC_END */
/* 776 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 778 */	NdrFcShort( 0x8 ),	/* 8 */
/* 780 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 782 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 784 */	NdrFcShort( 0x4 ),	/* 4 */
/* 786 */	NdrFcShort( 0x4 ),	/* 4 */
/* 788 */	0x12, 0x1,	/* FC_UP [all_nodes] */
/* 790 */	NdrFcShort( 0xffca ),	/* Offset= -54 (736) */
/* 792 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 794 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 796 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 798 */	NdrFcShort( 0x8 ),	/* 8 */
/* 800 */	NdrFcShort( 0x0 ),	/* 0 */
/* 802 */	NdrFcShort( 0x0 ),	/* Offset= 0 (802) */
/* 804 */	0x8,		/* FC_LONG */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 806 */	0x0,		/* 0 */
			NdrFcShort( 0xff79 ),	/* Offset= -135 (672) */
			0x5b,		/* FC_END */
/* 810 */	
			0x11, 0x0,	/* FC_RP */
/* 812 */	NdrFcShort( 0x2 ),	/* Offset= 2 (814) */
/* 814 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0x9,		/* FC_ULONG */
/* 816 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 818 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 820 */	NdrFcShort( 0x2 ),	/* Offset= 2 (822) */
/* 822 */	NdrFcShort( 0x4 ),	/* 4 */
/* 824 */	NdrFcShort( 0x3002 ),	/* 12290 */
/* 826 */	NdrFcLong( 0x2 ),	/* 2 */
/* 830 */	NdrFcShort( 0xa ),	/* Offset= 10 (840) */
/* 832 */	NdrFcLong( 0x3 ),	/* 3 */
/* 836 */	NdrFcShort( 0x8 ),	/* Offset= 8 (844) */
/* 838 */	NdrFcShort( 0x0 ),	/* Offset= 0 (838) */
/* 840 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 842 */	NdrFcShort( 0xfe86 ),	/* Offset= -378 (464) */
/* 844 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 846 */	NdrFcShort( 0xff70 ),	/* Offset= -144 (702) */
/* 848 */	
			0x11, 0x0,	/* FC_RP */
/* 850 */	NdrFcShort( 0x1ee ),	/* Offset= 494 (1344) */
/* 852 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0x9,		/* FC_ULONG */
/* 854 */	0x9,		/* Corr desc: FC_ULONG */
			0x0,		/*  */
/* 856 */	NdrFcShort( 0xfffc ),	/* -4 */
/* 858 */	NdrFcShort( 0x2 ),	/* Offset= 2 (860) */
/* 860 */	NdrFcShort( 0x4 ),	/* 4 */
/* 862 */	NdrFcShort( 0x3005 ),	/* 12293 */
/* 864 */	NdrFcLong( 0x0 ),	/* 0 */
/* 868 */	NdrFcShort( 0xfcc0 ),	/* Offset= -832 (36) */
/* 870 */	NdrFcLong( 0x1 ),	/* 1 */
/* 874 */	NdrFcShort( 0x16 ),	/* Offset= 22 (896) */
/* 876 */	NdrFcLong( 0x2 ),	/* 2 */
/* 880 */	NdrFcShort( 0x72 ),	/* Offset= 114 (994) */
/* 882 */	NdrFcLong( 0xa ),	/* 10 */
/* 886 */	NdrFcShort( 0xe2 ),	/* Offset= 226 (1112) */
/* 888 */	NdrFcLong( 0x1f6 ),	/* 502 */
/* 892 */	NdrFcShort( 0x13c ),	/* Offset= 316 (1208) */
/* 894 */	NdrFcShort( 0x0 ),	/* Offset= 0 (894) */
/* 896 */	
			0x12, 0x0,	/* FC_UP */
/* 898 */	NdrFcShort( 0x4c ),	/* Offset= 76 (974) */
/* 900 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 902 */	NdrFcShort( 0x18 ),	/* 24 */
/* 904 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 906 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 908 */	NdrFcShort( 0x0 ),	/* 0 */
/* 910 */	NdrFcShort( 0x0 ),	/* 0 */
/* 912 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 914 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 916 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 918 */	NdrFcShort( 0x4 ),	/* 4 */
/* 920 */	NdrFcShort( 0x4 ),	/* 4 */
/* 922 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 924 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 926 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 928 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 930 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 932 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 934 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 936 */	NdrFcShort( 0x18 ),	/* 24 */
/* 938 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 940 */	NdrFcShort( 0x0 ),	/* 0 */
/* 942 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 944 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 946 */	NdrFcShort( 0x18 ),	/* 24 */
/* 948 */	NdrFcShort( 0x0 ),	/* 0 */
/* 950 */	NdrFcShort( 0x2 ),	/* 2 */
/* 952 */	NdrFcShort( 0x0 ),	/* 0 */
/* 954 */	NdrFcShort( 0x0 ),	/* 0 */
/* 956 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 958 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 960 */	NdrFcShort( 0x4 ),	/* 4 */
/* 962 */	NdrFcShort( 0x4 ),	/* 4 */
/* 964 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 966 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 968 */	
			0x5b,		/* FC_END */

			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 970 */	0x0,		/* 0 */
			NdrFcShort( 0xffb9 ),	/* Offset= -71 (900) */
			0x5b,		/* FC_END */
/* 974 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 976 */	NdrFcShort( 0x8 ),	/* 8 */
/* 978 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 980 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 982 */	NdrFcShort( 0x4 ),	/* 4 */
/* 984 */	NdrFcShort( 0x4 ),	/* 4 */
/* 986 */	0x12, 0x1,	/* FC_UP [all_nodes] */
/* 988 */	NdrFcShort( 0xffca ),	/* Offset= -54 (934) */
/* 990 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 992 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 994 */	
			0x12, 0x0,	/* FC_UP */
/* 996 */	NdrFcShort( 0x60 ),	/* Offset= 96 (1092) */
/* 998 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 1000 */	NdrFcShort( 0x1c ),	/* 28 */
/* 1002 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1004 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1006 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1008 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1010 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1012 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1014 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1016 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1018 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1020 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1022 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1024 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1026 */	NdrFcShort( 0x18 ),	/* 24 */
/* 1028 */	NdrFcShort( 0x18 ),	/* 24 */
/* 1030 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1032 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1034 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 1036 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1038 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1040 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1042 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1044 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 1046 */	NdrFcShort( 0x1c ),	/* 28 */
/* 1048 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1050 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1052 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1054 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 1056 */	NdrFcShort( 0x1c ),	/* 28 */
/* 1058 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1060 */	NdrFcShort( 0x3 ),	/* 3 */
/* 1062 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1064 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1066 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1068 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1070 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1072 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1074 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1076 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1078 */	NdrFcShort( 0x18 ),	/* 24 */
/* 1080 */	NdrFcShort( 0x18 ),	/* 24 */
/* 1082 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1084 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1086 */	
			0x5b,		/* FC_END */

			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 1088 */	0x0,		/* 0 */
			NdrFcShort( 0xffa5 ),	/* Offset= -91 (998) */
			0x5b,		/* FC_END */
/* 1092 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 1094 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1096 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1098 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1100 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1102 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1104 */	0x12, 0x1,	/* FC_UP [all_nodes] */
/* 1106 */	NdrFcShort( 0xffc2 ),	/* Offset= -62 (1044) */
/* 1108 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 1110 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1112 */	
			0x12, 0x0,	/* FC_UP */
/* 1114 */	NdrFcShort( 0x4a ),	/* Offset= 74 (1188) */
/* 1116 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 1118 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1120 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1122 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1124 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1126 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1128 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1130 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1132 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1134 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1136 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1138 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1140 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1142 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 1144 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1146 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1148 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 1150 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1152 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1154 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1156 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1158 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 1160 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1162 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1164 */	NdrFcShort( 0x2 ),	/* 2 */
/* 1166 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1168 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1170 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1172 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1174 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1176 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1178 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1180 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1182 */	
			0x5b,		/* FC_END */

			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 1184 */	0x0,		/* 0 */
			NdrFcShort( 0xffbb ),	/* Offset= -69 (1116) */
			0x5b,		/* FC_END */
/* 1188 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 1190 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1192 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1194 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1196 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1198 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1200 */	0x12, 0x1,	/* FC_UP [all_nodes] */
/* 1202 */	NdrFcShort( 0xffca ),	/* Offset= -54 (1148) */
/* 1204 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 1206 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1208 */	
			0x12, 0x0,	/* FC_UP */
/* 1210 */	NdrFcShort( 0x72 ),	/* Offset= 114 (1324) */
/* 1212 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 1214 */	NdrFcShort( 0x20 ),	/* 32 */
/* 1216 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1218 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1220 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1222 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1224 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1226 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1228 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1230 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1232 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1234 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1236 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1238 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1240 */	NdrFcShort( 0x18 ),	/* 24 */
/* 1242 */	NdrFcShort( 0x18 ),	/* 24 */
/* 1244 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1246 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1248 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1250 */	NdrFcShort( 0x1c ),	/* 28 */
/* 1252 */	NdrFcShort( 0x1c ),	/* 28 */
/* 1254 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1256 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1258 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 1260 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1262 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1264 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1266 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1268 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 1270 */	NdrFcShort( 0x20 ),	/* 32 */
/* 1272 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1274 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1276 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1278 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 1280 */	NdrFcShort( 0x20 ),	/* 32 */
/* 1282 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1284 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1286 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1288 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1290 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1292 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1294 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1296 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1298 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1300 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1302 */	NdrFcShort( 0x18 ),	/* 24 */
/* 1304 */	NdrFcShort( 0x18 ),	/* 24 */
/* 1306 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1308 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1310 */	NdrFcShort( 0x1c ),	/* 28 */
/* 1312 */	NdrFcShort( 0x1c ),	/* 28 */
/* 1314 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1316 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1318 */	
			0x5b,		/* FC_END */

			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 1320 */	0x0,		/* 0 */
			NdrFcShort( 0xff93 ),	/* Offset= -109 (1212) */
			0x5b,		/* FC_END */
/* 1324 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 1326 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1328 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1330 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1332 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1334 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1336 */	0x12, 0x1,	/* FC_UP [all_nodes] */
/* 1338 */	NdrFcShort( 0xffba ),	/* Offset= -70 (1268) */
/* 1340 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 1342 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1344 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1346 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1348 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1350 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1350) */
/* 1352 */	0x8,		/* FC_LONG */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 1354 */	0x0,		/* 0 */
			NdrFcShort( 0xfe09 ),	/* Offset= -503 (852) */
			0x5b,		/* FC_END */
/* 1358 */	
			0x11, 0x0,	/* FC_RP */
/* 1360 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1362) */
/* 1362 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0x9,		/* FC_ULONG */
/* 1364 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 1366 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1368 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1370) */
/* 1370 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1372 */	NdrFcShort( 0x3009 ),	/* 12297 */
/* 1374 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1378 */	NdrFcShort( 0xfba6 ),	/* Offset= -1114 (264) */
/* 1380 */	NdrFcLong( 0x1 ),	/* 1 */
/* 1384 */	NdrFcShort( 0x2e ),	/* Offset= 46 (1430) */
/* 1386 */	NdrFcLong( 0x2 ),	/* 2 */
/* 1390 */	NdrFcShort( 0x4c ),	/* Offset= 76 (1466) */
/* 1392 */	NdrFcLong( 0x1f6 ),	/* 502 */
/* 1396 */	NdrFcShort( 0x82 ),	/* Offset= 130 (1526) */
/* 1398 */	NdrFcLong( 0x3ec ),	/* 1004 */
/* 1402 */	NdrFcShort( 0xfc5c ),	/* Offset= -932 (470) */
/* 1404 */	NdrFcLong( 0x3ee ),	/* 1006 */
/* 1408 */	NdrFcShort( 0xfc4c ),	/* Offset= -948 (460) */
/* 1410 */	NdrFcLong( 0x5dd ),	/* 1501 */
/* 1414 */	NdrFcShort( 0xc2 ),	/* Offset= 194 (1608) */
/* 1416 */	NdrFcLong( 0x3ed ),	/* 1005 */
/* 1420 */	NdrFcShort( 0xfc40 ),	/* Offset= -960 (460) */
/* 1422 */	NdrFcLong( 0x1f5 ),	/* 501 */
/* 1426 */	NdrFcShort( 0xfb7a ),	/* Offset= -1158 (268) */
/* 1428 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1428) */
/* 1430 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 1432 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1434) */
/* 1434 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 1436 */	NdrFcShort( 0xc ),	/* 12 */
/* 1438 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1440 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1442 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1444 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1446 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1448 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1450 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1452 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1454 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1456 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1458 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1460 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 1462 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1464 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1466 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 1468 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1470) */
/* 1470 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 1472 */	NdrFcShort( 0x20 ),	/* 32 */
/* 1474 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1476 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1478 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1480 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1482 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1484 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1486 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1488 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1490 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1492 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1494 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1496 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1498 */	NdrFcShort( 0x18 ),	/* 24 */
/* 1500 */	NdrFcShort( 0x18 ),	/* 24 */
/* 1502 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1504 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1506 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1508 */	NdrFcShort( 0x1c ),	/* 28 */
/* 1510 */	NdrFcShort( 0x1c ),	/* 28 */
/* 1512 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1514 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1516 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 1518 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1520 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1522 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1524 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1526 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 1528 */	NdrFcShort( 0xc ),	/* Offset= 12 (1540) */
/* 1530 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 1532 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1534 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1536 */	NdrFcShort( 0x20 ),	/* 32 */
/* 1538 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 1540 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 1542 */	NdrFcShort( 0x28 ),	/* 40 */
/* 1544 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1546 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1548 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1550 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1552 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1554 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1556 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1558 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1560 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1562 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1564 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1566 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1568 */	NdrFcShort( 0x18 ),	/* 24 */
/* 1570 */	NdrFcShort( 0x18 ),	/* 24 */
/* 1572 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1574 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1576 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1578 */	NdrFcShort( 0x1c ),	/* 28 */
/* 1580 */	NdrFcShort( 0x1c ),	/* 28 */
/* 1582 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1584 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1586 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1588 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1590 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1592 */	0x12, 0x0,	/* FC_UP */
/* 1594 */	NdrFcShort( 0xffc0 ),	/* Offset= -64 (1530) */
/* 1596 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 1598 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1600 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1602 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1604 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1606 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1608 */	
			0x12, 0x0,	/* FC_UP */
/* 1610 */	NdrFcShort( 0xc ),	/* Offset= 12 (1622) */
/* 1612 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 1614 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1616 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1618 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1620 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 1622 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 1624 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1626 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1628 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1630 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1632 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1634 */	0x12, 0x0,	/* FC_UP */
/* 1636 */	NdrFcShort( 0xffe8 ),	/* Offset= -24 (1612) */
/* 1638 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 1640 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1642 */	
			0x11, 0x0,	/* FC_RP */
/* 1644 */	NdrFcShort( 0x116 ),	/* Offset= 278 (1922) */
/* 1646 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0x9,		/* FC_ULONG */
/* 1648 */	0x9,		/* Corr desc: FC_ULONG */
			0x0,		/*  */
/* 1650 */	NdrFcShort( 0xfffc ),	/* -4 */
/* 1652 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1654) */
/* 1654 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1656 */	NdrFcShort( 0x3005 ),	/* 12293 */
/* 1658 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1662 */	NdrFcShort( 0xf9a6 ),	/* Offset= -1626 (36) */
/* 1664 */	NdrFcLong( 0x1 ),	/* 1 */
/* 1668 */	NdrFcShort( 0x16 ),	/* Offset= 22 (1690) */
/* 1670 */	NdrFcLong( 0x2 ),	/* 2 */
/* 1674 */	NdrFcShort( 0x50 ),	/* Offset= 80 (1754) */
/* 1676 */	NdrFcLong( 0x1f6 ),	/* 502 */
/* 1680 */	NdrFcShort( 0x9a ),	/* Offset= 154 (1834) */
/* 1682 */	NdrFcLong( 0x1f5 ),	/* 501 */
/* 1686 */	NdrFcShort( 0xf9da ),	/* Offset= -1574 (112) */
/* 1688 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1688) */
/* 1690 */	
			0x12, 0x0,	/* FC_UP */
/* 1692 */	NdrFcShort( 0x2a ),	/* Offset= 42 (1734) */
/* 1694 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 1696 */	NdrFcShort( 0xc ),	/* 12 */
/* 1698 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1700 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1702 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1704 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 1706 */	NdrFcShort( 0xc ),	/* 12 */
/* 1708 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1710 */	NdrFcShort( 0x2 ),	/* 2 */
/* 1712 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1714 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1716 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1718 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1720 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1722 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1724 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1726 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1728 */	
			0x5b,		/* FC_END */

			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 1730 */	0x0,		/* 0 */
			NdrFcShort( 0xfed7 ),	/* Offset= -297 (1434) */
			0x5b,		/* FC_END */
/* 1734 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 1736 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1738 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1740 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1742 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1744 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1746 */	0x12, 0x1,	/* FC_UP [all_nodes] */
/* 1748 */	NdrFcShort( 0xffca ),	/* Offset= -54 (1694) */
/* 1750 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 1752 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1754 */	
			0x12, 0x0,	/* FC_UP */
/* 1756 */	NdrFcShort( 0x3a ),	/* Offset= 58 (1814) */
/* 1758 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 1760 */	NdrFcShort( 0x20 ),	/* 32 */
/* 1762 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1764 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1766 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1768 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 1770 */	NdrFcShort( 0x20 ),	/* 32 */
/* 1772 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1774 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1776 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1778 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1780 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1782 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1784 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1786 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1788 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1790 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1792 */	NdrFcShort( 0x18 ),	/* 24 */
/* 1794 */	NdrFcShort( 0x18 ),	/* 24 */
/* 1796 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1798 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1800 */	NdrFcShort( 0x1c ),	/* 28 */
/* 1802 */	NdrFcShort( 0x1c ),	/* 28 */
/* 1804 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1806 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1808 */	
			0x5b,		/* FC_END */

			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 1810 */	0x0,		/* 0 */
			NdrFcShort( 0xfeab ),	/* Offset= -341 (1470) */
			0x5b,		/* FC_END */
/* 1814 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 1816 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1818 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1820 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1822 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1824 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1826 */	0x12, 0x1,	/* FC_UP [all_nodes] */
/* 1828 */	NdrFcShort( 0xffba ),	/* Offset= -70 (1758) */
/* 1830 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 1832 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1834 */	
			0x12, 0x0,	/* FC_UP */
/* 1836 */	NdrFcShort( 0x42 ),	/* Offset= 66 (1902) */
/* 1838 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 1840 */	NdrFcShort( 0x28 ),	/* 40 */
/* 1842 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1844 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1846 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1848 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 1850 */	NdrFcShort( 0x28 ),	/* 40 */
/* 1852 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1854 */	NdrFcShort( 0x5 ),	/* 5 */
/* 1856 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1858 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1860 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1862 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1864 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1866 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1868 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1870 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1872 */	NdrFcShort( 0x18 ),	/* 24 */
/* 1874 */	NdrFcShort( 0x18 ),	/* 24 */
/* 1876 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1878 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1880 */	NdrFcShort( 0x1c ),	/* 28 */
/* 1882 */	NdrFcShort( 0x1c ),	/* 28 */
/* 1884 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1886 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1888 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1890 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1892 */	0x12, 0x0,	/* FC_UP */
/* 1894 */	NdrFcShort( 0xfe94 ),	/* Offset= -364 (1530) */
/* 1896 */	
			0x5b,		/* FC_END */

			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 1898 */	0x0,		/* 0 */
			NdrFcShort( 0xfe99 ),	/* Offset= -359 (1540) */
			0x5b,		/* FC_END */
/* 1902 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 1904 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1906 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1908 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1910 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1912 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1914 */	0x12, 0x1,	/* FC_UP [all_nodes] */
/* 1916 */	NdrFcShort( 0xffb2 ),	/* Offset= -78 (1838) */
/* 1918 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 1920 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1922 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1924 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1926 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1928 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1928) */
/* 1930 */	0x8,		/* FC_LONG */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 1932 */	0x0,		/* 0 */
			NdrFcShort( 0xfee1 ),	/* Offset= -287 (1646) */
			0x5b,		/* FC_END */
/* 1936 */	
			0x11, 0x0,	/* FC_RP */
/* 1938 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1940) */
/* 1940 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0x9,		/* FC_ULONG */
/* 1942 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 1944 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1946 */	NdrFcShort( 0xfdc0 ),	/* Offset= -576 (1370) */
/* 1948 */	
			0x11, 0x0,	/* FC_RP */
/* 1950 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1952) */
/* 1952 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0x9,		/* FC_ULONG */
/* 1954 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 1956 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1958 */	NdrFcShort( 0xfdb4 ),	/* Offset= -588 (1370) */
/* 1960 */	
			0x11, 0x0,	/* FC_RP */
/* 1962 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1964) */
/* 1964 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0x9,		/* FC_ULONG */
/* 1966 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 1968 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1970 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1972) */
/* 1972 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1974 */	NdrFcShort( 0x303b ),	/* 12347 */
/* 1976 */	NdrFcLong( 0x64 ),	/* 100 */
/* 1980 */	NdrFcShort( 0x160 ),	/* Offset= 352 (2332) */
/* 1982 */	NdrFcLong( 0x65 ),	/* 101 */
/* 1986 */	NdrFcShort( 0x172 ),	/* Offset= 370 (2356) */
/* 1988 */	NdrFcLong( 0x66 ),	/* 102 */
/* 1992 */	NdrFcShort( 0x192 ),	/* Offset= 402 (2394) */
/* 1994 */	NdrFcLong( 0x192 ),	/* 402 */
/* 1998 */	NdrFcShort( 0x1c4 ),	/* Offset= 452 (2450) */
/* 2000 */	NdrFcLong( 0x193 ),	/* 403 */
/* 2004 */	NdrFcShort( 0x208 ),	/* Offset= 520 (2524) */
/* 2006 */	NdrFcLong( 0x1f6 ),	/* 502 */
/* 2010 */	NdrFcShort( 0x258 ),	/* Offset= 600 (2610) */
/* 2012 */	NdrFcLong( 0x1f7 ),	/* 503 */
/* 2016 */	NdrFcShort( 0x26e ),	/* Offset= 622 (2638) */
/* 2018 */	NdrFcLong( 0x257 ),	/* 599 */
/* 2022 */	NdrFcShort( 0x2a8 ),	/* Offset= 680 (2702) */
/* 2024 */	NdrFcLong( 0x3ed ),	/* 1005 */
/* 2028 */	NdrFcShort( 0xf9ea ),	/* Offset= -1558 (470) */
/* 2030 */	NdrFcLong( 0x453 ),	/* 1107 */
/* 2034 */	NdrFcShort( 0xf9da ),	/* Offset= -1574 (460) */
/* 2036 */	NdrFcLong( 0x3f2 ),	/* 1010 */
/* 2040 */	NdrFcShort( 0xf9d4 ),	/* Offset= -1580 (460) */
/* 2042 */	NdrFcLong( 0x3f8 ),	/* 1016 */
/* 2046 */	NdrFcShort( 0xf9ce ),	/* Offset= -1586 (460) */
/* 2048 */	NdrFcLong( 0x3f9 ),	/* 1017 */
/* 2052 */	NdrFcShort( 0xf9c8 ),	/* Offset= -1592 (460) */
/* 2054 */	NdrFcLong( 0x3fa ),	/* 1018 */
/* 2058 */	NdrFcShort( 0xf9c2 ),	/* Offset= -1598 (460) */
/* 2060 */	NdrFcLong( 0x5dd ),	/* 1501 */
/* 2064 */	NdrFcShort( 0xf9bc ),	/* Offset= -1604 (460) */
/* 2066 */	NdrFcLong( 0x5de ),	/* 1502 */
/* 2070 */	NdrFcShort( 0xf9b6 ),	/* Offset= -1610 (460) */
/* 2072 */	NdrFcLong( 0x5df ),	/* 1503 */
/* 2076 */	NdrFcShort( 0xf9b0 ),	/* Offset= -1616 (460) */
/* 2078 */	NdrFcLong( 0x5e2 ),	/* 1506 */
/* 2082 */	NdrFcShort( 0xf9aa ),	/* Offset= -1622 (460) */
/* 2084 */	NdrFcLong( 0x5e5 ),	/* 1509 */
/* 2088 */	NdrFcShort( 0xf9a4 ),	/* Offset= -1628 (460) */
/* 2090 */	NdrFcLong( 0x5e6 ),	/* 1510 */
/* 2094 */	NdrFcShort( 0xf99e ),	/* Offset= -1634 (460) */
/* 2096 */	NdrFcLong( 0x5e7 ),	/* 1511 */
/* 2100 */	NdrFcShort( 0xf998 ),	/* Offset= -1640 (460) */
/* 2102 */	NdrFcLong( 0x5e8 ),	/* 1512 */
/* 2106 */	NdrFcShort( 0xf992 ),	/* Offset= -1646 (460) */
/* 2108 */	NdrFcLong( 0x5e9 ),	/* 1513 */
/* 2112 */	NdrFcShort( 0xf98c ),	/* Offset= -1652 (460) */
/* 2114 */	NdrFcLong( 0x5ea ),	/* 1514 */
/* 2118 */	NdrFcShort( 0xf986 ),	/* Offset= -1658 (460) */
/* 2120 */	NdrFcLong( 0x5eb ),	/* 1515 */
/* 2124 */	NdrFcShort( 0xf980 ),	/* Offset= -1664 (460) */
/* 2126 */	NdrFcLong( 0x5ec ),	/* 1516 */
/* 2130 */	NdrFcShort( 0xf97a ),	/* Offset= -1670 (460) */
/* 2132 */	NdrFcLong( 0x5ee ),	/* 1518 */
/* 2136 */	NdrFcShort( 0xf974 ),	/* Offset= -1676 (460) */
/* 2138 */	NdrFcLong( 0x5f0 ),	/* 1520 */
/* 2142 */	NdrFcShort( 0xf96e ),	/* Offset= -1682 (460) */
/* 2144 */	NdrFcLong( 0x5f1 ),	/* 1521 */
/* 2148 */	NdrFcShort( 0xf968 ),	/* Offset= -1688 (460) */
/* 2150 */	NdrFcLong( 0x5f2 ),	/* 1522 */
/* 2154 */	NdrFcShort( 0xf962 ),	/* Offset= -1694 (460) */
/* 2156 */	NdrFcLong( 0x5f3 ),	/* 1523 */
/* 2160 */	NdrFcShort( 0xf95c ),	/* Offset= -1700 (460) */
/* 2162 */	NdrFcLong( 0x5f4 ),	/* 1524 */
/* 2166 */	NdrFcShort( 0xf956 ),	/* Offset= -1706 (460) */
/* 2168 */	NdrFcLong( 0x5f5 ),	/* 1525 */
/* 2172 */	NdrFcShort( 0xf950 ),	/* Offset= -1712 (460) */
/* 2174 */	NdrFcLong( 0x5f8 ),	/* 1528 */
/* 2178 */	NdrFcShort( 0xf94a ),	/* Offset= -1718 (460) */
/* 2180 */	NdrFcLong( 0x5f9 ),	/* 1529 */
/* 2184 */	NdrFcShort( 0xf944 ),	/* Offset= -1724 (460) */
/* 2186 */	NdrFcLong( 0x5fa ),	/* 1530 */
/* 2190 */	NdrFcShort( 0xf93e ),	/* Offset= -1730 (460) */
/* 2192 */	NdrFcLong( 0x5fd ),	/* 1533 */
/* 2196 */	NdrFcShort( 0xf938 ),	/* Offset= -1736 (460) */
/* 2198 */	NdrFcLong( 0x5fe ),	/* 1534 */
/* 2202 */	NdrFcShort( 0xf932 ),	/* Offset= -1742 (460) */
/* 2204 */	NdrFcLong( 0x5ff ),	/* 1535 */
/* 2208 */	NdrFcShort( 0xf92c ),	/* Offset= -1748 (460) */
/* 2210 */	NdrFcLong( 0x600 ),	/* 1536 */
/* 2214 */	NdrFcShort( 0xf926 ),	/* Offset= -1754 (460) */
/* 2216 */	NdrFcLong( 0x601 ),	/* 1537 */
/* 2220 */	NdrFcShort( 0xf920 ),	/* Offset= -1760 (460) */
/* 2222 */	NdrFcLong( 0x602 ),	/* 1538 */
/* 2226 */	NdrFcShort( 0xf91a ),	/* Offset= -1766 (460) */
/* 2228 */	NdrFcLong( 0x603 ),	/* 1539 */
/* 2232 */	NdrFcShort( 0xf914 ),	/* Offset= -1772 (460) */
/* 2234 */	NdrFcLong( 0x604 ),	/* 1540 */
/* 2238 */	NdrFcShort( 0xf90e ),	/* Offset= -1778 (460) */
/* 2240 */	NdrFcLong( 0x605 ),	/* 1541 */
/* 2244 */	NdrFcShort( 0xf908 ),	/* Offset= -1784 (460) */
/* 2246 */	NdrFcLong( 0x606 ),	/* 1542 */
/* 2250 */	NdrFcShort( 0xf902 ),	/* Offset= -1790 (460) */
/* 2252 */	NdrFcLong( 0x607 ),	/* 1543 */
/* 2256 */	NdrFcShort( 0xf8fc ),	/* Offset= -1796 (460) */
/* 2258 */	NdrFcLong( 0x608 ),	/* 1544 */
/* 2262 */	NdrFcShort( 0xf8f6 ),	/* Offset= -1802 (460) */
/* 2264 */	NdrFcLong( 0x609 ),	/* 1545 */
/* 2268 */	NdrFcShort( 0xf8f0 ),	/* Offset= -1808 (460) */
/* 2270 */	NdrFcLong( 0x60a ),	/* 1546 */
/* 2274 */	NdrFcShort( 0xf8ea ),	/* Offset= -1814 (460) */
/* 2276 */	NdrFcLong( 0x60b ),	/* 1547 */
/* 2280 */	NdrFcShort( 0xf8e4 ),	/* Offset= -1820 (460) */
/* 2282 */	NdrFcLong( 0x60c ),	/* 1548 */
/* 2286 */	NdrFcShort( 0xf8de ),	/* Offset= -1826 (460) */
/* 2288 */	NdrFcLong( 0x60d ),	/* 1549 */
/* 2292 */	NdrFcShort( 0xf8d8 ),	/* Offset= -1832 (460) */
/* 2294 */	NdrFcLong( 0x60e ),	/* 1550 */
/* 2298 */	NdrFcShort( 0xf8d2 ),	/* Offset= -1838 (460) */
/* 2300 */	NdrFcLong( 0x610 ),	/* 1552 */
/* 2304 */	NdrFcShort( 0xf8cc ),	/* Offset= -1844 (460) */
/* 2306 */	NdrFcLong( 0x611 ),	/* 1553 */
/* 2310 */	NdrFcShort( 0xf8c6 ),	/* Offset= -1850 (460) */
/* 2312 */	NdrFcLong( 0x612 ),	/* 1554 */
/* 2316 */	NdrFcShort( 0xf8c0 ),	/* Offset= -1856 (460) */
/* 2318 */	NdrFcLong( 0x613 ),	/* 1555 */
/* 2322 */	NdrFcShort( 0xf8ba ),	/* Offset= -1862 (460) */
/* 2324 */	NdrFcLong( 0x614 ),	/* 1556 */
/* 2328 */	NdrFcShort( 0xf8b4 ),	/* Offset= -1868 (460) */
/* 2330 */	NdrFcShort( 0x0 ),	/* Offset= 0 (2330) */
/* 2332 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 2334 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2336) */
/* 2336 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 2338 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2340 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 2342 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 2344 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2346 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2348 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 2350 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 2352 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 2354 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 2356 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 2358 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2360) */
/* 2360 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 2362 */	NdrFcShort( 0x18 ),	/* 24 */
/* 2364 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 2366 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 2368 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2370 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2372 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 2374 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 2376 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 2378 */	NdrFcShort( 0x14 ),	/* 20 */
/* 2380 */	NdrFcShort( 0x14 ),	/* 20 */
/* 2382 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 2384 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 2386 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 2388 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2390 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2392 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 2394 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 2396 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2398) */
/* 2398 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 2400 */	NdrFcShort( 0x34 ),	/* 52 */
/* 2402 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 2404 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 2406 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2408 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2410 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 2412 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 2414 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 2416 */	NdrFcShort( 0x14 ),	/* 20 */
/* 2418 */	NdrFcShort( 0x14 ),	/* 20 */
/* 2420 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 2422 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 2424 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 2426 */	NdrFcShort( 0x30 ),	/* 48 */
/* 2428 */	NdrFcShort( 0x30 ),	/* 48 */
/* 2430 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 2432 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 2434 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 2436 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2438 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2440 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2442 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2444 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2446 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2448 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 2450 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 2452 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2454) */
/* 2454 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 2456 */	NdrFcShort( 0x7c ),	/* 124 */
/* 2458 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 2460 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 2462 */	NdrFcShort( 0xc ),	/* 12 */
/* 2464 */	NdrFcShort( 0xc ),	/* 12 */
/* 2466 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 2468 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 2470 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 2472 */	NdrFcShort( 0x1c ),	/* 28 */
/* 2474 */	NdrFcShort( 0x1c ),	/* 28 */
/* 2476 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 2478 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 2480 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 2482 */	NdrFcShort( 0x78 ),	/* 120 */
/* 2484 */	NdrFcShort( 0x78 ),	/* 120 */
/* 2486 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 2488 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 2490 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 2492 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2494 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2496 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2498 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2500 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2502 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2504 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2506 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2508 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2510 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2512 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2514 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2516 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2518 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2520 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2522 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 2524 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 2526 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2528) */
/* 2528 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 2530 */	NdrFcShort( 0x88 ),	/* 136 */
/* 2532 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 2534 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 2536 */	NdrFcShort( 0xc ),	/* 12 */
/* 2538 */	NdrFcShort( 0xc ),	/* 12 */
/* 2540 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 2542 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 2544 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 2546 */	NdrFcShort( 0x1c ),	/* 28 */
/* 2548 */	NdrFcShort( 0x1c ),	/* 28 */
/* 2550 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 2552 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 2554 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 2556 */	NdrFcShort( 0x78 ),	/* 120 */
/* 2558 */	NdrFcShort( 0x78 ),	/* 120 */
/* 2560 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 2562 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 2564 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 2566 */	NdrFcShort( 0x84 ),	/* 132 */
/* 2568 */	NdrFcShort( 0x84 ),	/* 132 */
/* 2570 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 2572 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 2574 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 2576 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2578 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2580 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2582 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2584 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2586 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2588 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2590 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2592 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2594 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2596 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2598 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2600 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2602 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2604 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2606 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2608 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 2610 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 2612 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2614) */
/* 2614 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 2616 */	NdrFcShort( 0x48 ),	/* 72 */
/* 2618 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2620 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2622 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2624 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2626 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2628 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2630 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2632 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2634 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2636 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 2638 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 2640 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2642) */
/* 2642 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 2644 */	NdrFcShort( 0xa8 ),	/* 168 */
/* 2646 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 2648 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 2650 */	NdrFcShort( 0x48 ),	/* 72 */
/* 2652 */	NdrFcShort( 0x48 ),	/* 72 */
/* 2654 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 2656 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 2658 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 2660 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2662 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2664 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2666 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2668 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2670 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2672 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2674 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2676 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2678 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2680 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2682 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2684 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2686 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2688 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2690 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2692 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2694 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2696 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2698 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2700 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 2702 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 2704 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2706) */
/* 2706 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 2708 */	NdrFcShort( 0xe0 ),	/* 224 */
/* 2710 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 2712 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 2714 */	NdrFcShort( 0x48 ),	/* 72 */
/* 2716 */	NdrFcShort( 0x48 ),	/* 72 */
/* 2718 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 2720 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 2722 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 2724 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2726 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2728 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2730 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2732 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2734 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2736 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2738 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2740 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2742 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2744 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2746 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2748 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2750 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2752 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2754 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2756 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2758 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2760 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2762 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2764 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2766 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2768 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2770 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2772 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2774 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2776 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2778 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 2780 */	
			0x11, 0x0,	/* FC_RP */
/* 2782 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2784) */
/* 2784 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0x9,		/* FC_ULONG */
/* 2786 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 2788 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2790 */	NdrFcShort( 0xfcce ),	/* Offset= -818 (1972) */
/* 2792 */	
			0x11, 0x0,	/* FC_RP */
/* 2794 */	NdrFcShort( 0x26 ),	/* Offset= 38 (2832) */
/* 2796 */	
			0x29,		/* FC_WSTRING */
			0x5c,		/* FC_PAD */
/* 2798 */	NdrFcShort( 0x3 ),	/* 3 */
/* 2800 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x1,		/* 1 */
/* 2802 */	NdrFcShort( 0x6 ),	/* 6 */
/* 2804 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2806 */	NdrFcShort( 0x0 ),	/* Offset= 0 (2806) */
/* 2808 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 2810 */	NdrFcShort( 0xfff2 ),	/* Offset= -14 (2796) */
/* 2812 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 2814 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x1,		/* 1 */
/* 2816 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2818 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 2820 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2822 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 2824 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2826 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 2828 */	NdrFcShort( 0xffe4 ),	/* Offset= -28 (2800) */
/* 2830 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 2832 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 2834 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2836 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 2838 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 2840 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2842 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2844 */	0x12, 0x1,	/* FC_UP [all_nodes] */
/* 2846 */	NdrFcShort( 0xffe0 ),	/* Offset= -32 (2814) */
/* 2848 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 2850 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 2852 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 2854 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2856) */
/* 2856 */	
			0x12, 0x0,	/* FC_UP */
/* 2858 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2860) */
/* 2860 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 2862 */	NdrFcShort( 0x44 ),	/* 68 */
/* 2864 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2866 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2868 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2870 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2872 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2874 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2876 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2878 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2880 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 2882 */	
			0x11, 0x1,	/* FC_RP [all_nodes] */
/* 2884 */	NdrFcShort( 0xc ),	/* Offset= 12 (2896) */
/* 2886 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 2888 */	NdrFcShort( 0x1 ),	/* 1 */
/* 2890 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 2892 */	NdrFcShort( 0xc ),	/* 12 */
/* 2894 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 2896 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 2898 */	NdrFcShort( 0x14 ),	/* 20 */
/* 2900 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 2902 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 2904 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2906 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2908 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 2910 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 2912 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 2914 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2916 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2918 */	0x12, 0x0,	/* FC_UP */
/* 2920 */	NdrFcShort( 0xffde ),	/* Offset= -34 (2886) */
/* 2922 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 2924 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2926 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2928 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 2930 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 2932 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 2934 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2936 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2938 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 2940 */	
			0x11, 0x0,	/* FC_RP */
/* 2942 */	NdrFcShort( 0x210 ),	/* Offset= 528 (3470) */
/* 2944 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0x9,		/* FC_ULONG */
/* 2946 */	0x9,		/* Corr desc: FC_ULONG */
			0x0,		/*  */
/* 2948 */	NdrFcShort( 0xfffc ),	/* -4 */
/* 2950 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2952) */
/* 2952 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2954 */	NdrFcShort( 0x3004 ),	/* 12292 */
/* 2956 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2960 */	NdrFcShort( 0x16 ),	/* Offset= 22 (2982) */
/* 2962 */	NdrFcLong( 0x1 ),	/* 1 */
/* 2966 */	NdrFcShort( 0x58 ),	/* Offset= 88 (3054) */
/* 2968 */	NdrFcLong( 0x2 ),	/* 2 */
/* 2972 */	NdrFcShort( 0xd8 ),	/* Offset= 216 (3188) */
/* 2974 */	NdrFcLong( 0x3 ),	/* 3 */
/* 2978 */	NdrFcShort( 0x15a ),	/* Offset= 346 (3324) */
/* 2980 */	NdrFcShort( 0x0 ),	/* Offset= 0 (2980) */
/* 2982 */	
			0x12, 0x0,	/* FC_UP */
/* 2984 */	NdrFcShort( 0x32 ),	/* Offset= 50 (3034) */
/* 2986 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 2988 */	NdrFcShort( 0x14 ),	/* 20 */
/* 2990 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 2992 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2994 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 2996 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 2998 */	NdrFcShort( 0x14 ),	/* 20 */
/* 3000 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3002 */	NdrFcShort( 0x3 ),	/* 3 */
/* 3004 */	NdrFcShort( 0x4 ),	/* 4 */
/* 3006 */	NdrFcShort( 0x4 ),	/* 4 */
/* 3008 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 3010 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 3012 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3014 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3016 */	0x12, 0x0,	/* FC_UP */
/* 3018 */	NdrFcShort( 0xff7c ),	/* Offset= -132 (2886) */
/* 3020 */	NdrFcShort( 0x10 ),	/* 16 */
/* 3022 */	NdrFcShort( 0x10 ),	/* 16 */
/* 3024 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 3026 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 3028 */	
			0x5b,		/* FC_END */

			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 3030 */	0x0,		/* 0 */
			NdrFcShort( 0xff79 ),	/* Offset= -135 (2896) */
			0x5b,		/* FC_END */
/* 3034 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 3036 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3038 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 3040 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 3042 */	NdrFcShort( 0x4 ),	/* 4 */
/* 3044 */	NdrFcShort( 0x4 ),	/* 4 */
/* 3046 */	0x12, 0x1,	/* FC_UP [all_nodes] */
/* 3048 */	NdrFcShort( 0xffc2 ),	/* Offset= -62 (2986) */
/* 3050 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 3052 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 3054 */	
			0x12, 0x0,	/* FC_UP */
/* 3056 */	NdrFcShort( 0x70 ),	/* Offset= 112 (3168) */
/* 3058 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 3060 */	NdrFcShort( 0x18 ),	/* 24 */
/* 3062 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 3064 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 3066 */	NdrFcShort( 0x4 ),	/* 4 */
/* 3068 */	NdrFcShort( 0x4 ),	/* 4 */
/* 3070 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 3072 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 3074 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 3076 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3078 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3080 */	0x12, 0x0,	/* FC_UP */
/* 3082 */	NdrFcShort( 0xff3c ),	/* Offset= -196 (2886) */
/* 3084 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 3086 */	NdrFcShort( 0x10 ),	/* 16 */
/* 3088 */	NdrFcShort( 0x10 ),	/* 16 */
/* 3090 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 3092 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 3094 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 3096 */	NdrFcShort( 0x14 ),	/* 20 */
/* 3098 */	NdrFcShort( 0x14 ),	/* 20 */
/* 3100 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 3102 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 3104 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 3106 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 3108 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 3110 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 3112 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 3114 */	NdrFcShort( 0x18 ),	/* 24 */
/* 3116 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 3118 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3120 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 3122 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 3124 */	NdrFcShort( 0x18 ),	/* 24 */
/* 3126 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3128 */	NdrFcShort( 0x4 ),	/* 4 */
/* 3130 */	NdrFcShort( 0x4 ),	/* 4 */
/* 3132 */	NdrFcShort( 0x4 ),	/* 4 */
/* 3134 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 3136 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 3138 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3140 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3142 */	0x12, 0x0,	/* FC_UP */
/* 3144 */	NdrFcShort( 0xfefe ),	/* Offset= -258 (2886) */
/* 3146 */	NdrFcShort( 0x10 ),	/* 16 */
/* 3148 */	NdrFcShort( 0x10 ),	/* 16 */
/* 3150 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 3152 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 3154 */	NdrFcShort( 0x14 ),	/* 20 */
/* 3156 */	NdrFcShort( 0x14 ),	/* 20 */
/* 3158 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 3160 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 3162 */	
			0x5b,		/* FC_END */

			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 3164 */	0x0,		/* 0 */
			NdrFcShort( 0xff95 ),	/* Offset= -107 (3058) */
			0x5b,		/* FC_END */
/* 3168 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 3170 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3172 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 3174 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 3176 */	NdrFcShort( 0x4 ),	/* 4 */
/* 3178 */	NdrFcShort( 0x4 ),	/* 4 */
/* 3180 */	0x12, 0x1,	/* FC_UP [all_nodes] */
/* 3182 */	NdrFcShort( 0xffba ),	/* Offset= -70 (3112) */
/* 3184 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 3186 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 3188 */	
			0x12, 0x0,	/* FC_UP */
/* 3190 */	NdrFcShort( 0x72 ),	/* Offset= 114 (3304) */
/* 3192 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 3194 */	NdrFcShort( 0x1c ),	/* 28 */
/* 3196 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 3198 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 3200 */	NdrFcShort( 0x4 ),	/* 4 */
/* 3202 */	NdrFcShort( 0x4 ),	/* 4 */
/* 3204 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 3206 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 3208 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 3210 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3212 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3214 */	0x12, 0x0,	/* FC_UP */
/* 3216 */	NdrFcShort( 0xfeb6 ),	/* Offset= -330 (2886) */
/* 3218 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 3220 */	NdrFcShort( 0x10 ),	/* 16 */
/* 3222 */	NdrFcShort( 0x10 ),	/* 16 */
/* 3224 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 3226 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 3228 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 3230 */	NdrFcShort( 0x14 ),	/* 20 */
/* 3232 */	NdrFcShort( 0x14 ),	/* 20 */
/* 3234 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 3236 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 3238 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 3240 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 3242 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 3244 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 3246 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 3248 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 3250 */	NdrFcShort( 0x1c ),	/* 28 */
/* 3252 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 3254 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3256 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 3258 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 3260 */	NdrFcShort( 0x1c ),	/* 28 */
/* 3262 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3264 */	NdrFcShort( 0x4 ),	/* 4 */
/* 3266 */	NdrFcShort( 0x4 ),	/* 4 */
/* 3268 */	NdrFcShort( 0x4 ),	/* 4 */
/* 3270 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 3272 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 3274 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3276 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3278 */	0x12, 0x0,	/* FC_UP */
/* 3280 */	NdrFcShort( 0xfe76 ),	/* Offset= -394 (2886) */
/* 3282 */	NdrFcShort( 0x10 ),	/* 16 */
/* 3284 */	NdrFcShort( 0x10 ),	/* 16 */
/* 3286 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 3288 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 3290 */	NdrFcShort( 0x14 ),	/* 20 */
/* 3292 */	NdrFcShort( 0x14 ),	/* 20 */
/* 3294 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 3296 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 3298 */	
			0x5b,		/* FC_END */

			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 3300 */	0x0,		/* 0 */
			NdrFcShort( 0xff93 ),	/* Offset= -109 (3192) */
			0x5b,		/* FC_END */
/* 3304 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 3306 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3308 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 3310 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 3312 */	NdrFcShort( 0x4 ),	/* 4 */
/* 3314 */	NdrFcShort( 0x4 ),	/* 4 */
/* 3316 */	0x12, 0x1,	/* FC_UP [all_nodes] */
/* 3318 */	NdrFcShort( 0xffba ),	/* Offset= -70 (3248) */
/* 3320 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 3322 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 3324 */	
			0x12, 0x0,	/* FC_UP */
/* 3326 */	NdrFcShort( 0x7c ),	/* Offset= 124 (3450) */
/* 3328 */	
			0x1d,		/* FC_SMFARRAY */
			0x0,		/* 0 */
/* 3330 */	NdrFcShort( 0x100 ),	/* 256 */
/* 3332 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 3334 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 3336 */	NdrFcShort( 0x120 ),	/* 288 */
/* 3338 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 3340 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 3342 */	NdrFcShort( 0x4 ),	/* 4 */
/* 3344 */	NdrFcShort( 0x4 ),	/* 4 */
/* 3346 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 3348 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 3350 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 3352 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3354 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3356 */	0x12, 0x0,	/* FC_UP */
/* 3358 */	NdrFcShort( 0xfe28 ),	/* Offset= -472 (2886) */
/* 3360 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 3362 */	NdrFcShort( 0x10 ),	/* 16 */
/* 3364 */	NdrFcShort( 0x10 ),	/* 16 */
/* 3366 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 3368 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 3370 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 3372 */	NdrFcShort( 0x14 ),	/* 20 */
/* 3374 */	NdrFcShort( 0x14 ),	/* 20 */
/* 3376 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 3378 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 3380 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 3382 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 3384 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 3386 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 3388 */	0x8,		/* FC_LONG */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 3390 */	0x0,		/* 0 */
			NdrFcShort( 0xffc1 ),	/* Offset= -63 (3328) */
			0x5b,		/* FC_END */
/* 3394 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 3396 */	NdrFcShort( 0x120 ),	/* 288 */
/* 3398 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 3400 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3402 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 3404 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 3406 */	NdrFcShort( 0x120 ),	/* 288 */
/* 3408 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3410 */	NdrFcShort( 0x4 ),	/* 4 */
/* 3412 */	NdrFcShort( 0x4 ),	/* 4 */
/* 3414 */	NdrFcShort( 0x4 ),	/* 4 */
/* 3416 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 3418 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 3420 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3422 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3424 */	0x12, 0x0,	/* FC_UP */
/* 3426 */	NdrFcShort( 0xfde4 ),	/* Offset= -540 (2886) */
/* 3428 */	NdrFcShort( 0x10 ),	/* 16 */
/* 3430 */	NdrFcShort( 0x10 ),	/* 16 */
/* 3432 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 3434 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 3436 */	NdrFcShort( 0x14 ),	/* 20 */
/* 3438 */	NdrFcShort( 0x14 ),	/* 20 */
/* 3440 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 3442 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 3444 */	
			0x5b,		/* FC_END */

			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 3446 */	0x0,		/* 0 */
			NdrFcShort( 0xff8f ),	/* Offset= -113 (3334) */
			0x5b,		/* FC_END */
/* 3450 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 3452 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3454 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 3456 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 3458 */	NdrFcShort( 0x4 ),	/* 4 */
/* 3460 */	NdrFcShort( 0x4 ),	/* 4 */
/* 3462 */	0x12, 0x0,	/* FC_UP */
/* 3464 */	NdrFcShort( 0xffba ),	/* Offset= -70 (3394) */
/* 3466 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 3468 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 3470 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 3472 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3474 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3476 */	NdrFcShort( 0x0 ),	/* Offset= 0 (3476) */
/* 3478 */	0x8,		/* FC_LONG */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 3480 */	0x0,		/* 0 */
			NdrFcShort( 0xfde7 ),	/* Offset= -537 (2944) */
			0x5b,		/* FC_END */
/* 3484 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 3486 */	NdrFcShort( 0x2 ),	/* Offset= 2 (3488) */
/* 3488 */	
			0x12, 0x0,	/* FC_UP */
/* 3490 */	NdrFcShort( 0x2 ),	/* Offset= 2 (3492) */
/* 3492 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 3494 */	NdrFcShort( 0x30 ),	/* 48 */
/* 3496 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 3498 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 3500 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 3502 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 3504 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 3506 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 3508 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 3510 */	
			0x11, 0x0,	/* FC_RP */
/* 3512 */	NdrFcShort( 0x2 ),	/* Offset= 2 (3514) */
/* 3514 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 3516 */	NdrFcShort( 0x1 ),	/* 1 */
/* 3518 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 3520 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3522 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 3524 */	
			0x11, 0x0,	/* FC_RP */
/* 3526 */	NdrFcShort( 0x2 ),	/* Offset= 2 (3528) */
/* 3528 */	
			0x1b,		/* FC_CARRAY */
			0x1,		/* 1 */
/* 3530 */	NdrFcShort( 0x2 ),	/* 2 */
/* 3532 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 3534 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3536 */	0x5,		/* FC_WCHAR */
			0x5b,		/* FC_END */
/* 3538 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 3540 */	NdrFcShort( 0x2 ),	/* Offset= 2 (3542) */
/* 3542 */	0x30,		/* FC_BIND_CONTEXT */
			0xa8,		/* Ctxt flags:  via ptr, out, strict, */
/* 3544 */	0x0,		/* 0 */
			0x3,		/* 3 */
/* 3546 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 3548 */	NdrFcShort( 0x2 ),	/* Offset= 2 (3550) */
/* 3550 */	0x30,		/* FC_BIND_CONTEXT */
			0xe9,		/* Ctxt flags:  via ptr, in, out, strict, can't be null */
/* 3552 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 3554 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 3556 */	NdrFcShort( 0xf864 ),	/* Offset= -1948 (1608) */
/* 3558 */	
			0x11, 0x0,	/* FC_RP */
/* 3560 */	NdrFcShort( 0xf86e ),	/* Offset= -1938 (1622) */
/* 3562 */	
			0x11, 0x0,	/* FC_RP */
/* 3564 */	NdrFcShort( 0x2 ),	/* Offset= 2 (3566) */
/* 3566 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0x9,		/* FC_ULONG */
/* 3568 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 3570 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3572 */	NdrFcShort( 0x2 ),	/* Offset= 2 (3574) */
/* 3574 */	NdrFcShort( 0x120 ),	/* 288 */
/* 3576 */	NdrFcShort( 0x3004 ),	/* 12292 */
/* 3578 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3582 */	NdrFcShort( 0xfd52 ),	/* Offset= -686 (2896) */
/* 3584 */	NdrFcLong( 0x1 ),	/* 1 */
/* 3588 */	NdrFcShort( 0xfdee ),	/* Offset= -530 (3058) */
/* 3590 */	NdrFcLong( 0x2 ),	/* 2 */
/* 3594 */	NdrFcShort( 0xfe6e ),	/* Offset= -402 (3192) */
/* 3596 */	NdrFcLong( 0x3 ),	/* 3 */
/* 3600 */	NdrFcShort( 0xfef6 ),	/* Offset= -266 (3334) */
/* 3602 */	NdrFcShort( 0x0 ),	/* Offset= 0 (3602) */
/* 3604 */	
			0x11, 0x0,	/* FC_RP */
/* 3606 */	NdrFcShort( 0x8 ),	/* Offset= 8 (3614) */
/* 3608 */	
			0x1d,		/* FC_SMFARRAY */
			0x0,		/* 0 */
/* 3610 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3612 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 3614 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 3616 */	NdrFcShort( 0x10 ),	/* 16 */
/* 3618 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 3620 */	0x6,		/* FC_SHORT */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 3622 */	0x0,		/* 0 */
			NdrFcShort( 0xfff1 ),	/* Offset= -15 (3608) */
			0x5b,		/* FC_END */
/* 3626 */	
			0x11, 0x0,	/* FC_RP */
/* 3628 */	NdrFcShort( 0x3a ),	/* Offset= 58 (3686) */
/* 3630 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 3632 */	NdrFcShort( 0x14 ),	/* 20 */
/* 3634 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 3636 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 3638 */	NdrFcShort( 0x10 ),	/* 16 */
/* 3640 */	NdrFcShort( 0x10 ),	/* 16 */
/* 3642 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 3644 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 3646 */	
			0x5b,		/* FC_END */

			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 3648 */	0x0,		/* 0 */
			NdrFcShort( 0xffdd ),	/* Offset= -35 (3614) */
			0x8,		/* FC_LONG */
/* 3652 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 3654 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 3656 */	NdrFcShort( 0x14 ),	/* 20 */
/* 3658 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 3660 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3662 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 3664 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 3666 */	NdrFcShort( 0x14 ),	/* 20 */
/* 3668 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3670 */	NdrFcShort( 0x1 ),	/* 1 */
/* 3672 */	NdrFcShort( 0x10 ),	/* 16 */
/* 3674 */	NdrFcShort( 0x10 ),	/* 16 */
/* 3676 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 3678 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 3680 */	
			0x5b,		/* FC_END */

			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 3682 */	0x0,		/* 0 */
			NdrFcShort( 0xffcb ),	/* Offset= -53 (3630) */
			0x5b,		/* FC_END */
/* 3686 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 3688 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3690 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 3692 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 3694 */	NdrFcShort( 0x4 ),	/* 4 */
/* 3696 */	NdrFcShort( 0x4 ),	/* 4 */
/* 3698 */	0x12, 0x0,	/* FC_UP */
/* 3700 */	NdrFcShort( 0xffd2 ),	/* Offset= -46 (3654) */
/* 3702 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 3704 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 3706 */	
			0x11, 0x0,	/* FC_RP */
/* 3708 */	NdrFcShort( 0x2 ),	/* Offset= 2 (3710) */
/* 3710 */	
			0x1b,		/* FC_CARRAY */
			0x1,		/* 1 */
/* 3712 */	NdrFcShort( 0x2 ),	/* 2 */
/* 3714 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 3716 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3718 */	0x5,		/* FC_WCHAR */
			0x5b,		/* FC_END */
/* 3720 */	
			0x12, 0x10,	/* FC_UP [pointer_deref] */
/* 3722 */	NdrFcShort( 0x2 ),	/* Offset= 2 (3724) */
/* 3724 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 3726 */	NdrFcShort( 0x10 ),	/* Offset= 16 (3742) */
/* 3728 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 3730 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3732 */	0x9,		/* Corr desc: FC_ULONG */
			0x0,		/*  */
/* 3734 */	NdrFcShort( 0xfffc ),	/* -4 */
/* 3736 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 3738 */	NdrFcShort( 0xfa86 ),	/* Offset= -1402 (2336) */
/* 3740 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 3742 */	
			0x18,		/* FC_CPSTRUCT */
			0x3,		/* 3 */
/* 3744 */	NdrFcShort( 0x4 ),	/* 4 */
/* 3746 */	NdrFcShort( 0xffee ),	/* Offset= -18 (3728) */
/* 3748 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 3750 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 3752 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3754 */	NdrFcShort( 0x4 ),	/* 4 */
/* 3756 */	NdrFcShort( 0x1 ),	/* 1 */
/* 3758 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3760 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3762 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 3764 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 3766 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 3768 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 3770 */	
			0x11, 0x0,	/* FC_RP */
/* 3772 */	NdrFcShort( 0x2 ),	/* Offset= 2 (3774) */
/* 3774 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0x9,		/* FC_ULONG */
/* 3776 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 3778 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3780 */	NdrFcShort( 0xff32 ),	/* Offset= -206 (3574) */

			0x0
        }
    };

static const GENERIC_BINDING_ROUTINE_PAIR BindingRoutines[ GENERIC_BINDING_TABLE_SIZE ] = 
        {
        {
            (GENERIC_BINDING_ROUTINE)SRVSVC_HANDLE_bind,
            (GENERIC_UNBIND_ROUTINE)SRVSVC_HANDLE_unbind
         }
        
        };


static const unsigned short srvsvc_FormatStringOffsetTable[] =
    {
    0,
    36,
    68,
    96,
    136,
    172,
    208,
    234,
    264,
    304,
    348,
    378,
    402,
    446,
    476,
    508,
    544,
    576,
    612,
    640,
    668,
    698,
    726,
    758,
    796,
    830,
    858,
    894,
    922,
    948,
    978,
    1010,
    1052,
    1086,
    1116,
    1152,
    1186,
    1222,
    1254,
    1276,
    1312,
    1348,
    1376,
    1412,
    1438,
    1482,
    1512,
    1544,
    1574,
    1612,
    1644,
    1674,
    1722,
    1748
    };


static const MIDL_STUB_DESC srvsvc_StubDesc = 
    {
    (void *)& srvsvc___RpcClientInterface,
    MIDL_user_allocate,
    MIDL_user_free,
    &srvsvc_bhandle,
    0,
    BindingRoutines,
    0,
    0,
    __MIDL_TypeFormatString.Format,
    0, /* -error bounds_check flag */
    0x10001, /* Ndr library version */
    0,
    0x6000169, /* MIDL Version 6.0.361 */
    0,
    0,
    0,  /* notify & notify_flag routine table */
    0x1, /* MIDL flag */
    0, /* cs routines */
    0,   /* proxy/server info */
    0   /* Reserved5 */
    };
#if _MSC_VER >= 1200
#pragma warning(pop)
#endif


#endif /* !defined(_M_IA64) && !defined(_M_AMD64)*/



/* this ALWAYS GENERATED file contains the RPC client stubs */


 /* File created by MIDL compiler version 6.00.0361 */
/* at Sun Apr 02 10:36:50 2023
 */
/* Compiler settings for srvsvc.idl:
    Oicf, W1, Zp8, env=Win64 (32b run,appending)
    protocol : dce , ms_ext, c_ext, oldnames, robust
    error checks: allocation ref 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#if defined(_M_IA64) || defined(_M_AMD64)


#pragma warning( disable: 4049 )  /* more than 64k source lines */
#if _MSC_VER >= 1200
#pragma warning(push)
#endif

#pragma warning( disable: 4211 )  /* redefine extent to static */
#pragma warning( disable: 4232 )  /* dllimport identity*/
#include <string.h>

#include "srvsvc.h"

#define TYPE_FORMAT_STRING_SIZE   3425                              
#define PROC_FORMAT_STRING_SIZE   3433                              
#define TRANSMIT_AS_TABLE_SIZE    0            
#define WIRE_MARSHAL_TABLE_SIZE   0            

typedef struct _MIDL_TYPE_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ TYPE_FORMAT_STRING_SIZE ];
    } MIDL_TYPE_FORMAT_STRING;

typedef struct _MIDL_PROC_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ PROC_FORMAT_STRING_SIZE ];
    } MIDL_PROC_FORMAT_STRING;


static RPC_SYNTAX_IDENTIFIER  _RpcTransferSyntax = 
{{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}};


extern const MIDL_TYPE_FORMAT_STRING __MIDL_TypeFormatString;
extern const MIDL_PROC_FORMAT_STRING __MIDL_ProcFormatString;

#define GENERIC_BINDING_TABLE_SIZE   1            


/* Standard interface: srvsvc, ver. 3.0,
   GUID={0x4B324FC8,0x1670,0x01D3,{0x12,0x78,0x5A,0x47,0xBF,0x6E,0xE1,0x88}} */

handle_t srvsvc_bhandle;


static const RPC_CLIENT_INTERFACE srvsvc___RpcClientInterface =
    {
    sizeof(RPC_CLIENT_INTERFACE),
    {{0x4B324FC8,0x1670,0x01D3,{0x12,0x78,0x5A,0x47,0xBF,0x6E,0xE1,0x88}},{3,0}},
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    0,
    0,
    0,
    0,
    0x00000000
    };
RPC_IF_HANDLE srvsvc_ClientIfHandle = (RPC_IF_HANDLE)& srvsvc___RpcClientInterface;

extern const MIDL_STUB_DESC srvsvc_StubDesc;

static RPC_BINDING_HANDLE srvsvc__MIDL_AutoBindHandle;


DWORD NetrCharDevEnum( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [out][in] */ LPCHARDEV_ENUM_STRUCT InfoStruct,
    /* [in] */ DWORD PreferedMaximumLength,
    /* [out] */ LPDWORD TotalEntries,
    /* [unique][out][in] */ LPDWORD ResumeHandle)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[0],
                  ServerName,
                  InfoStruct,
                  PreferedMaximumLength,
                  TotalEntries,
                  ResumeHandle);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetrCharDevGetInfo( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [string][in] */ wchar_t *DevName,
    /* [in] */ DWORD Level,
    /* [switch_is][out] */ LPCHARDEV_INFO InfoStruct)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[68],
                  ServerName,
                  DevName,
                  Level,
                  InfoStruct);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetrCharDevControl( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [string][in] */ wchar_t *DevName,
    /* [in] */ DWORD Opcode)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[130],
                  ServerName,
                  DevName,
                  Opcode);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetrCharDevQEnum( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [unique][string][in] */ wchar_t *UserName,
    /* [out][in] */ LPCHARDEVQ_ENUM_STRUCT InfoStruct,
    /* [in] */ DWORD PreferedMaximumLength,
    /* [out] */ LPDWORD TotalEntries,
    /* [unique][out][in] */ LPDWORD ResumeHandle)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[186],
                  ServerName,
                  UserName,
                  InfoStruct,
                  PreferedMaximumLength,
                  TotalEntries,
                  ResumeHandle);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetrCharDevQGetInfo( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [string][in] */ wchar_t *QueueName,
    /* [string][in] */ wchar_t *UserName,
    /* [in] */ DWORD Level,
    /* [switch_is][out] */ LPCHARDEVQ_INFO InfoStruct)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[260],
                  ServerName,
                  QueueName,
                  UserName,
                  Level,
                  InfoStruct);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetrCharDevQSetInfo( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [string][in] */ wchar_t *QueueName,
    /* [in] */ DWORD Level,
    /* [switch_is][in] */ LPCHARDEVQ_INFO CharDevQInfo,
    /* [unique][out][in] */ LPDWORD ParmErr)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[328],
                  ServerName,
                  QueueName,
                  Level,
                  CharDevQInfo,
                  ParmErr);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetrCharDevQPurge( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [string][in] */ wchar_t *QueueName)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[396],
                  ServerName,
                  QueueName);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetrCharDevQPurgeSelf( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [string][in] */ wchar_t *QueueName,
    /* [string][in] */ wchar_t *ComputerName)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[446],
                  ServerName,
                  QueueName,
                  ComputerName);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetrConnectionEnum( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [unique][string][in] */ wchar_t *Qualifier,
    /* [out][in] */ LPCONNECT_ENUM_STRUCT InfoStruct,
    /* [in] */ DWORD PreferedMaximumLength,
    /* [out] */ LPDWORD TotalEntries,
    /* [unique][out][in] */ LPDWORD ResumeHandle)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[502],
                  ServerName,
                  Qualifier,
                  InfoStruct,
                  PreferedMaximumLength,
                  TotalEntries,
                  ResumeHandle);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetrFileEnum( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [unique][string][in] */ wchar_t *BasePath,
    /* [unique][string][in] */ wchar_t *UserName,
    /* [out][in] */ PFILE_ENUM_STRUCT InfoStruct,
    /* [in] */ DWORD PreferedMaximumLength,
    /* [out] */ LPDWORD TotalEntries,
    /* [unique][out][in] */ LPDWORD ResumeHandle)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[576],
                  ServerName,
                  BasePath,
                  UserName,
                  InfoStruct,
                  PreferedMaximumLength,
                  TotalEntries,
                  ResumeHandle);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetrFileGetInfo( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [in] */ DWORD FileId,
    /* [in] */ DWORD Level,
    /* [switch_is][out] */ LPFILE_INFO InfoStruct)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[656],
                  ServerName,
                  FileId,
                  Level,
                  InfoStruct);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetrFileClose( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [in] */ DWORD FileId)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[718],
                  ServerName,
                  FileId);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetrSessionEnum( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [unique][string][in] */ wchar_t *ClientName,
    /* [unique][string][in] */ wchar_t *UserName,
    /* [out][in] */ PSESSION_ENUM_STRUCT InfoStruct,
    /* [in] */ DWORD PreferedMaximumLength,
    /* [out] */ LPDWORD TotalEntries,
    /* [unique][out][in] */ LPDWORD ResumeHandle)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[768],
                  ServerName,
                  ClientName,
                  UserName,
                  InfoStruct,
                  PreferedMaximumLength,
                  TotalEntries,
                  ResumeHandle);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetrSessionDel( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [unique][string][in] */ wchar_t *ClientName,
    /* [unique][string][in] */ wchar_t *UserName)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[848],
                  ServerName,
                  ClientName,
                  UserName);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetrShareAdd( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [in] */ DWORD Level,
    /* [switch_is][in] */ LPSHARE_INFO InfoStruct,
    /* [unique][out][in] */ LPDWORD ParmErr)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[904],
                  ServerName,
                  Level,
                  InfoStruct,
                  ParmErr);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetrShareEnum( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [out][in] */ LPSHARE_ENUM_STRUCT InfoStruct,
    /* [in] */ DWORD PreferedMaximumLength,
    /* [out] */ LPDWORD TotalEntries,
    /* [unique][out][in] */ LPDWORD ResumeHandle)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[966],
                  ServerName,
                  InfoStruct,
                  PreferedMaximumLength,
                  TotalEntries,
                  ResumeHandle);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetrShareGetInfo( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [string][in] */ wchar_t *NetName,
    /* [in] */ DWORD Level,
    /* [switch_is][out] */ LPSHARE_INFO InfoStruct)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1034],
                  ServerName,
                  NetName,
                  Level,
                  InfoStruct);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetrShareSetInfo( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [string][in] */ wchar_t *NetName,
    /* [in] */ DWORD Level,
    /* [switch_is][in] */ LPSHARE_INFO ShareInfo,
    /* [unique][out][in] */ LPDWORD ParmErr)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1096],
                  ServerName,
                  NetName,
                  Level,
                  ShareInfo,
                  ParmErr);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetrShareDel( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [string][in] */ wchar_t *NetName,
    /* [in] */ DWORD Reserved)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1164],
                  ServerName,
                  NetName,
                  Reserved);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetrShareDelSticky( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [string][in] */ wchar_t *NetName,
    /* [in] */ DWORD Reserved)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1220],
                  ServerName,
                  NetName,
                  Reserved);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetrShareCheck( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [string][in] */ wchar_t *Device,
    /* [out] */ LPDWORD Type)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1276],
                  ServerName,
                  Device,
                  Type);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetrServerGetInfo( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [in] */ DWORD Level,
    /* [switch_is][out] */ LPSERVER_INFO InfoStruct)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1332],
                  ServerName,
                  Level,
                  InfoStruct);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetrServerSetInfo( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [in] */ DWORD Level,
    /* [switch_is][in] */ LPSERVER_INFO ServerInfo,
    /* [unique][out][in] */ LPDWORD ParmErr)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1388],
                  ServerName,
                  Level,
                  ServerInfo,
                  ParmErr);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetrServerDiskEnum( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [in] */ DWORD Level,
    /* [out][in] */ DISK_ENUM_CONTAINER *DiskInfoStruct,
    /* [in] */ DWORD PreferredMaximumLength,
    /* [out] */ LPDWORD TotalEntries,
    /* [unique][out][in] */ LPDWORD ResumeHandle)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1450],
                  ServerName,
                  Level,
                  DiskInfoStruct,
                  PreferredMaximumLength,
                  TotalEntries,
                  ResumeHandle);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetrServerStatisticsGet( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [unique][string][in] */ wchar_t *Service,
    /* [in] */ DWORD Level,
    /* [in] */ DWORD Options,
    /* [out] */ LPSTAT_SERVER_0 *InfoStruct)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1524],
                  ServerName,
                  Service,
                  Level,
                  Options,
                  InfoStruct);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetrServerTransportAdd( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [in] */ DWORD Level,
    /* [in] */ LPSERVER_TRANSPORT_INFO_0 Buffer)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1592],
                  ServerName,
                  Level,
                  Buffer);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetrServerTransportEnum( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [out][in] */ LPSERVER_XPORT_ENUM_STRUCT InfoStruct,
    /* [in] */ DWORD PreferedMaximumLength,
    /* [out] */ LPDWORD TotalEntries,
    /* [unique][out][in] */ LPDWORD ResumeHandle)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1648],
                  ServerName,
                  InfoStruct,
                  PreferedMaximumLength,
                  TotalEntries,
                  ResumeHandle);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetrServerTransportDel( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [in] */ DWORD Level,
    /* [in] */ LPSERVER_TRANSPORT_INFO_0 Buffer)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1716],
                  ServerName,
                  Level,
                  Buffer);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetrRemoteTOD( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [out] */ LPTIME_OF_DAY_INFO *BufferPtr)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1772],
                  ServerName,
                  BufferPtr);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD I_NetrServerSetServiceBits( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [unique][string][in] */ wchar_t *TransportName,
    /* [in] */ DWORD ServiceBits,
    /* [in] */ DWORD UpdateImmediately)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1822],
                  ServerName,
                  TransportName,
                  ServiceBits,
                  UpdateImmediately);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetprPathType( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [string][in] */ wchar_t *PathName,
    /* [out] */ LPDWORD PathType,
    /* [in] */ DWORD Flags)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1884],
                  ServerName,
                  PathName,
                  PathType,
                  Flags);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetprPathCanonicalize( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [string][in] */ wchar_t *PathName,
    /* [size_is][out] */ LPBYTE Outbuf,
    /* [range][in] */ DWORD OutbufLen,
    /* [string][in] */ wchar_t *Prefix,
    /* [out][in] */ LPDWORD PathType,
    /* [in] */ DWORD Flags)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1946],
                  ServerName,
                  PathName,
                  Outbuf,
                  OutbufLen,
                  Prefix,
                  PathType,
                  Flags);
    return ( DWORD  )_RetVal.Simple;
    
}


LONG NetprPathCompare( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [string][in] */ wchar_t *PathName1,
    /* [string][in] */ wchar_t *PathName2,
    /* [in] */ DWORD PathType,
    /* [in] */ DWORD Flags)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[2026],
                  ServerName,
                  PathName1,
                  PathName2,
                  PathType,
                  Flags);
    return ( LONG  )_RetVal.Simple;
    
}


DWORD NetprNameValidate( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [string][in] */ wchar_t *Name,
    /* [in] */ DWORD NameType,
    /* [in] */ DWORD Flags)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[2094],
                  ServerName,
                  Name,
                  NameType,
                  Flags);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetprNameCanonicalize( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [string][in] */ wchar_t *Name,
    /* [size_is][out] */ wchar_t *Outbuf,
    /* [range][in] */ DWORD OutbufLen,
    /* [in] */ DWORD NameType,
    /* [in] */ DWORD Flags)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[2156],
                  ServerName,
                  Name,
                  Outbuf,
                  OutbufLen,
                  NameType,
                  Flags);
    return ( DWORD  )_RetVal.Simple;
    
}


LONG NetprNameCompare( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [string][in] */ wchar_t *Name1,
    /* [string][in] */ wchar_t *Name2,
    /* [in] */ DWORD NameType,
    /* [in] */ DWORD Flags)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[2230],
                  ServerName,
                  Name1,
                  Name2,
                  NameType,
                  Flags);
    return ( LONG  )_RetVal.Simple;
    
}


DWORD NetrShareEnumSticky( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [out][in] */ LPSHARE_ENUM_STRUCT InfoStruct,
    /* [in] */ DWORD PreferedMaximumLength,
    /* [out] */ LPDWORD TotalEntries,
    /* [unique][out][in] */ LPDWORD ResumeHandle)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[2298],
                  ServerName,
                  InfoStruct,
                  PreferedMaximumLength,
                  TotalEntries,
                  ResumeHandle);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetrShareDelStart( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [string][in] */ wchar_t *NetName,
    /* [in] */ DWORD Reserved,
    /* [out] */ PSHARE_DEL_HANDLE ContextHandle)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[2366],
                  ServerName,
                  NetName,
                  Reserved,
                  ContextHandle);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetrShareDelCommit( 
    /* [out][in] */ PSHARE_DEL_HANDLE ContextHandle)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[2428],
                  ContextHandle);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetrpGetFileSecurity( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [unique][string][in] */ LPWSTR ShareName,
    /* [string][in] */ LPWSTR lpFileName,
    /* [in] */ SECURITY_INFORMATION RequestedInformation,
    /* [out] */ PADT_SECURITY_DESCRIPTOR *SecurityDescriptor)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[2472],
                  ServerName,
                  ShareName,
                  lpFileName,
                  RequestedInformation,
                  SecurityDescriptor);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetrpSetFileSecurity( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [unique][string][in] */ LPWSTR ShareName,
    /* [string][in] */ LPWSTR lpFileName,
    /* [in] */ SECURITY_INFORMATION SecurityInformation,
    /* [in] */ PADT_SECURITY_DESCRIPTOR SecurityDescriptor)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[2540],
                  ServerName,
                  ShareName,
                  lpFileName,
                  SecurityInformation,
                  SecurityDescriptor);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetrServerTransportAddEx( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [in] */ DWORD Level,
    /* [switch_is][in] */ LPTRANSPORT_INFO Buffer)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[2608],
                  ServerName,
                  Level,
                  Buffer);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD I_NetrServerSetServiceBitsEx( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [unique][string][in] */ LPWSTR EmulatedServerName,
    /* [unique][string][in] */ wchar_t *TransportName,
    /* [in] */ DWORD ServiceBitsOfInterest,
    /* [in] */ DWORD ServiceBits,
    /* [in] */ DWORD UpdateImmediately)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[2664],
                  ServerName,
                  EmulatedServerName,
                  TransportName,
                  ServiceBitsOfInterest,
                  ServiceBits,
                  UpdateImmediately);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD __stdcall NetrDfsGetVersion( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [out] */ LPDWORD Version)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[2738],
                  ServerName,
                  Version);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD __stdcall NetrDfsCreateLocalPartition( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [string][in] */ LPWSTR ShareName,
    /* [in] */ LPGUID EntryUid,
    /* [string][in] */ LPWSTR EntryPrefix,
    /* [string][in] */ LPWSTR ShortName,
    /* [in] */ LPNET_DFS_ENTRY_ID_CONTAINER RelationInfo,
    /* [in] */ BOOL Force)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[2788],
                  ServerName,
                  ShareName,
                  EntryUid,
                  EntryPrefix,
                  ShortName,
                  RelationInfo,
                  Force);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD __stdcall NetrDfsDeleteLocalPartition( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [in] */ LPGUID Uid,
    /* [string][in] */ LPWSTR Prefix)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[2868],
                  ServerName,
                  Uid,
                  Prefix);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD __stdcall NetrDfsSetLocalVolumeState( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [in] */ LPGUID Uid,
    /* [string][in] */ LPWSTR Prefix,
    /* [in] */ ULONG State)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[2924],
                  ServerName,
                  Uid,
                  Prefix,
                  State);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD __stdcall NetrDfsSetServerInfo( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [in] */ LPGUID Uid,
    /* [string][in] */ LPWSTR Prefix)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[2986],
                  ServerName,
                  Uid,
                  Prefix);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD __stdcall NetrDfsCreateExitPoint( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [in] */ LPGUID Uid,
    /* [string][in] */ LPWSTR Prefix,
    /* [in] */ ULONG Type,
    /* [range][in] */ DWORD ShortPrefixLen,
    /* [size_is][out] */ LPWSTR ShortPrefix)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[3042],
                  ServerName,
                  Uid,
                  Prefix,
                  Type,
                  ShortPrefixLen,
                  ShortPrefix);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD __stdcall NetrDfsDeleteExitPoint( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [in] */ LPGUID Uid,
    /* [string][in] */ LPWSTR Prefix,
    /* [in] */ ULONG Type)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[3116],
                  ServerName,
                  Uid,
                  Prefix,
                  Type);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD __stdcall NetrDfsModifyPrefix( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [in] */ LPGUID Uid,
    /* [string][in] */ LPWSTR Prefix)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[3178],
                  ServerName,
                  Uid,
                  Prefix);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD __stdcall NetrDfsFixLocalVolume( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [string][in] */ LPWSTR VolumeName,
    /* [in] */ ULONG EntryType,
    /* [in] */ ULONG ServiceType,
    /* [string][in] */ LPWSTR StgId,
    /* [in] */ LPGUID EntryUid,
    /* [string][in] */ LPWSTR EntryPrefix,
    /* [in] */ LPNET_DFS_ENTRY_ID_CONTAINER RelationInfo,
    /* [in] */ ULONG CreateDisposition)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[3234],
                  ServerName,
                  VolumeName,
                  EntryType,
                  ServiceType,
                  StgId,
                  EntryUid,
                  EntryPrefix,
                  RelationInfo,
                  CreateDisposition);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD __stdcall NetrDfsManagerReportSiteInfo( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [unique][out][in] */ LPDFS_SITELIST_INFO *ppSiteInfo)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[3326],
                  ServerName,
                  ppSiteInfo);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetrServerTransportDelEx( 
    /* [unique][string][in] */ SRVSVC_HANDLE ServerName,
    /* [in] */ DWORD Level,
    /* [switch_is][in] */ LPTRANSPORT_INFO Buffer)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&srvsvc_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[3376],
                  ServerName,
                  Level,
                  Buffer);
    return ( DWORD  )_RetVal.Simple;
    
}

extern const GENERIC_BINDING_ROUTINE_PAIR BindingRoutines[ GENERIC_BINDING_TABLE_SIZE ];

#if !defined(__RPC_WIN64__)
#error  Invalid build platform for this stub.
#endif

static const MIDL_PROC_FORMAT_STRING __MIDL_ProcFormatString =
    {
        0,
        {

	/* Procedure NetrCharDevEnum */

			0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/*  2 */	NdrFcLong( 0x0 ),	/* 0 */
/*  6 */	NdrFcShort( 0x0 ),	/* 0 */
/*  8 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 10 */	0x31,		/* FC_BIND_GENERIC */
			0x8,		/* 8 */
/* 12 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 14 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 16 */	NdrFcShort( 0x24 ),	/* 36 */
/* 18 */	NdrFcShort( 0x40 ),	/* 64 */
/* 20 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x6,		/* 6 */
/* 22 */	0xa,		/* 10 */
			0x7,		/* Ext Flags:  new corr desc, clt corr check, srv corr check, */
/* 24 */	NdrFcShort( 0x3 ),	/* 3 */
/* 26 */	NdrFcShort( 0x3 ),	/* 3 */
/* 28 */	NdrFcShort( 0x0 ),	/* 0 */
/* 30 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ServerName */

/* 32 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 34 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 36 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter InfoStruct */

/* 38 */	NdrFcShort( 0x11b ),	/* Flags:  must size, must free, in, out, simple ref, */
/* 40 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 42 */	NdrFcShort( 0xa0 ),	/* Type Offset=160 */

	/* Parameter PreferedMaximumLength */

/* 44 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 46 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 48 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter TotalEntries */

/* 50 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 52 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 54 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ResumeHandle */

/* 56 */	NdrFcShort( 0x1a ),	/* Flags:  must free, in, out, */
/* 58 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 60 */	NdrFcShort( 0xb4 ),	/* Type Offset=180 */

	/* Return value */

/* 62 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 64 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 66 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure NetrCharDevGetInfo */

/* 68 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 70 */	NdrFcLong( 0x0 ),	/* 0 */
/* 74 */	NdrFcShort( 0x1 ),	/* 1 */
/* 76 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 78 */	0x31,		/* FC_BIND_GENERIC */
			0x8,		/* 8 */
/* 80 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 82 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 84 */	NdrFcShort( 0x8 ),	/* 8 */
/* 86 */	NdrFcShort( 0x8 ),	/* 8 */
/* 88 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 90 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 92 */	NdrFcShort( 0x1 ),	/* 1 */
/* 94 */	NdrFcShort( 0x0 ),	/* 0 */
/* 96 */	NdrFcShort( 0x0 ),	/* 0 */
/* 98 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ServerName */

/* 100 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 102 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 104 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter DevName */

/* 106 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 108 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 110 */	NdrFcShort( 0xba ),	/* Type Offset=186 */

	/* Parameter Level */

/* 112 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 114 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 116 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter InfoStruct */

/* 118 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 120 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 122 */	NdrFcShort( 0xc0 ),	/* Type Offset=192 */

	/* Return value */

/* 124 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 126 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 128 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure NetrCharDevControl */

/* 130 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 132 */	NdrFcLong( 0x0 ),	/* 0 */
/* 136 */	NdrFcShort( 0x2 ),	/* 2 */
/* 138 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 140 */	0x31,		/* FC_BIND_GENERIC */
			0x8,		/* 8 */
/* 142 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 144 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 146 */	NdrFcShort( 0x8 ),	/* 8 */
/* 148 */	NdrFcShort( 0x8 ),	/* 8 */
/* 150 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 152 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 154 */	NdrFcShort( 0x0 ),	/* 0 */
/* 156 */	NdrFcShort( 0x0 ),	/* 0 */
/* 158 */	NdrFcShort( 0x0 ),	/* 0 */
/* 160 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ServerName */

/* 162 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 164 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 166 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter DevName */

/* 168 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 170 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 172 */	NdrFcShort( 0xba ),	/* Type Offset=186 */

	/* Parameter Opcode */

/* 174 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 176 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 178 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 180 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 182 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 184 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure NetrCharDevQEnum */

/* 186 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 188 */	NdrFcLong( 0x0 ),	/* 0 */
/* 192 */	NdrFcShort( 0x3 ),	/* 3 */
/* 194 */	NdrFcShort( 0x38 ),	/* ia64 Stack size/offset = 56 */
/* 196 */	0x31,		/* FC_BIND_GENERIC */
			0x8,		/* 8 */
/* 198 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 200 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 202 */	NdrFcShort( 0x24 ),	/* 36 */
/* 204 */	NdrFcShort( 0x40 ),	/* 64 */
/* 206 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x7,		/* 7 */
/* 208 */	0xa,		/* 10 */
			0x7,		/* Ext Flags:  new corr desc, clt corr check, srv corr check, */
/* 210 */	NdrFcShort( 0x3 ),	/* 3 */
/* 212 */	NdrFcShort( 0x3 ),	/* 3 */
/* 214 */	NdrFcShort( 0x0 ),	/* 0 */
/* 216 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ServerName */

/* 218 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 220 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 222 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter UserName */

/* 224 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 226 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 228 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter InfoStruct */

/* 230 */	NdrFcShort( 0x11b ),	/* Flags:  must size, must free, in, out, simple ref, */
/* 232 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 234 */	NdrFcShort( 0x17e ),	/* Type Offset=382 */

	/* Parameter PreferedMaximumLength */

/* 236 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 238 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 240 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter TotalEntries */

/* 242 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 244 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 246 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ResumeHandle */

/* 248 */	NdrFcShort( 0x1a ),	/* Flags:  must free, in, out, */
/* 250 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 252 */	NdrFcShort( 0xb4 ),	/* Type Offset=180 */

	/* Return value */

/* 254 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 256 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 258 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure NetrCharDevQGetInfo */

/* 260 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 262 */	NdrFcLong( 0x0 ),	/* 0 */
/* 266 */	NdrFcShort( 0x4 ),	/* 4 */
/* 268 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 270 */	0x31,		/* FC_BIND_GENERIC */
			0x8,		/* 8 */
/* 272 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 274 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 276 */	NdrFcShort( 0x8 ),	/* 8 */
/* 278 */	NdrFcShort( 0x8 ),	/* 8 */
/* 280 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x6,		/* 6 */
/* 282 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 284 */	NdrFcShort( 0x1 ),	/* 1 */
/* 286 */	NdrFcShort( 0x0 ),	/* 0 */
/* 288 */	NdrFcShort( 0x0 ),	/* 0 */
/* 290 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ServerName */

/* 292 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 294 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 296 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter QueueName */

/* 298 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 300 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 302 */	NdrFcShort( 0xba ),	/* Type Offset=186 */

	/* Parameter UserName */

/* 304 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 306 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 308 */	NdrFcShort( 0xba ),	/* Type Offset=186 */

	/* Parameter Level */

/* 310 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 312 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 314 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter InfoStruct */

/* 316 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 318 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 320 */	NdrFcShort( 0x192 ),	/* Type Offset=402 */

	/* Return value */

/* 322 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 324 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 326 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure NetrCharDevQSetInfo */

/* 328 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 330 */	NdrFcLong( 0x0 ),	/* 0 */
/* 334 */	NdrFcShort( 0x5 ),	/* 5 */
/* 336 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 338 */	0x31,		/* FC_BIND_GENERIC */
			0x8,		/* 8 */
/* 340 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 342 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 344 */	NdrFcShort( 0x24 ),	/* 36 */
/* 346 */	NdrFcShort( 0x24 ),	/* 36 */
/* 348 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x6,		/* 6 */
/* 350 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 352 */	NdrFcShort( 0x0 ),	/* 0 */
/* 354 */	NdrFcShort( 0x1 ),	/* 1 */
/* 356 */	NdrFcShort( 0x0 ),	/* 0 */
/* 358 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ServerName */

/* 360 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 362 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 364 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter QueueName */

/* 366 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 368 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 370 */	NdrFcShort( 0xba ),	/* Type Offset=186 */

	/* Parameter Level */

/* 372 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 374 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 376 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter CharDevQInfo */

/* 378 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 380 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 382 */	NdrFcShort( 0x1e2 ),	/* Type Offset=482 */

	/* Parameter ParmErr */

/* 384 */	NdrFcShort( 0x1a ),	/* Flags:  must free, in, out, */
/* 386 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 388 */	NdrFcShort( 0xb4 ),	/* Type Offset=180 */

	/* Return value */

/* 390 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 392 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 394 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure NetrCharDevQPurge */

/* 396 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 398 */	NdrFcLong( 0x0 ),	/* 0 */
/* 402 */	NdrFcShort( 0x6 ),	/* 6 */
/* 404 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 406 */	0x31,		/* FC_BIND_GENERIC */
			0x8,		/* 8 */
/* 408 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 410 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 412 */	NdrFcShort( 0x0 ),	/* 0 */
/* 414 */	NdrFcShort( 0x8 ),	/* 8 */
/* 416 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 418 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 420 */	NdrFcShort( 0x0 ),	/* 0 */
/* 422 */	NdrFcShort( 0x0 ),	/* 0 */
/* 424 */	NdrFcShort( 0x0 ),	/* 0 */
/* 426 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ServerName */

/* 428 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 430 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 432 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter QueueName */

/* 434 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 436 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 438 */	NdrFcShort( 0xba ),	/* Type Offset=186 */

	/* Return value */

/* 440 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 442 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 444 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure NetrCharDevQPurgeSelf */

/* 446 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 448 */	NdrFcLong( 0x0 ),	/* 0 */
/* 452 */	NdrFcShort( 0x7 ),	/* 7 */
/* 454 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 456 */	0x31,		/* FC_BIND_GENERIC */
			0x8,		/* 8 */
/* 458 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 460 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 462 */	NdrFcShort( 0x0 ),	/* 0 */
/* 464 */	NdrFcShort( 0x8 ),	/* 8 */
/* 466 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 468 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 470 */	NdrFcShort( 0x0 ),	/* 0 */
/* 472 */	NdrFcShort( 0x0 ),	/* 0 */
/* 474 */	NdrFcShort( 0x0 ),	/* 0 */
/* 476 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ServerName */

/* 478 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 480 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 482 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter QueueName */

/* 484 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 486 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 488 */	NdrFcShort( 0xba ),	/* Type Offset=186 */

	/* Parameter ComputerName */

/* 490 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 492 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 494 */	NdrFcShort( 0xba ),	/* Type Offset=186 */

	/* Return value */

/* 496 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 498 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 500 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure NetrConnectionEnum */

/* 502 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 504 */	NdrFcLong( 0x0 ),	/* 0 */
/* 508 */	NdrFcShort( 0x8 ),	/* 8 */
/* 510 */	NdrFcShort( 0x38 ),	/* ia64 Stack size/offset = 56 */
/* 512 */	0x31,		/* FC_BIND_GENERIC */
			0x8,		/* 8 */
/* 514 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 516 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 518 */	NdrFcShort( 0x24 ),	/* 36 */
/* 520 */	NdrFcShort( 0x40 ),	/* 64 */
/* 522 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x7,		/* 7 */
/* 524 */	0xa,		/* 10 */
			0x7,		/* Ext Flags:  new corr desc, clt corr check, srv corr check, */
/* 526 */	NdrFcShort( 0x3 ),	/* 3 */
/* 528 */	NdrFcShort( 0x3 ),	/* 3 */
/* 530 */	NdrFcShort( 0x0 ),	/* 0 */
/* 532 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ServerName */

/* 534 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 536 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 538 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter Qualifier */

/* 540 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 542 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 544 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter InfoStruct */

/* 546 */	NdrFcShort( 0x11b ),	/* Flags:  must size, must free, in, out, simple ref, */
/* 548 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 550 */	NdrFcShort( 0x274 ),	/* Type Offset=628 */

	/* Parameter PreferedMaximumLength */

/* 552 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 554 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 556 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter TotalEntries */

/* 558 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 560 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 562 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ResumeHandle */

/* 564 */	NdrFcShort( 0x1a ),	/* Flags:  must free, in, out, */
/* 566 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 568 */	NdrFcShort( 0xb4 ),	/* Type Offset=180 */

	/* Return value */

/* 570 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 572 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 574 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure NetrFileEnum */

/* 576 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 578 */	NdrFcLong( 0x0 ),	/* 0 */
/* 582 */	NdrFcShort( 0x9 ),	/* 9 */
/* 584 */	NdrFcShort( 0x40 ),	/* ia64 Stack size/offset = 64 */
/* 586 */	0x31,		/* FC_BIND_GENERIC */
			0x8,		/* 8 */
/* 588 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 590 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 592 */	NdrFcShort( 0x24 ),	/* 36 */
/* 594 */	NdrFcShort( 0x40 ),	/* 64 */
/* 596 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x8,		/* 8 */
/* 598 */	0xa,		/* 10 */
			0x7,		/* Ext Flags:  new corr desc, clt corr check, srv corr check, */
/* 600 */	NdrFcShort( 0x3 ),	/* 3 */
/* 602 */	NdrFcShort( 0x3 ),	/* 3 */
/* 604 */	NdrFcShort( 0x0 ),	/* 0 */
/* 606 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ServerName */

/* 608 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 610 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 612 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter BasePath */

/* 614 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 616 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 618 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter UserName */

/* 620 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 622 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 624 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter InfoStruct */

/* 626 */	NdrFcShort( 0x11b ),	/* Flags:  must size, must free, in, out, simple ref, */
/* 628 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 630 */	NdrFcShort( 0x2fa ),	/* Type Offset=762 */

	/* Parameter PreferedMaximumLength */

/* 632 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 634 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 636 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter TotalEntries */

/* 638 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 640 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 642 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ResumeHandle */

/* 644 */	NdrFcShort( 0x1a ),	/* Flags:  must free, in, out, */
/* 646 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 648 */	NdrFcShort( 0xb4 ),	/* Type Offset=180 */

	/* Return value */

/* 650 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 652 */	NdrFcShort( 0x38 ),	/* ia64 Stack size/offset = 56 */
/* 654 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure NetrFileGetInfo */

/* 656 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 658 */	NdrFcLong( 0x0 ),	/* 0 */
/* 662 */	NdrFcShort( 0xa ),	/* 10 */
/* 664 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 666 */	0x31,		/* FC_BIND_GENERIC */
			0x8,		/* 8 */
/* 668 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 670 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 672 */	NdrFcShort( 0x10 ),	/* 16 */
/* 674 */	NdrFcShort( 0x8 ),	/* 8 */
/* 676 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 678 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 680 */	NdrFcShort( 0x1 ),	/* 1 */
/* 682 */	NdrFcShort( 0x0 ),	/* 0 */
/* 684 */	NdrFcShort( 0x0 ),	/* 0 */
/* 686 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ServerName */

/* 688 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 690 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 692 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter FileId */

/* 694 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 696 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 698 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter Level */

/* 700 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 702 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 704 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter InfoStruct */

/* 706 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 708 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 710 */	NdrFcShort( 0x30e ),	/* Type Offset=782 */

	/* Return value */

/* 712 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 714 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 716 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure NetrFileClose */

/* 718 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 720 */	NdrFcLong( 0x0 ),	/* 0 */
/* 724 */	NdrFcShort( 0xb ),	/* 11 */
/* 726 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 728 */	0x31,		/* FC_BIND_GENERIC */
			0x8,		/* 8 */
/* 730 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 732 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 734 */	NdrFcShort( 0x8 ),	/* 8 */
/* 736 */	NdrFcShort( 0x8 ),	/* 8 */
/* 738 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 740 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 742 */	NdrFcShort( 0x0 ),	/* 0 */
/* 744 */	NdrFcShort( 0x0 ),	/* 0 */
/* 746 */	NdrFcShort( 0x0 ),	/* 0 */
/* 748 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ServerName */

/* 750 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 752 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 754 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter FileId */

/* 756 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 758 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 760 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 762 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 764 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 766 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure NetrSessionEnum */

/* 768 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 770 */	NdrFcLong( 0x0 ),	/* 0 */
/* 774 */	NdrFcShort( 0xc ),	/* 12 */
/* 776 */	NdrFcShort( 0x40 ),	/* ia64 Stack size/offset = 64 */
/* 778 */	0x31,		/* FC_BIND_GENERIC */
			0x8,		/* 8 */
/* 780 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 782 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 784 */	NdrFcShort( 0x24 ),	/* 36 */
/* 786 */	NdrFcShort( 0x40 ),	/* 64 */
/* 788 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x8,		/* 8 */
/* 790 */	0xa,		/* 10 */
			0x7,		/* Ext Flags:  new corr desc, clt corr check, srv corr check, */
/* 792 */	NdrFcShort( 0x6 ),	/* 6 */
/* 794 */	NdrFcShort( 0x6 ),	/* 6 */
/* 796 */	NdrFcShort( 0x0 ),	/* 0 */
/* 798 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ServerName */

/* 800 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 802 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 804 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter ClientName */

/* 806 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 808 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 810 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter UserName */

/* 812 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 814 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 816 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter InfoStruct */

/* 818 */	NdrFcShort( 0x11b ),	/* Flags:  must size, must free, in, out, simple ref, */
/* 820 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 822 */	NdrFcShort( 0x4b0 ),	/* Type Offset=1200 */

	/* Parameter PreferedMaximumLength */

/* 824 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 826 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 828 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter TotalEntries */

/* 830 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 832 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 834 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ResumeHandle */

/* 836 */	NdrFcShort( 0x1a ),	/* Flags:  must free, in, out, */
/* 838 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 840 */	NdrFcShort( 0xb4 ),	/* Type Offset=180 */

	/* Return value */

/* 842 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 844 */	NdrFcShort( 0x38 ),	/* ia64 Stack size/offset = 56 */
/* 846 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure NetrSessionDel */

/* 848 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 850 */	NdrFcLong( 0x0 ),	/* 0 */
/* 854 */	NdrFcShort( 0xd ),	/* 13 */
/* 856 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 858 */	0x31,		/* FC_BIND_GENERIC */
			0x8,		/* 8 */
/* 860 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 862 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 864 */	NdrFcShort( 0x0 ),	/* 0 */
/* 866 */	NdrFcShort( 0x8 ),	/* 8 */
/* 868 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 870 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 872 */	NdrFcShort( 0x0 ),	/* 0 */
/* 874 */	NdrFcShort( 0x0 ),	/* 0 */
/* 876 */	NdrFcShort( 0x0 ),	/* 0 */
/* 878 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ServerName */

/* 880 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 882 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 884 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter ClientName */

/* 886 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 888 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 890 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter UserName */

/* 892 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 894 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 896 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Return value */

/* 898 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 900 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 902 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure NetrShareAdd */

/* 904 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 906 */	NdrFcLong( 0x0 ),	/* 0 */
/* 910 */	NdrFcShort( 0xe ),	/* 14 */
/* 912 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 914 */	0x31,		/* FC_BIND_GENERIC */
			0x8,		/* 8 */
/* 916 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 918 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 920 */	NdrFcShort( 0x24 ),	/* 36 */
/* 922 */	NdrFcShort( 0x24 ),	/* 36 */
/* 924 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 926 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 928 */	NdrFcShort( 0x0 ),	/* 0 */
/* 930 */	NdrFcShort( 0x3 ),	/* 3 */
/* 932 */	NdrFcShort( 0x0 ),	/* 0 */
/* 934 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ServerName */

/* 936 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 938 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 940 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter Level */

/* 942 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 944 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 946 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter InfoStruct */

/* 948 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 950 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 952 */	NdrFcShort( 0x4c4 ),	/* Type Offset=1220 */

	/* Parameter ParmErr */

/* 954 */	NdrFcShort( 0x1a ),	/* Flags:  must free, in, out, */
/* 956 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 958 */	NdrFcShort( 0xb4 ),	/* Type Offset=180 */

	/* Return value */

/* 960 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 962 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 964 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure NetrShareEnum */

/* 966 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 968 */	NdrFcLong( 0x0 ),	/* 0 */
/* 972 */	NdrFcShort( 0xf ),	/* 15 */
/* 974 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 976 */	0x31,		/* FC_BIND_GENERIC */
			0x8,		/* 8 */
/* 978 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 980 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 982 */	NdrFcShort( 0x24 ),	/* 36 */
/* 984 */	NdrFcShort( 0x40 ),	/* 64 */
/* 986 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x6,		/* 6 */
/* 988 */	0xa,		/* 10 */
			0x7,		/* Ext Flags:  new corr desc, clt corr check, srv corr check, */
/* 990 */	NdrFcShort( 0x7 ),	/* 7 */
/* 992 */	NdrFcShort( 0x7 ),	/* 7 */
/* 994 */	NdrFcShort( 0x0 ),	/* 0 */
/* 996 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ServerName */

/* 998 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1000 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 1002 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter InfoStruct */

/* 1004 */	NdrFcShort( 0x11b ),	/* Flags:  must size, must free, in, out, simple ref, */
/* 1006 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1008 */	NdrFcShort( 0x6ea ),	/* Type Offset=1770 */

	/* Parameter PreferedMaximumLength */

/* 1010 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1012 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1014 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter TotalEntries */

/* 1016 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1018 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1020 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ResumeHandle */

/* 1022 */	NdrFcShort( 0x1a ),	/* Flags:  must free, in, out, */
/* 1024 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 1026 */	NdrFcShort( 0xb4 ),	/* Type Offset=180 */

	/* Return value */

/* 1028 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1030 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 1032 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure NetrShareGetInfo */

/* 1034 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1036 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1040 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1042 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 1044 */	0x31,		/* FC_BIND_GENERIC */
			0x8,		/* 8 */
/* 1046 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 1048 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 1050 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1052 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1054 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 1056 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 1058 */	NdrFcShort( 0x3 ),	/* 3 */
/* 1060 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1062 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1064 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ServerName */

/* 1066 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1068 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 1070 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter NetName */

/* 1072 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 1074 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1076 */	NdrFcShort( 0xba ),	/* Type Offset=186 */

	/* Parameter Level */

/* 1078 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1080 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1082 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter InfoStruct */

/* 1084 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 1086 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1088 */	NdrFcShort( 0x6fe ),	/* Type Offset=1790 */

	/* Return value */

/* 1090 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1092 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 1094 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure NetrShareSetInfo */

/* 1096 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1098 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1102 */	NdrFcShort( 0x11 ),	/* 17 */
/* 1104 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 1106 */	0x31,		/* FC_BIND_GENERIC */
			0x8,		/* 8 */
/* 1108 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 1110 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 1112 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1114 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1116 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x6,		/* 6 */
/* 1118 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 1120 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1122 */	NdrFcShort( 0x3 ),	/* 3 */
/* 1124 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1126 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ServerName */

/* 1128 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1130 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 1132 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter NetName */

/* 1134 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 1136 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1138 */	NdrFcShort( 0xba ),	/* Type Offset=186 */

	/* Parameter Level */

/* 1140 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1142 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1144 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ShareInfo */

/* 1146 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 1148 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1150 */	NdrFcShort( 0x70c ),	/* Type Offset=1804 */

	/* Parameter ParmErr */

/* 1152 */	NdrFcShort( 0x1a ),	/* Flags:  must free, in, out, */
/* 1154 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 1156 */	NdrFcShort( 0xb4 ),	/* Type Offset=180 */

	/* Return value */

/* 1158 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1160 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 1162 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure NetrShareDel */

/* 1164 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1166 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1170 */	NdrFcShort( 0x12 ),	/* 18 */
/* 1172 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 1174 */	0x31,		/* FC_BIND_GENERIC */
			0x8,		/* 8 */
/* 1176 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 1178 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 1180 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1182 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1184 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 1186 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1188 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1190 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1192 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1194 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ServerName */

/* 1196 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1198 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 1200 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter NetName */

/* 1202 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 1204 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1206 */	NdrFcShort( 0xba ),	/* Type Offset=186 */

	/* Parameter Reserved */

/* 1208 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1210 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1212 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1214 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1216 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1218 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure NetrShareDelSticky */

/* 1220 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1222 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1226 */	NdrFcShort( 0x13 ),	/* 19 */
/* 1228 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 1230 */	0x31,		/* FC_BIND_GENERIC */
			0x8,		/* 8 */
/* 1232 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 1234 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 1236 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1238 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1240 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 1242 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1244 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1246 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1248 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1250 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ServerName */

/* 1252 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1254 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 1256 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter NetName */

/* 1258 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 1260 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1262 */	NdrFcShort( 0xba ),	/* Type Offset=186 */

	/* Parameter Reserved */

/* 1264 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1266 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1268 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1270 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1272 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1274 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure NetrShareCheck */

/* 1276 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1278 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1282 */	NdrFcShort( 0x14 ),	/* 20 */
/* 1284 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 1286 */	0x31,		/* FC_BIND_GENERIC */
			0x8,		/* 8 */
/* 1288 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 1290 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 1292 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1294 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1296 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 1298 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1300 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1302 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1304 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1306 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ServerName */

/* 1308 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1310 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 1312 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter Device */

/* 1314 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 1316 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1318 */	NdrFcShort( 0xba ),	/* Type Offset=186 */

	/* Parameter Type */

/* 1320 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1322 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1324 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1326 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1328 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1330 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure NetrServerGetInfo */

/* 1332 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1334 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1338 */	NdrFcShort( 0x15 ),	/* 21 */
/* 1340 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 1342 */	0x31,		/* FC_BIND_GENERIC */
			0x8,		/* 8 */
/* 1344 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 1346 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 1348 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1350 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1352 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 1354 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 1356 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1358 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1360 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1362 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ServerName */

/* 1364 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1366 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 1368 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter Level */

/* 1370 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1372 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1374 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter InfoStruct */

/* 1376 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 1378 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1380 */	NdrFcShort( 0x71a ),	/* Type Offset=1818 */

	/* Return value */

/* 1382 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1384 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1386 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure NetrServerSetInfo */

/* 1388 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1390 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1394 */	NdrFcShort( 0x16 ),	/* 22 */
/* 1396 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 1398 */	0x31,		/* FC_BIND_GENERIC */
			0x8,		/* 8 */
/* 1400 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 1402 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 1404 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1406 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1408 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 1410 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 1412 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1414 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1416 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1418 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ServerName */

/* 1420 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1422 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 1424 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter Level */

/* 1426 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1428 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1430 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ServerInfo */

/* 1432 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 1434 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1436 */	NdrFcShort( 0xa1a ),	/* Type Offset=2586 */

	/* Parameter ParmErr */

/* 1438 */	NdrFcShort( 0x1a ),	/* Flags:  must free, in, out, */
/* 1440 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1442 */	NdrFcShort( 0xb4 ),	/* Type Offset=180 */

	/* Return value */

/* 1444 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1446 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 1448 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure NetrServerDiskEnum */

/* 1450 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1452 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1456 */	NdrFcShort( 0x17 ),	/* 23 */
/* 1458 */	NdrFcShort( 0x38 ),	/* ia64 Stack size/offset = 56 */
/* 1460 */	0x31,		/* FC_BIND_GENERIC */
			0x8,		/* 8 */
/* 1462 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 1464 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 1466 */	NdrFcShort( 0x2c ),	/* 44 */
/* 1468 */	NdrFcShort( 0x40 ),	/* 64 */
/* 1470 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x7,		/* 7 */
/* 1472 */	0xa,		/* 10 */
			0x7,		/* Ext Flags:  new corr desc, clt corr check, srv corr check, */
/* 1474 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1476 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1478 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1480 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ServerName */

/* 1482 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1484 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 1486 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter Level */

/* 1488 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1490 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1492 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter DiskInfoStruct */

/* 1494 */	NdrFcShort( 0x11b ),	/* Flags:  must size, must free, in, out, simple ref, */
/* 1496 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1498 */	NdrFcShort( 0xa50 ),	/* Type Offset=2640 */

	/* Parameter PreferredMaximumLength */

/* 1500 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1502 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1504 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter TotalEntries */

/* 1506 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1508 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 1510 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ResumeHandle */

/* 1512 */	NdrFcShort( 0x1a ),	/* Flags:  must free, in, out, */
/* 1514 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 1516 */	NdrFcShort( 0xb4 ),	/* Type Offset=180 */

	/* Return value */

/* 1518 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1520 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 1522 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure NetrServerStatisticsGet */

/* 1524 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1526 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1530 */	NdrFcShort( 0x18 ),	/* 24 */
/* 1532 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 1534 */	0x31,		/* FC_BIND_GENERIC */
			0x8,		/* 8 */
/* 1536 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 1538 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 1540 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1542 */	NdrFcShort( 0x84 ),	/* 132 */
/* 1544 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x6,		/* 6 */
/* 1546 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1548 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1550 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1552 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1554 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ServerName */

/* 1556 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1558 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 1560 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter Service */

/* 1562 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1564 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1566 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter Level */

/* 1568 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1570 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1572 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter Options */

/* 1574 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1576 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1578 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter InfoStruct */

/* 1580 */	NdrFcShort( 0x2012 ),	/* Flags:  must free, out, srv alloc size=8 */
/* 1582 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 1584 */	NdrFcShort( 0xa60 ),	/* Type Offset=2656 */

	/* Return value */

/* 1586 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1588 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 1590 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure NetrServerTransportAdd */

/* 1592 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1594 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1598 */	NdrFcShort( 0x19 ),	/* 25 */
/* 1600 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 1602 */	0x31,		/* FC_BIND_GENERIC */
			0x8,		/* 8 */
/* 1604 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 1606 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 1608 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1610 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1612 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 1614 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 1616 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1618 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1620 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1622 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ServerName */

/* 1624 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1626 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 1628 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter Level */

/* 1630 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1632 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1634 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter Buffer */

/* 1636 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1638 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1640 */	NdrFcShort( 0xa7e ),	/* Type Offset=2686 */

	/* Return value */

/* 1642 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1644 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1646 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure NetrServerTransportEnum */

/* 1648 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1650 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1654 */	NdrFcShort( 0x1a ),	/* 26 */
/* 1656 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 1658 */	0x31,		/* FC_BIND_GENERIC */
			0x8,		/* 8 */
/* 1660 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 1662 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 1664 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1666 */	NdrFcShort( 0x40 ),	/* 64 */
/* 1668 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x6,		/* 6 */
/* 1670 */	0xa,		/* 10 */
			0x7,		/* Ext Flags:  new corr desc, clt corr check, srv corr check, */
/* 1672 */	NdrFcShort( 0x9 ),	/* 9 */
/* 1674 */	NdrFcShort( 0x9 ),	/* 9 */
/* 1676 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1678 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ServerName */

/* 1680 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1682 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 1684 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter InfoStruct */

/* 1686 */	NdrFcShort( 0x11b ),	/* Flags:  must size, must free, in, out, simple ref, */
/* 1688 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1690 */	NdrFcShort( 0xbf2 ),	/* Type Offset=3058 */

	/* Parameter PreferedMaximumLength */

/* 1692 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1694 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1696 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter TotalEntries */

/* 1698 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1700 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1702 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ResumeHandle */

/* 1704 */	NdrFcShort( 0x1a ),	/* Flags:  must free, in, out, */
/* 1706 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 1708 */	NdrFcShort( 0xb4 ),	/* Type Offset=180 */

	/* Return value */

/* 1710 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1712 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 1714 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure NetrServerTransportDel */

/* 1716 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1718 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1722 */	NdrFcShort( 0x1b ),	/* 27 */
/* 1724 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 1726 */	0x31,		/* FC_BIND_GENERIC */
			0x8,		/* 8 */
/* 1728 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 1730 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 1732 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1734 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1736 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 1738 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 1740 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1742 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1744 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1746 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ServerName */

/* 1748 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1750 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 1752 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter Level */

/* 1754 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1756 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1758 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter Buffer */

/* 1760 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1762 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1764 */	NdrFcShort( 0xa7e ),	/* Type Offset=2686 */

	/* Return value */

/* 1766 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1768 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1770 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure NetrRemoteTOD */

/* 1772 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1774 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1778 */	NdrFcShort( 0x1c ),	/* 28 */
/* 1780 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1782 */	0x31,		/* FC_BIND_GENERIC */
			0x8,		/* 8 */
/* 1784 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 1786 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 1788 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1790 */	NdrFcShort( 0x70 ),	/* 112 */
/* 1792 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 1794 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1796 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1798 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1800 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1802 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ServerName */

/* 1804 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1806 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 1808 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter BufferPtr */

/* 1810 */	NdrFcShort( 0x2012 ),	/* Flags:  must free, out, srv alloc size=8 */
/* 1812 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1814 */	NdrFcShort( 0xc02 ),	/* Type Offset=3074 */

	/* Return value */

/* 1816 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1818 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1820 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure I_NetrServerSetServiceBits */

/* 1822 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1824 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1828 */	NdrFcShort( 0x1d ),	/* 29 */
/* 1830 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 1832 */	0x31,		/* FC_BIND_GENERIC */
			0x8,		/* 8 */
/* 1834 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 1836 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 1838 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1840 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1842 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 1844 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1846 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1848 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1850 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1852 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ServerName */

/* 1854 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1856 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 1858 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter TransportName */

/* 1860 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1862 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1864 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter ServiceBits */

/* 1866 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1868 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1870 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter UpdateImmediately */

/* 1872 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1874 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1876 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1878 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1880 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 1882 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure NetprPathType */

/* 1884 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1886 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1890 */	NdrFcShort( 0x1e ),	/* 30 */
/* 1892 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 1894 */	0x31,		/* FC_BIND_GENERIC */
			0x8,		/* 8 */
/* 1896 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 1898 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 1900 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1902 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1904 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 1906 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1908 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1910 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1912 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1914 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ServerName */

/* 1916 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1918 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 1920 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter PathName */

/* 1922 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 1924 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1926 */	NdrFcShort( 0xba ),	/* Type Offset=186 */

	/* Parameter PathType */

/* 1928 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1930 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1932 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter Flags */

/* 1934 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1936 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1938 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1940 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1942 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 1944 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure NetprPathCanonicalize */

/* 1946 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1948 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1952 */	NdrFcShort( 0x1f ),	/* 31 */
/* 1954 */	NdrFcShort( 0x40 ),	/* ia64 Stack size/offset = 64 */
/* 1956 */	0x31,		/* FC_BIND_GENERIC */
			0x8,		/* 8 */
/* 1958 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 1960 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 1962 */	NdrFcShort( 0x2c ),	/* 44 */
/* 1964 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1966 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x8,		/* 8 */
/* 1968 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 1970 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1972 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1974 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1976 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ServerName */

/* 1978 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1980 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 1982 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter PathName */

/* 1984 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 1986 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1988 */	NdrFcShort( 0xba ),	/* Type Offset=186 */

	/* Parameter Outbuf */

/* 1990 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 1992 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1994 */	NdrFcShort( 0xc20 ),	/* Type Offset=3104 */

	/* Parameter OutbufLen */

/* 1996 */	NdrFcShort( 0x88 ),	/* Flags:  in, by val, */
/* 1998 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2000 */	NdrFcShort( 0xc2c ),	/* 3116 */

	/* Parameter Prefix */

/* 2002 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2004 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 2006 */	NdrFcShort( 0xba ),	/* Type Offset=186 */

	/* Parameter PathType */

/* 2008 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 2010 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 2012 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter Flags */

/* 2014 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2016 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 2018 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2020 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2022 */	NdrFcShort( 0x38 ),	/* ia64 Stack size/offset = 56 */
/* 2024 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure NetprPathCompare */

/* 2026 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 2028 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2032 */	NdrFcShort( 0x20 ),	/* 32 */
/* 2034 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 2036 */	0x31,		/* FC_BIND_GENERIC */
			0x8,		/* 8 */
/* 2038 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 2040 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 2042 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2044 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2046 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x6,		/* 6 */
/* 2048 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2050 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2052 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2054 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2056 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ServerName */

/* 2058 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2060 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 2062 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter PathName1 */

/* 2064 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2066 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2068 */	NdrFcShort( 0xba ),	/* Type Offset=186 */

	/* Parameter PathName2 */

/* 2070 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2072 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2074 */	NdrFcShort( 0xba ),	/* Type Offset=186 */

	/* Parameter PathType */

/* 2076 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2078 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2080 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter Flags */

/* 2082 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2084 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 2086 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2088 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2090 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 2092 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure NetprNameValidate */

/* 2094 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 2096 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2100 */	NdrFcShort( 0x21 ),	/* 33 */
/* 2102 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 2104 */	0x31,		/* FC_BIND_GENERIC */
			0x8,		/* 8 */
/* 2106 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 2108 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 2110 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2112 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2114 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 2116 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2118 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2120 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2122 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2124 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ServerName */

/* 2126 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2128 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 2130 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter Name */

/* 2132 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2134 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2136 */	NdrFcShort( 0xba ),	/* Type Offset=186 */

	/* Parameter NameType */

/* 2138 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2140 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2142 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter Flags */

/* 2144 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2146 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2148 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2150 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2152 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 2154 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure NetprNameCanonicalize */

/* 2156 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 2158 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2162 */	NdrFcShort( 0x22 ),	/* 34 */
/* 2164 */	NdrFcShort( 0x38 ),	/* ia64 Stack size/offset = 56 */
/* 2166 */	0x31,		/* FC_BIND_GENERIC */
			0x8,		/* 8 */
/* 2168 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 2170 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 2172 */	NdrFcShort( 0x18 ),	/* 24 */
/* 2174 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2176 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x7,		/* 7 */
/* 2178 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 2180 */	NdrFcShort( 0x1 ),	/* 1 */
/* 2182 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2184 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2186 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ServerName */

/* 2188 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2190 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 2192 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter Name */

/* 2194 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2196 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2198 */	NdrFcShort( 0xba ),	/* Type Offset=186 */

	/* Parameter Outbuf */

/* 2200 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 2202 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2204 */	NdrFcShort( 0xc3e ),	/* Type Offset=3134 */

	/* Parameter OutbufLen */

/* 2206 */	NdrFcShort( 0x88 ),	/* Flags:  in, by val, */
/* 2208 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2210 */	NdrFcShort( 0xc4a ),	/* 3146 */

	/* Parameter NameType */

/* 2212 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2214 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 2216 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter Flags */

/* 2218 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2220 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 2222 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2224 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2226 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 2228 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure NetprNameCompare */

/* 2230 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 2232 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2236 */	NdrFcShort( 0x23 ),	/* 35 */
/* 2238 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 2240 */	0x31,		/* FC_BIND_GENERIC */
			0x8,		/* 8 */
/* 2242 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 2244 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 2246 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2248 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2250 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x6,		/* 6 */
/* 2252 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2254 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2256 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2258 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2260 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ServerName */

/* 2262 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2264 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 2266 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter Name1 */

/* 2268 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2270 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2272 */	NdrFcShort( 0xba ),	/* Type Offset=186 */

	/* Parameter Name2 */

/* 2274 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2276 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2278 */	NdrFcShort( 0xba ),	/* Type Offset=186 */

	/* Parameter NameType */

/* 2280 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2282 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2284 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter Flags */

/* 2286 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2288 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 2290 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2292 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2294 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 2296 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure NetrShareEnumSticky */

/* 2298 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 2300 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2304 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2306 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 2308 */	0x31,		/* FC_BIND_GENERIC */
			0x8,		/* 8 */
/* 2310 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 2312 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 2314 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2316 */	NdrFcShort( 0x40 ),	/* 64 */
/* 2318 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x6,		/* 6 */
/* 2320 */	0xa,		/* 10 */
			0x7,		/* Ext Flags:  new corr desc, clt corr check, srv corr check, */
/* 2322 */	NdrFcShort( 0x7 ),	/* 7 */
/* 2324 */	NdrFcShort( 0x7 ),	/* 7 */
/* 2326 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2328 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ServerName */

/* 2330 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2332 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 2334 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter InfoStruct */

/* 2336 */	NdrFcShort( 0x11b ),	/* Flags:  must size, must free, in, out, simple ref, */
/* 2338 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2340 */	NdrFcShort( 0x6ea ),	/* Type Offset=1770 */

	/* Parameter PreferedMaximumLength */

/* 2342 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2344 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2346 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter TotalEntries */

/* 2348 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 2350 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2352 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ResumeHandle */

/* 2354 */	NdrFcShort( 0x1a ),	/* Flags:  must free, in, out, */
/* 2356 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 2358 */	NdrFcShort( 0xb4 ),	/* Type Offset=180 */

	/* Return value */

/* 2360 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2362 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 2364 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure NetrShareDelStart */

/* 2366 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 2368 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2372 */	NdrFcShort( 0x25 ),	/* 37 */
/* 2374 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 2376 */	0x31,		/* FC_BIND_GENERIC */
			0x8,		/* 8 */
/* 2378 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 2380 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 2382 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2384 */	NdrFcShort( 0x40 ),	/* 64 */
/* 2386 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 2388 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2390 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2392 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2394 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2396 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ServerName */

/* 2398 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2400 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 2402 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter NetName */

/* 2404 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2406 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2408 */	NdrFcShort( 0xba ),	/* Type Offset=186 */

	/* Parameter Reserved */

/* 2410 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2412 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2414 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ContextHandle */

/* 2416 */	NdrFcShort( 0x110 ),	/* Flags:  out, simple ref, */
/* 2418 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2420 */	NdrFcShort( 0xc58 ),	/* Type Offset=3160 */

	/* Return value */

/* 2422 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2424 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 2426 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure NetrShareDelCommit */

/* 2428 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 2430 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2434 */	NdrFcShort( 0x26 ),	/* 38 */
/* 2436 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2438 */	0x30,		/* FC_BIND_CONTEXT */
			0xe8,		/* Ctxt flags:  via ptr, in, out, strict, */
/* 2440 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 2442 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 2444 */	NdrFcShort( 0x38 ),	/* 56 */
/* 2446 */	NdrFcShort( 0x40 ),	/* 64 */
/* 2448 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 2450 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2452 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2454 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2456 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2458 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ContextHandle */

/* 2460 */	NdrFcShort( 0x118 ),	/* Flags:  in, out, simple ref, */
/* 2462 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 2464 */	NdrFcShort( 0xc60 ),	/* Type Offset=3168 */

	/* Return value */

/* 2466 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2468 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2470 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure NetrpGetFileSecurity */

/* 2472 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 2474 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2478 */	NdrFcShort( 0x27 ),	/* 39 */
/* 2480 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 2482 */	0x31,		/* FC_BIND_GENERIC */
			0x8,		/* 8 */
/* 2484 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 2486 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 2488 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2490 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2492 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x6,		/* 6 */
/* 2494 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 2496 */	NdrFcShort( 0x1 ),	/* 1 */
/* 2498 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2500 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2502 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ServerName */

/* 2504 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2506 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 2508 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter ShareName */

/* 2510 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2512 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2514 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter lpFileName */

/* 2516 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2518 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2520 */	NdrFcShort( 0xba ),	/* Type Offset=186 */

	/* Parameter RequestedInformation */

/* 2522 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2524 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2526 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter SecurityDescriptor */

/* 2528 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 2530 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 2532 */	NdrFcShort( 0xc64 ),	/* Type Offset=3172 */

	/* Return value */

/* 2534 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2536 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 2538 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure NetrpSetFileSecurity */

/* 2540 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 2542 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2546 */	NdrFcShort( 0x28 ),	/* 40 */
/* 2548 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 2550 */	0x31,		/* FC_BIND_GENERIC */
			0x8,		/* 8 */
/* 2552 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 2554 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 2556 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2558 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2560 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x6,		/* 6 */
/* 2562 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 2564 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2566 */	NdrFcShort( 0x1 ),	/* 1 */
/* 2568 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2570 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ServerName */

/* 2572 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2574 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 2576 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter ShareName */

/* 2578 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2580 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2582 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter lpFileName */

/* 2584 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2586 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2588 */	NdrFcShort( 0xba ),	/* Type Offset=186 */

	/* Parameter SecurityInformation */

/* 2590 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2592 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2594 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter SecurityDescriptor */

/* 2596 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2598 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 2600 */	NdrFcShort( 0xc6c ),	/* Type Offset=3180 */

	/* Return value */

/* 2602 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2604 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 2606 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure NetrServerTransportAddEx */

/* 2608 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 2610 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2614 */	NdrFcShort( 0x29 ),	/* 41 */
/* 2616 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 2618 */	0x31,		/* FC_BIND_GENERIC */
			0x8,		/* 8 */
/* 2620 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 2622 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 2624 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2626 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2628 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 2630 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 2632 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2634 */	NdrFcShort( 0x5 ),	/* 5 */
/* 2636 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2638 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ServerName */

/* 2640 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2642 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 2644 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter Level */

/* 2646 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2648 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2650 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter Buffer */

/* 2652 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2654 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2656 */	NdrFcShort( 0xc84 ),	/* Type Offset=3204 */

	/* Return value */

/* 2658 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2660 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2662 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure I_NetrServerSetServiceBitsEx */

/* 2664 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 2666 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2670 */	NdrFcShort( 0x2a ),	/* 42 */
/* 2672 */	NdrFcShort( 0x38 ),	/* ia64 Stack size/offset = 56 */
/* 2674 */	0x31,		/* FC_BIND_GENERIC */
			0x8,		/* 8 */
/* 2676 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 2678 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 2680 */	NdrFcShort( 0x18 ),	/* 24 */
/* 2682 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2684 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x7,		/* 7 */
/* 2686 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2688 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2690 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2692 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2694 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ServerName */

/* 2696 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2698 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 2700 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter EmulatedServerName */

/* 2702 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2704 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2706 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter TransportName */

/* 2708 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2710 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2712 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter ServiceBitsOfInterest */

/* 2714 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2716 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2718 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ServiceBits */

/* 2720 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2722 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 2724 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter UpdateImmediately */

/* 2726 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2728 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 2730 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2732 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2734 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 2736 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure NetrDfsGetVersion */

/* 2738 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 2740 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2744 */	NdrFcShort( 0x2b ),	/* 43 */
/* 2746 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2748 */	0x31,		/* FC_BIND_GENERIC */
			0x8,		/* 8 */
/* 2750 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 2752 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 2754 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2756 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2758 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 2760 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2762 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2764 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2766 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2768 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ServerName */

/* 2770 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2772 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 2774 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter Version */

/* 2776 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 2778 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2780 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2782 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2784 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2786 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure NetrDfsCreateLocalPartition */

/* 2788 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 2790 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2794 */	NdrFcShort( 0x2c ),	/* 44 */
/* 2796 */	NdrFcShort( 0x40 ),	/* ia64 Stack size/offset = 64 */
/* 2798 */	0x31,		/* FC_BIND_GENERIC */
			0x8,		/* 8 */
/* 2800 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 2802 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 2804 */	NdrFcShort( 0x4c ),	/* 76 */
/* 2806 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2808 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x8,		/* 8 */
/* 2810 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 2812 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2814 */	NdrFcShort( 0x1 ),	/* 1 */
/* 2816 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2818 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ServerName */

/* 2820 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2822 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 2824 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter ShareName */

/* 2826 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2828 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2830 */	NdrFcShort( 0xba ),	/* Type Offset=186 */

	/* Parameter EntryUid */

/* 2832 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 2834 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2836 */	NdrFcShort( 0xcb6 ),	/* Type Offset=3254 */

	/* Parameter EntryPrefix */

/* 2838 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2840 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2842 */	NdrFcShort( 0xba ),	/* Type Offset=186 */

	/* Parameter ShortName */

/* 2844 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2846 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 2848 */	NdrFcShort( 0xba ),	/* Type Offset=186 */

	/* Parameter RelationInfo */

/* 2850 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2852 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 2854 */	NdrFcShort( 0xcee ),	/* Type Offset=3310 */

	/* Parameter Force */

/* 2856 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2858 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 2860 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2862 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2864 */	NdrFcShort( 0x38 ),	/* ia64 Stack size/offset = 56 */
/* 2866 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure NetrDfsDeleteLocalPartition */

/* 2868 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 2870 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2874 */	NdrFcShort( 0x2d ),	/* 45 */
/* 2876 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 2878 */	0x31,		/* FC_BIND_GENERIC */
			0x8,		/* 8 */
/* 2880 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 2882 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 2884 */	NdrFcShort( 0x44 ),	/* 68 */
/* 2886 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2888 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 2890 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2892 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2894 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2896 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2898 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ServerName */

/* 2900 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2902 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 2904 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter Uid */

/* 2906 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 2908 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2910 */	NdrFcShort( 0xcb6 ),	/* Type Offset=3254 */

	/* Parameter Prefix */

/* 2912 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2914 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2916 */	NdrFcShort( 0xba ),	/* Type Offset=186 */

	/* Return value */

/* 2918 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2920 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2922 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure NetrDfsSetLocalVolumeState */

/* 2924 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 2926 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2930 */	NdrFcShort( 0x2e ),	/* 46 */
/* 2932 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 2934 */	0x31,		/* FC_BIND_GENERIC */
			0x8,		/* 8 */
/* 2936 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 2938 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 2940 */	NdrFcShort( 0x4c ),	/* 76 */
/* 2942 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2944 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 2946 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2948 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2950 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2952 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2954 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ServerName */

/* 2956 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2958 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 2960 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter Uid */

/* 2962 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 2964 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2966 */	NdrFcShort( 0xcb6 ),	/* Type Offset=3254 */

	/* Parameter Prefix */

/* 2968 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2970 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2972 */	NdrFcShort( 0xba ),	/* Type Offset=186 */

	/* Parameter State */

/* 2974 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2976 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2978 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2980 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2982 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 2984 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure NetrDfsSetServerInfo */

/* 2986 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 2988 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2992 */	NdrFcShort( 0x2f ),	/* 47 */
/* 2994 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 2996 */	0x31,		/* FC_BIND_GENERIC */
			0x8,		/* 8 */
/* 2998 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 3000 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 3002 */	NdrFcShort( 0x44 ),	/* 68 */
/* 3004 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3006 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 3008 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3010 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3012 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3014 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3016 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ServerName */

/* 3018 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 3020 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 3022 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter Uid */

/* 3024 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 3026 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 3028 */	NdrFcShort( 0xcb6 ),	/* Type Offset=3254 */

	/* Parameter Prefix */

/* 3030 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 3032 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 3034 */	NdrFcShort( 0xba ),	/* Type Offset=186 */

	/* Return value */

/* 3036 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3038 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 3040 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure NetrDfsCreateExitPoint */

/* 3042 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 3044 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3048 */	NdrFcShort( 0x30 ),	/* 48 */
/* 3050 */	NdrFcShort( 0x38 ),	/* ia64 Stack size/offset = 56 */
/* 3052 */	0x31,		/* FC_BIND_GENERIC */
			0x8,		/* 8 */
/* 3054 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 3056 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 3058 */	NdrFcShort( 0x54 ),	/* 84 */
/* 3060 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3062 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x7,		/* 7 */
/* 3064 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 3066 */	NdrFcShort( 0x1 ),	/* 1 */
/* 3068 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3070 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3072 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ServerName */

/* 3074 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 3076 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 3078 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter Uid */

/* 3080 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 3082 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 3084 */	NdrFcShort( 0xcb6 ),	/* Type Offset=3254 */

	/* Parameter Prefix */

/* 3086 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 3088 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 3090 */	NdrFcShort( 0xba ),	/* Type Offset=186 */

	/* Parameter Type */

/* 3092 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3094 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 3096 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ShortPrefixLen */

/* 3098 */	NdrFcShort( 0x88 ),	/* Flags:  in, by val, */
/* 3100 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 3102 */	NdrFcShort( 0xcfe ),	/* 3326 */

	/* Parameter ShortPrefix */

/* 3104 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 3106 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 3108 */	NdrFcShort( 0xd0c ),	/* Type Offset=3340 */

	/* Return value */

/* 3110 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3112 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 3114 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure NetrDfsDeleteExitPoint */

/* 3116 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 3118 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3122 */	NdrFcShort( 0x31 ),	/* 49 */
/* 3124 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 3126 */	0x31,		/* FC_BIND_GENERIC */
			0x8,		/* 8 */
/* 3128 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 3130 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 3132 */	NdrFcShort( 0x4c ),	/* 76 */
/* 3134 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3136 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 3138 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3140 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3142 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3144 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3146 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ServerName */

/* 3148 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 3150 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 3152 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter Uid */

/* 3154 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 3156 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 3158 */	NdrFcShort( 0xcb6 ),	/* Type Offset=3254 */

	/* Parameter Prefix */

/* 3160 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 3162 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 3164 */	NdrFcShort( 0xba ),	/* Type Offset=186 */

	/* Parameter Type */

/* 3166 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3168 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 3170 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3172 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3174 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 3176 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure NetrDfsModifyPrefix */

/* 3178 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 3180 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3184 */	NdrFcShort( 0x32 ),	/* 50 */
/* 3186 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 3188 */	0x31,		/* FC_BIND_GENERIC */
			0x8,		/* 8 */
/* 3190 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 3192 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 3194 */	NdrFcShort( 0x44 ),	/* 68 */
/* 3196 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3198 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 3200 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3202 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3204 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3206 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3208 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ServerName */

/* 3210 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 3212 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 3214 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter Uid */

/* 3216 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 3218 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 3220 */	NdrFcShort( 0xcb6 ),	/* Type Offset=3254 */

	/* Parameter Prefix */

/* 3222 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 3224 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 3226 */	NdrFcShort( 0xba ),	/* Type Offset=186 */

	/* Return value */

/* 3228 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3230 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 3232 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure NetrDfsFixLocalVolume */

/* 3234 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 3236 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3240 */	NdrFcShort( 0x33 ),	/* 51 */
/* 3242 */	NdrFcShort( 0x50 ),	/* ia64 Stack size/offset = 80 */
/* 3244 */	0x31,		/* FC_BIND_GENERIC */
			0x8,		/* 8 */
/* 3246 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 3248 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 3250 */	NdrFcShort( 0x5c ),	/* 92 */
/* 3252 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3254 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0xa,		/* 10 */
/* 3256 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 3258 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3260 */	NdrFcShort( 0x1 ),	/* 1 */
/* 3262 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3264 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ServerName */

/* 3266 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 3268 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 3270 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter VolumeName */

/* 3272 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 3274 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 3276 */	NdrFcShort( 0xba ),	/* Type Offset=186 */

	/* Parameter EntryType */

/* 3278 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3280 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 3282 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ServiceType */

/* 3284 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3286 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 3288 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter StgId */

/* 3290 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 3292 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 3294 */	NdrFcShort( 0xba ),	/* Type Offset=186 */

	/* Parameter EntryUid */

/* 3296 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 3298 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 3300 */	NdrFcShort( 0xcb6 ),	/* Type Offset=3254 */

	/* Parameter EntryPrefix */

/* 3302 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 3304 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 3306 */	NdrFcShort( 0xba ),	/* Type Offset=186 */

	/* Parameter RelationInfo */

/* 3308 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 3310 */	NdrFcShort( 0x38 ),	/* ia64 Stack size/offset = 56 */
/* 3312 */	NdrFcShort( 0xcee ),	/* Type Offset=3310 */

	/* Parameter CreateDisposition */

/* 3314 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3316 */	NdrFcShort( 0x40 ),	/* ia64 Stack size/offset = 64 */
/* 3318 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3320 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3322 */	NdrFcShort( 0x48 ),	/* ia64 Stack size/offset = 72 */
/* 3324 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure NetrDfsManagerReportSiteInfo */

/* 3326 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 3328 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3332 */	NdrFcShort( 0x34 ),	/* 52 */
/* 3334 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 3336 */	0x31,		/* FC_BIND_GENERIC */
			0x8,		/* 8 */
/* 3338 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 3340 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 3342 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3344 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3346 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 3348 */	0xa,		/* 10 */
			0x7,		/* Ext Flags:  new corr desc, clt corr check, srv corr check, */
/* 3350 */	NdrFcShort( 0x1 ),	/* 1 */
/* 3352 */	NdrFcShort( 0x1 ),	/* 1 */
/* 3354 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3356 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ServerName */

/* 3358 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 3360 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 3362 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter ppSiteInfo */

/* 3364 */	NdrFcShort( 0x201b ),	/* Flags:  must size, must free, in, out, srv alloc size=8 */
/* 3366 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 3368 */	NdrFcShort( 0xd18 ),	/* Type Offset=3352 */

	/* Return value */

/* 3370 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3372 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 3374 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure NetrServerTransportDelEx */

/* 3376 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 3378 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3382 */	NdrFcShort( 0x35 ),	/* 53 */
/* 3384 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 3386 */	0x31,		/* FC_BIND_GENERIC */
			0x8,		/* 8 */
/* 3388 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 3390 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 3392 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3394 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3396 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 3398 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 3400 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3402 */	NdrFcShort( 0x5 ),	/* 5 */
/* 3404 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3406 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ServerName */

/* 3408 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 3410 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 3412 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter Level */

/* 3414 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3416 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 3418 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter Buffer */

/* 3420 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 3422 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 3424 */	NdrFcShort( 0xd56 ),	/* Type Offset=3414 */

	/* Return value */

/* 3426 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3428 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 3430 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

			0x0
        }
    };

static const MIDL_TYPE_FORMAT_STRING __MIDL_TypeFormatString =
    {
        0,
        {
			NdrFcShort( 0x0 ),	/* 0 */
/*  2 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/*  4 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/*  6 */	
			0x11, 0x0,	/* FC_RP */
/*  8 */	NdrFcShort( 0x98 ),	/* Offset= 152 (160) */
/* 10 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0x9,		/* FC_ULONG */
/* 12 */	0x9,		/* Corr desc: FC_ULONG */
			0x0,		/*  */
/* 14 */	NdrFcShort( 0xfff8 ),	/* -8 */
/* 16 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 18 */	NdrFcShort( 0x2 ),	/* Offset= 2 (20) */
/* 20 */	NdrFcShort( 0x8 ),	/* 8 */
/* 22 */	NdrFcShort( 0x3002 ),	/* 12290 */
/* 24 */	NdrFcLong( 0x0 ),	/* 0 */
/* 28 */	NdrFcShort( 0xa ),	/* Offset= 10 (38) */
/* 30 */	NdrFcLong( 0x1 ),	/* 1 */
/* 34 */	NdrFcShort( 0x3c ),	/* Offset= 60 (94) */
/* 36 */	NdrFcShort( 0x0 ),	/* Offset= 0 (36) */
/* 38 */	
			0x12, 0x0,	/* FC_UP */
/* 40 */	NdrFcShort( 0x26 ),	/* Offset= 38 (78) */
/* 42 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 44 */	NdrFcShort( 0x8 ),	/* 8 */
/* 46 */	NdrFcShort( 0x0 ),	/* 0 */
/* 48 */	NdrFcShort( 0x4 ),	/* Offset= 4 (52) */
/* 50 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 52 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 54 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 56 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 58 */	NdrFcShort( 0x0 ),	/* 0 */
/* 60 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 62 */	NdrFcShort( 0x0 ),	/* 0 */
/* 64 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 66 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 70 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 72 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 74 */	NdrFcShort( 0xffe0 ),	/* Offset= -32 (42) */
/* 76 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 78 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 80 */	NdrFcShort( 0x10 ),	/* 16 */
/* 82 */	NdrFcShort( 0x0 ),	/* 0 */
/* 84 */	NdrFcShort( 0x6 ),	/* Offset= 6 (90) */
/* 86 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 88 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 90 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 92 */	NdrFcShort( 0xffdc ),	/* Offset= -36 (56) */
/* 94 */	
			0x12, 0x0,	/* FC_UP */
/* 96 */	NdrFcShort( 0x30 ),	/* Offset= 48 (144) */
/* 98 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 100 */	NdrFcShort( 0x20 ),	/* 32 */
/* 102 */	NdrFcShort( 0x0 ),	/* 0 */
/* 104 */	NdrFcShort( 0xa ),	/* Offset= 10 (114) */
/* 106 */	0x36,		/* FC_POINTER */
			0x8,		/* FC_LONG */
/* 108 */	0x40,		/* FC_STRUCTPAD4 */
			0x36,		/* FC_POINTER */
/* 110 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 112 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 114 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 116 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 118 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 120 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 122 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 124 */	NdrFcShort( 0x0 ),	/* 0 */
/* 126 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 128 */	NdrFcShort( 0x0 ),	/* 0 */
/* 130 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 132 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 136 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 138 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 140 */	NdrFcShort( 0xffd6 ),	/* Offset= -42 (98) */
/* 142 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 144 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 146 */	NdrFcShort( 0x10 ),	/* 16 */
/* 148 */	NdrFcShort( 0x0 ),	/* 0 */
/* 150 */	NdrFcShort( 0x6 ),	/* Offset= 6 (156) */
/* 152 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 154 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 156 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 158 */	NdrFcShort( 0xffdc ),	/* Offset= -36 (122) */
/* 160 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 162 */	NdrFcShort( 0x10 ),	/* 16 */
/* 164 */	NdrFcShort( 0x0 ),	/* 0 */
/* 166 */	NdrFcShort( 0x0 ),	/* Offset= 0 (166) */
/* 168 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 170 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 172 */	NdrFcShort( 0xff5e ),	/* Offset= -162 (10) */
/* 174 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 176 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 178 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 180 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 182 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 184 */	
			0x11, 0x8,	/* FC_RP [simple_pointer] */
/* 186 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 188 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 190 */	NdrFcShort( 0x2 ),	/* Offset= 2 (192) */
/* 192 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0x9,		/* FC_ULONG */
/* 194 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 196 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 198 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 200 */	NdrFcShort( 0x2 ),	/* Offset= 2 (202) */
/* 202 */	NdrFcShort( 0x8 ),	/* 8 */
/* 204 */	NdrFcShort( 0x3002 ),	/* 12290 */
/* 206 */	NdrFcLong( 0x0 ),	/* 0 */
/* 210 */	NdrFcShort( 0xa ),	/* Offset= 10 (220) */
/* 212 */	NdrFcLong( 0x1 ),	/* 1 */
/* 216 */	NdrFcShort( 0x8 ),	/* Offset= 8 (224) */
/* 218 */	NdrFcShort( 0x0 ),	/* Offset= 0 (218) */
/* 220 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 222 */	NdrFcShort( 0xff4c ),	/* Offset= -180 (42) */
/* 224 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 226 */	NdrFcShort( 0xff80 ),	/* Offset= -128 (98) */
/* 228 */	
			0x11, 0x0,	/* FC_RP */
/* 230 */	NdrFcShort( 0x98 ),	/* Offset= 152 (382) */
/* 232 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0x9,		/* FC_ULONG */
/* 234 */	0x9,		/* Corr desc: FC_ULONG */
			0x0,		/*  */
/* 236 */	NdrFcShort( 0xfff8 ),	/* -8 */
/* 238 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 240 */	NdrFcShort( 0x2 ),	/* Offset= 2 (242) */
/* 242 */	NdrFcShort( 0x8 ),	/* 8 */
/* 244 */	NdrFcShort( 0x3002 ),	/* 12290 */
/* 246 */	NdrFcLong( 0x0 ),	/* 0 */
/* 250 */	NdrFcShort( 0xa ),	/* Offset= 10 (260) */
/* 252 */	NdrFcLong( 0x1 ),	/* 1 */
/* 256 */	NdrFcShort( 0x3c ),	/* Offset= 60 (316) */
/* 258 */	NdrFcShort( 0x0 ),	/* Offset= 0 (258) */
/* 260 */	
			0x12, 0x0,	/* FC_UP */
/* 262 */	NdrFcShort( 0x26 ),	/* Offset= 38 (300) */
/* 264 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 266 */	NdrFcShort( 0x8 ),	/* 8 */
/* 268 */	NdrFcShort( 0x0 ),	/* 0 */
/* 270 */	NdrFcShort( 0x4 ),	/* Offset= 4 (274) */
/* 272 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 274 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 276 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 278 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 280 */	NdrFcShort( 0x0 ),	/* 0 */
/* 282 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 284 */	NdrFcShort( 0x0 ),	/* 0 */
/* 286 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 288 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 292 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 294 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 296 */	NdrFcShort( 0xffe0 ),	/* Offset= -32 (264) */
/* 298 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 300 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 302 */	NdrFcShort( 0x10 ),	/* 16 */
/* 304 */	NdrFcShort( 0x0 ),	/* 0 */
/* 306 */	NdrFcShort( 0x6 ),	/* Offset= 6 (312) */
/* 308 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 310 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 312 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 314 */	NdrFcShort( 0xffdc ),	/* Offset= -36 (278) */
/* 316 */	
			0x12, 0x0,	/* FC_UP */
/* 318 */	NdrFcShort( 0x30 ),	/* Offset= 48 (366) */
/* 320 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 322 */	NdrFcShort( 0x20 ),	/* 32 */
/* 324 */	NdrFcShort( 0x0 ),	/* 0 */
/* 326 */	NdrFcShort( 0xa ),	/* Offset= 10 (336) */
/* 328 */	0x36,		/* FC_POINTER */
			0x8,		/* FC_LONG */
/* 330 */	0x40,		/* FC_STRUCTPAD4 */
			0x36,		/* FC_POINTER */
/* 332 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 334 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 336 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 338 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 340 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 342 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 344 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 346 */	NdrFcShort( 0x0 ),	/* 0 */
/* 348 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 350 */	NdrFcShort( 0x0 ),	/* 0 */
/* 352 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 354 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 358 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 360 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 362 */	NdrFcShort( 0xffd6 ),	/* Offset= -42 (320) */
/* 364 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 366 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 368 */	NdrFcShort( 0x10 ),	/* 16 */
/* 370 */	NdrFcShort( 0x0 ),	/* 0 */
/* 372 */	NdrFcShort( 0x6 ),	/* Offset= 6 (378) */
/* 374 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 376 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 378 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 380 */	NdrFcShort( 0xffdc ),	/* Offset= -36 (344) */
/* 382 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 384 */	NdrFcShort( 0x10 ),	/* 16 */
/* 386 */	NdrFcShort( 0x0 ),	/* 0 */
/* 388 */	NdrFcShort( 0x0 ),	/* Offset= 0 (388) */
/* 390 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 392 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 394 */	NdrFcShort( 0xff5e ),	/* Offset= -162 (232) */
/* 396 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 398 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 400 */	NdrFcShort( 0x2 ),	/* Offset= 2 (402) */
/* 402 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0x9,		/* FC_ULONG */
/* 404 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 406 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 408 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 410 */	NdrFcShort( 0x2 ),	/* Offset= 2 (412) */
/* 412 */	NdrFcShort( 0x8 ),	/* 8 */
/* 414 */	NdrFcShort( 0x3004 ),	/* 12292 */
/* 416 */	NdrFcLong( 0x0 ),	/* 0 */
/* 420 */	NdrFcShort( 0x16 ),	/* Offset= 22 (442) */
/* 422 */	NdrFcLong( 0x1 ),	/* 1 */
/* 426 */	NdrFcShort( 0x14 ),	/* Offset= 20 (446) */
/* 428 */	NdrFcLong( 0x3ea ),	/* 1002 */
/* 432 */	NdrFcShort( 0x12 ),	/* Offset= 18 (450) */
/* 434 */	NdrFcLong( 0x3eb ),	/* 1003 */
/* 438 */	NdrFcShort( 0x16 ),	/* Offset= 22 (460) */
/* 440 */	NdrFcShort( 0x0 ),	/* Offset= 0 (440) */
/* 442 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 444 */	NdrFcShort( 0xff4c ),	/* Offset= -180 (264) */
/* 446 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 448 */	NdrFcShort( 0xff80 ),	/* Offset= -128 (320) */
/* 450 */	
			0x12, 0x0,	/* FC_UP */
/* 452 */	NdrFcShort( 0x2 ),	/* Offset= 2 (454) */
/* 454 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 456 */	NdrFcShort( 0x4 ),	/* 4 */
/* 458 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 460 */	
			0x12, 0x0,	/* FC_UP */
/* 462 */	NdrFcShort( 0x2 ),	/* Offset= 2 (464) */
/* 464 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 466 */	NdrFcShort( 0x8 ),	/* 8 */
/* 468 */	NdrFcShort( 0x0 ),	/* 0 */
/* 470 */	NdrFcShort( 0x4 ),	/* Offset= 4 (474) */
/* 472 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 474 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 476 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 478 */	
			0x11, 0x0,	/* FC_RP */
/* 480 */	NdrFcShort( 0x2 ),	/* Offset= 2 (482) */
/* 482 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0x9,		/* FC_ULONG */
/* 484 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 486 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 488 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 490 */	NdrFcShort( 0xffb2 ),	/* Offset= -78 (412) */
/* 492 */	
			0x11, 0x0,	/* FC_RP */
/* 494 */	NdrFcShort( 0x86 ),	/* Offset= 134 (628) */
/* 496 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0x9,		/* FC_ULONG */
/* 498 */	0x9,		/* Corr desc: FC_ULONG */
			0x0,		/*  */
/* 500 */	NdrFcShort( 0xfff8 ),	/* -8 */
/* 502 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 504 */	NdrFcShort( 0x2 ),	/* Offset= 2 (506) */
/* 506 */	NdrFcShort( 0x8 ),	/* 8 */
/* 508 */	NdrFcShort( 0x3002 ),	/* 12290 */
/* 510 */	NdrFcLong( 0x0 ),	/* 0 */
/* 514 */	NdrFcShort( 0xa ),	/* Offset= 10 (524) */
/* 516 */	NdrFcLong( 0x1 ),	/* 1 */
/* 520 */	NdrFcShort( 0x28 ),	/* Offset= 40 (560) */
/* 522 */	NdrFcShort( 0x0 ),	/* Offset= 0 (522) */
/* 524 */	
			0x12, 0x0,	/* FC_UP */
/* 526 */	NdrFcShort( 0x12 ),	/* Offset= 18 (544) */
/* 528 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 530 */	NdrFcShort( 0x4 ),	/* 4 */
/* 532 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 534 */	NdrFcShort( 0x0 ),	/* 0 */
/* 536 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 538 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 540 */	NdrFcShort( 0xffaa ),	/* Offset= -86 (454) */
/* 542 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 544 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 546 */	NdrFcShort( 0x10 ),	/* 16 */
/* 548 */	NdrFcShort( 0x0 ),	/* 0 */
/* 550 */	NdrFcShort( 0x6 ),	/* Offset= 6 (556) */
/* 552 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 554 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 556 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 558 */	NdrFcShort( 0xffe2 ),	/* Offset= -30 (528) */
/* 560 */	
			0x12, 0x0,	/* FC_UP */
/* 562 */	NdrFcShort( 0x32 ),	/* Offset= 50 (612) */
/* 564 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 566 */	NdrFcShort( 0x28 ),	/* 40 */
/* 568 */	NdrFcShort( 0x0 ),	/* 0 */
/* 570 */	NdrFcShort( 0xc ),	/* Offset= 12 (582) */
/* 572 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 574 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 576 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 578 */	0x36,		/* FC_POINTER */
			0x36,		/* FC_POINTER */
/* 580 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 582 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 584 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 586 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 588 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 590 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 592 */	NdrFcShort( 0x0 ),	/* 0 */
/* 594 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 596 */	NdrFcShort( 0x0 ),	/* 0 */
/* 598 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 600 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 604 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 606 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 608 */	NdrFcShort( 0xffd4 ),	/* Offset= -44 (564) */
/* 610 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 612 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 614 */	NdrFcShort( 0x10 ),	/* 16 */
/* 616 */	NdrFcShort( 0x0 ),	/* 0 */
/* 618 */	NdrFcShort( 0x6 ),	/* Offset= 6 (624) */
/* 620 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 622 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 624 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 626 */	NdrFcShort( 0xffdc ),	/* Offset= -36 (590) */
/* 628 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 630 */	NdrFcShort( 0x10 ),	/* 16 */
/* 632 */	NdrFcShort( 0x0 ),	/* 0 */
/* 634 */	NdrFcShort( 0x0 ),	/* Offset= 0 (634) */
/* 636 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 638 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 640 */	NdrFcShort( 0xff70 ),	/* Offset= -144 (496) */
/* 642 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 644 */	
			0x11, 0x0,	/* FC_RP */
/* 646 */	NdrFcShort( 0x74 ),	/* Offset= 116 (762) */
/* 648 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0x9,		/* FC_ULONG */
/* 650 */	0x9,		/* Corr desc: FC_ULONG */
			0x0,		/*  */
/* 652 */	NdrFcShort( 0xfff8 ),	/* -8 */
/* 654 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 656 */	NdrFcShort( 0x2 ),	/* Offset= 2 (658) */
/* 658 */	NdrFcShort( 0x8 ),	/* 8 */
/* 660 */	NdrFcShort( 0x3002 ),	/* 12290 */
/* 662 */	NdrFcLong( 0x2 ),	/* 2 */
/* 666 */	NdrFcShort( 0xa ),	/* Offset= 10 (676) */
/* 668 */	NdrFcLong( 0x3 ),	/* 3 */
/* 672 */	NdrFcShort( 0x18 ),	/* Offset= 24 (696) */
/* 674 */	NdrFcShort( 0x0 ),	/* Offset= 0 (674) */
/* 676 */	
			0x12, 0x0,	/* FC_UP */
/* 678 */	NdrFcShort( 0x2 ),	/* Offset= 2 (680) */
/* 680 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 682 */	NdrFcShort( 0x10 ),	/* 16 */
/* 684 */	NdrFcShort( 0x0 ),	/* 0 */
/* 686 */	NdrFcShort( 0x6 ),	/* Offset= 6 (692) */
/* 688 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 690 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 692 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 694 */	NdrFcShort( 0xff5a ),	/* Offset= -166 (528) */
/* 696 */	
			0x12, 0x0,	/* FC_UP */
/* 698 */	NdrFcShort( 0x30 ),	/* Offset= 48 (746) */
/* 700 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 702 */	NdrFcShort( 0x20 ),	/* 32 */
/* 704 */	NdrFcShort( 0x0 ),	/* 0 */
/* 706 */	NdrFcShort( 0xa ),	/* Offset= 10 (716) */
/* 708 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 710 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 712 */	0x36,		/* FC_POINTER */
			0x36,		/* FC_POINTER */
/* 714 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 716 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 718 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 720 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 722 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 724 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 726 */	NdrFcShort( 0x0 ),	/* 0 */
/* 728 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 730 */	NdrFcShort( 0x0 ),	/* 0 */
/* 732 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 734 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 738 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 740 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 742 */	NdrFcShort( 0xffd6 ),	/* Offset= -42 (700) */
/* 744 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 746 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 748 */	NdrFcShort( 0x10 ),	/* 16 */
/* 750 */	NdrFcShort( 0x0 ),	/* 0 */
/* 752 */	NdrFcShort( 0x6 ),	/* Offset= 6 (758) */
/* 754 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 756 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 758 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 760 */	NdrFcShort( 0xffdc ),	/* Offset= -36 (724) */
/* 762 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 764 */	NdrFcShort( 0x10 ),	/* 16 */
/* 766 */	NdrFcShort( 0x0 ),	/* 0 */
/* 768 */	NdrFcShort( 0x0 ),	/* Offset= 0 (768) */
/* 770 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 772 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 774 */	NdrFcShort( 0xff82 ),	/* Offset= -126 (648) */
/* 776 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 778 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 780 */	NdrFcShort( 0x2 ),	/* Offset= 2 (782) */
/* 782 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0x9,		/* FC_ULONG */
/* 784 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 786 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 788 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 790 */	NdrFcShort( 0x2 ),	/* Offset= 2 (792) */
/* 792 */	NdrFcShort( 0x8 ),	/* 8 */
/* 794 */	NdrFcShort( 0x3002 ),	/* 12290 */
/* 796 */	NdrFcLong( 0x2 ),	/* 2 */
/* 800 */	NdrFcShort( 0xa ),	/* Offset= 10 (810) */
/* 802 */	NdrFcLong( 0x3 ),	/* 3 */
/* 806 */	NdrFcShort( 0x8 ),	/* Offset= 8 (814) */
/* 808 */	NdrFcShort( 0x0 ),	/* Offset= 0 (808) */
/* 810 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 812 */	NdrFcShort( 0xfe9a ),	/* Offset= -358 (454) */
/* 814 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 816 */	NdrFcShort( 0xff8c ),	/* Offset= -116 (700) */
/* 818 */	
			0x11, 0x0,	/* FC_RP */
/* 820 */	NdrFcShort( 0x17c ),	/* Offset= 380 (1200) */
/* 822 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0x9,		/* FC_ULONG */
/* 824 */	0x9,		/* Corr desc: FC_ULONG */
			0x0,		/*  */
/* 826 */	NdrFcShort( 0xfff8 ),	/* -8 */
/* 828 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 830 */	NdrFcShort( 0x2 ),	/* Offset= 2 (832) */
/* 832 */	NdrFcShort( 0x8 ),	/* 8 */
/* 834 */	NdrFcShort( 0x3005 ),	/* 12293 */
/* 836 */	NdrFcLong( 0x0 ),	/* 0 */
/* 840 */	NdrFcShort( 0x1c ),	/* Offset= 28 (868) */
/* 842 */	NdrFcLong( 0x1 ),	/* 1 */
/* 846 */	NdrFcShort( 0x4e ),	/* Offset= 78 (924) */
/* 848 */	NdrFcLong( 0x2 ),	/* 2 */
/* 852 */	NdrFcShort( 0x8a ),	/* Offset= 138 (990) */
/* 854 */	NdrFcLong( 0xa ),	/* 10 */
/* 858 */	NdrFcShort( 0xca ),	/* Offset= 202 (1060) */
/* 860 */	NdrFcLong( 0x1f6 ),	/* 502 */
/* 864 */	NdrFcShort( 0x104 ),	/* Offset= 260 (1124) */
/* 866 */	NdrFcShort( 0x0 ),	/* Offset= 0 (866) */
/* 868 */	
			0x12, 0x0,	/* FC_UP */
/* 870 */	NdrFcShort( 0x26 ),	/* Offset= 38 (908) */
/* 872 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 874 */	NdrFcShort( 0x8 ),	/* 8 */
/* 876 */	NdrFcShort( 0x0 ),	/* 0 */
/* 878 */	NdrFcShort( 0x4 ),	/* Offset= 4 (882) */
/* 880 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 882 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 884 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 886 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 888 */	NdrFcShort( 0x0 ),	/* 0 */
/* 890 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 892 */	NdrFcShort( 0x0 ),	/* 0 */
/* 894 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 896 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 900 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 902 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 904 */	NdrFcShort( 0xffe0 ),	/* Offset= -32 (872) */
/* 906 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 908 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 910 */	NdrFcShort( 0x10 ),	/* 16 */
/* 912 */	NdrFcShort( 0x0 ),	/* 0 */
/* 914 */	NdrFcShort( 0x6 ),	/* Offset= 6 (920) */
/* 916 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 918 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 920 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 922 */	NdrFcShort( 0xffdc ),	/* Offset= -36 (886) */
/* 924 */	
			0x12, 0x0,	/* FC_UP */
/* 926 */	NdrFcShort( 0x30 ),	/* Offset= 48 (974) */
/* 928 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 930 */	NdrFcShort( 0x20 ),	/* 32 */
/* 932 */	NdrFcShort( 0x0 ),	/* 0 */
/* 934 */	NdrFcShort( 0xa ),	/* Offset= 10 (944) */
/* 936 */	0x36,		/* FC_POINTER */
			0x36,		/* FC_POINTER */
/* 938 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 940 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 942 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 944 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 946 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 948 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 950 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 952 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 954 */	NdrFcShort( 0x0 ),	/* 0 */
/* 956 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 958 */	NdrFcShort( 0x0 ),	/* 0 */
/* 960 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 962 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 966 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 968 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 970 */	NdrFcShort( 0xffd6 ),	/* Offset= -42 (928) */
/* 972 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 974 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 976 */	NdrFcShort( 0x10 ),	/* 16 */
/* 978 */	NdrFcShort( 0x0 ),	/* 0 */
/* 980 */	NdrFcShort( 0x6 ),	/* Offset= 6 (986) */
/* 982 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 984 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 986 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 988 */	NdrFcShort( 0xffdc ),	/* Offset= -36 (952) */
/* 990 */	
			0x12, 0x0,	/* FC_UP */
/* 992 */	NdrFcShort( 0x34 ),	/* Offset= 52 (1044) */
/* 994 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 996 */	NdrFcShort( 0x28 ),	/* 40 */
/* 998 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1000 */	NdrFcShort( 0xa ),	/* Offset= 10 (1010) */
/* 1002 */	0x36,		/* FC_POINTER */
			0x36,		/* FC_POINTER */
/* 1004 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1006 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1008 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1010 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1012 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1014 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1016 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1018 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1020 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1022 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 1024 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1026 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1028 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1030 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1032 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 1036 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1038 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1040 */	NdrFcShort( 0xffd2 ),	/* Offset= -46 (994) */
/* 1042 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1044 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1046 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1048 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1050 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1056) */
/* 1052 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1054 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1056 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 1058 */	NdrFcShort( 0xffdc ),	/* Offset= -36 (1022) */
/* 1060 */	
			0x12, 0x0,	/* FC_UP */
/* 1062 */	NdrFcShort( 0x2e ),	/* Offset= 46 (1108) */
/* 1064 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1066 */	NdrFcShort( 0x18 ),	/* 24 */
/* 1068 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1070 */	NdrFcShort( 0x8 ),	/* Offset= 8 (1078) */
/* 1072 */	0x36,		/* FC_POINTER */
			0x36,		/* FC_POINTER */
/* 1074 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1076 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1078 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1080 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1082 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1084 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1086 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 1088 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1090 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1092 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1094 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1096 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 1100 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1102 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1104 */	NdrFcShort( 0xffd8 ),	/* Offset= -40 (1064) */
/* 1106 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1108 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1110 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1112 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1114 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1120) */
/* 1116 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1118 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1120 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 1122 */	NdrFcShort( 0xffdc ),	/* Offset= -36 (1086) */
/* 1124 */	
			0x12, 0x0,	/* FC_UP */
/* 1126 */	NdrFcShort( 0x3a ),	/* Offset= 58 (1184) */
/* 1128 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1130 */	NdrFcShort( 0x30 ),	/* 48 */
/* 1132 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1134 */	NdrFcShort( 0xc ),	/* Offset= 12 (1146) */
/* 1136 */	0x36,		/* FC_POINTER */
			0x36,		/* FC_POINTER */
/* 1138 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1140 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1142 */	0x36,		/* FC_POINTER */
			0x36,		/* FC_POINTER */
/* 1144 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1146 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1148 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1150 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1152 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1154 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1156 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1158 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1160 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1162 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 1164 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1166 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1168 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1170 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1172 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 1176 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1178 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1180 */	NdrFcShort( 0xffcc ),	/* Offset= -52 (1128) */
/* 1182 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1184 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1186 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1188 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1190 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1196) */
/* 1192 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1194 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1196 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 1198 */	NdrFcShort( 0xffdc ),	/* Offset= -36 (1162) */
/* 1200 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1202 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1204 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1206 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1206) */
/* 1208 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1210 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1212 */	NdrFcShort( 0xfe7a ),	/* Offset= -390 (822) */
/* 1214 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1216 */	
			0x11, 0x0,	/* FC_RP */
/* 1218 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1220) */
/* 1220 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0x9,		/* FC_ULONG */
/* 1222 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 1224 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1226 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1228 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1230) */
/* 1230 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1232 */	NdrFcShort( 0x3009 ),	/* 12297 */
/* 1234 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1238 */	NdrFcShort( 0x34 ),	/* Offset= 52 (1290) */
/* 1240 */	NdrFcLong( 0x1 ),	/* 1 */
/* 1244 */	NdrFcShort( 0x40 ),	/* Offset= 64 (1308) */
/* 1246 */	NdrFcLong( 0x2 ),	/* 2 */
/* 1250 */	NdrFcShort( 0x54 ),	/* Offset= 84 (1334) */
/* 1252 */	NdrFcLong( 0x1f6 ),	/* 502 */
/* 1256 */	NdrFcShort( 0x76 ),	/* Offset= 118 (1374) */
/* 1258 */	NdrFcLong( 0x3ec ),	/* 1004 */
/* 1262 */	NdrFcShort( 0xaa ),	/* Offset= 170 (1432) */
/* 1264 */	NdrFcLong( 0x3ee ),	/* 1006 */
/* 1268 */	NdrFcShort( 0xfcce ),	/* Offset= -818 (450) */
/* 1270 */	NdrFcLong( 0x5dd ),	/* 1501 */
/* 1274 */	NdrFcShort( 0xb0 ),	/* Offset= 176 (1450) */
/* 1276 */	NdrFcLong( 0x3ed ),	/* 1005 */
/* 1280 */	NdrFcShort( 0xfcc2 ),	/* Offset= -830 (450) */
/* 1282 */	NdrFcLong( 0x1f5 ),	/* 501 */
/* 1286 */	NdrFcShort( 0xc4 ),	/* Offset= 196 (1482) */
/* 1288 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1288) */
/* 1290 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 1292 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1294) */
/* 1294 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1296 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1298 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1300 */	NdrFcShort( 0x4 ),	/* Offset= 4 (1304) */
/* 1302 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1304 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1306 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1308 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 1310 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1312) */
/* 1312 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1314 */	NdrFcShort( 0x18 ),	/* 24 */
/* 1316 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1318 */	NdrFcShort( 0x8 ),	/* Offset= 8 (1326) */
/* 1320 */	0x36,		/* FC_POINTER */
			0x8,		/* FC_LONG */
/* 1322 */	0x40,		/* FC_STRUCTPAD4 */
			0x36,		/* FC_POINTER */
/* 1324 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1326 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1328 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1330 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1332 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1334 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 1336 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1338) */
/* 1338 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1340 */	NdrFcShort( 0x38 ),	/* 56 */
/* 1342 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1344 */	NdrFcShort( 0xe ),	/* Offset= 14 (1358) */
/* 1346 */	0x36,		/* FC_POINTER */
			0x8,		/* FC_LONG */
/* 1348 */	0x40,		/* FC_STRUCTPAD4 */
			0x36,		/* FC_POINTER */
/* 1350 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1352 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1354 */	0x36,		/* FC_POINTER */
			0x36,		/* FC_POINTER */
/* 1356 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1358 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1360 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1362 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1364 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1366 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1368 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1370 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1372 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1374 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 1376 */	NdrFcShort( 0xe ),	/* Offset= 14 (1390) */
/* 1378 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 1380 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1382 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1384 */	NdrFcShort( 0x38 ),	/* 56 */
/* 1386 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1388 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 1390 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1392 */	NdrFcShort( 0x48 ),	/* 72 */
/* 1394 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1396 */	NdrFcShort( 0x10 ),	/* Offset= 16 (1412) */
/* 1398 */	0x36,		/* FC_POINTER */
			0x8,		/* FC_LONG */
/* 1400 */	0x40,		/* FC_STRUCTPAD4 */
			0x36,		/* FC_POINTER */
/* 1402 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1404 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1406 */	0x36,		/* FC_POINTER */
			0x36,		/* FC_POINTER */
/* 1408 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1410 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1412 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1414 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1416 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1418 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1420 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1422 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1424 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1426 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1428 */	
			0x12, 0x0,	/* FC_UP */
/* 1430 */	NdrFcShort( 0xffcc ),	/* Offset= -52 (1378) */
/* 1432 */	
			0x12, 0x0,	/* FC_UP */
/* 1434 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1436) */
/* 1436 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1438 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1440 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1442 */	NdrFcShort( 0x4 ),	/* Offset= 4 (1446) */
/* 1444 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1446 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1448 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1450 */	
			0x12, 0x0,	/* FC_UP */
/* 1452 */	NdrFcShort( 0xe ),	/* Offset= 14 (1466) */
/* 1454 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 1456 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1458 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1460 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1462 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1464 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 1466 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1468 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1470 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1472 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1478) */
/* 1474 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1476 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1478 */	
			0x12, 0x0,	/* FC_UP */
/* 1480 */	NdrFcShort( 0xffe6 ),	/* Offset= -26 (1454) */
/* 1482 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 1484 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1486) */
/* 1486 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1488 */	NdrFcShort( 0x20 ),	/* 32 */
/* 1490 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1492 */	NdrFcShort( 0xa ),	/* Offset= 10 (1502) */
/* 1494 */	0x36,		/* FC_POINTER */
			0x8,		/* FC_LONG */
/* 1496 */	0x40,		/* FC_STRUCTPAD4 */
			0x36,		/* FC_POINTER */
/* 1498 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1500 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1502 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1504 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1506 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1508 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1510 */	
			0x11, 0x0,	/* FC_RP */
/* 1512 */	NdrFcShort( 0x102 ),	/* Offset= 258 (1770) */
/* 1514 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0x9,		/* FC_ULONG */
/* 1516 */	0x9,		/* Corr desc: FC_ULONG */
			0x0,		/*  */
/* 1518 */	NdrFcShort( 0xfff8 ),	/* -8 */
/* 1520 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1522 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1524) */
/* 1524 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1526 */	NdrFcShort( 0x3005 ),	/* 12293 */
/* 1528 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1532 */	NdrFcShort( 0x1c ),	/* Offset= 28 (1560) */
/* 1534 */	NdrFcLong( 0x1 ),	/* 1 */
/* 1538 */	NdrFcShort( 0x40 ),	/* Offset= 64 (1602) */
/* 1540 */	NdrFcLong( 0x2 ),	/* 2 */
/* 1544 */	NdrFcShort( 0x64 ),	/* Offset= 100 (1644) */
/* 1546 */	NdrFcLong( 0x1f6 ),	/* 502 */
/* 1550 */	NdrFcShort( 0x88 ),	/* Offset= 136 (1686) */
/* 1552 */	NdrFcLong( 0x1f5 ),	/* 501 */
/* 1556 */	NdrFcShort( 0xac ),	/* Offset= 172 (1728) */
/* 1558 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1558) */
/* 1560 */	
			0x12, 0x0,	/* FC_UP */
/* 1562 */	NdrFcShort( 0x18 ),	/* Offset= 24 (1586) */
/* 1564 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 1566 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1568 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1570 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1572 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1574 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 1578 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1580 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1582 */	NdrFcShort( 0xfee0 ),	/* Offset= -288 (1294) */
/* 1584 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1586 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1588 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1590 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1592 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1598) */
/* 1594 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1596 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1598 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 1600 */	NdrFcShort( 0xffdc ),	/* Offset= -36 (1564) */
/* 1602 */	
			0x12, 0x0,	/* FC_UP */
/* 1604 */	NdrFcShort( 0x18 ),	/* Offset= 24 (1628) */
/* 1606 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 1608 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1610 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1612 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1614 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1616 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 1620 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1622 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1624 */	NdrFcShort( 0xfec8 ),	/* Offset= -312 (1312) */
/* 1626 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1628 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1630 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1632 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1634 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1640) */
/* 1636 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1638 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1640 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 1642 */	NdrFcShort( 0xffdc ),	/* Offset= -36 (1606) */
/* 1644 */	
			0x12, 0x0,	/* FC_UP */
/* 1646 */	NdrFcShort( 0x18 ),	/* Offset= 24 (1670) */
/* 1648 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 1650 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1652 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1654 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1656 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1658 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 1662 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1664 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1666 */	NdrFcShort( 0xfeb8 ),	/* Offset= -328 (1338) */
/* 1668 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1670 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1672 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1674 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1676 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1682) */
/* 1678 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1680 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1682 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 1684 */	NdrFcShort( 0xffdc ),	/* Offset= -36 (1648) */
/* 1686 */	
			0x12, 0x0,	/* FC_UP */
/* 1688 */	NdrFcShort( 0x18 ),	/* Offset= 24 (1712) */
/* 1690 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 1692 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1694 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1696 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1698 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1700 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 1704 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1706 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1708 */	NdrFcShort( 0xfec2 ),	/* Offset= -318 (1390) */
/* 1710 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1712 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1714 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1716 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1718 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1724) */
/* 1720 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1722 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1724 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 1726 */	NdrFcShort( 0xffdc ),	/* Offset= -36 (1690) */
/* 1728 */	
			0x12, 0x0,	/* FC_UP */
/* 1730 */	NdrFcShort( 0x18 ),	/* Offset= 24 (1754) */
/* 1732 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 1734 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1736 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1738 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1740 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1742 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 1746 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1748 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1750 */	NdrFcShort( 0xfef8 ),	/* Offset= -264 (1486) */
/* 1752 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1754 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1756 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1758 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1760 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1766) */
/* 1762 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1764 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1766 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 1768 */	NdrFcShort( 0xffdc ),	/* Offset= -36 (1732) */
/* 1770 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1772 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1774 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1776 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1776) */
/* 1778 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1780 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1782 */	NdrFcShort( 0xfef4 ),	/* Offset= -268 (1514) */
/* 1784 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1786 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 1788 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1790) */
/* 1790 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0x9,		/* FC_ULONG */
/* 1792 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 1794 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1796 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1798 */	NdrFcShort( 0xfdc8 ),	/* Offset= -568 (1230) */
/* 1800 */	
			0x11, 0x0,	/* FC_RP */
/* 1802 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1804) */
/* 1804 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0x9,		/* FC_ULONG */
/* 1806 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 1808 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1810 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1812 */	NdrFcShort( 0xfdba ),	/* Offset= -582 (1230) */
/* 1814 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 1816 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1818) */
/* 1818 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0x9,		/* FC_ULONG */
/* 1820 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 1822 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1824 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1826 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1828) */
/* 1828 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1830 */	NdrFcShort( 0x303b ),	/* 12347 */
/* 1832 */	NdrFcLong( 0x64 ),	/* 100 */
/* 1836 */	NdrFcShort( 0x160 ),	/* Offset= 352 (2188) */
/* 1838 */	NdrFcLong( 0x65 ),	/* 101 */
/* 1842 */	NdrFcShort( 0x16e ),	/* Offset= 366 (2208) */
/* 1844 */	NdrFcLong( 0x66 ),	/* 102 */
/* 1848 */	NdrFcShort( 0x186 ),	/* Offset= 390 (2238) */
/* 1850 */	NdrFcLong( 0x192 ),	/* 402 */
/* 1854 */	NdrFcShort( 0x1a8 ),	/* Offset= 424 (2278) */
/* 1856 */	NdrFcLong( 0x193 ),	/* 403 */
/* 1860 */	NdrFcShort( 0x1dc ),	/* Offset= 476 (2336) */
/* 1862 */	NdrFcLong( 0x1f6 ),	/* 502 */
/* 1866 */	NdrFcShort( 0x218 ),	/* Offset= 536 (2402) */
/* 1868 */	NdrFcLong( 0x1f7 ),	/* 503 */
/* 1872 */	NdrFcShort( 0x22e ),	/* Offset= 558 (2430) */
/* 1874 */	NdrFcLong( 0x257 ),	/* 599 */
/* 1878 */	NdrFcShort( 0x264 ),	/* Offset= 612 (2490) */
/* 1880 */	NdrFcLong( 0x3ed ),	/* 1005 */
/* 1884 */	NdrFcShort( 0x2a8 ),	/* Offset= 680 (2564) */
/* 1886 */	NdrFcLong( 0x453 ),	/* 1107 */
/* 1890 */	NdrFcShort( 0xfa60 ),	/* Offset= -1440 (450) */
/* 1892 */	NdrFcLong( 0x3f2 ),	/* 1010 */
/* 1896 */	NdrFcShort( 0xfa5a ),	/* Offset= -1446 (450) */
/* 1898 */	NdrFcLong( 0x3f8 ),	/* 1016 */
/* 1902 */	NdrFcShort( 0xfa54 ),	/* Offset= -1452 (450) */
/* 1904 */	NdrFcLong( 0x3f9 ),	/* 1017 */
/* 1908 */	NdrFcShort( 0xfa4e ),	/* Offset= -1458 (450) */
/* 1910 */	NdrFcLong( 0x3fa ),	/* 1018 */
/* 1914 */	NdrFcShort( 0xfa48 ),	/* Offset= -1464 (450) */
/* 1916 */	NdrFcLong( 0x5dd ),	/* 1501 */
/* 1920 */	NdrFcShort( 0xfa42 ),	/* Offset= -1470 (450) */
/* 1922 */	NdrFcLong( 0x5de ),	/* 1502 */
/* 1926 */	NdrFcShort( 0xfa3c ),	/* Offset= -1476 (450) */
/* 1928 */	NdrFcLong( 0x5df ),	/* 1503 */
/* 1932 */	NdrFcShort( 0xfa36 ),	/* Offset= -1482 (450) */
/* 1934 */	NdrFcLong( 0x5e2 ),	/* 1506 */
/* 1938 */	NdrFcShort( 0xfa30 ),	/* Offset= -1488 (450) */
/* 1940 */	NdrFcLong( 0x5e5 ),	/* 1509 */
/* 1944 */	NdrFcShort( 0xfa2a ),	/* Offset= -1494 (450) */
/* 1946 */	NdrFcLong( 0x5e6 ),	/* 1510 */
/* 1950 */	NdrFcShort( 0xfa24 ),	/* Offset= -1500 (450) */
/* 1952 */	NdrFcLong( 0x5e7 ),	/* 1511 */
/* 1956 */	NdrFcShort( 0xfa1e ),	/* Offset= -1506 (450) */
/* 1958 */	NdrFcLong( 0x5e8 ),	/* 1512 */
/* 1962 */	NdrFcShort( 0xfa18 ),	/* Offset= -1512 (450) */
/* 1964 */	NdrFcLong( 0x5e9 ),	/* 1513 */
/* 1968 */	NdrFcShort( 0xfa12 ),	/* Offset= -1518 (450) */
/* 1970 */	NdrFcLong( 0x5ea ),	/* 1514 */
/* 1974 */	NdrFcShort( 0xfa0c ),	/* Offset= -1524 (450) */
/* 1976 */	NdrFcLong( 0x5eb ),	/* 1515 */
/* 1980 */	NdrFcShort( 0xfa06 ),	/* Offset= -1530 (450) */
/* 1982 */	NdrFcLong( 0x5ec ),	/* 1516 */
/* 1986 */	NdrFcShort( 0xfa00 ),	/* Offset= -1536 (450) */
/* 1988 */	NdrFcLong( 0x5ee ),	/* 1518 */
/* 1992 */	NdrFcShort( 0xf9fa ),	/* Offset= -1542 (450) */
/* 1994 */	NdrFcLong( 0x5f0 ),	/* 1520 */
/* 1998 */	NdrFcShort( 0xf9f4 ),	/* Offset= -1548 (450) */
/* 2000 */	NdrFcLong( 0x5f1 ),	/* 1521 */
/* 2004 */	NdrFcShort( 0xf9ee ),	/* Offset= -1554 (450) */
/* 2006 */	NdrFcLong( 0x5f2 ),	/* 1522 */
/* 2010 */	NdrFcShort( 0xf9e8 ),	/* Offset= -1560 (450) */
/* 2012 */	NdrFcLong( 0x5f3 ),	/* 1523 */
/* 2016 */	NdrFcShort( 0xf9e2 ),	/* Offset= -1566 (450) */
/* 2018 */	NdrFcLong( 0x5f4 ),	/* 1524 */
/* 2022 */	NdrFcShort( 0xf9dc ),	/* Offset= -1572 (450) */
/* 2024 */	NdrFcLong( 0x5f5 ),	/* 1525 */
/* 2028 */	NdrFcShort( 0xf9d6 ),	/* Offset= -1578 (450) */
/* 2030 */	NdrFcLong( 0x5f8 ),	/* 1528 */
/* 2034 */	NdrFcShort( 0xf9d0 ),	/* Offset= -1584 (450) */
/* 2036 */	NdrFcLong( 0x5f9 ),	/* 1529 */
/* 2040 */	NdrFcShort( 0xf9ca ),	/* Offset= -1590 (450) */
/* 2042 */	NdrFcLong( 0x5fa ),	/* 1530 */
/* 2046 */	NdrFcShort( 0xf9c4 ),	/* Offset= -1596 (450) */
/* 2048 */	NdrFcLong( 0x5fd ),	/* 1533 */
/* 2052 */	NdrFcShort( 0xf9be ),	/* Offset= -1602 (450) */
/* 2054 */	NdrFcLong( 0x5fe ),	/* 1534 */
/* 2058 */	NdrFcShort( 0xf9b8 ),	/* Offset= -1608 (450) */
/* 2060 */	NdrFcLong( 0x5ff ),	/* 1535 */
/* 2064 */	NdrFcShort( 0xf9b2 ),	/* Offset= -1614 (450) */
/* 2066 */	NdrFcLong( 0x600 ),	/* 1536 */
/* 2070 */	NdrFcShort( 0xf9ac ),	/* Offset= -1620 (450) */
/* 2072 */	NdrFcLong( 0x601 ),	/* 1537 */
/* 2076 */	NdrFcShort( 0xf9a6 ),	/* Offset= -1626 (450) */
/* 2078 */	NdrFcLong( 0x602 ),	/* 1538 */
/* 2082 */	NdrFcShort( 0xf9a0 ),	/* Offset= -1632 (450) */
/* 2084 */	NdrFcLong( 0x603 ),	/* 1539 */
/* 2088 */	NdrFcShort( 0xf99a ),	/* Offset= -1638 (450) */
/* 2090 */	NdrFcLong( 0x604 ),	/* 1540 */
/* 2094 */	NdrFcShort( 0xf994 ),	/* Offset= -1644 (450) */
/* 2096 */	NdrFcLong( 0x605 ),	/* 1541 */
/* 2100 */	NdrFcShort( 0xf98e ),	/* Offset= -1650 (450) */
/* 2102 */	NdrFcLong( 0x606 ),	/* 1542 */
/* 2106 */	NdrFcShort( 0xf988 ),	/* Offset= -1656 (450) */
/* 2108 */	NdrFcLong( 0x607 ),	/* 1543 */
/* 2112 */	NdrFcShort( 0xf982 ),	/* Offset= -1662 (450) */
/* 2114 */	NdrFcLong( 0x608 ),	/* 1544 */
/* 2118 */	NdrFcShort( 0xf97c ),	/* Offset= -1668 (450) */
/* 2120 */	NdrFcLong( 0x609 ),	/* 1545 */
/* 2124 */	NdrFcShort( 0xf976 ),	/* Offset= -1674 (450) */
/* 2126 */	NdrFcLong( 0x60a ),	/* 1546 */
/* 2130 */	NdrFcShort( 0xf970 ),	/* Offset= -1680 (450) */
/* 2132 */	NdrFcLong( 0x60b ),	/* 1547 */
/* 2136 */	NdrFcShort( 0xf96a ),	/* Offset= -1686 (450) */
/* 2138 */	NdrFcLong( 0x60c ),	/* 1548 */
/* 2142 */	NdrFcShort( 0xf964 ),	/* Offset= -1692 (450) */
/* 2144 */	NdrFcLong( 0x60d ),	/* 1549 */
/* 2148 */	NdrFcShort( 0xf95e ),	/* Offset= -1698 (450) */
/* 2150 */	NdrFcLong( 0x60e ),	/* 1550 */
/* 2154 */	NdrFcShort( 0xf958 ),	/* Offset= -1704 (450) */
/* 2156 */	NdrFcLong( 0x610 ),	/* 1552 */
/* 2160 */	NdrFcShort( 0xf952 ),	/* Offset= -1710 (450) */
/* 2162 */	NdrFcLong( 0x611 ),	/* 1553 */
/* 2166 */	NdrFcShort( 0xf94c ),	/* Offset= -1716 (450) */
/* 2168 */	NdrFcLong( 0x612 ),	/* 1554 */
/* 2172 */	NdrFcShort( 0xf946 ),	/* Offset= -1722 (450) */
/* 2174 */	NdrFcLong( 0x613 ),	/* 1555 */
/* 2178 */	NdrFcShort( 0xf940 ),	/* Offset= -1728 (450) */
/* 2180 */	NdrFcLong( 0x614 ),	/* 1556 */
/* 2184 */	NdrFcShort( 0xf93a ),	/* Offset= -1734 (450) */
/* 2186 */	NdrFcShort( 0x0 ),	/* Offset= 0 (2186) */
/* 2188 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 2190 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2192) */
/* 2192 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 2194 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2196 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2198 */	NdrFcShort( 0x6 ),	/* Offset= 6 (2204) */
/* 2200 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 2202 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 2204 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 2206 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 2208 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 2210 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2212) */
/* 2212 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 2214 */	NdrFcShort( 0x28 ),	/* 40 */
/* 2216 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2218 */	NdrFcShort( 0xc ),	/* Offset= 12 (2230) */
/* 2220 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 2222 */	0x36,		/* FC_POINTER */
			0x8,		/* FC_LONG */
/* 2224 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2226 */	0x40,		/* FC_STRUCTPAD4 */
			0x36,		/* FC_POINTER */
/* 2228 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 2230 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 2232 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 2234 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 2236 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 2238 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 2240 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2242) */
/* 2242 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 2244 */	NdrFcShort( 0x48 ),	/* 72 */
/* 2246 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2248 */	NdrFcShort( 0x12 ),	/* Offset= 18 (2266) */
/* 2250 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 2252 */	0x36,		/* FC_POINTER */
			0x8,		/* FC_LONG */
/* 2254 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2256 */	0x40,		/* FC_STRUCTPAD4 */
			0x36,		/* FC_POINTER */
/* 2258 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2260 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2262 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2264 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 2266 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 2268 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 2270 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 2272 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 2274 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 2276 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 2278 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 2280 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2282) */
/* 2282 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 2284 */	NdrFcShort( 0x90 ),	/* 144 */
/* 2286 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2288 */	NdrFcShort( 0x24 ),	/* Offset= 36 (2324) */
/* 2290 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2292 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 2294 */	0x36,		/* FC_POINTER */
			0x8,		/* FC_LONG */
/* 2296 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2298 */	0x40,		/* FC_STRUCTPAD4 */
			0x36,		/* FC_POINTER */
/* 2300 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2302 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2304 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2306 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2308 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2310 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2312 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2314 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2316 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2318 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2320 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2322 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 2324 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 2326 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 2328 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 2330 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 2332 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 2334 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 2336 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 2338 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2340) */
/* 2340 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 2342 */	NdrFcShort( 0xa0 ),	/* 160 */
/* 2344 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2346 */	NdrFcShort( 0x28 ),	/* Offset= 40 (2386) */
/* 2348 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2350 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 2352 */	0x36,		/* FC_POINTER */
			0x8,		/* FC_LONG */
/* 2354 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2356 */	0x40,		/* FC_STRUCTPAD4 */
			0x36,		/* FC_POINTER */
/* 2358 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2360 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2362 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2364 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2366 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2368 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2370 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2372 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2374 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2376 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2378 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2380 */	0x36,		/* FC_POINTER */
			0x8,		/* FC_LONG */
/* 2382 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 2384 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 2386 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 2388 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 2390 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 2392 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 2394 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 2396 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 2398 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 2400 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 2402 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 2404 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2406) */
/* 2406 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 2408 */	NdrFcShort( 0x48 ),	/* 72 */
/* 2410 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2412 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2414 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2416 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2418 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2420 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2422 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2424 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2426 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2428 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 2430 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 2432 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2434) */
/* 2434 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 2436 */	NdrFcShort( 0xb0 ),	/* 176 */
/* 2438 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2440 */	NdrFcShort( 0x2e ),	/* Offset= 46 (2486) */
/* 2442 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2444 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2446 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2448 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2450 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2452 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2454 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2456 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2458 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2460 */	0x36,		/* FC_POINTER */
			0x8,		/* FC_LONG */
/* 2462 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2464 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2466 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2468 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2470 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2472 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2474 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2476 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2478 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2480 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2482 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2484 */	0x40,		/* FC_STRUCTPAD4 */
			0x5b,		/* FC_END */
/* 2486 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 2488 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 2490 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 2492 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2494) */
/* 2494 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 2496 */	NdrFcShort( 0xe8 ),	/* 232 */
/* 2498 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2500 */	NdrFcShort( 0x3c ),	/* Offset= 60 (2560) */
/* 2502 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2504 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2506 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2508 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2510 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2512 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2514 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2516 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2518 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2520 */	0x36,		/* FC_POINTER */
			0x8,		/* FC_LONG */
/* 2522 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2524 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2526 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2528 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2530 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2532 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2534 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2536 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2538 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2540 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2542 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2544 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2546 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2548 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2550 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2552 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2554 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2556 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2558 */	0x40,		/* FC_STRUCTPAD4 */
			0x5b,		/* FC_END */
/* 2560 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 2562 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 2564 */	
			0x12, 0x0,	/* FC_UP */
/* 2566 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2568) */
/* 2568 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 2570 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2572 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2574 */	NdrFcShort( 0x4 ),	/* Offset= 4 (2578) */
/* 2576 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 2578 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 2580 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 2582 */	
			0x11, 0x0,	/* FC_RP */
/* 2584 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2586) */
/* 2586 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0x9,		/* FC_ULONG */
/* 2588 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 2590 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2592 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 2594 */	NdrFcShort( 0xfd02 ),	/* Offset= -766 (1828) */
/* 2596 */	
			0x11, 0x0,	/* FC_RP */
/* 2598 */	NdrFcShort( 0x2a ),	/* Offset= 42 (2640) */
/* 2600 */	
			0x29,		/* FC_WSTRING */
			0x5c,		/* FC_PAD */
/* 2602 */	NdrFcShort( 0x3 ),	/* 3 */
/* 2604 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x1,		/* 1 */
/* 2606 */	NdrFcShort( 0x6 ),	/* 6 */
/* 2608 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2610 */	NdrFcShort( 0x0 ),	/* Offset= 0 (2610) */
/* 2612 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 2614 */	NdrFcShort( 0xfff2 ),	/* Offset= -14 (2600) */
/* 2616 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 2618 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x1,		/* 1 */
/* 2620 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2622 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 2624 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2626 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 2628 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 2630 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2632 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 2634 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 2636 */	NdrFcShort( 0xffe0 ),	/* Offset= -32 (2604) */
/* 2638 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 2640 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 2642 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2644 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2646 */	NdrFcShort( 0x6 ),	/* Offset= 6 (2652) */
/* 2648 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 2650 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 2652 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 2654 */	NdrFcShort( 0xffdc ),	/* Offset= -36 (2618) */
/* 2656 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 2658 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2660) */
/* 2660 */	
			0x12, 0x0,	/* FC_UP */
/* 2662 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2664) */
/* 2664 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 2666 */	NdrFcShort( 0x44 ),	/* 68 */
/* 2668 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2670 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2672 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2674 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2676 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2678 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2680 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2682 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2684 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 2686 */	
			0x11, 0x1,	/* FC_RP [all_nodes] */
/* 2688 */	NdrFcShort( 0xe ),	/* Offset= 14 (2702) */
/* 2690 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 2692 */	NdrFcShort( 0x1 ),	/* 1 */
/* 2694 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 2696 */	NdrFcShort( 0x18 ),	/* 24 */
/* 2698 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 2700 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 2702 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 2704 */	NdrFcShort( 0x28 ),	/* 40 */
/* 2706 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2708 */	NdrFcShort( 0xa ),	/* Offset= 10 (2718) */
/* 2710 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 2712 */	0x36,		/* FC_POINTER */
			0x36,		/* FC_POINTER */
/* 2714 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 2716 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 2718 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 2720 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 2722 */	
			0x12, 0x0,	/* FC_UP */
/* 2724 */	NdrFcShort( 0xffde ),	/* Offset= -34 (2690) */
/* 2726 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 2728 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 2730 */	
			0x11, 0x0,	/* FC_RP */
/* 2732 */	NdrFcShort( 0x146 ),	/* Offset= 326 (3058) */
/* 2734 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0x9,		/* FC_ULONG */
/* 2736 */	0x9,		/* Corr desc: FC_ULONG */
			0x0,		/*  */
/* 2738 */	NdrFcShort( 0xfff8 ),	/* -8 */
/* 2740 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 2742 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2744) */
/* 2744 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2746 */	NdrFcShort( 0x3004 ),	/* 12292 */
/* 2748 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2752 */	NdrFcShort( 0x16 ),	/* Offset= 22 (2774) */
/* 2754 */	NdrFcLong( 0x1 ),	/* 1 */
/* 2758 */	NdrFcShort( 0x3a ),	/* Offset= 58 (2816) */
/* 2760 */	NdrFcLong( 0x2 ),	/* 2 */
/* 2764 */	NdrFcShort( 0x80 ),	/* Offset= 128 (2892) */
/* 2766 */	NdrFcLong( 0x3 ),	/* 3 */
/* 2770 */	NdrFcShort( 0xc8 ),	/* Offset= 200 (2970) */
/* 2772 */	NdrFcShort( 0x0 ),	/* Offset= 0 (2772) */
/* 2774 */	
			0x12, 0x0,	/* FC_UP */
/* 2776 */	NdrFcShort( 0x18 ),	/* Offset= 24 (2800) */
/* 2778 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 2780 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2782 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 2784 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2786 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 2788 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 2792 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 2794 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 2796 */	NdrFcShort( 0xffa2 ),	/* Offset= -94 (2702) */
/* 2798 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 2800 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 2802 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2804 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2806 */	NdrFcShort( 0x6 ),	/* Offset= 6 (2812) */
/* 2808 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 2810 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 2812 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 2814 */	NdrFcShort( 0xffdc ),	/* Offset= -36 (2778) */
/* 2816 */	
			0x12, 0x0,	/* FC_UP */
/* 2818 */	NdrFcShort( 0x3a ),	/* Offset= 58 (2876) */
/* 2820 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 2822 */	NdrFcShort( 0x30 ),	/* 48 */
/* 2824 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2826 */	NdrFcShort( 0xc ),	/* Offset= 12 (2838) */
/* 2828 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 2830 */	0x36,		/* FC_POINTER */
			0x36,		/* FC_POINTER */
/* 2832 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 2834 */	0x36,		/* FC_POINTER */
			0x36,		/* FC_POINTER */
/* 2836 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 2838 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 2840 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 2842 */	
			0x12, 0x0,	/* FC_UP */
/* 2844 */	NdrFcShort( 0xff66 ),	/* Offset= -154 (2690) */
/* 2846 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 2848 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 2850 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 2852 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 2854 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 2856 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2858 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 2860 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2862 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 2864 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 2868 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 2870 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 2872 */	NdrFcShort( 0xffcc ),	/* Offset= -52 (2820) */
/* 2874 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 2876 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 2878 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2880 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2882 */	NdrFcShort( 0x6 ),	/* Offset= 6 (2888) */
/* 2884 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 2886 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 2888 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 2890 */	NdrFcShort( 0xffdc ),	/* Offset= -36 (2854) */
/* 2892 */	
			0x12, 0x0,	/* FC_UP */
/* 2894 */	NdrFcShort( 0x3c ),	/* Offset= 60 (2954) */
/* 2896 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 2898 */	NdrFcShort( 0x38 ),	/* 56 */
/* 2900 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2902 */	NdrFcShort( 0xe ),	/* Offset= 14 (2916) */
/* 2904 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 2906 */	0x36,		/* FC_POINTER */
			0x36,		/* FC_POINTER */
/* 2908 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 2910 */	0x36,		/* FC_POINTER */
			0x36,		/* FC_POINTER */
/* 2912 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 2914 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 2916 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 2918 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 2920 */	
			0x12, 0x0,	/* FC_UP */
/* 2922 */	NdrFcShort( 0xff18 ),	/* Offset= -232 (2690) */
/* 2924 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 2926 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 2928 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 2930 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 2932 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 2934 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2936 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 2938 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2940 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 2942 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 2946 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 2948 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 2950 */	NdrFcShort( 0xffca ),	/* Offset= -54 (2896) */
/* 2952 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 2954 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 2956 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2958 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2960 */	NdrFcShort( 0x6 ),	/* Offset= 6 (2966) */
/* 2962 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 2964 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 2966 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 2968 */	NdrFcShort( 0xffdc ),	/* Offset= -36 (2932) */
/* 2970 */	
			0x12, 0x0,	/* FC_UP */
/* 2972 */	NdrFcShort( 0x46 ),	/* Offset= 70 (3042) */
/* 2974 */	
			0x1d,		/* FC_SMFARRAY */
			0x0,		/* 0 */
/* 2976 */	NdrFcShort( 0x100 ),	/* 256 */
/* 2978 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 2980 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 2982 */	NdrFcShort( 0x138 ),	/* 312 */
/* 2984 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2986 */	NdrFcShort( 0x12 ),	/* Offset= 18 (3004) */
/* 2988 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 2990 */	0x36,		/* FC_POINTER */
			0x36,		/* FC_POINTER */
/* 2992 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 2994 */	0x36,		/* FC_POINTER */
			0x36,		/* FC_POINTER */
/* 2996 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2998 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 3000 */	NdrFcShort( 0xffe6 ),	/* Offset= -26 (2974) */
/* 3002 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 3004 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 3006 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 3008 */	
			0x12, 0x0,	/* FC_UP */
/* 3010 */	NdrFcShort( 0xfec0 ),	/* Offset= -320 (2690) */
/* 3012 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 3014 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 3016 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 3018 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 3020 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 3022 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3024 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 3026 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3028 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 3030 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 3034 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 3036 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 3038 */	NdrFcShort( 0xffc6 ),	/* Offset= -58 (2980) */
/* 3040 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 3042 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 3044 */	NdrFcShort( 0x10 ),	/* 16 */
/* 3046 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3048 */	NdrFcShort( 0x6 ),	/* Offset= 6 (3054) */
/* 3050 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 3052 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 3054 */	
			0x12, 0x0,	/* FC_UP */
/* 3056 */	NdrFcShort( 0xffdc ),	/* Offset= -36 (3020) */
/* 3058 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 3060 */	NdrFcShort( 0x10 ),	/* 16 */
/* 3062 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3064 */	NdrFcShort( 0x0 ),	/* Offset= 0 (3064) */
/* 3066 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 3068 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 3070 */	NdrFcShort( 0xfeb0 ),	/* Offset= -336 (2734) */
/* 3072 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 3074 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 3076 */	NdrFcShort( 0x2 ),	/* Offset= 2 (3078) */
/* 3078 */	
			0x12, 0x0,	/* FC_UP */
/* 3080 */	NdrFcShort( 0x2 ),	/* Offset= 2 (3082) */
/* 3082 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 3084 */	NdrFcShort( 0x30 ),	/* 48 */
/* 3086 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 3088 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 3090 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 3092 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 3094 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 3096 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 3098 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 3100 */	
			0x11, 0x0,	/* FC_RP */
/* 3102 */	NdrFcShort( 0x2 ),	/* Offset= 2 (3104) */
/* 3104 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 3106 */	NdrFcShort( 0x1 ),	/* 1 */
/* 3108 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 3110 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 3112 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 3114 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 3116 */	0xb7,		/* FC_RANGE */
			0x8,		/* 8 */
/* 3118 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3122 */	NdrFcLong( 0xfa00 ),	/* 64000 */
/* 3126 */	
			0x11, 0x8,	/* FC_RP [simple_pointer] */
/* 3128 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 3130 */	
			0x11, 0x0,	/* FC_RP */
/* 3132 */	NdrFcShort( 0x2 ),	/* Offset= 2 (3134) */
/* 3134 */	
			0x1b,		/* FC_CARRAY */
			0x1,		/* 1 */
/* 3136 */	NdrFcShort( 0x2 ),	/* 2 */
/* 3138 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 3140 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 3142 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 3144 */	0x5,		/* FC_WCHAR */
			0x5b,		/* FC_END */
/* 3146 */	0xb7,		/* FC_RANGE */
			0x8,		/* 8 */
/* 3148 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3152 */	NdrFcLong( 0xfa00 ),	/* 64000 */
/* 3156 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 3158 */	NdrFcShort( 0x2 ),	/* Offset= 2 (3160) */
/* 3160 */	0x30,		/* FC_BIND_CONTEXT */
			0xa8,		/* Ctxt flags:  via ptr, out, strict, */
/* 3162 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 3164 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 3166 */	NdrFcShort( 0x2 ),	/* Offset= 2 (3168) */
/* 3168 */	0x30,		/* FC_BIND_CONTEXT */
			0xe9,		/* Ctxt flags:  via ptr, in, out, strict, can't be null */
/* 3170 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 3172 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 3174 */	NdrFcShort( 0x2 ),	/* Offset= 2 (3176) */
/* 3176 */	
			0x12, 0x0,	/* FC_UP */
/* 3178 */	NdrFcShort( 0x2 ),	/* Offset= 2 (3180) */
/* 3180 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 3182 */	NdrFcShort( 0x10 ),	/* 16 */
/* 3184 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3186 */	NdrFcShort( 0x6 ),	/* Offset= 6 (3192) */
/* 3188 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 3190 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 3192 */	
			0x12, 0x0,	/* FC_UP */
/* 3194 */	NdrFcShort( 0xf934 ),	/* Offset= -1740 (1454) */
/* 3196 */	
			0x11, 0x0,	/* FC_RP */
/* 3198 */	NdrFcShort( 0xffee ),	/* Offset= -18 (3180) */
/* 3200 */	
			0x11, 0x0,	/* FC_RP */
/* 3202 */	NdrFcShort( 0x2 ),	/* Offset= 2 (3204) */
/* 3204 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0x9,		/* FC_ULONG */
/* 3206 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 3208 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 3210 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 3212 */	NdrFcShort( 0x2 ),	/* Offset= 2 (3214) */
/* 3214 */	NdrFcShort( 0x138 ),	/* 312 */
/* 3216 */	NdrFcShort( 0x3004 ),	/* 12292 */
/* 3218 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3222 */	NdrFcShort( 0xfdf8 ),	/* Offset= -520 (2702) */
/* 3224 */	NdrFcLong( 0x1 ),	/* 1 */
/* 3228 */	NdrFcShort( 0xfe68 ),	/* Offset= -408 (2820) */
/* 3230 */	NdrFcLong( 0x2 ),	/* 2 */
/* 3234 */	NdrFcShort( 0xfeae ),	/* Offset= -338 (2896) */
/* 3236 */	NdrFcLong( 0x3 ),	/* 3 */
/* 3240 */	NdrFcShort( 0xfefc ),	/* Offset= -260 (2980) */
/* 3242 */	NdrFcShort( 0x0 ),	/* Offset= 0 (3242) */
/* 3244 */	
			0x11, 0x0,	/* FC_RP */
/* 3246 */	NdrFcShort( 0x8 ),	/* Offset= 8 (3254) */
/* 3248 */	
			0x1d,		/* FC_SMFARRAY */
			0x0,		/* 0 */
/* 3250 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3252 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 3254 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 3256 */	NdrFcShort( 0x10 ),	/* 16 */
/* 3258 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 3260 */	0x6,		/* FC_SHORT */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 3262 */	0x0,		/* 0 */
			NdrFcShort( 0xfff1 ),	/* Offset= -15 (3248) */
			0x5b,		/* FC_END */
/* 3266 */	
			0x11, 0x0,	/* FC_RP */
/* 3268 */	NdrFcShort( 0x2a ),	/* Offset= 42 (3310) */
/* 3270 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 3272 */	NdrFcShort( 0x18 ),	/* 24 */
/* 3274 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3276 */	NdrFcShort( 0x8 ),	/* Offset= 8 (3284) */
/* 3278 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 3280 */	NdrFcShort( 0xffe6 ),	/* Offset= -26 (3254) */
/* 3282 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 3284 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 3286 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 3288 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 3290 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3292 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 3294 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3296 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 3298 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 3302 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 3304 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 3306 */	NdrFcShort( 0xffdc ),	/* Offset= -36 (3270) */
/* 3308 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 3310 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 3312 */	NdrFcShort( 0x10 ),	/* 16 */
/* 3314 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3316 */	NdrFcShort( 0x6 ),	/* Offset= 6 (3322) */
/* 3318 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 3320 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 3322 */	
			0x12, 0x0,	/* FC_UP */
/* 3324 */	NdrFcShort( 0xffdc ),	/* Offset= -36 (3288) */
/* 3326 */	0xb7,		/* FC_RANGE */
			0x8,		/* 8 */
/* 3328 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3332 */	NdrFcLong( 0x20 ),	/* 32 */
/* 3336 */	
			0x11, 0x0,	/* FC_RP */
/* 3338 */	NdrFcShort( 0x2 ),	/* Offset= 2 (3340) */
/* 3340 */	
			0x1b,		/* FC_CARRAY */
			0x1,		/* 1 */
/* 3342 */	NdrFcShort( 0x2 ),	/* 2 */
/* 3344 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 3346 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 3348 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 3350 */	0x5,		/* FC_WCHAR */
			0x5b,		/* FC_END */
/* 3352 */	
			0x12, 0x14,	/* FC_UP [alloced_on_stack] [pointer_deref] */
/* 3354 */	NdrFcShort( 0x2 ),	/* Offset= 2 (3356) */
/* 3356 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 3358 */	NdrFcShort( 0x28 ),	/* Offset= 40 (3398) */
/* 3360 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 3362 */	NdrFcShort( 0x10 ),	/* 16 */
/* 3364 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3366 */	NdrFcShort( 0x6 ),	/* Offset= 6 (3372) */
/* 3368 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 3370 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 3372 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 3374 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 3376 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 3378 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3380 */	0x9,		/* Corr desc: FC_ULONG */
			0x0,		/*  */
/* 3382 */	NdrFcShort( 0xfff8 ),	/* -8 */
/* 3384 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 3386 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 3390 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 3392 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 3394 */	NdrFcShort( 0xffde ),	/* Offset= -34 (3360) */
/* 3396 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 3398 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 3400 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3402 */	NdrFcShort( 0xffe6 ),	/* Offset= -26 (3376) */
/* 3404 */	NdrFcShort( 0x0 ),	/* Offset= 0 (3404) */
/* 3406 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 3408 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 3410 */	
			0x11, 0x0,	/* FC_RP */
/* 3412 */	NdrFcShort( 0x2 ),	/* Offset= 2 (3414) */
/* 3414 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0x9,		/* FC_ULONG */
/* 3416 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 3418 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 3420 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 3422 */	NdrFcShort( 0xff30 ),	/* Offset= -208 (3214) */

			0x0
        }
    };

static const GENERIC_BINDING_ROUTINE_PAIR BindingRoutines[ GENERIC_BINDING_TABLE_SIZE ] = 
        {
        {
            (GENERIC_BINDING_ROUTINE)SRVSVC_HANDLE_bind,
            (GENERIC_UNBIND_ROUTINE)SRVSVC_HANDLE_unbind
         }
        
        };


static const unsigned short srvsvc_FormatStringOffsetTable[] =
    {
    0,
    68,
    130,
    186,
    260,
    328,
    396,
    446,
    502,
    576,
    656,
    718,
    768,
    848,
    904,
    966,
    1034,
    1096,
    1164,
    1220,
    1276,
    1332,
    1388,
    1450,
    1524,
    1592,
    1648,
    1716,
    1772,
    1822,
    1884,
    1946,
    2026,
    2094,
    2156,
    2230,
    2298,
    2366,
    2428,
    2472,
    2540,
    2608,
    2664,
    2738,
    2788,
    2868,
    2924,
    2986,
    3042,
    3116,
    3178,
    3234,
    3326,
    3376
    };


static const MIDL_STUB_DESC srvsvc_StubDesc = 
    {
    (void *)& srvsvc___RpcClientInterface,
    MIDL_user_allocate,
    MIDL_user_free,
    &srvsvc_bhandle,
    0,
    BindingRoutines,
    0,
    0,
    __MIDL_TypeFormatString.Format,
    0, /* -error bounds_check flag */
    0x50002, /* Ndr library version */
    0,
    0x6000169, /* MIDL Version 6.0.361 */
    0,
    0,
    0,  /* notify & notify_flag routine table */
    0x1, /* MIDL flag */
    0, /* cs routines */
    0,   /* proxy/server info */
    0   /* Reserved5 */
    };
#if _MSC_VER >= 1200
#pragma warning(pop)
#endif


#endif /* defined(_M_IA64) || defined(_M_AMD64)*/

