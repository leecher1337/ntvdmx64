

/* this ALWAYS GENERATED file contains the RPC client stubs */


 /* File created by MIDL compiler version 6.00.0361 */
/* at Mon Mar 27 00:27:27 2023
 */
/* Compiler settings for .\bowser.idl:
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

#include "bowser.h"

#define TYPE_FORMAT_STRING_SIZE   579                               
#define PROC_FORMAT_STRING_SIZE   361                               
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


/* Standard interface: browser, ver. 0.0,
   GUID={0x6BFFD098,0xA112,0x3610,{0x98,0x33,0x01,0x28,0x92,0x02,0x01,0x62}} */

handle_t browser_bhandle;


static const RPC_CLIENT_INTERFACE browser___RpcClientInterface =
    {
    sizeof(RPC_CLIENT_INTERFACE),
    {{0x6BFFD098,0xA112,0x3610,{0x98,0x33,0x01,0x28,0x92,0x02,0x01,0x62}},{0,0}},
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    0,
    0,
    0,
    0,
    0x00000000
    };
RPC_IF_HANDLE browser_ClientIfHandle = (RPC_IF_HANDLE)& browser___RpcClientInterface;

extern const MIDL_STUB_DESC browser_StubDesc;

static RPC_BINDING_HANDLE browser__MIDL_AutoBindHandle;


DWORD I_BrowserrServerEnum( 
    /* [unique][string][in] */ BROWSER_IDENTIFY_HANDLE ServerName,
    /* [unique][string][in] */ wchar_t *TransportName,
    /* [unique][string][in] */ wchar_t *ClientName,
    /* [out][in] */ LPSERVER_ENUM_STRUCT InfoStruct,
    /* [in] */ DWORD PreferedMaximumLength,
    /* [out] */ LPDWORD TotalEntries,
    /* [in] */ DWORD ServerType,
    /* [unique][string][in] */ wchar_t *Domain,
    /* [unique][out][in] */ LPDWORD ResumeHandle)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&browser_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[0],
                 ( unsigned char * )&ServerName);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD I_BrowserrDebugCall( 
    /* [unique][string][in] */ BROWSER_IDENTIFY_HANDLE ServerName,
    /* [in] */ DWORD DebugFunction,
    /* [in] */ DWORD OptionalValue)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&browser_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[50],
                 ( unsigned char * )&ServerName);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD I_BrowserrQueryOtherDomains( 
    /* [unique][string][in] */ BROWSER_IDENTIFY_HANDLE ServerName,
    /* [out][in] */ LPSERVER_ENUM_STRUCT InfoStruct,
    /* [out] */ LPDWORD TotalEntries)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&browser_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[76],
                 ( unsigned char * )&ServerName);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD I_BrowserrResetNetlogonState( 
    /* [unique][string][in] */ BROWSER_IDENTIFY_HANDLE ServerName)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&browser_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[106],
                 ( unsigned char * )&ServerName);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD I_BrowserrDebugTrace( 
    /* [unique][string][in] */ BROWSER_IDENTIFY_HANDLE ServerName,
    /* [string][in] */ LPSTR TraceString)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&browser_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[128],
                 ( unsigned char * )&ServerName);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD I_BrowserrQueryStatistics( 
    /* [unique][string][in] */ BROWSER_IDENTIFY_HANDLE servername,
    /* [out] */ LPBROWSER_STATISTICS *statistics)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&browser_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[154],
                 ( unsigned char * )&servername);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD I_BrowserrResetStatistics( 
    /* [unique][string][in] */ BROWSER_IDENTIFY_HANDLE servername)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&browser_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[180],
                 ( unsigned char * )&servername);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetrBrowserStatisticsClear( 
    /* [unique][string][in] */ BROWSER_IDENTIFY_HANDLE servername)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&browser_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[202],
                 ( unsigned char * )&servername);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetrBrowserStatisticsGet( 
    /* [unique][string][in] */ BROWSER_IDENTIFY_HANDLE servername,
    /* [in] */ DWORD Level,
    /* [out][in] */ LPBROWSER_STATISTICS_STRUCT StatisticsStruct)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&browser_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[224],
                 ( unsigned char * )&servername);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD I_BrowserrSetNetlogonState( 
    /* [unique][string][in] */ BROWSER_IDENTIFY_HANDLE ServerName,
    /* [string][in] */ wchar_t *DomainName,
    /* [unique][string][in] */ wchar_t *EmulatedComputerName,
    /* [in] */ DWORD Role)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&browser_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[252],
                 ( unsigned char * )&ServerName);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD I_BrowserrQueryEmulatedDomains( 
    /* [unique][string][in] */ BROWSER_IDENTIFY_HANDLE ServerName,
    /* [out][in] */ PBROWSER_EMULATED_DOMAIN_CONTAINER EmulatedDomains)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&browser_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[284],
                 ( unsigned char * )&ServerName);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD I_BrowserrServerEnumEx( 
    /* [unique][string][in] */ BROWSER_IDENTIFY_HANDLE ServerName,
    /* [unique][string][in] */ wchar_t *TransportName,
    /* [unique][string][in] */ wchar_t *ClientName,
    /* [out][in] */ LPSERVER_ENUM_STRUCT InfoStruct,
    /* [in] */ DWORD PreferedMaximumLength,
    /* [out] */ LPDWORD TotalEntries,
    /* [in] */ DWORD ServerType,
    /* [unique][string][in] */ wchar_t *Domain,
    /* [unique][string][in] */ wchar_t *FirstNameToReturn)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&browser_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[310],
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
/*  8 */	NdrFcShort( 0x28 ),	/* x86 Stack size/offset = 40 */
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
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 22 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 24 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 26 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 28 */	
			0x50,		/* FC_IN_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 30 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */
/* 32 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 34 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 36 */	NdrFcShort( 0xe0 ),	/* Type Offset=224 */
/* 38 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 40 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 42 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 44 */	
			0x50,		/* FC_IN_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 46 */	NdrFcShort( 0xe4 ),	/* Type Offset=228 */
/* 48 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 50 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 52 */	NdrFcLong( 0x0 ),	/* 0 */
/* 56 */	NdrFcShort( 0x1 ),	/* 1 */
/* 58 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 60 */	0x31,		/* FC_BIND_GENERIC */
			0x4,		/* 4 */
/* 62 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 64 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 66 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 68 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 70 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 72 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 74 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 76 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 78 */	NdrFcLong( 0x0 ),	/* 0 */
/* 82 */	NdrFcShort( 0x2 ),	/* 2 */
/* 84 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 86 */	0x31,		/* FC_BIND_GENERIC */
			0x4,		/* 4 */
/* 88 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 90 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 92 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 94 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 96 */	
			0x50,		/* FC_IN_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 98 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */
/* 100 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 102 */	NdrFcShort( 0xe0 ),	/* Type Offset=224 */
/* 104 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 106 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 108 */	NdrFcLong( 0x0 ),	/* 0 */
/* 112 */	NdrFcShort( 0x3 ),	/* 3 */
/* 114 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 116 */	0x31,		/* FC_BIND_GENERIC */
			0x4,		/* 4 */
/* 118 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 120 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 122 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 124 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 126 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 128 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 130 */	NdrFcLong( 0x0 ),	/* 0 */
/* 134 */	NdrFcShort( 0x4 ),	/* 4 */
/* 136 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 138 */	0x31,		/* FC_BIND_GENERIC */
			0x4,		/* 4 */
/* 140 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 142 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 144 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 146 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 148 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 150 */	NdrFcShort( 0xe8 ),	/* Type Offset=232 */
/* 152 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 154 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 156 */	NdrFcLong( 0x0 ),	/* 0 */
/* 160 */	NdrFcShort( 0x5 ),	/* 5 */
/* 162 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 164 */	0x31,		/* FC_BIND_GENERIC */
			0x4,		/* 4 */
/* 166 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 168 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 170 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 172 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 174 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 176 */	NdrFcShort( 0xec ),	/* Type Offset=236 */
/* 178 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 180 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 182 */	NdrFcLong( 0x0 ),	/* 0 */
/* 186 */	NdrFcShort( 0x6 ),	/* 6 */
/* 188 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 190 */	0x31,		/* FC_BIND_GENERIC */
			0x4,		/* 4 */
/* 192 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 194 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 196 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 198 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 200 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 202 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 204 */	NdrFcLong( 0x0 ),	/* 0 */
/* 208 */	NdrFcShort( 0x7 ),	/* 7 */
/* 210 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 212 */	0x31,		/* FC_BIND_GENERIC */
			0x4,		/* 4 */
/* 214 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 216 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 218 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 220 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 222 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 224 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 226 */	NdrFcLong( 0x0 ),	/* 0 */
/* 230 */	NdrFcShort( 0x8 ),	/* 8 */
/* 232 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 234 */	0x31,		/* FC_BIND_GENERIC */
			0x4,		/* 4 */
/* 236 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 238 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 240 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 242 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 244 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 246 */	
			0x50,		/* FC_IN_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 248 */	NdrFcShort( 0x120 ),	/* Type Offset=288 */
/* 250 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 252 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 254 */	NdrFcLong( 0x0 ),	/* 0 */
/* 258 */	NdrFcShort( 0x9 ),	/* 9 */
/* 260 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 262 */	0x31,		/* FC_BIND_GENERIC */
			0x4,		/* 4 */
/* 264 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 266 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 268 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 270 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 272 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 274 */	NdrFcShort( 0x1de ),	/* Type Offset=478 */
/* 276 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 278 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 280 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 282 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 284 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 286 */	NdrFcLong( 0x0 ),	/* 0 */
/* 290 */	NdrFcShort( 0xa ),	/* 10 */
/* 292 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 294 */	0x31,		/* FC_BIND_GENERIC */
			0x4,		/* 4 */
/* 296 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 298 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 300 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 302 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 304 */	
			0x50,		/* FC_IN_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 306 */	NdrFcShort( 0x1e2 ),	/* Type Offset=482 */
/* 308 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 310 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 312 */	NdrFcLong( 0x0 ),	/* 0 */
/* 316 */	NdrFcShort( 0xb ),	/* 11 */
/* 318 */	NdrFcShort( 0x28 ),	/* x86 Stack size/offset = 40 */
/* 320 */	0x31,		/* FC_BIND_GENERIC */
			0x4,		/* 4 */
/* 322 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 324 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 326 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 328 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 330 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 332 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 334 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 336 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 338 */	
			0x50,		/* FC_IN_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 340 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */
/* 342 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 344 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 346 */	NdrFcShort( 0xe0 ),	/* Type Offset=224 */
/* 348 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 350 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 352 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 354 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 356 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 358 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
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
/*  8 */	NdrFcShort( 0xca ),	/* Offset= 202 (210) */
/* 10 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0x9,		/* FC_ULONG */
/* 12 */	0x9,		/* Corr desc: FC_ULONG */
			0x0,		/*  */
/* 14 */	NdrFcShort( 0xfffc ),	/* -4 */
/* 16 */	NdrFcShort( 0x2 ),	/* Offset= 2 (18) */
/* 18 */	NdrFcShort( 0x4 ),	/* 4 */
/* 20 */	NdrFcShort( 0x3002 ),	/* 12290 */
/* 22 */	NdrFcLong( 0x64 ),	/* 100 */
/* 26 */	NdrFcShort( 0xa ),	/* Offset= 10 (36) */
/* 28 */	NdrFcLong( 0x65 ),	/* 101 */
/* 32 */	NdrFcShort( 0x50 ),	/* Offset= 80 (112) */
/* 34 */	NdrFcShort( 0x0 ),	/* Offset= 0 (34) */
/* 36 */	
			0x12, 0x0,	/* FC_UP */
/* 38 */	NdrFcShort( 0x36 ),	/* Offset= 54 (92) */
/* 40 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 42 */	NdrFcShort( 0x8 ),	/* 8 */
/* 44 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 46 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 48 */	NdrFcShort( 0x4 ),	/* 4 */
/* 50 */	NdrFcShort( 0x4 ),	/* 4 */
/* 52 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 54 */	0x5,		/* FC_WCHAR */
			0x5c,		/* FC_PAD */
/* 56 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 58 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 60 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 62 */	NdrFcShort( 0x8 ),	/* 8 */
/* 64 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 66 */	NdrFcShort( 0x0 ),	/* 0 */
/* 68 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 70 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 72 */	NdrFcShort( 0x8 ),	/* 8 */
/* 74 */	NdrFcShort( 0x0 ),	/* 0 */
/* 76 */	NdrFcShort( 0x1 ),	/* 1 */
/* 78 */	NdrFcShort( 0x4 ),	/* 4 */
/* 80 */	NdrFcShort( 0x4 ),	/* 4 */
/* 82 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 84 */	0x5,		/* FC_WCHAR */
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
/* 114 */	NdrFcShort( 0x4c ),	/* Offset= 76 (190) */
/* 116 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 118 */	NdrFcShort( 0x18 ),	/* 24 */
/* 120 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 122 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 124 */	NdrFcShort( 0x4 ),	/* 4 */
/* 126 */	NdrFcShort( 0x4 ),	/* 4 */
/* 128 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 130 */	0x5,		/* FC_WCHAR */
			0x5c,		/* FC_PAD */
/* 132 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 134 */	NdrFcShort( 0x14 ),	/* 20 */
/* 136 */	NdrFcShort( 0x14 ),	/* 20 */
/* 138 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 140 */	0x5,		/* FC_WCHAR */
			0x5c,		/* FC_PAD */
/* 142 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 144 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 146 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 148 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 150 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 152 */	NdrFcShort( 0x18 ),	/* 24 */
/* 154 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 156 */	NdrFcShort( 0x0 ),	/* 0 */
/* 158 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 160 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 162 */	NdrFcShort( 0x18 ),	/* 24 */
/* 164 */	NdrFcShort( 0x0 ),	/* 0 */
/* 166 */	NdrFcShort( 0x2 ),	/* 2 */
/* 168 */	NdrFcShort( 0x4 ),	/* 4 */
/* 170 */	NdrFcShort( 0x4 ),	/* 4 */
/* 172 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 174 */	0x5,		/* FC_WCHAR */
			0x5c,		/* FC_PAD */
/* 176 */	NdrFcShort( 0x14 ),	/* 20 */
/* 178 */	NdrFcShort( 0x14 ),	/* 20 */
/* 180 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 182 */	0x5,		/* FC_WCHAR */
			0x5c,		/* FC_PAD */
/* 184 */	
			0x5b,		/* FC_END */

			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 186 */	0x0,		/* 0 */
			NdrFcShort( 0xffb9 ),	/* Offset= -71 (116) */
			0x5b,		/* FC_END */
/* 190 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 192 */	NdrFcShort( 0x8 ),	/* 8 */
/* 194 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 196 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 198 */	NdrFcShort( 0x4 ),	/* 4 */
/* 200 */	NdrFcShort( 0x4 ),	/* 4 */
/* 202 */	0x12, 0x1,	/* FC_UP [all_nodes] */
/* 204 */	NdrFcShort( 0xffca ),	/* Offset= -54 (150) */
/* 206 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 208 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 210 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 212 */	NdrFcShort( 0x8 ),	/* 8 */
/* 214 */	NdrFcShort( 0x0 ),	/* 0 */
/* 216 */	NdrFcShort( 0x0 ),	/* Offset= 0 (216) */
/* 218 */	0x8,		/* FC_LONG */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 220 */	0x0,		/* 0 */
			NdrFcShort( 0xff2d ),	/* Offset= -211 (10) */
			0x5b,		/* FC_END */
/* 224 */	
			0x11, 0x8,	/* FC_RP [simple_pointer] */
/* 226 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 228 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 230 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 232 */	
			0x11, 0x8,	/* FC_RP [simple_pointer] */
/* 234 */	
			0x22,		/* FC_C_CSTRING */
			0x5c,		/* FC_PAD */
/* 236 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 238 */	NdrFcShort( 0x2 ),	/* Offset= 2 (240) */
/* 240 */	
			0x12, 0x0,	/* FC_UP */
/* 242 */	NdrFcShort( 0x8 ),	/* Offset= 8 (250) */
/* 244 */	
			0x15,		/* FC_STRUCT */
			0x7,		/* 7 */
/* 246 */	NdrFcShort( 0x8 ),	/* 8 */
/* 248 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 250 */	
			0x15,		/* FC_STRUCT */
			0x7,		/* 7 */
/* 252 */	NdrFcShort( 0x60 ),	/* 96 */
/* 254 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 256 */	NdrFcShort( 0xfff4 ),	/* Offset= -12 (244) */
/* 258 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 260 */	NdrFcShort( 0xfff0 ),	/* Offset= -16 (244) */
/* 262 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 264 */	NdrFcShort( 0xffec ),	/* Offset= -20 (244) */
/* 266 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 268 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 270 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 272 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 274 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 276 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 278 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 280 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 282 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 284 */	NdrFcShort( 0xffd8 ),	/* Offset= -40 (244) */
/* 286 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 288 */	
			0x11, 0x0,	/* FC_RP */
/* 290 */	NdrFcShort( 0xae ),	/* Offset= 174 (464) */
/* 292 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0x9,		/* FC_ULONG */
/* 294 */	0x9,		/* Corr desc: FC_ULONG */
			0x0,		/*  */
/* 296 */	NdrFcShort( 0xfffc ),	/* -4 */
/* 298 */	NdrFcShort( 0x2 ),	/* Offset= 2 (300) */
/* 300 */	NdrFcShort( 0x4 ),	/* 4 */
/* 302 */	NdrFcShort( 0x3002 ),	/* 12290 */
/* 304 */	NdrFcLong( 0x64 ),	/* 100 */
/* 308 */	NdrFcShort( 0xa ),	/* Offset= 10 (318) */
/* 310 */	NdrFcLong( 0x65 ),	/* 101 */
/* 314 */	NdrFcShort( 0x44 ),	/* Offset= 68 (382) */
/* 316 */	NdrFcShort( 0x0 ),	/* Offset= 0 (316) */
/* 318 */	
			0x12, 0x0,	/* FC_UP */
/* 320 */	NdrFcShort( 0x2a ),	/* Offset= 42 (362) */
/* 322 */	
			0x15,		/* FC_STRUCT */
			0x7,		/* 7 */
/* 324 */	NdrFcShort( 0x30 ),	/* 48 */
/* 326 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 328 */	NdrFcShort( 0xffac ),	/* Offset= -84 (244) */
/* 330 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 332 */	NdrFcShort( 0xffa8 ),	/* Offset= -88 (244) */
/* 334 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 336 */	NdrFcShort( 0xffa4 ),	/* Offset= -92 (244) */
/* 338 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 340 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 342 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 344 */	NdrFcShort( 0xff9c ),	/* Offset= -100 (244) */
/* 346 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 348 */	
			0x1b,		/* FC_CARRAY */
			0x7,		/* 7 */
/* 350 */	NdrFcShort( 0x30 ),	/* 48 */
/* 352 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 354 */	NdrFcShort( 0x0 ),	/* 0 */
/* 356 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 358 */	NdrFcShort( 0xffdc ),	/* Offset= -36 (322) */
/* 360 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 362 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 364 */	NdrFcShort( 0x8 ),	/* 8 */
/* 366 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 368 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 370 */	NdrFcShort( 0x4 ),	/* 4 */
/* 372 */	NdrFcShort( 0x4 ),	/* 4 */
/* 374 */	0x12, 0x0,	/* FC_UP */
/* 376 */	NdrFcShort( 0xffe4 ),	/* Offset= -28 (348) */
/* 378 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 380 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 382 */	
			0x12, 0x0,	/* FC_UP */
/* 384 */	NdrFcShort( 0x3c ),	/* Offset= 60 (444) */
/* 386 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x7,		/* 7 */
/* 388 */	NdrFcShort( 0x58 ),	/* 88 */
/* 390 */	NdrFcShort( 0x0 ),	/* 0 */
/* 392 */	NdrFcShort( 0x0 ),	/* Offset= 0 (392) */
/* 394 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 396 */	NdrFcShort( 0xff68 ),	/* Offset= -152 (244) */
/* 398 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 400 */	NdrFcShort( 0xff64 ),	/* Offset= -156 (244) */
/* 402 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 404 */	NdrFcShort( 0xff60 ),	/* Offset= -160 (244) */
/* 406 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 408 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 410 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 412 */	NdrFcShort( 0xff58 ),	/* Offset= -168 (244) */
/* 414 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 416 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 418 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 420 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 422 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 424 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 426 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x7,		/* 7 */
/* 428 */	NdrFcShort( 0x0 ),	/* 0 */
/* 430 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 432 */	NdrFcShort( 0x0 ),	/* 0 */
/* 434 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 438 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 440 */	NdrFcShort( 0xffca ),	/* Offset= -54 (386) */
/* 442 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 444 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 446 */	NdrFcShort( 0x8 ),	/* 8 */
/* 448 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 450 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 452 */	NdrFcShort( 0x4 ),	/* 4 */
/* 454 */	NdrFcShort( 0x4 ),	/* 4 */
/* 456 */	0x12, 0x0,	/* FC_UP */
/* 458 */	NdrFcShort( 0xffe0 ),	/* Offset= -32 (426) */
/* 460 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 462 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 464 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 466 */	NdrFcShort( 0x8 ),	/* 8 */
/* 468 */	NdrFcShort( 0x0 ),	/* 0 */
/* 470 */	NdrFcShort( 0x0 ),	/* Offset= 0 (470) */
/* 472 */	0x8,		/* FC_LONG */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 474 */	0x0,		/* 0 */
			NdrFcShort( 0xff49 ),	/* Offset= -183 (292) */
			0x5b,		/* FC_END */
/* 478 */	
			0x11, 0x8,	/* FC_RP [simple_pointer] */
/* 480 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 482 */	
			0x11, 0x0,	/* FC_RP */
/* 484 */	NdrFcShort( 0x4a ),	/* Offset= 74 (558) */
/* 486 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 488 */	NdrFcShort( 0xc ),	/* 12 */
/* 490 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 492 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 494 */	NdrFcShort( 0x0 ),	/* 0 */
/* 496 */	NdrFcShort( 0x0 ),	/* 0 */
/* 498 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 500 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 502 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 504 */	NdrFcShort( 0x4 ),	/* 4 */
/* 506 */	NdrFcShort( 0x4 ),	/* 4 */
/* 508 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 510 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 512 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 514 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 516 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 518 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 520 */	NdrFcShort( 0xc ),	/* 12 */
/* 522 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 524 */	NdrFcShort( 0x0 ),	/* 0 */
/* 526 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 528 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 530 */	NdrFcShort( 0xc ),	/* 12 */
/* 532 */	NdrFcShort( 0x0 ),	/* 0 */
/* 534 */	NdrFcShort( 0x2 ),	/* 2 */
/* 536 */	NdrFcShort( 0x0 ),	/* 0 */
/* 538 */	NdrFcShort( 0x0 ),	/* 0 */
/* 540 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 542 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 544 */	NdrFcShort( 0x4 ),	/* 4 */
/* 546 */	NdrFcShort( 0x4 ),	/* 4 */
/* 548 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 550 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 552 */	
			0x5b,		/* FC_END */

			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 554 */	0x0,		/* 0 */
			NdrFcShort( 0xffbb ),	/* Offset= -69 (486) */
			0x5b,		/* FC_END */
/* 558 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 560 */	NdrFcShort( 0x8 ),	/* 8 */
/* 562 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 564 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 566 */	NdrFcShort( 0x4 ),	/* 4 */
/* 568 */	NdrFcShort( 0x4 ),	/* 4 */
/* 570 */	0x12, 0x1,	/* FC_UP [all_nodes] */
/* 572 */	NdrFcShort( 0xffca ),	/* Offset= -54 (518) */
/* 574 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 576 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */

			0x0
        }
    };

static const GENERIC_BINDING_ROUTINE_PAIR BindingRoutines[ GENERIC_BINDING_TABLE_SIZE ] = 
        {
        {
            (GENERIC_BINDING_ROUTINE)BROWSER_IDENTIFY_HANDLE_bind,
            (GENERIC_UNBIND_ROUTINE)BROWSER_IDENTIFY_HANDLE_unbind
         }
        
        };


static const unsigned short browser_FormatStringOffsetTable[] =
    {
    0,
    50,
    76,
    106,
    128,
    154,
    180,
    202,
    224,
    252,
    284,
    310
    };


static const MIDL_STUB_DESC browser_StubDesc = 
    {
    (void *)& browser___RpcClientInterface,
    MIDL_user_allocate,
    MIDL_user_free,
    &browser_bhandle,
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
/* at Mon Mar 27 00:27:27 2023
 */
/* Compiler settings for .\bowser.idl:
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

#include "bowser.h"

#define TYPE_FORMAT_STRING_SIZE   505                               
#define PROC_FORMAT_STRING_SIZE   697                               
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


/* Standard interface: browser, ver. 0.0,
   GUID={0x6BFFD098,0xA112,0x3610,{0x98,0x33,0x01,0x28,0x92,0x02,0x01,0x62}} */

handle_t browser_bhandle;


static const RPC_CLIENT_INTERFACE browser___RpcClientInterface =
    {
    sizeof(RPC_CLIENT_INTERFACE),
    {{0x6BFFD098,0xA112,0x3610,{0x98,0x33,0x01,0x28,0x92,0x02,0x01,0x62}},{0,0}},
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    0,
    0,
    0,
    0,
    0x00000000
    };
RPC_IF_HANDLE browser_ClientIfHandle = (RPC_IF_HANDLE)& browser___RpcClientInterface;

extern const MIDL_STUB_DESC browser_StubDesc;

static RPC_BINDING_HANDLE browser__MIDL_AutoBindHandle;


DWORD I_BrowserrServerEnum( 
    /* [unique][string][in] */ BROWSER_IDENTIFY_HANDLE ServerName,
    /* [unique][string][in] */ wchar_t *TransportName,
    /* [unique][string][in] */ wchar_t *ClientName,
    /* [out][in] */ LPSERVER_ENUM_STRUCT InfoStruct,
    /* [in] */ DWORD PreferedMaximumLength,
    /* [out] */ LPDWORD TotalEntries,
    /* [in] */ DWORD ServerType,
    /* [unique][string][in] */ wchar_t *Domain,
    /* [unique][out][in] */ LPDWORD ResumeHandle)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&browser_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[0],
                  ServerName,
                  TransportName,
                  ClientName,
                  InfoStruct,
                  PreferedMaximumLength,
                  TotalEntries,
                  ServerType,
                  Domain,
                  ResumeHandle);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD I_BrowserrDebugCall( 
    /* [unique][string][in] */ BROWSER_IDENTIFY_HANDLE ServerName,
    /* [in] */ DWORD DebugFunction,
    /* [in] */ DWORD OptionalValue)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&browser_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[92],
                  ServerName,
                  DebugFunction,
                  OptionalValue);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD I_BrowserrQueryOtherDomains( 
    /* [unique][string][in] */ BROWSER_IDENTIFY_HANDLE ServerName,
    /* [out][in] */ LPSERVER_ENUM_STRUCT InfoStruct,
    /* [out] */ LPDWORD TotalEntries)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&browser_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[148],
                  ServerName,
                  InfoStruct,
                  TotalEntries);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD I_BrowserrResetNetlogonState( 
    /* [unique][string][in] */ BROWSER_IDENTIFY_HANDLE ServerName)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&browser_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[204],
                  ServerName);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD I_BrowserrDebugTrace( 
    /* [unique][string][in] */ BROWSER_IDENTIFY_HANDLE ServerName,
    /* [string][in] */ LPSTR TraceString)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&browser_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[248],
                  ServerName,
                  TraceString);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD I_BrowserrQueryStatistics( 
    /* [unique][string][in] */ BROWSER_IDENTIFY_HANDLE servername,
    /* [out] */ LPBROWSER_STATISTICS *statistics)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&browser_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[298],
                  servername,
                  statistics);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD I_BrowserrResetStatistics( 
    /* [unique][string][in] */ BROWSER_IDENTIFY_HANDLE servername)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&browser_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[348],
                  servername);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetrBrowserStatisticsClear( 
    /* [unique][string][in] */ BROWSER_IDENTIFY_HANDLE servername)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&browser_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[392],
                  servername);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD NetrBrowserStatisticsGet( 
    /* [unique][string][in] */ BROWSER_IDENTIFY_HANDLE servername,
    /* [in] */ DWORD Level,
    /* [out][in] */ LPBROWSER_STATISTICS_STRUCT StatisticsStruct)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&browser_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[436],
                  servername,
                  Level,
                  StatisticsStruct);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD I_BrowserrSetNetlogonState( 
    /* [unique][string][in] */ BROWSER_IDENTIFY_HANDLE ServerName,
    /* [string][in] */ wchar_t *DomainName,
    /* [unique][string][in] */ wchar_t *EmulatedComputerName,
    /* [in] */ DWORD Role)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&browser_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[492],
                  ServerName,
                  DomainName,
                  EmulatedComputerName,
                  Role);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD I_BrowserrQueryEmulatedDomains( 
    /* [unique][string][in] */ BROWSER_IDENTIFY_HANDLE ServerName,
    /* [out][in] */ PBROWSER_EMULATED_DOMAIN_CONTAINER EmulatedDomains)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&browser_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[554],
                  ServerName,
                  EmulatedDomains);
    return ( DWORD  )_RetVal.Simple;
    
}


DWORD I_BrowserrServerEnumEx( 
    /* [unique][string][in] */ BROWSER_IDENTIFY_HANDLE ServerName,
    /* [unique][string][in] */ wchar_t *TransportName,
    /* [unique][string][in] */ wchar_t *ClientName,
    /* [out][in] */ LPSERVER_ENUM_STRUCT InfoStruct,
    /* [in] */ DWORD PreferedMaximumLength,
    /* [out] */ LPDWORD TotalEntries,
    /* [in] */ DWORD ServerType,
    /* [unique][string][in] */ wchar_t *Domain,
    /* [unique][string][in] */ wchar_t *FirstNameToReturn)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&browser_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[604],
                  ServerName,
                  TransportName,
                  ClientName,
                  InfoStruct,
                  PreferedMaximumLength,
                  TotalEntries,
                  ServerType,
                  Domain,
                  FirstNameToReturn);
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

	/* Procedure I_BrowserrServerEnum */

			0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/*  2 */	NdrFcLong( 0x0 ),	/* 0 */
/*  6 */	NdrFcShort( 0x0 ),	/* 0 */
/*  8 */	NdrFcShort( 0x50 ),	/* ia64 Stack size/offset = 80 */
/* 10 */	0x31,		/* FC_BIND_GENERIC */
			0x8,		/* 8 */
/* 12 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 14 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 16 */	NdrFcShort( 0x2c ),	/* 44 */
/* 18 */	NdrFcShort( 0x40 ),	/* 64 */
/* 20 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0xa,		/* 10 */
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

	/* Parameter TransportName */

/* 38 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 40 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 42 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter ClientName */

/* 44 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 46 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 48 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter InfoStruct */

/* 50 */	NdrFcShort( 0x11b ),	/* Flags:  must size, must free, in, out, simple ref, */
/* 52 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 54 */	NdrFcShort( 0xa4 ),	/* Type Offset=164 */

	/* Parameter PreferedMaximumLength */

/* 56 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 58 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 60 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter TotalEntries */

/* 62 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 64 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 66 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ServerType */

/* 68 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 70 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 72 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter Domain */

/* 74 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 76 */	NdrFcShort( 0x38 ),	/* ia64 Stack size/offset = 56 */
/* 78 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter ResumeHandle */

/* 80 */	NdrFcShort( 0x1a ),	/* Flags:  must free, in, out, */
/* 82 */	NdrFcShort( 0x40 ),	/* ia64 Stack size/offset = 64 */
/* 84 */	NdrFcShort( 0xb8 ),	/* Type Offset=184 */

	/* Return value */

/* 86 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 88 */	NdrFcShort( 0x48 ),	/* ia64 Stack size/offset = 72 */
/* 90 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure I_BrowserrDebugCall */

/* 92 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 94 */	NdrFcLong( 0x0 ),	/* 0 */
/* 98 */	NdrFcShort( 0x1 ),	/* 1 */
/* 100 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 102 */	0x31,		/* FC_BIND_GENERIC */
			0x8,		/* 8 */
/* 104 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 106 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 108 */	NdrFcShort( 0x10 ),	/* 16 */
/* 110 */	NdrFcShort( 0x8 ),	/* 8 */
/* 112 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 114 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 116 */	NdrFcShort( 0x0 ),	/* 0 */
/* 118 */	NdrFcShort( 0x0 ),	/* 0 */
/* 120 */	NdrFcShort( 0x0 ),	/* 0 */
/* 122 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ServerName */

/* 124 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 126 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 128 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter DebugFunction */

/* 130 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 132 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 134 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter OptionalValue */

/* 136 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 138 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 140 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 142 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 144 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 146 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure I_BrowserrQueryOtherDomains */

/* 148 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 150 */	NdrFcLong( 0x0 ),	/* 0 */
/* 154 */	NdrFcShort( 0x2 ),	/* 2 */
/* 156 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 158 */	0x31,		/* FC_BIND_GENERIC */
			0x8,		/* 8 */
/* 160 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 162 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 164 */	NdrFcShort( 0x0 ),	/* 0 */
/* 166 */	NdrFcShort( 0x24 ),	/* 36 */
/* 168 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 170 */	0xa,		/* 10 */
			0x7,		/* Ext Flags:  new corr desc, clt corr check, srv corr check, */
/* 172 */	NdrFcShort( 0x3 ),	/* 3 */
/* 174 */	NdrFcShort( 0x3 ),	/* 3 */
/* 176 */	NdrFcShort( 0x0 ),	/* 0 */
/* 178 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ServerName */

/* 180 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 182 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 184 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter InfoStruct */

/* 186 */	NdrFcShort( 0x11b ),	/* Flags:  must size, must free, in, out, simple ref, */
/* 188 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 190 */	NdrFcShort( 0xa4 ),	/* Type Offset=164 */

	/* Parameter TotalEntries */

/* 192 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 194 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 196 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 198 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 200 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 202 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure I_BrowserrResetNetlogonState */

/* 204 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 206 */	NdrFcLong( 0x0 ),	/* 0 */
/* 210 */	NdrFcShort( 0x3 ),	/* 3 */
/* 212 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 214 */	0x31,		/* FC_BIND_GENERIC */
			0x8,		/* 8 */
/* 216 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 218 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 220 */	NdrFcShort( 0x0 ),	/* 0 */
/* 222 */	NdrFcShort( 0x8 ),	/* 8 */
/* 224 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 226 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 228 */	NdrFcShort( 0x0 ),	/* 0 */
/* 230 */	NdrFcShort( 0x0 ),	/* 0 */
/* 232 */	NdrFcShort( 0x0 ),	/* 0 */
/* 234 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ServerName */

/* 236 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 238 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 240 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Return value */

/* 242 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 244 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 246 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure I_BrowserrDebugTrace */

/* 248 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 250 */	NdrFcLong( 0x0 ),	/* 0 */
/* 254 */	NdrFcShort( 0x4 ),	/* 4 */
/* 256 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 258 */	0x31,		/* FC_BIND_GENERIC */
			0x8,		/* 8 */
/* 260 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 262 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 264 */	NdrFcShort( 0x0 ),	/* 0 */
/* 266 */	NdrFcShort( 0x8 ),	/* 8 */
/* 268 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 270 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 272 */	NdrFcShort( 0x0 ),	/* 0 */
/* 274 */	NdrFcShort( 0x0 ),	/* 0 */
/* 276 */	NdrFcShort( 0x0 ),	/* 0 */
/* 278 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ServerName */

/* 280 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 282 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 284 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter TraceString */

/* 286 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 288 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 290 */	NdrFcShort( 0xbe ),	/* Type Offset=190 */

	/* Return value */

/* 292 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 294 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 296 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure I_BrowserrQueryStatistics */

/* 298 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 300 */	NdrFcLong( 0x0 ),	/* 0 */
/* 304 */	NdrFcShort( 0x5 ),	/* 5 */
/* 306 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 308 */	0x31,		/* FC_BIND_GENERIC */
			0x8,		/* 8 */
/* 310 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 312 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 314 */	NdrFcShort( 0x0 ),	/* 0 */
/* 316 */	NdrFcShort( 0xe0 ),	/* 224 */
/* 318 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 320 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 322 */	NdrFcShort( 0x0 ),	/* 0 */
/* 324 */	NdrFcShort( 0x0 ),	/* 0 */
/* 326 */	NdrFcShort( 0x0 ),	/* 0 */
/* 328 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter servername */

/* 330 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 332 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 334 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter statistics */

/* 336 */	NdrFcShort( 0x2012 ),	/* Flags:  must free, out, srv alloc size=8 */
/* 338 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 340 */	NdrFcShort( 0xc0 ),	/* Type Offset=192 */

	/* Return value */

/* 342 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 344 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 346 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure I_BrowserrResetStatistics */

/* 348 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 350 */	NdrFcLong( 0x0 ),	/* 0 */
/* 354 */	NdrFcShort( 0x6 ),	/* 6 */
/* 356 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 358 */	0x31,		/* FC_BIND_GENERIC */
			0x8,		/* 8 */
/* 360 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 362 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 364 */	NdrFcShort( 0x0 ),	/* 0 */
/* 366 */	NdrFcShort( 0x8 ),	/* 8 */
/* 368 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 370 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 372 */	NdrFcShort( 0x0 ),	/* 0 */
/* 374 */	NdrFcShort( 0x0 ),	/* 0 */
/* 376 */	NdrFcShort( 0x0 ),	/* 0 */
/* 378 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter servername */

/* 380 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 382 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 384 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Return value */

/* 386 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 388 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 390 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure NetrBrowserStatisticsClear */

/* 392 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 394 */	NdrFcLong( 0x0 ),	/* 0 */
/* 398 */	NdrFcShort( 0x7 ),	/* 7 */
/* 400 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 402 */	0x31,		/* FC_BIND_GENERIC */
			0x8,		/* 8 */
/* 404 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 406 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 408 */	NdrFcShort( 0x0 ),	/* 0 */
/* 410 */	NdrFcShort( 0x8 ),	/* 8 */
/* 412 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 414 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 416 */	NdrFcShort( 0x0 ),	/* 0 */
/* 418 */	NdrFcShort( 0x0 ),	/* 0 */
/* 420 */	NdrFcShort( 0x0 ),	/* 0 */
/* 422 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter servername */

/* 424 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 426 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 428 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Return value */

/* 430 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 432 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 434 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure NetrBrowserStatisticsGet */

/* 436 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 438 */	NdrFcLong( 0x0 ),	/* 0 */
/* 442 */	NdrFcShort( 0x8 ),	/* 8 */
/* 444 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 446 */	0x31,		/* FC_BIND_GENERIC */
			0x8,		/* 8 */
/* 448 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 450 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 452 */	NdrFcShort( 0x8 ),	/* 8 */
/* 454 */	NdrFcShort( 0x8 ),	/* 8 */
/* 456 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 458 */	0xa,		/* 10 */
			0x7,		/* Ext Flags:  new corr desc, clt corr check, srv corr check, */
/* 460 */	NdrFcShort( 0x3 ),	/* 3 */
/* 462 */	NdrFcShort( 0x3 ),	/* 3 */
/* 464 */	NdrFcShort( 0x0 ),	/* 0 */
/* 466 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter servername */

/* 468 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 470 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 472 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter Level */

/* 474 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 476 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 478 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter StatisticsStruct */

/* 480 */	NdrFcShort( 0x11b ),	/* Flags:  must size, must free, in, out, simple ref, */
/* 482 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 484 */	NdrFcShort( 0x1a4 ),	/* Type Offset=420 */

	/* Return value */

/* 486 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 488 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 490 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure I_BrowserrSetNetlogonState */

/* 492 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 494 */	NdrFcLong( 0x0 ),	/* 0 */
/* 498 */	NdrFcShort( 0x9 ),	/* 9 */
/* 500 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 502 */	0x31,		/* FC_BIND_GENERIC */
			0x8,		/* 8 */
/* 504 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 506 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 508 */	NdrFcShort( 0x8 ),	/* 8 */
/* 510 */	NdrFcShort( 0x8 ),	/* 8 */
/* 512 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 514 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 516 */	NdrFcShort( 0x0 ),	/* 0 */
/* 518 */	NdrFcShort( 0x0 ),	/* 0 */
/* 520 */	NdrFcShort( 0x0 ),	/* 0 */
/* 522 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ServerName */

/* 524 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 526 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 528 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter DomainName */

/* 530 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 532 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 534 */	NdrFcShort( 0x1b6 ),	/* Type Offset=438 */

	/* Parameter EmulatedComputerName */

/* 536 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 538 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 540 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter Role */

/* 542 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 544 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 546 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 548 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 550 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 552 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure I_BrowserrQueryEmulatedDomains */

/* 554 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 556 */	NdrFcLong( 0x0 ),	/* 0 */
/* 560 */	NdrFcShort( 0xa ),	/* 10 */
/* 562 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 564 */	0x31,		/* FC_BIND_GENERIC */
			0x8,		/* 8 */
/* 566 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 568 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 570 */	NdrFcShort( 0x0 ),	/* 0 */
/* 572 */	NdrFcShort( 0x8 ),	/* 8 */
/* 574 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 576 */	0xa,		/* 10 */
			0x7,		/* Ext Flags:  new corr desc, clt corr check, srv corr check, */
/* 578 */	NdrFcShort( 0x1 ),	/* 1 */
/* 580 */	NdrFcShort( 0x1 ),	/* 1 */
/* 582 */	NdrFcShort( 0x0 ),	/* 0 */
/* 584 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ServerName */

/* 586 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 588 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 590 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter EmulatedDomains */

/* 592 */	NdrFcShort( 0x11b ),	/* Flags:  must size, must free, in, out, simple ref, */
/* 594 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 596 */	NdrFcShort( 0x1e8 ),	/* Type Offset=488 */

	/* Return value */

/* 598 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 600 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 602 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure I_BrowserrServerEnumEx */

/* 604 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 606 */	NdrFcLong( 0x0 ),	/* 0 */
/* 610 */	NdrFcShort( 0xb ),	/* 11 */
/* 612 */	NdrFcShort( 0x50 ),	/* ia64 Stack size/offset = 80 */
/* 614 */	0x31,		/* FC_BIND_GENERIC */
			0x8,		/* 8 */
/* 616 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 618 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 620 */	NdrFcShort( 0x10 ),	/* 16 */
/* 622 */	NdrFcShort( 0x24 ),	/* 36 */
/* 624 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0xa,		/* 10 */
/* 626 */	0xa,		/* 10 */
			0x7,		/* Ext Flags:  new corr desc, clt corr check, srv corr check, */
/* 628 */	NdrFcShort( 0x3 ),	/* 3 */
/* 630 */	NdrFcShort( 0x3 ),	/* 3 */
/* 632 */	NdrFcShort( 0x0 ),	/* 0 */
/* 634 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ServerName */

/* 636 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 638 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 640 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter TransportName */

/* 642 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 644 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 646 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter ClientName */

/* 648 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 650 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 652 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter InfoStruct */

/* 654 */	NdrFcShort( 0x11b ),	/* Flags:  must size, must free, in, out, simple ref, */
/* 656 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 658 */	NdrFcShort( 0xa4 ),	/* Type Offset=164 */

	/* Parameter PreferedMaximumLength */

/* 660 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 662 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 664 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter TotalEntries */

/* 666 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 668 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 670 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ServerType */

/* 672 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 674 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 676 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter Domain */

/* 678 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 680 */	NdrFcShort( 0x38 ),	/* ia64 Stack size/offset = 56 */
/* 682 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter FirstNameToReturn */

/* 684 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 686 */	NdrFcShort( 0x40 ),	/* ia64 Stack size/offset = 64 */
/* 688 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Return value */

/* 690 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 692 */	NdrFcShort( 0x48 ),	/* ia64 Stack size/offset = 72 */
/* 694 */	0x8,		/* FC_LONG */
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
/*  8 */	NdrFcShort( 0x9c ),	/* Offset= 156 (164) */
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
/* 24 */	NdrFcLong( 0x64 ),	/* 100 */
/* 28 */	NdrFcShort( 0xa ),	/* Offset= 10 (38) */
/* 30 */	NdrFcLong( 0x65 ),	/* 101 */
/* 34 */	NdrFcShort( 0x3e ),	/* Offset= 62 (96) */
/* 36 */	NdrFcShort( 0x0 ),	/* Offset= 0 (36) */
/* 38 */	
			0x12, 0x0,	/* FC_UP */
/* 40 */	NdrFcShort( 0x28 ),	/* Offset= 40 (80) */
/* 42 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 44 */	NdrFcShort( 0x10 ),	/* 16 */
/* 46 */	NdrFcShort( 0x0 ),	/* 0 */
/* 48 */	NdrFcShort( 0x6 ),	/* Offset= 6 (54) */
/* 50 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 52 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 54 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 56 */	0x5,		/* FC_WCHAR */
			0x5c,		/* FC_PAD */
/* 58 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 60 */	NdrFcShort( 0x0 ),	/* 0 */
/* 62 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 64 */	NdrFcShort( 0x0 ),	/* 0 */
/* 66 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 68 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 72 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 74 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 76 */	NdrFcShort( 0xffde ),	/* Offset= -34 (42) */
/* 78 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 80 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 82 */	NdrFcShort( 0x10 ),	/* 16 */
/* 84 */	NdrFcShort( 0x0 ),	/* 0 */
/* 86 */	NdrFcShort( 0x6 ),	/* Offset= 6 (92) */
/* 88 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 90 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 92 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 94 */	NdrFcShort( 0xffdc ),	/* Offset= -36 (58) */
/* 96 */	
			0x12, 0x0,	/* FC_UP */
/* 98 */	NdrFcShort( 0x32 ),	/* Offset= 50 (148) */
/* 100 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 102 */	NdrFcShort( 0x28 ),	/* 40 */
/* 104 */	NdrFcShort( 0x0 ),	/* 0 */
/* 106 */	NdrFcShort( 0xc ),	/* Offset= 12 (118) */
/* 108 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 110 */	0x36,		/* FC_POINTER */
			0x8,		/* FC_LONG */
/* 112 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 114 */	0x40,		/* FC_STRUCTPAD4 */
			0x36,		/* FC_POINTER */
/* 116 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 118 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 120 */	0x5,		/* FC_WCHAR */
			0x5c,		/* FC_PAD */
/* 122 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 124 */	0x5,		/* FC_WCHAR */
			0x5c,		/* FC_PAD */
/* 126 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 128 */	NdrFcShort( 0x0 ),	/* 0 */
/* 130 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 132 */	NdrFcShort( 0x0 ),	/* 0 */
/* 134 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 136 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 140 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 142 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 144 */	NdrFcShort( 0xffd4 ),	/* Offset= -44 (100) */
/* 146 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 148 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 150 */	NdrFcShort( 0x10 ),	/* 16 */
/* 152 */	NdrFcShort( 0x0 ),	/* 0 */
/* 154 */	NdrFcShort( 0x6 ),	/* Offset= 6 (160) */
/* 156 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 158 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 160 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 162 */	NdrFcShort( 0xffdc ),	/* Offset= -36 (126) */
/* 164 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 166 */	NdrFcShort( 0x10 ),	/* 16 */
/* 168 */	NdrFcShort( 0x0 ),	/* 0 */
/* 170 */	NdrFcShort( 0x0 ),	/* Offset= 0 (170) */
/* 172 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 174 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 176 */	NdrFcShort( 0xff5a ),	/* Offset= -166 (10) */
/* 178 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 180 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 182 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 184 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 186 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 188 */	
			0x11, 0x8,	/* FC_RP [simple_pointer] */
/* 190 */	
			0x22,		/* FC_C_CSTRING */
			0x5c,		/* FC_PAD */
/* 192 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 194 */	NdrFcShort( 0x2 ),	/* Offset= 2 (196) */
/* 196 */	
			0x12, 0x0,	/* FC_UP */
/* 198 */	NdrFcShort( 0x8 ),	/* Offset= 8 (206) */
/* 200 */	
			0x15,		/* FC_STRUCT */
			0x7,		/* 7 */
/* 202 */	NdrFcShort( 0x8 ),	/* 8 */
/* 204 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 206 */	
			0x15,		/* FC_STRUCT */
			0x7,		/* 7 */
/* 208 */	NdrFcShort( 0x60 ),	/* 96 */
/* 210 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 212 */	NdrFcShort( 0xfff4 ),	/* Offset= -12 (200) */
/* 214 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 216 */	NdrFcShort( 0xfff0 ),	/* Offset= -16 (200) */
/* 218 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 220 */	NdrFcShort( 0xffec ),	/* Offset= -20 (200) */
/* 222 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 224 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 226 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 228 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 230 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 232 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 234 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 236 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 238 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 240 */	NdrFcShort( 0xffd8 ),	/* Offset= -40 (200) */
/* 242 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 244 */	
			0x11, 0x0,	/* FC_RP */
/* 246 */	NdrFcShort( 0xae ),	/* Offset= 174 (420) */
/* 248 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0x9,		/* FC_ULONG */
/* 250 */	0x9,		/* Corr desc: FC_ULONG */
			0x0,		/*  */
/* 252 */	NdrFcShort( 0xfff8 ),	/* -8 */
/* 254 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 256 */	NdrFcShort( 0x2 ),	/* Offset= 2 (258) */
/* 258 */	NdrFcShort( 0x8 ),	/* 8 */
/* 260 */	NdrFcShort( 0x3002 ),	/* 12290 */
/* 262 */	NdrFcLong( 0x64 ),	/* 100 */
/* 266 */	NdrFcShort( 0xa ),	/* Offset= 10 (276) */
/* 268 */	NdrFcLong( 0x65 ),	/* 101 */
/* 272 */	NdrFcShort( 0x42 ),	/* Offset= 66 (338) */
/* 274 */	NdrFcShort( 0x0 ),	/* Offset= 0 (274) */
/* 276 */	
			0x12, 0x0,	/* FC_UP */
/* 278 */	NdrFcShort( 0x2c ),	/* Offset= 44 (322) */
/* 280 */	
			0x15,		/* FC_STRUCT */
			0x7,		/* 7 */
/* 282 */	NdrFcShort( 0x30 ),	/* 48 */
/* 284 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 286 */	NdrFcShort( 0xffaa ),	/* Offset= -86 (200) */
/* 288 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 290 */	NdrFcShort( 0xffa6 ),	/* Offset= -90 (200) */
/* 292 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 294 */	NdrFcShort( 0xffa2 ),	/* Offset= -94 (200) */
/* 296 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 298 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 300 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 302 */	NdrFcShort( 0xff9a ),	/* Offset= -102 (200) */
/* 304 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 306 */	
			0x1b,		/* FC_CARRAY */
			0x7,		/* 7 */
/* 308 */	NdrFcShort( 0x30 ),	/* 48 */
/* 310 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 312 */	NdrFcShort( 0x0 ),	/* 0 */
/* 314 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 316 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 318 */	NdrFcShort( 0xffda ),	/* Offset= -38 (280) */
/* 320 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 322 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 324 */	NdrFcShort( 0x10 ),	/* 16 */
/* 326 */	NdrFcShort( 0x0 ),	/* 0 */
/* 328 */	NdrFcShort( 0x6 ),	/* Offset= 6 (334) */
/* 330 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 332 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 334 */	
			0x12, 0x0,	/* FC_UP */
/* 336 */	NdrFcShort( 0xffe2 ),	/* Offset= -30 (306) */
/* 338 */	
			0x12, 0x0,	/* FC_UP */
/* 340 */	NdrFcShort( 0x40 ),	/* Offset= 64 (404) */
/* 342 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x7,		/* 7 */
/* 344 */	NdrFcShort( 0x58 ),	/* 88 */
/* 346 */	NdrFcShort( 0x0 ),	/* 0 */
/* 348 */	NdrFcShort( 0x0 ),	/* Offset= 0 (348) */
/* 350 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 352 */	NdrFcShort( 0xff68 ),	/* Offset= -152 (200) */
/* 354 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 356 */	NdrFcShort( 0xff64 ),	/* Offset= -156 (200) */
/* 358 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 360 */	NdrFcShort( 0xff60 ),	/* Offset= -160 (200) */
/* 362 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 364 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 366 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 368 */	NdrFcShort( 0xff58 ),	/* Offset= -168 (200) */
/* 370 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 372 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 374 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 376 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 378 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 380 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 382 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x7,		/* 7 */
/* 384 */	NdrFcShort( 0x0 ),	/* 0 */
/* 386 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 388 */	NdrFcShort( 0x0 ),	/* 0 */
/* 390 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 392 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 396 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 398 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 400 */	NdrFcShort( 0xffc6 ),	/* Offset= -58 (342) */
/* 402 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 404 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 406 */	NdrFcShort( 0x10 ),	/* 16 */
/* 408 */	NdrFcShort( 0x0 ),	/* 0 */
/* 410 */	NdrFcShort( 0x6 ),	/* Offset= 6 (416) */
/* 412 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 414 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 416 */	
			0x12, 0x0,	/* FC_UP */
/* 418 */	NdrFcShort( 0xffdc ),	/* Offset= -36 (382) */
/* 420 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 422 */	NdrFcShort( 0x10 ),	/* 16 */
/* 424 */	NdrFcShort( 0x0 ),	/* 0 */
/* 426 */	NdrFcShort( 0x0 ),	/* Offset= 0 (426) */
/* 428 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 430 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 432 */	NdrFcShort( 0xff48 ),	/* Offset= -184 (248) */
/* 434 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 436 */	
			0x11, 0x8,	/* FC_RP [simple_pointer] */
/* 438 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 440 */	
			0x11, 0x0,	/* FC_RP */
/* 442 */	NdrFcShort( 0x2e ),	/* Offset= 46 (488) */
/* 444 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 446 */	NdrFcShort( 0x18 ),	/* 24 */
/* 448 */	NdrFcShort( 0x0 ),	/* 0 */
/* 450 */	NdrFcShort( 0x8 ),	/* Offset= 8 (458) */
/* 452 */	0x36,		/* FC_POINTER */
			0x36,		/* FC_POINTER */
/* 454 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 456 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 458 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 460 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 462 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 464 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 466 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 468 */	NdrFcShort( 0x0 ),	/* 0 */
/* 470 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 472 */	NdrFcShort( 0x0 ),	/* 0 */
/* 474 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 476 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 480 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 482 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 484 */	NdrFcShort( 0xffd8 ),	/* Offset= -40 (444) */
/* 486 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 488 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 490 */	NdrFcShort( 0x10 ),	/* 16 */
/* 492 */	NdrFcShort( 0x0 ),	/* 0 */
/* 494 */	NdrFcShort( 0x6 ),	/* Offset= 6 (500) */
/* 496 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 498 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 500 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 502 */	NdrFcShort( 0xffdc ),	/* Offset= -36 (466) */

			0x0
        }
    };

static const GENERIC_BINDING_ROUTINE_PAIR BindingRoutines[ GENERIC_BINDING_TABLE_SIZE ] = 
        {
        {
            (GENERIC_BINDING_ROUTINE)BROWSER_IDENTIFY_HANDLE_bind,
            (GENERIC_UNBIND_ROUTINE)BROWSER_IDENTIFY_HANDLE_unbind
         }
        
        };


static const unsigned short browser_FormatStringOffsetTable[] =
    {
    0,
    92,
    148,
    204,
    248,
    298,
    348,
    392,
    436,
    492,
    554,
    604
    };


static const MIDL_STUB_DESC browser_StubDesc = 
    {
    (void *)& browser___RpcClientInterface,
    MIDL_user_allocate,
    MIDL_user_free,
    &browser_bhandle,
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

