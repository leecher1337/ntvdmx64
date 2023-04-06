

/* this ALWAYS GENERATED file contains the RPC client stubs */


 /* File created by MIDL compiler version 6.00.0361 */
/* at Fri Mar 31 14:17:00 2023
 */
/* Compiler settings for samrpc.idl, samcli.acf:
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

#include "samrpc_c.h"

#define TYPE_FORMAT_STRING_SIZE   3129                              
#define PROC_FORMAT_STRING_SIZE   1783                              
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


/* Standard interface: samr, ver. 1.0,
   GUID={0x12345778,0x1234,0xABCD,{0xEF,0x00,0x01,0x23,0x45,0x67,0x89,0xAC}} */

handle_t samcli_handle;


static const RPC_CLIENT_INTERFACE samr___RpcClientInterface =
    {
    sizeof(RPC_CLIENT_INTERFACE),
    {{0x12345778,0x1234,0xABCD,{0xEF,0x00,0x01,0x23,0x45,0x67,0x89,0xAC}},{1,0}},
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    0,
    0,
    0,
    0,
    0x00000000
    };
RPC_IF_HANDLE samr_ClientIfHandle = (RPC_IF_HANDLE)& samr___RpcClientInterface;

extern const MIDL_STUB_DESC samr_StubDesc;

static RPC_BINDING_HANDLE samr__MIDL_AutoBindHandle;


NTSTATUS SamrConnect( 
    /* [unique][in] */ PSAMPR_SERVER_NAME ServerName,
    /* [out] */ SAMPR_HANDLE *ServerHandle,
    /* [in] */ ACCESS_MASK DesiredAccess)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&samr_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[0],
                 ( unsigned char * )&ServerName);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrCloseHandle( 
    /* [out][in] */ SAMPR_HANDLE *SamHandle)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&samr_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[28],
                 ( unsigned char * )&SamHandle);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrSetSecurityObject( 
    /* [in] */ SAMPR_HANDLE ObjectHandle,
    /* [in] */ SECURITY_INFORMATION SecurityInformation,
    /* [in] */ PSAMPR_SR_SECURITY_DESCRIPTOR SecurityDescriptor)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&samr_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[50],
                 ( unsigned char * )&ObjectHandle);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrQuerySecurityObject( 
    /* [in] */ SAMPR_HANDLE ObjectHandle,
    /* [in] */ SECURITY_INFORMATION SecurityInformation,
    /* [out] */ PSAMPR_SR_SECURITY_DESCRIPTOR *SecurityDescriptor)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&samr_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[78],
                 ( unsigned char * )&ObjectHandle);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrShutdownSamServer( 
    /* [in] */ SAMPR_HANDLE ServerHandle)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&samr_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[106],
                 ( unsigned char * )&ServerHandle);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrLookupDomainInSamServer( 
    /* [in] */ SAMPR_HANDLE ServerHandle,
    /* [in] */ PRPC_UNICODE_STRING Name,
    /* [out] */ PRPC_SID *DomainId)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&samr_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[128],
                 ( unsigned char * )&ServerHandle);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrEnumerateDomainsInSamServer( 
    /* [in] */ SAMPR_HANDLE ServerHandle,
    /* [out][in] */ PSAM_ENUMERATE_HANDLE EnumerationContext,
    /* [out] */ PSAMPR_ENUMERATION_BUFFER *Buffer,
    /* [in] */ ULONG PreferedMaximumLength,
    /* [out] */ PULONG CountReturned)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&samr_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[158],
                 ( unsigned char * )&ServerHandle);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrOpenDomain( 
    /* [in] */ SAMPR_HANDLE ServerHandle,
    /* [in] */ ACCESS_MASK DesiredAccess,
    /* [in] */ PRPC_SID DomainId,
    /* [out] */ SAMPR_HANDLE *DomainHandle)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&samr_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[194],
                 ( unsigned char * )&ServerHandle);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrQueryInformationDomain( 
    /* [in] */ SAMPR_HANDLE DomainHandle,
    /* [in] */ DOMAIN_INFORMATION_CLASS DomainInformationClass,
    /* [switch_is][out] */ PSAMPR_DOMAIN_INFO_BUFFER *Buffer)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&samr_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[226],
                 ( unsigned char * )&DomainHandle);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrSetInformationDomain( 
    /* [in] */ SAMPR_HANDLE DomainHandle,
    /* [in] */ DOMAIN_INFORMATION_CLASS DomainInformationClass,
    /* [switch_is][in] */ PSAMPR_DOMAIN_INFO_BUFFER DomainInformation)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&samr_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[254],
                 ( unsigned char * )&DomainHandle);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrCreateGroupInDomain( 
    /* [in] */ SAMPR_HANDLE DomainHandle,
    /* [in] */ PRPC_UNICODE_STRING Name,
    /* [in] */ ACCESS_MASK DesiredAccess,
    /* [out] */ SAMPR_HANDLE *GroupHandle,
    /* [out] */ PULONG RelativeId)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&samr_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[282],
                 ( unsigned char * )&DomainHandle);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrEnumerateGroupsInDomain( 
    /* [in] */ SAMPR_HANDLE DomainHandle,
    /* [out][in] */ PSAM_ENUMERATE_HANDLE EnumerationContext,
    /* [out] */ PSAMPR_ENUMERATION_BUFFER *Buffer,
    /* [in] */ ULONG PreferedMaximumLength,
    /* [out] */ PULONG CountReturned)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&samr_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[318],
                 ( unsigned char * )&DomainHandle);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrCreateUserInDomain( 
    /* [in] */ SAMPR_HANDLE DomainHandle,
    /* [in] */ PRPC_UNICODE_STRING Name,
    /* [in] */ ACCESS_MASK DesiredAccess,
    /* [out] */ SAMPR_HANDLE *UserHandle,
    /* [out] */ PULONG RelativeId)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&samr_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[354],
                 ( unsigned char * )&DomainHandle);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrEnumerateUsersInDomain( 
    /* [in] */ SAMPR_HANDLE DomainHandle,
    /* [out][in] */ PSAM_ENUMERATE_HANDLE EnumerationContext,
    /* [in] */ ULONG UserAccountControl,
    /* [out] */ PSAMPR_ENUMERATION_BUFFER *Buffer,
    /* [in] */ ULONG PreferedMaximumLength,
    /* [out] */ PULONG CountReturned)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&samr_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[390],
                 ( unsigned char * )&DomainHandle);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrCreateAliasInDomain( 
    /* [in] */ SAMPR_HANDLE DomainHandle,
    /* [in] */ PRPC_UNICODE_STRING AccountName,
    /* [in] */ ACCESS_MASK DesiredAccess,
    /* [out] */ SAMPR_HANDLE *AliasHandle,
    /* [out] */ PULONG RelativeId)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&samr_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[428],
                 ( unsigned char * )&DomainHandle);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrEnumerateAliasesInDomain( 
    /* [in] */ SAMPR_HANDLE DomainHandle,
    /* [out][in] */ PSAM_ENUMERATE_HANDLE EnumerationContext,
    /* [out] */ PSAMPR_ENUMERATION_BUFFER *Buffer,
    /* [in] */ ULONG PreferedMaximumLength,
    /* [out] */ PULONG CountReturned)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&samr_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[464],
                 ( unsigned char * )&DomainHandle);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrGetAliasMembership( 
    /* [in] */ SAMPR_HANDLE DomainHandle,
    /* [in] */ PSAMPR_PSID_ARRAY SidArray,
    /* [out] */ PSAMPR_ULONG_ARRAY Membership)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&samr_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[500],
                 ( unsigned char * )&DomainHandle);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrLookupNamesInDomain( 
    /* [in] */ SAMPR_HANDLE DomainHandle,
    /* [in] */ ULONG Count,
    /* [length_is][size_is][in] */ RPC_UNICODE_STRING Names[  ],
    /* [out] */ PSAMPR_ULONG_ARRAY RelativeIds,
    /* [out] */ PSAMPR_ULONG_ARRAY Use)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&samr_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[530],
                 ( unsigned char * )&DomainHandle);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrLookupIdsInDomain( 
    /* [in] */ SAMPR_HANDLE DomainHandle,
    /* [in] */ ULONG Count,
    /* [length_is][size_is][in] */ PULONG RelativeIds,
    /* [out] */ PSAMPR_RETURNED_USTRING_ARRAY Names,
    /* [out] */ PSAMPR_ULONG_ARRAY Use)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&samr_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[566],
                 ( unsigned char * )&DomainHandle);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrOpenGroup( 
    /* [in] */ SAMPR_HANDLE DomainHandle,
    /* [in] */ ACCESS_MASK DesiredAccess,
    /* [in] */ ULONG GroupId,
    /* [out] */ SAMPR_HANDLE *GroupHandle)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&samr_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[602],
                 ( unsigned char * )&DomainHandle);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrQueryInformationGroup( 
    /* [in] */ SAMPR_HANDLE GroupHandle,
    /* [in] */ GROUP_INFORMATION_CLASS GroupInformationClass,
    /* [switch_is][out] */ PSAMPR_GROUP_INFO_BUFFER *Buffer)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&samr_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[632],
                 ( unsigned char * )&GroupHandle);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrSetInformationGroup( 
    /* [in] */ SAMPR_HANDLE GroupHandle,
    /* [in] */ GROUP_INFORMATION_CLASS GroupInformationClass,
    /* [switch_is][in] */ PSAMPR_GROUP_INFO_BUFFER Buffer)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&samr_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[660],
                 ( unsigned char * )&GroupHandle);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrAddMemberToGroup( 
    /* [in] */ SAMPR_HANDLE GroupHandle,
    /* [in] */ ULONG MemberId,
    /* [in] */ ULONG Attributes)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&samr_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[688],
                 ( unsigned char * )&GroupHandle);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrDeleteGroup( 
    /* [out][in] */ SAMPR_HANDLE *GroupHandle)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&samr_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[714],
                 ( unsigned char * )&GroupHandle);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrRemoveMemberFromGroup( 
    /* [in] */ SAMPR_HANDLE GroupHandle,
    /* [in] */ ULONG MemberId)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&samr_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[736],
                 ( unsigned char * )&GroupHandle);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrGetMembersInGroup( 
    /* [in] */ SAMPR_HANDLE GroupHandle,
    /* [out] */ PSAMPR_GET_MEMBERS_BUFFER *Members)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&samr_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[760],
                 ( unsigned char * )&GroupHandle);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrSetMemberAttributesOfGroup( 
    /* [in] */ SAMPR_HANDLE GroupHandle,
    /* [in] */ ULONG MemberId,
    /* [in] */ ULONG Attributes)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&samr_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[786],
                 ( unsigned char * )&GroupHandle);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrOpenAlias( 
    /* [in] */ SAMPR_HANDLE DomainHandle,
    /* [in] */ ACCESS_MASK DesiredAccess,
    /* [in] */ ULONG AliasId,
    /* [out] */ SAMPR_HANDLE *AliasHandle)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&samr_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[812],
                 ( unsigned char * )&DomainHandle);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrQueryInformationAlias( 
    /* [in] */ SAMPR_HANDLE AliasHandle,
    /* [in] */ ALIAS_INFORMATION_CLASS AliasInformationClass,
    /* [switch_is][out] */ PSAMPR_ALIAS_INFO_BUFFER *Buffer)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&samr_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[842],
                 ( unsigned char * )&AliasHandle);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrSetInformationAlias( 
    /* [in] */ SAMPR_HANDLE AliasHandle,
    /* [in] */ ALIAS_INFORMATION_CLASS AliasInformationClass,
    /* [switch_is][in] */ PSAMPR_ALIAS_INFO_BUFFER Buffer)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&samr_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[870],
                 ( unsigned char * )&AliasHandle);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrDeleteAlias( 
    /* [out][in] */ SAMPR_HANDLE *AliasHandle)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&samr_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[898],
                 ( unsigned char * )&AliasHandle);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrAddMemberToAlias( 
    /* [in] */ SAMPR_HANDLE AliasHandle,
    /* [in] */ PRPC_SID MemberId)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&samr_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[920],
                 ( unsigned char * )&AliasHandle);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrRemoveMemberFromAlias( 
    /* [in] */ SAMPR_HANDLE AliasHandle,
    /* [in] */ PRPC_SID MemberId)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&samr_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[946],
                 ( unsigned char * )&AliasHandle);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrGetMembersInAlias( 
    /* [in] */ SAMPR_HANDLE AliasHandle,
    /* [out] */ PSAMPR_PSID_ARRAY Members)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&samr_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[972],
                 ( unsigned char * )&AliasHandle);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrOpenUser( 
    /* [in] */ SAMPR_HANDLE DomainHandle,
    /* [in] */ ACCESS_MASK DesiredAccess,
    /* [in] */ ULONG UserId,
    /* [out] */ SAMPR_HANDLE *UserHandle)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&samr_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[998],
                 ( unsigned char * )&DomainHandle);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrDeleteUser( 
    /* [out][in] */ SAMPR_HANDLE *UserHandle)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&samr_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1028],
                 ( unsigned char * )&UserHandle);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrQueryInformationUser( 
    /* [in] */ SAMPR_HANDLE UserHandle,
    /* [in] */ USER_INFORMATION_CLASS UserInformationClass,
    /* [switch_is][out] */ PSAMPR_USER_INFO_BUFFER *Buffer)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&samr_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1050],
                 ( unsigned char * )&UserHandle);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrSetInformationUser( 
    /* [in] */ SAMPR_HANDLE UserHandle,
    /* [in] */ USER_INFORMATION_CLASS UserInformationClass,
    /* [switch_is][in] */ PSAMPR_USER_INFO_BUFFER Buffer)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&samr_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1078],
                 ( unsigned char * )&UserHandle);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrChangePasswordUser( 
    /* [in] */ SAMPR_HANDLE UserHandle,
    /* [in] */ BOOLEAN LmPresent,
    /* [unique][in] */ PENCRYPTED_LM_OWF_PASSWORD LmOldEncryptedWithLmNew,
    /* [unique][in] */ PENCRYPTED_LM_OWF_PASSWORD LmNewEncryptedWithLmOld,
    /* [in] */ BOOLEAN NtPresent,
    /* [unique][in] */ PENCRYPTED_NT_OWF_PASSWORD NtOldEncryptedWithNtNew,
    /* [unique][in] */ PENCRYPTED_NT_OWF_PASSWORD NtNewEncryptedWithNtOld,
    /* [in] */ BOOLEAN NtCrossEncryptionPresent,
    /* [unique][in] */ PENCRYPTED_NT_OWF_PASSWORD NtNewEncryptedWithLmNew,
    /* [in] */ BOOLEAN LmCrossEncryptionPresent,
    /* [unique][in] */ PENCRYPTED_LM_OWF_PASSWORD LmNtNewEncryptedWithNtNew)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&samr_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1106],
                 ( unsigned char * )&UserHandle);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrGetGroupsForUser( 
    /* [in] */ SAMPR_HANDLE UserHandle,
    /* [out] */ PSAMPR_GET_GROUPS_BUFFER *Groups)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&samr_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1160],
                 ( unsigned char * )&UserHandle);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrQueryDisplayInformation( 
    /* [in] */ SAMPR_HANDLE DomainHandle,
    /* [in] */ DOMAIN_DISPLAY_INFORMATION DisplayInformationClass,
    /* [in] */ ULONG Index,
    /* [in] */ ULONG EntryCount,
    /* [in] */ ULONG PreferredMaximumLength,
    /* [out] */ PULONG TotalAvailable,
    /* [out] */ PULONG TotalReturned,
    /* [switch_is][out] */ PSAMPR_DISPLAY_INFO_BUFFER Buffer)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&samr_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1186],
                 ( unsigned char * )&DomainHandle);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrGetDisplayEnumerationIndex( 
    /* [in] */ SAMPR_HANDLE DomainHandle,
    /* [in] */ DOMAIN_DISPLAY_INFORMATION DisplayInformationClass,
    /* [in] */ PRPC_UNICODE_STRING Prefix,
    /* [out] */ PULONG Index)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&samr_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1228],
                 ( unsigned char * )&DomainHandle);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrTestPrivateFunctionsDomain( 
    /* [in] */ SAMPR_HANDLE DomainHandle)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&samr_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1260],
                 ( unsigned char * )&DomainHandle);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrTestPrivateFunctionsUser( 
    /* [in] */ SAMPR_HANDLE UserHandle)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&samr_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1282],
                 ( unsigned char * )&UserHandle);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrGetUserDomainPasswordInformation( 
    /* [in] */ SAMPR_HANDLE UserHandle,
    /* [out] */ PUSER_DOMAIN_PASSWORD_INFORMATION PasswordInformation)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&samr_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1304],
                 ( unsigned char * )&UserHandle);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrRemoveMemberFromForeignDomain( 
    /* [in] */ SAMPR_HANDLE DomainHandle,
    /* [in] */ PRPC_SID MemberSid)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&samr_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1330],
                 ( unsigned char * )&DomainHandle);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrQueryInformationDomain2( 
    /* [in] */ SAMPR_HANDLE DomainHandle,
    /* [in] */ DOMAIN_INFORMATION_CLASS DomainInformationClass,
    /* [switch_is][out] */ PSAMPR_DOMAIN_INFO_BUFFER *Buffer)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&samr_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1356],
                 ( unsigned char * )&DomainHandle);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrQueryInformationUser2( 
    /* [in] */ SAMPR_HANDLE UserHandle,
    /* [in] */ USER_INFORMATION_CLASS UserInformationClass,
    /* [switch_is][out] */ PSAMPR_USER_INFO_BUFFER *Buffer)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&samr_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1384],
                 ( unsigned char * )&UserHandle);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrQueryDisplayInformation2( 
    /* [in] */ SAMPR_HANDLE DomainHandle,
    /* [in] */ DOMAIN_DISPLAY_INFORMATION DisplayInformationClass,
    /* [in] */ ULONG Index,
    /* [in] */ ULONG EntryCount,
    /* [in] */ ULONG PreferredMaximumLength,
    /* [out] */ PULONG TotalAvailable,
    /* [out] */ PULONG TotalReturned,
    /* [switch_is][out] */ PSAMPR_DISPLAY_INFO_BUFFER Buffer)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&samr_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1412],
                 ( unsigned char * )&DomainHandle);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrGetDisplayEnumerationIndex2( 
    /* [in] */ SAMPR_HANDLE DomainHandle,
    /* [in] */ DOMAIN_DISPLAY_INFORMATION DisplayInformationClass,
    /* [in] */ PRPC_UNICODE_STRING Prefix,
    /* [out] */ PULONG Index)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&samr_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1454],
                 ( unsigned char * )&DomainHandle);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrCreateUser2InDomain( 
    /* [in] */ SAMPR_HANDLE DomainHandle,
    /* [in] */ PRPC_UNICODE_STRING Name,
    /* [in] */ ULONG AccountType,
    /* [in] */ ACCESS_MASK DesiredAccess,
    /* [out] */ SAMPR_HANDLE *UserHandle,
    /* [out] */ PULONG GrantedAccess,
    /* [out] */ PULONG RelativeId)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&samr_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1486],
                 ( unsigned char * )&DomainHandle);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrQueryDisplayInformation3( 
    /* [in] */ SAMPR_HANDLE DomainHandle,
    /* [in] */ DOMAIN_DISPLAY_INFORMATION DisplayInformationClass,
    /* [in] */ ULONG Index,
    /* [in] */ ULONG EntryCount,
    /* [in] */ ULONG PreferredMaximumLength,
    /* [out] */ PULONG TotalAvailable,
    /* [out] */ PULONG TotalReturned,
    /* [switch_is][out] */ PSAMPR_DISPLAY_INFO_BUFFER Buffer)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&samr_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1528],
                 ( unsigned char * )&DomainHandle);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrAddMultipleMembersToAlias( 
    /* [in] */ SAMPR_HANDLE AliasHandle,
    /* [in] */ PSAMPR_PSID_ARRAY MembersBuffer)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&samr_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1570],
                 ( unsigned char * )&AliasHandle);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrRemoveMultipleMembersFromAlias( 
    /* [in] */ SAMPR_HANDLE AliasHandle,
    /* [in] */ PSAMPR_PSID_ARRAY MembersBuffer)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&samr_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1596],
                 ( unsigned char * )&AliasHandle);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrOemChangePasswordUser2( 
    /* [in] */ handle_t BindingHandle,
    /* [unique][in] */ PRPC_STRING ServerName,
    /* [in] */ PRPC_STRING UserName,
    /* [unique][in] */ PSAMPR_ENCRYPTED_USER_PASSWORD NewPasswordEncryptedWithOldLm,
    /* [unique][in] */ PENCRYPTED_LM_OWF_PASSWORD OldLmOwfPassswordEncryptedWithNewLm)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&samr_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1622],
                 ( unsigned char * )&BindingHandle);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrUnicodeChangePasswordUser2( 
    /* [in] */ handle_t BindingHandle,
    /* [unique][in] */ PRPC_UNICODE_STRING ServerName,
    /* [in] */ PRPC_UNICODE_STRING UserName,
    /* [unique][in] */ PSAMPR_ENCRYPTED_USER_PASSWORD NewPasswordEncryptedWithOldNt,
    /* [unique][in] */ PENCRYPTED_NT_OWF_PASSWORD OldNtOwfPasswordEncryptedWithNewNt,
    /* [in] */ BOOLEAN LmPresent,
    /* [unique][in] */ PSAMPR_ENCRYPTED_USER_PASSWORD NewPasswordEncryptedWithOldLm,
    /* [unique][in] */ PENCRYPTED_LM_OWF_PASSWORD OldLmOwfPassswordEncryptedWithNewLmOrNt)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&samr_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1656],
                 ( unsigned char * )&BindingHandle);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrGetDomainPasswordInformation( 
    /* [in] */ handle_t BindingHandle,
    /* [unique][in] */ PRPC_UNICODE_STRING ServerName,
    /* [out] */ PUSER_DOMAIN_PASSWORD_INFORMATION PasswordInformation)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&samr_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1700],
                 ( unsigned char * )&BindingHandle);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrConnect2( 
    /* [string][unique][in] */ PSAMPR_SERVER_NAME ServerName,
    /* [out] */ SAMPR_HANDLE *ServerHandle,
    /* [in] */ ACCESS_MASK DesiredAccess)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&samr_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1726],
                 ( unsigned char * )&ServerName);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrSetInformationUser2( 
    /* [in] */ SAMPR_HANDLE UserHandle,
    /* [in] */ USER_INFORMATION_CLASS UserInformationClass,
    /* [switch_is][in] */ PSAMPR_USER_INFO_BUFFER Buffer)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall(
                 ( PMIDL_STUB_DESC  )&samr_StubDesc,
                 (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1754],
                 ( unsigned char * )&UserHandle);
    return ( NTSTATUS  )_RetVal.Simple;
    
}

extern const GENERIC_BINDING_ROUTINE_PAIR BindingRoutines[ GENERIC_BINDING_TABLE_SIZE ];
extern const EXPR_EVAL ExprEvalRoutines[];

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
/*  8 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
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
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 22 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */
/* 24 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 26 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 28 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 30 */	NdrFcLong( 0x0 ),	/* 0 */
/* 34 */	NdrFcShort( 0x1 ),	/* 1 */
/* 36 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 38 */	0x30,		/* FC_BIND_CONTEXT */
			0xe0,		/* Ctxt flags:  via ptr, in, out, */
/* 40 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 42 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 44 */	
			0x50,		/* FC_IN_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 46 */	NdrFcShort( 0xe ),	/* Type Offset=14 */
/* 48 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 50 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 52 */	NdrFcLong( 0x0 ),	/* 0 */
/* 56 */	NdrFcShort( 0x2 ),	/* 2 */
/* 58 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 60 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 62 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 64 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 66 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 68 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */
/* 70 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 72 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 74 */	NdrFcShort( 0x1a ),	/* Type Offset=26 */
/* 76 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 78 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 80 */	NdrFcLong( 0x0 ),	/* 0 */
/* 84 */	NdrFcShort( 0x3 ),	/* 3 */
/* 86 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 88 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 90 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 92 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 94 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 96 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */
/* 98 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 100 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 102 */	NdrFcShort( 0x3c ),	/* Type Offset=60 */
/* 104 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 106 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 108 */	NdrFcLong( 0x0 ),	/* 0 */
/* 112 */	NdrFcShort( 0x4 ),	/* 4 */
/* 114 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 116 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 118 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 120 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 122 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 124 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */
/* 126 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 128 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 130 */	NdrFcLong( 0x0 ),	/* 0 */
/* 134 */	NdrFcShort( 0x5 ),	/* 5 */
/* 136 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 138 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 140 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 142 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 144 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 146 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */
/* 148 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 150 */	NdrFcShort( 0x44 ),	/* Type Offset=68 */
/* 152 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 154 */	NdrFcShort( 0x6c ),	/* Type Offset=108 */
/* 156 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 158 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 160 */	NdrFcLong( 0x0 ),	/* 0 */
/* 164 */	NdrFcShort( 0x6 ),	/* 6 */
/* 166 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 168 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 170 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 172 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 174 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 176 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */
/* 178 */	
			0x50,		/* FC_IN_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 180 */	NdrFcShort( 0x9c ),	/* Type Offset=156 */
/* 182 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 184 */	NdrFcShort( 0xa0 ),	/* Type Offset=160 */
/* 186 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 188 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 190 */	NdrFcShort( 0x9c ),	/* Type Offset=156 */
/* 192 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 194 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 196 */	NdrFcLong( 0x0 ),	/* 0 */
/* 200 */	NdrFcShort( 0x7 ),	/* 7 */
/* 202 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 204 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 206 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 208 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 210 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 212 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */
/* 214 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 216 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 218 */	NdrFcShort( 0x100 ),	/* Type Offset=256 */
/* 220 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 222 */	NdrFcShort( 0x104 ),	/* Type Offset=260 */
/* 224 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 226 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 228 */	NdrFcLong( 0x0 ),	/* 0 */
/* 232 */	NdrFcShort( 0x8 ),	/* 8 */
/* 234 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 236 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 238 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 240 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 242 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 244 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */
/* 246 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0xd,		/* FC_ENUM16 */
/* 248 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 250 */	NdrFcShort( 0x10c ),	/* Type Offset=268 */
/* 252 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 254 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 256 */	NdrFcLong( 0x0 ),	/* 0 */
/* 260 */	NdrFcShort( 0x9 ),	/* 9 */
/* 262 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 264 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 266 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 268 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 270 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 272 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */
/* 274 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0xd,		/* FC_ENUM16 */
/* 276 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 278 */	NdrFcShort( 0x250 ),	/* Type Offset=592 */
/* 280 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 282 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 284 */	NdrFcLong( 0x0 ),	/* 0 */
/* 288 */	NdrFcShort( 0xa ),	/* 10 */
/* 290 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 292 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 294 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 296 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 298 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 300 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */
/* 302 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 304 */	NdrFcShort( 0x44 ),	/* Type Offset=68 */
/* 306 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 308 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 310 */	NdrFcShort( 0x104 ),	/* Type Offset=260 */
/* 312 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 314 */	NdrFcShort( 0x9c ),	/* Type Offset=156 */
/* 316 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 318 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 320 */	NdrFcLong( 0x0 ),	/* 0 */
/* 324 */	NdrFcShort( 0xb ),	/* 11 */
/* 326 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 328 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 330 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 332 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 334 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 336 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */
/* 338 */	
			0x50,		/* FC_IN_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 340 */	NdrFcShort( 0x9c ),	/* Type Offset=156 */
/* 342 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 344 */	NdrFcShort( 0xa0 ),	/* Type Offset=160 */
/* 346 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 348 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 350 */	NdrFcShort( 0x9c ),	/* Type Offset=156 */
/* 352 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 354 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 356 */	NdrFcLong( 0x0 ),	/* 0 */
/* 360 */	NdrFcShort( 0xc ),	/* 12 */
/* 362 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 364 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 366 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 368 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 370 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 372 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */
/* 374 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 376 */	NdrFcShort( 0x44 ),	/* Type Offset=68 */
/* 378 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 380 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 382 */	NdrFcShort( 0x104 ),	/* Type Offset=260 */
/* 384 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 386 */	NdrFcShort( 0x9c ),	/* Type Offset=156 */
/* 388 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 390 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 392 */	NdrFcLong( 0x0 ),	/* 0 */
/* 396 */	NdrFcShort( 0xd ),	/* 13 */
/* 398 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 400 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 402 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 404 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 406 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 408 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */
/* 410 */	
			0x50,		/* FC_IN_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 412 */	NdrFcShort( 0x9c ),	/* Type Offset=156 */
/* 414 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 416 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 418 */	NdrFcShort( 0xa0 ),	/* Type Offset=160 */
/* 420 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 422 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 424 */	NdrFcShort( 0x9c ),	/* Type Offset=156 */
/* 426 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 428 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 430 */	NdrFcLong( 0x0 ),	/* 0 */
/* 434 */	NdrFcShort( 0xe ),	/* 14 */
/* 436 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 438 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 440 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 442 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 444 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 446 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */
/* 448 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 450 */	NdrFcShort( 0x44 ),	/* Type Offset=68 */
/* 452 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 454 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 456 */	NdrFcShort( 0x104 ),	/* Type Offset=260 */
/* 458 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 460 */	NdrFcShort( 0x9c ),	/* Type Offset=156 */
/* 462 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 464 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 466 */	NdrFcLong( 0x0 ),	/* 0 */
/* 470 */	NdrFcShort( 0xf ),	/* 15 */
/* 472 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 474 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 476 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 478 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 480 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 482 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */
/* 484 */	
			0x50,		/* FC_IN_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 486 */	NdrFcShort( 0x9c ),	/* Type Offset=156 */
/* 488 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 490 */	NdrFcShort( 0xa0 ),	/* Type Offset=160 */
/* 492 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 494 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 496 */	NdrFcShort( 0x9c ),	/* Type Offset=156 */
/* 498 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 500 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 502 */	NdrFcLong( 0x0 ),	/* 0 */
/* 506 */	NdrFcShort( 0x10 ),	/* 16 */
/* 508 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 510 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 512 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 514 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 516 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 518 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */
/* 520 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 522 */	NdrFcShort( 0x25c ),	/* Type Offset=604 */
/* 524 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 526 */	NdrFcShort( 0x2a8 ),	/* Type Offset=680 */
/* 528 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 530 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 532 */	NdrFcLong( 0x0 ),	/* 0 */
/* 536 */	NdrFcShort( 0x11 ),	/* 17 */
/* 538 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 540 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 542 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 544 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 546 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 548 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */
/* 550 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 552 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 554 */	NdrFcShort( 0x2ca ),	/* Type Offset=714 */
/* 556 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 558 */	NdrFcShort( 0x2a8 ),	/* Type Offset=680 */
/* 560 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 562 */	NdrFcShort( 0x2a8 ),	/* Type Offset=680 */
/* 564 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 566 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 568 */	NdrFcLong( 0x0 ),	/* 0 */
/* 572 */	NdrFcShort( 0x12 ),	/* 18 */
/* 574 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 576 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 578 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 580 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 582 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 584 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */
/* 586 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 588 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 590 */	NdrFcShort( 0x2ee ),	/* Type Offset=750 */
/* 592 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 594 */	NdrFcShort( 0x300 ),	/* Type Offset=768 */
/* 596 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 598 */	NdrFcShort( 0x2a8 ),	/* Type Offset=680 */
/* 600 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 602 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 604 */	NdrFcLong( 0x0 ),	/* 0 */
/* 608 */	NdrFcShort( 0x13 ),	/* 19 */
/* 610 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 612 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 614 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 616 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 618 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 620 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */
/* 622 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 624 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 626 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 628 */	NdrFcShort( 0x104 ),	/* Type Offset=260 */
/* 630 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 632 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 634 */	NdrFcLong( 0x0 ),	/* 0 */
/* 638 */	NdrFcShort( 0x14 ),	/* 20 */
/* 640 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 642 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 644 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 646 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 648 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 650 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */
/* 652 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0xd,		/* FC_ENUM16 */
/* 654 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 656 */	NdrFcShort( 0x338 ),	/* Type Offset=824 */
/* 658 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 660 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 662 */	NdrFcLong( 0x0 ),	/* 0 */
/* 666 */	NdrFcShort( 0x15 ),	/* 21 */
/* 668 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 670 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 672 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 674 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 676 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 678 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */
/* 680 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0xd,		/* FC_ENUM16 */
/* 682 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 684 */	NdrFcShort( 0x390 ),	/* Type Offset=912 */
/* 686 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 688 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 690 */	NdrFcLong( 0x0 ),	/* 0 */
/* 694 */	NdrFcShort( 0x16 ),	/* 22 */
/* 696 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 698 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 700 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 702 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 704 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 706 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */
/* 708 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 710 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 712 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 714 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 716 */	NdrFcLong( 0x0 ),	/* 0 */
/* 720 */	NdrFcShort( 0x17 ),	/* 23 */
/* 722 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 724 */	0x30,		/* FC_BIND_CONTEXT */
			0xe0,		/* Ctxt flags:  via ptr, in, out, */
/* 726 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 728 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 730 */	
			0x50,		/* FC_IN_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 732 */	NdrFcShort( 0xe ),	/* Type Offset=14 */
/* 734 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 736 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 738 */	NdrFcLong( 0x0 ),	/* 0 */
/* 742 */	NdrFcShort( 0x18 ),	/* 24 */
/* 744 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 746 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 748 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 750 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 752 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 754 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */
/* 756 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 758 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 760 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 762 */	NdrFcLong( 0x0 ),	/* 0 */
/* 766 */	NdrFcShort( 0x19 ),	/* 25 */
/* 768 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 770 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 772 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 774 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 776 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 778 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */
/* 780 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 782 */	NdrFcShort( 0x39c ),	/* Type Offset=924 */
/* 784 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 786 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 788 */	NdrFcLong( 0x0 ),	/* 0 */
/* 792 */	NdrFcShort( 0x1a ),	/* 26 */
/* 794 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 796 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 798 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 800 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 802 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 804 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */
/* 806 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 808 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 810 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 812 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 814 */	NdrFcLong( 0x0 ),	/* 0 */
/* 818 */	NdrFcShort( 0x1b ),	/* 27 */
/* 820 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 822 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 824 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 826 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 828 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 830 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */
/* 832 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 834 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 836 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 838 */	NdrFcShort( 0x104 ),	/* Type Offset=260 */
/* 840 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 842 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 844 */	NdrFcLong( 0x0 ),	/* 0 */
/* 848 */	NdrFcShort( 0x1c ),	/* 28 */
/* 850 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 852 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 854 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 856 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 858 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 860 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */
/* 862 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0xd,		/* FC_ENUM16 */
/* 864 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 866 */	NdrFcShort( 0x3c4 ),	/* Type Offset=964 */
/* 868 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 870 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 872 */	NdrFcLong( 0x0 ),	/* 0 */
/* 876 */	NdrFcShort( 0x1d ),	/* 29 */
/* 878 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 880 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 882 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 884 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 886 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 888 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */
/* 890 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0xd,		/* FC_ENUM16 */
/* 892 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 894 */	NdrFcShort( 0x41e ),	/* Type Offset=1054 */
/* 896 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 898 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 900 */	NdrFcLong( 0x0 ),	/* 0 */
/* 904 */	NdrFcShort( 0x1e ),	/* 30 */
/* 906 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 908 */	0x30,		/* FC_BIND_CONTEXT */
			0xe0,		/* Ctxt flags:  via ptr, in, out, */
/* 910 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 912 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 914 */	
			0x50,		/* FC_IN_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 916 */	NdrFcShort( 0xe ),	/* Type Offset=14 */
/* 918 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 920 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 922 */	NdrFcLong( 0x0 ),	/* 0 */
/* 926 */	NdrFcShort( 0x1f ),	/* 31 */
/* 928 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 930 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 932 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 934 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 936 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 938 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */
/* 940 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 942 */	NdrFcShort( 0x100 ),	/* Type Offset=256 */
/* 944 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 946 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 948 */	NdrFcLong( 0x0 ),	/* 0 */
/* 952 */	NdrFcShort( 0x20 ),	/* 32 */
/* 954 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 956 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 958 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 960 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 962 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 964 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */
/* 966 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 968 */	NdrFcShort( 0x100 ),	/* Type Offset=256 */
/* 970 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 972 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 974 */	NdrFcLong( 0x0 ),	/* 0 */
/* 978 */	NdrFcShort( 0x21 ),	/* 33 */
/* 980 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 982 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 984 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 986 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 988 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 990 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */
/* 992 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 994 */	NdrFcShort( 0x25c ),	/* Type Offset=604 */
/* 996 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 998 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1000 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1004 */	NdrFcShort( 0x22 ),	/* 34 */
/* 1006 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1008 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 1010 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1012 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1014 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1016 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */
/* 1018 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1020 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1022 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1024 */	NdrFcShort( 0x104 ),	/* Type Offset=260 */
/* 1026 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1028 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1030 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1034 */	NdrFcShort( 0x23 ),	/* 35 */
/* 1036 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1038 */	0x30,		/* FC_BIND_CONTEXT */
			0xe0,		/* Ctxt flags:  via ptr, in, out, */
/* 1040 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1042 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1044 */	
			0x50,		/* FC_IN_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1046 */	NdrFcShort( 0xe ),	/* Type Offset=14 */
/* 1048 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1050 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1052 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1056 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1058 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1060 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 1062 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1064 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1066 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1068 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */
/* 1070 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0xd,		/* FC_ENUM16 */
/* 1072 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1074 */	NdrFcShort( 0x42a ),	/* Type Offset=1066 */
/* 1076 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1078 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1080 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1084 */	NdrFcShort( 0x25 ),	/* 37 */
/* 1086 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1088 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 1090 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1092 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1094 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1096 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */
/* 1098 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0xd,		/* FC_ENUM16 */
/* 1100 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1102 */	NdrFcShort( 0xa0e ),	/* Type Offset=2574 */
/* 1104 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1106 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1108 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1112 */	NdrFcShort( 0x26 ),	/* 38 */
/* 1114 */	NdrFcShort( 0x30 ),	/* x86 Stack size/offset = 48 */
/* 1116 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 1118 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1120 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1122 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1124 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */
/* 1126 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x2,		/* FC_CHAR */
/* 1128 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1130 */	NdrFcShort( 0xa1a ),	/* Type Offset=2586 */
/* 1132 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1134 */	NdrFcShort( 0xa1a ),	/* Type Offset=2586 */
/* 1136 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x2,		/* FC_CHAR */
/* 1138 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1140 */	NdrFcShort( 0xa1a ),	/* Type Offset=2586 */
/* 1142 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1144 */	NdrFcShort( 0xa1a ),	/* Type Offset=2586 */
/* 1146 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x2,		/* FC_CHAR */
/* 1148 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1150 */	NdrFcShort( 0xa1a ),	/* Type Offset=2586 */
/* 1152 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x2,		/* FC_CHAR */
/* 1154 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1156 */	NdrFcShort( 0xa1a ),	/* Type Offset=2586 */
/* 1158 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1160 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1162 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1166 */	NdrFcShort( 0x27 ),	/* 39 */
/* 1168 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1170 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 1172 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1174 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1176 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1178 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */
/* 1180 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1182 */	NdrFcShort( 0xa1e ),	/* Type Offset=2590 */
/* 1184 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1186 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1188 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1192 */	NdrFcShort( 0x28 ),	/* 40 */
/* 1194 */	NdrFcShort( 0x24 ),	/* x86 Stack size/offset = 36 */
/* 1196 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 1198 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1200 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1202 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1204 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */
/* 1206 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0xd,		/* FC_ENUM16 */
/* 1208 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1210 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1212 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1214 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1216 */	NdrFcShort( 0x9c ),	/* Type Offset=156 */
/* 1218 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1220 */	NdrFcShort( 0x9c ),	/* Type Offset=156 */
/* 1222 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1224 */	NdrFcShort( 0xa48 ),	/* Type Offset=2632 */
/* 1226 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1228 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1230 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1234 */	NdrFcShort( 0x29 ),	/* 41 */
/* 1236 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1238 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 1240 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1242 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1244 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1246 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */
/* 1248 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0xd,		/* FC_ENUM16 */
/* 1250 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1252 */	NdrFcShort( 0x44 ),	/* Type Offset=68 */
/* 1254 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1256 */	NdrFcShort( 0x9c ),	/* Type Offset=156 */
/* 1258 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1260 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1262 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1266 */	NdrFcShort( 0x2a ),	/* 42 */
/* 1268 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1270 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 1272 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1274 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1276 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1278 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */
/* 1280 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1282 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1284 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1288 */	NdrFcShort( 0x2b ),	/* 43 */
/* 1290 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1292 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 1294 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1296 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1298 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1300 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */
/* 1302 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1304 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1306 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1310 */	NdrFcShort( 0x2c ),	/* 44 */
/* 1312 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1314 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 1316 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1318 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1320 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1322 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */
/* 1324 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1326 */	NdrFcShort( 0xba8 ),	/* Type Offset=2984 */
/* 1328 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1330 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1332 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1336 */	NdrFcShort( 0x2d ),	/* 45 */
/* 1338 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1340 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 1342 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1344 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1346 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1348 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */
/* 1350 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1352 */	NdrFcShort( 0x100 ),	/* Type Offset=256 */
/* 1354 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1356 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1358 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1362 */	NdrFcShort( 0x2e ),	/* 46 */
/* 1364 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1366 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 1368 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1370 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1372 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1374 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */
/* 1376 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0xd,		/* FC_ENUM16 */
/* 1378 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1380 */	NdrFcShort( 0xbb4 ),	/* Type Offset=2996 */
/* 1382 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1384 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1386 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1390 */	NdrFcShort( 0x2f ),	/* 47 */
/* 1392 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1394 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 1396 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1398 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1400 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1402 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */
/* 1404 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0xd,		/* FC_ENUM16 */
/* 1406 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1408 */	NdrFcShort( 0xbc4 ),	/* Type Offset=3012 */
/* 1410 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1412 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1414 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1418 */	NdrFcShort( 0x30 ),	/* 48 */
/* 1420 */	NdrFcShort( 0x24 ),	/* x86 Stack size/offset = 36 */
/* 1422 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 1424 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1426 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1428 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1430 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */
/* 1432 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0xd,		/* FC_ENUM16 */
/* 1434 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1436 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1438 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1440 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1442 */	NdrFcShort( 0x9c ),	/* Type Offset=156 */
/* 1444 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1446 */	NdrFcShort( 0x9c ),	/* Type Offset=156 */
/* 1448 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1450 */	NdrFcShort( 0xbd4 ),	/* Type Offset=3028 */
/* 1452 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1454 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1456 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1460 */	NdrFcShort( 0x31 ),	/* 49 */
/* 1462 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1464 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 1466 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1468 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1470 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1472 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */
/* 1474 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0xd,		/* FC_ENUM16 */
/* 1476 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1478 */	NdrFcShort( 0x44 ),	/* Type Offset=68 */
/* 1480 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1482 */	NdrFcShort( 0x9c ),	/* Type Offset=156 */
/* 1484 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1486 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1488 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1492 */	NdrFcShort( 0x32 ),	/* 50 */
/* 1494 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 1496 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 1498 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1500 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1502 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1504 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */
/* 1506 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1508 */	NdrFcShort( 0x44 ),	/* Type Offset=68 */
/* 1510 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1512 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1514 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1516 */	NdrFcShort( 0xbe0 ),	/* Type Offset=3040 */
/* 1518 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1520 */	NdrFcShort( 0x9c ),	/* Type Offset=156 */
/* 1522 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1524 */	NdrFcShort( 0x9c ),	/* Type Offset=156 */
/* 1526 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1528 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1530 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1534 */	NdrFcShort( 0x33 ),	/* 51 */
/* 1536 */	NdrFcShort( 0x24 ),	/* x86 Stack size/offset = 36 */
/* 1538 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 1540 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1542 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1544 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1546 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */
/* 1548 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0xd,		/* FC_ENUM16 */
/* 1550 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1552 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1554 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1556 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1558 */	NdrFcShort( 0x9c ),	/* Type Offset=156 */
/* 1560 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1562 */	NdrFcShort( 0x9c ),	/* Type Offset=156 */
/* 1564 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1566 */	NdrFcShort( 0xbe8 ),	/* Type Offset=3048 */
/* 1568 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1570 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1572 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1576 */	NdrFcShort( 0x34 ),	/* 52 */
/* 1578 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1580 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 1582 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1584 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1586 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1588 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */
/* 1590 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1592 */	NdrFcShort( 0x25c ),	/* Type Offset=604 */
/* 1594 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1596 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1598 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1602 */	NdrFcShort( 0x35 ),	/* 53 */
/* 1604 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1606 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 1608 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1610 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1612 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1614 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */
/* 1616 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1618 */	NdrFcShort( 0x25c ),	/* Type Offset=604 */
/* 1620 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1622 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1624 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1628 */	NdrFcShort( 0x36 ),	/* 54 */
/* 1630 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 1632 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x0,		/* 0 */
/* 1634 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1636 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0xf,		/* FC_IGNORE */
/* 1638 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1640 */	NdrFcShort( 0xbf4 ),	/* Type Offset=3060 */
/* 1642 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1644 */	NdrFcShort( 0xc1c ),	/* Type Offset=3100 */
/* 1646 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1648 */	NdrFcShort( 0xc20 ),	/* Type Offset=3104 */
/* 1650 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1652 */	NdrFcShort( 0xa1a ),	/* Type Offset=2586 */
/* 1654 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1656 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1658 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1662 */	NdrFcShort( 0x37 ),	/* 55 */
/* 1664 */	NdrFcShort( 0x24 ),	/* x86 Stack size/offset = 36 */
/* 1666 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x0,		/* 0 */
/* 1668 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1670 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0xf,		/* FC_IGNORE */
/* 1672 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1674 */	NdrFcShort( 0xc24 ),	/* Type Offset=3108 */
/* 1676 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1678 */	NdrFcShort( 0x44 ),	/* Type Offset=68 */
/* 1680 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1682 */	NdrFcShort( 0xc20 ),	/* Type Offset=3104 */
/* 1684 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1686 */	NdrFcShort( 0xa1a ),	/* Type Offset=2586 */
/* 1688 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x2,		/* FC_CHAR */
/* 1690 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1692 */	NdrFcShort( 0xc20 ),	/* Type Offset=3104 */
/* 1694 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1696 */	NdrFcShort( 0xa1a ),	/* Type Offset=2586 */
/* 1698 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1700 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1702 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1706 */	NdrFcShort( 0x38 ),	/* 56 */
/* 1708 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1710 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x0,		/* 0 */
/* 1712 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1714 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0xf,		/* FC_IGNORE */
/* 1716 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1718 */	NdrFcShort( 0xc24 ),	/* Type Offset=3108 */
/* 1720 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1722 */	NdrFcShort( 0xba8 ),	/* Type Offset=2984 */
/* 1724 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1726 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1728 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1732 */	NdrFcShort( 0x39 ),	/* 57 */
/* 1734 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1736 */	0x31,		/* FC_BIND_GENERIC */
			0x4,		/* 4 */
/* 1738 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1740 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 1742 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1744 */	NdrFcShort( 0xc28 ),	/* Type Offset=3112 */
/* 1746 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1748 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */
/* 1750 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1752 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 1754 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1756 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1760 */	NdrFcShort( 0x3a ),	/* 58 */
/* 1762 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1764 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 1766 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1768 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1770 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1772 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */
/* 1774 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0xd,		/* FC_ENUM16 */
/* 1776 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 1778 */	NdrFcShort( 0xc2c ),	/* Type Offset=3116 */
/* 1780 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
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
/*  4 */	0x5,		/* FC_WCHAR */
			0x5c,		/* FC_PAD */
/*  6 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/*  8 */	NdrFcShort( 0x2 ),	/* Offset= 2 (10) */
/* 10 */	0x30,		/* FC_BIND_CONTEXT */
			0xa0,		/* Ctxt flags:  via ptr, out, */
/* 12 */	0x0,		/* 0 */
			0x1,		/* 1 */
/* 14 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 16 */	NdrFcShort( 0x2 ),	/* Offset= 2 (18) */
/* 18 */	0x30,		/* FC_BIND_CONTEXT */
			0xe1,		/* Ctxt flags:  via ptr, in, out, can't be null */
/* 20 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 22 */	0x30,		/* FC_BIND_CONTEXT */
			0x41,		/* Ctxt flags:  in, can't be null */
/* 24 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 26 */	
			0x11, 0x0,	/* FC_RP */
/* 28 */	NdrFcShort( 0xc ),	/* Offset= 12 (40) */
/* 30 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 32 */	NdrFcShort( 0x1 ),	/* 1 */
/* 34 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 36 */	NdrFcShort( 0x0 ),	/* 0 */
/* 38 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
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
/* 52 */	0x12, 0x0,	/* FC_UP */
/* 54 */	NdrFcShort( 0xffe8 ),	/* Offset= -24 (30) */
/* 56 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 58 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 60 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 62 */	NdrFcShort( 0x2 ),	/* Offset= 2 (64) */
/* 64 */	
			0x12, 0x0,	/* FC_UP */
/* 66 */	NdrFcShort( 0xffe6 ),	/* Offset= -26 (40) */
/* 68 */	
			0x11, 0x0,	/* FC_RP */
/* 70 */	NdrFcShort( 0x10 ),	/* Offset= 16 (86) */
/* 72 */	
			0x1c,		/* FC_CVARRAY */
			0x1,		/* 1 */
/* 74 */	NdrFcShort( 0x2 ),	/* 2 */
/* 76 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x55,		/* FC_DIV_2 */
/* 78 */	NdrFcShort( 0x2 ),	/* 2 */
/* 80 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x55,		/* FC_DIV_2 */
/* 82 */	NdrFcShort( 0x0 ),	/* 0 */
/* 84 */	0x5,		/* FC_WCHAR */
			0x5b,		/* FC_END */
/* 86 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 88 */	NdrFcShort( 0x8 ),	/* 8 */
/* 90 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 92 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 94 */	NdrFcShort( 0x4 ),	/* 4 */
/* 96 */	NdrFcShort( 0x4 ),	/* 4 */
/* 98 */	0x12, 0x0,	/* FC_UP */
/* 100 */	NdrFcShort( 0xffe4 ),	/* Offset= -28 (72) */
/* 102 */	
			0x5b,		/* FC_END */

			0x6,		/* FC_SHORT */
/* 104 */	0x6,		/* FC_SHORT */
			0x8,		/* FC_LONG */
/* 106 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 108 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 110 */	NdrFcShort( 0x2 ),	/* Offset= 2 (112) */
/* 112 */	
			0x12, 0x0,	/* FC_UP */
/* 114 */	NdrFcShort( 0x1c ),	/* Offset= 28 (142) */
/* 116 */	
			0x1d,		/* FC_SMFARRAY */
			0x0,		/* 0 */
/* 118 */	NdrFcShort( 0x6 ),	/* 6 */
/* 120 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 122 */	
			0x15,		/* FC_STRUCT */
			0x0,		/* 0 */
/* 124 */	NdrFcShort( 0x6 ),	/* 6 */
/* 126 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 128 */	NdrFcShort( 0xfff4 ),	/* Offset= -12 (116) */
/* 130 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 132 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 134 */	NdrFcShort( 0x4 ),	/* 4 */
/* 136 */	0x4,		/* Corr desc: FC_USMALL */
			0x0,		/*  */
/* 138 */	NdrFcShort( 0xfff9 ),	/* -7 */
/* 140 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 142 */	
			0x17,		/* FC_CSTRUCT */
			0x3,		/* 3 */
/* 144 */	NdrFcShort( 0x8 ),	/* 8 */
/* 146 */	NdrFcShort( 0xfff2 ),	/* Offset= -14 (132) */
/* 148 */	0x2,		/* FC_CHAR */
			0x2,		/* FC_CHAR */
/* 150 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 152 */	NdrFcShort( 0xffe2 ),	/* Offset= -30 (122) */
/* 154 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 156 */	
			0x11, 0x8,	/* FC_RP [simple_pointer] */
/* 158 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 160 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 162 */	NdrFcShort( 0x2 ),	/* Offset= 2 (164) */
/* 164 */	
			0x12, 0x0,	/* FC_UP */
/* 166 */	NdrFcShort( 0x46 ),	/* Offset= 70 (236) */
/* 168 */	
			0x1c,		/* FC_CVARRAY */
			0x1,		/* 1 */
/* 170 */	NdrFcShort( 0x2 ),	/* 2 */
/* 172 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x55,		/* FC_DIV_2 */
/* 174 */	NdrFcShort( 0x6 ),	/* 6 */
/* 176 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x55,		/* FC_DIV_2 */
/* 178 */	NdrFcShort( 0x4 ),	/* 4 */
/* 180 */	0x5,		/* FC_WCHAR */
			0x5b,		/* FC_END */
/* 182 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 184 */	NdrFcShort( 0xc ),	/* 12 */
/* 186 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 188 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 190 */	NdrFcShort( 0x8 ),	/* 8 */
/* 192 */	NdrFcShort( 0x8 ),	/* 8 */
/* 194 */	0x12, 0x0,	/* FC_UP */
/* 196 */	NdrFcShort( 0xffe4 ),	/* Offset= -28 (168) */
/* 198 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 200 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 202 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 204 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 206 */	NdrFcShort( 0xc ),	/* 12 */
/* 208 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 210 */	NdrFcShort( 0x0 ),	/* 0 */
/* 212 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 214 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 216 */	NdrFcShort( 0xc ),	/* 12 */
/* 218 */	NdrFcShort( 0x0 ),	/* 0 */
/* 220 */	NdrFcShort( 0x1 ),	/* 1 */
/* 222 */	NdrFcShort( 0x8 ),	/* 8 */
/* 224 */	NdrFcShort( 0x8 ),	/* 8 */
/* 226 */	0x12, 0x0,	/* FC_UP */
/* 228 */	NdrFcShort( 0xffc4 ),	/* Offset= -60 (168) */
/* 230 */	
			0x5b,		/* FC_END */

			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 232 */	0x0,		/* 0 */
			NdrFcShort( 0xffcd ),	/* Offset= -51 (182) */
			0x5b,		/* FC_END */
/* 236 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 238 */	NdrFcShort( 0x8 ),	/* 8 */
/* 240 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 242 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 244 */	NdrFcShort( 0x4 ),	/* 4 */
/* 246 */	NdrFcShort( 0x4 ),	/* 4 */
/* 248 */	0x12, 0x1,	/* FC_UP [all_nodes] */
/* 250 */	NdrFcShort( 0xffd2 ),	/* Offset= -46 (204) */
/* 252 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 254 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 256 */	
			0x11, 0x0,	/* FC_RP */
/* 258 */	NdrFcShort( 0xff8c ),	/* Offset= -116 (142) */
/* 260 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 262 */	NdrFcShort( 0x2 ),	/* Offset= 2 (264) */
/* 264 */	0x30,		/* FC_BIND_CONTEXT */
			0xa0,		/* Ctxt flags:  via ptr, out, */
/* 266 */	0x0,		/* 0 */
			0x3,		/* 3 */
/* 268 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 270 */	NdrFcShort( 0x2 ),	/* Offset= 2 (272) */
/* 272 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 274 */	NdrFcShort( 0x2 ),	/* Offset= 2 (276) */
/* 276 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0xd,		/* FC_ENUM16 */
/* 278 */	0x26,		/* Corr desc:  parameter, FC_SHORT */
			0x0,		/*  */
/* 280 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 282 */	NdrFcShort( 0x2 ),	/* Offset= 2 (284) */
/* 284 */	NdrFcShort( 0x58 ),	/* 88 */
/* 286 */	NdrFcShort( 0x700c ),	/* 28684 */
/* 288 */	NdrFcLong( 0x1 ),	/* 1 */
/* 292 */	NdrFcShort( 0x4e ),	/* Offset= 78 (370) */
/* 294 */	NdrFcLong( 0x2 ),	/* 2 */
/* 298 */	NdrFcShort( 0x82 ),	/* Offset= 130 (428) */
/* 300 */	NdrFcLong( 0x3 ),	/* 3 */
/* 304 */	NdrFcShort( 0xba ),	/* Offset= 186 (490) */
/* 306 */	NdrFcLong( 0x4 ),	/* 4 */
/* 310 */	NdrFcShort( 0xff20 ),	/* Offset= -224 (86) */
/* 312 */	NdrFcLong( 0x5 ),	/* 5 */
/* 316 */	NdrFcShort( 0xff1a ),	/* Offset= -230 (86) */
/* 318 */	NdrFcLong( 0x7 ),	/* 7 */
/* 322 */	NdrFcShort( 0xb2 ),	/* Offset= 178 (500) */
/* 324 */	NdrFcLong( 0x6 ),	/* 6 */
/* 328 */	NdrFcShort( 0xff0e ),	/* Offset= -242 (86) */
/* 330 */	NdrFcLong( 0x8 ),	/* 8 */
/* 334 */	NdrFcShort( 0xb0 ),	/* Offset= 176 (510) */
/* 336 */	NdrFcLong( 0x9 ),	/* 9 */
/* 340 */	NdrFcShort( 0xa0 ),	/* Offset= 160 (500) */
/* 342 */	NdrFcLong( 0xb ),	/* 11 */
/* 346 */	NdrFcShort( 0xb8 ),	/* Offset= 184 (530) */
/* 348 */	NdrFcLong( 0xc ),	/* 12 */
/* 352 */	NdrFcShort( 0xca ),	/* Offset= 202 (554) */
/* 354 */	NdrFcLong( 0xd ),	/* 13 */
/* 358 */	NdrFcShort( 0xd8 ),	/* Offset= 216 (574) */
/* 360 */	NdrFcShort( 0xffff ),	/* Offset= -1 (359) */
/* 362 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 364 */	NdrFcShort( 0x8 ),	/* 8 */
/* 366 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 368 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 370 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 372 */	NdrFcShort( 0x18 ),	/* 24 */
/* 374 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 376 */	0x8,		/* FC_LONG */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 378 */	0x0,		/* 0 */
			NdrFcShort( 0xffef ),	/* Offset= -17 (362) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 382 */	0x0,		/* 0 */
			NdrFcShort( 0xffeb ),	/* Offset= -21 (362) */
			0x5b,		/* FC_END */
/* 386 */	
			0x1c,		/* FC_CVARRAY */
			0x1,		/* 1 */
/* 388 */	NdrFcShort( 0x2 ),	/* 2 */
/* 390 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x55,		/* FC_DIV_2 */
/* 392 */	NdrFcShort( 0xa ),	/* 10 */
/* 394 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x55,		/* FC_DIV_2 */
/* 396 */	NdrFcShort( 0x8 ),	/* 8 */
/* 398 */	0x5,		/* FC_WCHAR */
			0x5b,		/* FC_END */
/* 400 */	
			0x1c,		/* FC_CVARRAY */
			0x1,		/* 1 */
/* 402 */	NdrFcShort( 0x2 ),	/* 2 */
/* 404 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x55,		/* FC_DIV_2 */
/* 406 */	NdrFcShort( 0x12 ),	/* 18 */
/* 408 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x55,		/* FC_DIV_2 */
/* 410 */	NdrFcShort( 0x10 ),	/* 16 */
/* 412 */	0x5,		/* FC_WCHAR */
			0x5b,		/* FC_END */
/* 414 */	
			0x1c,		/* FC_CVARRAY */
			0x1,		/* 1 */
/* 416 */	NdrFcShort( 0x2 ),	/* 2 */
/* 418 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x55,		/* FC_DIV_2 */
/* 420 */	NdrFcShort( 0x1a ),	/* 26 */
/* 422 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x55,		/* FC_DIV_2 */
/* 424 */	NdrFcShort( 0x18 ),	/* 24 */
/* 426 */	0x5,		/* FC_WCHAR */
			0x5b,		/* FC_END */
/* 428 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 430 */	NdrFcShort( 0x40 ),	/* 64 */
/* 432 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 434 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 436 */	NdrFcShort( 0xc ),	/* 12 */
/* 438 */	NdrFcShort( 0xc ),	/* 12 */
/* 440 */	0x12, 0x0,	/* FC_UP */
/* 442 */	NdrFcShort( 0xffc8 ),	/* Offset= -56 (386) */
/* 444 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 446 */	NdrFcShort( 0x14 ),	/* 20 */
/* 448 */	NdrFcShort( 0x14 ),	/* 20 */
/* 450 */	0x12, 0x0,	/* FC_UP */
/* 452 */	NdrFcShort( 0xffcc ),	/* Offset= -52 (400) */
/* 454 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 456 */	NdrFcShort( 0x1c ),	/* 28 */
/* 458 */	NdrFcShort( 0x1c ),	/* 28 */
/* 460 */	0x12, 0x0,	/* FC_UP */
/* 462 */	NdrFcShort( 0xffd0 ),	/* Offset= -48 (414) */
/* 464 */	
			0x5b,		/* FC_END */

			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 466 */	0x0,		/* 0 */
			NdrFcShort( 0xff97 ),	/* Offset= -105 (362) */
			0x6,		/* FC_SHORT */
/* 470 */	0x6,		/* FC_SHORT */
			0x8,		/* FC_LONG */
/* 472 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 474 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 476 */	0x6,		/* FC_SHORT */
			0x8,		/* FC_LONG */
/* 478 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 480 */	NdrFcShort( 0xff8a ),	/* Offset= -118 (362) */
/* 482 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 484 */	0x2,		/* FC_CHAR */
			0x3f,		/* FC_STRUCTPAD3 */
/* 486 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 488 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 490 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 492 */	NdrFcShort( 0x8 ),	/* 8 */
/* 494 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 496 */	NdrFcShort( 0xff7a ),	/* Offset= -134 (362) */
/* 498 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 500 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x1,		/* 1 */
/* 502 */	NdrFcShort( 0x4 ),	/* 4 */
/* 504 */	NdrFcShort( 0x0 ),	/* 0 */
/* 506 */	NdrFcShort( 0x0 ),	/* Offset= 0 (506) */
/* 508 */	0xd,		/* FC_ENUM16 */
			0x5b,		/* FC_END */
/* 510 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 512 */	NdrFcShort( 0x10 ),	/* 16 */
/* 514 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 516 */	NdrFcShort( 0xff66 ),	/* Offset= -154 (362) */
/* 518 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 520 */	NdrFcShort( 0xff62 ),	/* Offset= -158 (362) */
/* 522 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 524 */	
			0x15,		/* FC_STRUCT */
			0x7,		/* 7 */
/* 526 */	NdrFcShort( 0x8 ),	/* 8 */
/* 528 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 530 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x7,		/* 7 */
/* 532 */	NdrFcShort( 0x54 ),	/* 84 */
/* 534 */	NdrFcShort( 0x0 ),	/* 0 */
/* 536 */	NdrFcShort( 0x0 ),	/* Offset= 0 (536) */
/* 538 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 540 */	NdrFcShort( 0xff90 ),	/* Offset= -112 (428) */
/* 542 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 544 */	NdrFcShort( 0xffec ),	/* Offset= -20 (524) */
/* 546 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 548 */	NdrFcShort( 0xffe8 ),	/* Offset= -24 (524) */
/* 550 */	0x6,		/* FC_SHORT */
			0x3e,		/* FC_STRUCTPAD2 */
/* 552 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 554 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x7,		/* 7 */
/* 556 */	NdrFcShort( 0x18 ),	/* 24 */
/* 558 */	NdrFcShort( 0x0 ),	/* 0 */
/* 560 */	NdrFcShort( 0x0 ),	/* Offset= 0 (560) */
/* 562 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 564 */	NdrFcShort( 0xffd8 ),	/* Offset= -40 (524) */
/* 566 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 568 */	NdrFcShort( 0xffd4 ),	/* Offset= -44 (524) */
/* 570 */	0x6,		/* FC_SHORT */
			0x42,		/* FC_STRUCTPAD6 */
/* 572 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 574 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 576 */	NdrFcShort( 0x18 ),	/* 24 */
/* 578 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 580 */	NdrFcShort( 0xff26 ),	/* Offset= -218 (362) */
/* 582 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 584 */	NdrFcShort( 0xff22 ),	/* Offset= -222 (362) */
/* 586 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 588 */	NdrFcShort( 0xff1e ),	/* Offset= -226 (362) */
/* 590 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 592 */	
			0x11, 0x1,	/* FC_RP [all_nodes] */
/* 594 */	NdrFcShort( 0x2 ),	/* Offset= 2 (596) */
/* 596 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0xd,		/* FC_ENUM16 */
/* 598 */	0x26,		/* Corr desc:  parameter, FC_SHORT */
			0x0,		/*  */
/* 600 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 602 */	NdrFcShort( 0xfec2 ),	/* Offset= -318 (284) */
/* 604 */	
			0x11, 0x0,	/* FC_RP */
/* 606 */	NdrFcShort( 0x36 ),	/* Offset= 54 (660) */
/* 608 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 610 */	NdrFcShort( 0x4 ),	/* 4 */
/* 612 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 614 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 616 */	NdrFcShort( 0x0 ),	/* 0 */
/* 618 */	NdrFcShort( 0x0 ),	/* 0 */
/* 620 */	0x12, 0x0,	/* FC_UP */
/* 622 */	NdrFcShort( 0xfe20 ),	/* Offset= -480 (142) */
/* 624 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 626 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 628 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 630 */	NdrFcShort( 0x4 ),	/* 4 */
/* 632 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 634 */	NdrFcShort( 0x0 ),	/* 0 */
/* 636 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 638 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 640 */	NdrFcShort( 0x4 ),	/* 4 */
/* 642 */	NdrFcShort( 0x0 ),	/* 0 */
/* 644 */	NdrFcShort( 0x1 ),	/* 1 */
/* 646 */	NdrFcShort( 0x0 ),	/* 0 */
/* 648 */	NdrFcShort( 0x0 ),	/* 0 */
/* 650 */	0x12, 0x0,	/* FC_UP */
/* 652 */	NdrFcShort( 0xfe02 ),	/* Offset= -510 (142) */
/* 654 */	
			0x5b,		/* FC_END */

			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 656 */	0x0,		/* 0 */
			NdrFcShort( 0xffcf ),	/* Offset= -49 (608) */
			0x5b,		/* FC_END */
/* 660 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 662 */	NdrFcShort( 0x8 ),	/* 8 */
/* 664 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 666 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 668 */	NdrFcShort( 0x4 ),	/* 4 */
/* 670 */	NdrFcShort( 0x4 ),	/* 4 */
/* 672 */	0x12, 0x1,	/* FC_UP [all_nodes] */
/* 674 */	NdrFcShort( 0xffd2 ),	/* Offset= -46 (628) */
/* 676 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 678 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 680 */	
			0x11, 0x0,	/* FC_RP */
/* 682 */	NdrFcShort( 0xc ),	/* Offset= 12 (694) */
/* 684 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 686 */	NdrFcShort( 0x4 ),	/* 4 */
/* 688 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 690 */	NdrFcShort( 0x0 ),	/* 0 */
/* 692 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 694 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 696 */	NdrFcShort( 0x8 ),	/* 8 */
/* 698 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 700 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 702 */	NdrFcShort( 0x4 ),	/* 4 */
/* 704 */	NdrFcShort( 0x4 ),	/* 4 */
/* 706 */	0x12, 0x0,	/* FC_UP */
/* 708 */	NdrFcShort( 0xffe8 ),	/* Offset= -24 (684) */
/* 710 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 712 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 714 */	
			0x1c,		/* FC_CVARRAY */
			0x3,		/* 3 */
/* 716 */	NdrFcShort( 0x8 ),	/* 8 */
/* 718 */	0x40,		/* Corr desc:  constant, val=1000 */
			0x0,		/* 0 */
/* 720 */	NdrFcShort( 0x3e8 ),	/* 1000 */
/* 722 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 724 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 726 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 728 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x4a,		/* FC_VARIABLE_OFFSET */
/* 730 */	NdrFcShort( 0x8 ),	/* 8 */
/* 732 */	NdrFcShort( 0x0 ),	/* 0 */
/* 734 */	NdrFcShort( 0x1 ),	/* 1 */
/* 736 */	NdrFcShort( 0x4 ),	/* 4 */
/* 738 */	NdrFcShort( 0x4 ),	/* 4 */
/* 740 */	0x12, 0x0,	/* FC_UP */
/* 742 */	NdrFcShort( 0xfd62 ),	/* Offset= -670 (72) */
/* 744 */	
			0x5b,		/* FC_END */

			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 746 */	0x0,		/* 0 */
			NdrFcShort( 0xfd6b ),	/* Offset= -661 (86) */
			0x5b,		/* FC_END */
/* 750 */	
			0x11, 0x0,	/* FC_RP */
/* 752 */	NdrFcShort( 0x2 ),	/* Offset= 2 (754) */
/* 754 */	
			0x1c,		/* FC_CVARRAY */
			0x3,		/* 3 */
/* 756 */	NdrFcShort( 0x4 ),	/* 4 */
/* 758 */	0x40,		/* Corr desc:  constant, val=1000 */
			0x0,		/* 0 */
/* 760 */	NdrFcShort( 0x3e8 ),	/* 1000 */
/* 762 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 764 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 766 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 768 */	
			0x11, 0x0,	/* FC_RP */
/* 770 */	NdrFcShort( 0x22 ),	/* Offset= 34 (804) */
/* 772 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 774 */	NdrFcShort( 0x8 ),	/* 8 */
/* 776 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 778 */	NdrFcShort( 0x0 ),	/* 0 */
/* 780 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 782 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 784 */	NdrFcShort( 0x8 ),	/* 8 */
/* 786 */	NdrFcShort( 0x0 ),	/* 0 */
/* 788 */	NdrFcShort( 0x1 ),	/* 1 */
/* 790 */	NdrFcShort( 0x4 ),	/* 4 */
/* 792 */	NdrFcShort( 0x4 ),	/* 4 */
/* 794 */	0x12, 0x0,	/* FC_UP */
/* 796 */	NdrFcShort( 0xfd2c ),	/* Offset= -724 (72) */
/* 798 */	
			0x5b,		/* FC_END */

			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 800 */	0x0,		/* 0 */
			NdrFcShort( 0xfd35 ),	/* Offset= -715 (86) */
			0x5b,		/* FC_END */
/* 804 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 806 */	NdrFcShort( 0x8 ),	/* 8 */
/* 808 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 810 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 812 */	NdrFcShort( 0x4 ),	/* 4 */
/* 814 */	NdrFcShort( 0x4 ),	/* 4 */
/* 816 */	0x12, 0x1,	/* FC_UP [all_nodes] */
/* 818 */	NdrFcShort( 0xffd2 ),	/* Offset= -46 (772) */
/* 820 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 822 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 824 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 826 */	NdrFcShort( 0x2 ),	/* Offset= 2 (828) */
/* 828 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 830 */	NdrFcShort( 0x2 ),	/* Offset= 2 (832) */
/* 832 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0xd,		/* FC_ENUM16 */
/* 834 */	0x26,		/* Corr desc:  parameter, FC_SHORT */
			0x0,		/*  */
/* 836 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 838 */	NdrFcShort( 0x2 ),	/* Offset= 2 (840) */
/* 840 */	NdrFcShort( 0x18 ),	/* 24 */
/* 842 */	NdrFcShort( 0x3004 ),	/* 12292 */
/* 844 */	NdrFcLong( 0x1 ),	/* 1 */
/* 848 */	NdrFcShort( 0x16 ),	/* Offset= 22 (870) */
/* 850 */	NdrFcLong( 0x2 ),	/* 2 */
/* 854 */	NdrFcShort( 0xfd00 ),	/* Offset= -768 (86) */
/* 856 */	NdrFcLong( 0x3 ),	/* 3 */
/* 860 */	NdrFcShort( 0x2e ),	/* Offset= 46 (906) */
/* 862 */	NdrFcLong( 0x4 ),	/* 4 */
/* 866 */	NdrFcShort( 0xfcf4 ),	/* Offset= -780 (86) */
/* 868 */	NdrFcShort( 0xffff ),	/* Offset= -1 (867) */
/* 870 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 872 */	NdrFcShort( 0x18 ),	/* 24 */
/* 874 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 876 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 878 */	NdrFcShort( 0x4 ),	/* 4 */
/* 880 */	NdrFcShort( 0x4 ),	/* 4 */
/* 882 */	0x12, 0x0,	/* FC_UP */
/* 884 */	NdrFcShort( 0xfcd4 ),	/* Offset= -812 (72) */
/* 886 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 888 */	NdrFcShort( 0x14 ),	/* 20 */
/* 890 */	NdrFcShort( 0x14 ),	/* 20 */
/* 892 */	0x12, 0x0,	/* FC_UP */
/* 894 */	NdrFcShort( 0xfe12 ),	/* Offset= -494 (400) */
/* 896 */	
			0x5b,		/* FC_END */

			0x6,		/* FC_SHORT */
/* 898 */	0x6,		/* FC_SHORT */
			0x8,		/* FC_LONG */
/* 900 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 902 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 904 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 906 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 908 */	NdrFcShort( 0x4 ),	/* 4 */
/* 910 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 912 */	
			0x11, 0x1,	/* FC_RP [all_nodes] */
/* 914 */	NdrFcShort( 0x2 ),	/* Offset= 2 (916) */
/* 916 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0xd,		/* FC_ENUM16 */
/* 918 */	0x26,		/* Corr desc:  parameter, FC_SHORT */
			0x0,		/*  */
/* 920 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 922 */	NdrFcShort( 0xffae ),	/* Offset= -82 (840) */
/* 924 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 926 */	NdrFcShort( 0x2 ),	/* Offset= 2 (928) */
/* 928 */	
			0x12, 0x0,	/* FC_UP */
/* 930 */	NdrFcShort( 0x2 ),	/* Offset= 2 (932) */
/* 932 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 934 */	NdrFcShort( 0xc ),	/* 12 */
/* 936 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 938 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 940 */	NdrFcShort( 0x4 ),	/* 4 */
/* 942 */	NdrFcShort( 0x4 ),	/* 4 */
/* 944 */	0x12, 0x0,	/* FC_UP */
/* 946 */	NdrFcShort( 0xfefa ),	/* Offset= -262 (684) */
/* 948 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 950 */	NdrFcShort( 0x8 ),	/* 8 */
/* 952 */	NdrFcShort( 0x8 ),	/* 8 */
/* 954 */	0x12, 0x0,	/* FC_UP */
/* 956 */	NdrFcShort( 0xfef0 ),	/* Offset= -272 (684) */
/* 958 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 960 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 962 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 964 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 966 */	NdrFcShort( 0x2 ),	/* Offset= 2 (968) */
/* 968 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 970 */	NdrFcShort( 0x2 ),	/* Offset= 2 (972) */
/* 972 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0xd,		/* FC_ENUM16 */
/* 974 */	0x26,		/* Corr desc:  parameter, FC_SHORT */
			0x0,		/*  */
/* 976 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 978 */	NdrFcShort( 0x2 ),	/* Offset= 2 (980) */
/* 980 */	NdrFcShort( 0x14 ),	/* 20 */
/* 982 */	NdrFcShort( 0x3003 ),	/* 12291 */
/* 984 */	NdrFcLong( 0x1 ),	/* 1 */
/* 988 */	NdrFcShort( 0x1e ),	/* Offset= 30 (1018) */
/* 990 */	NdrFcLong( 0x2 ),	/* 2 */
/* 994 */	NdrFcShort( 0xfc74 ),	/* Offset= -908 (86) */
/* 996 */	NdrFcLong( 0x3 ),	/* 3 */
/* 1000 */	NdrFcShort( 0xfc6e ),	/* Offset= -914 (86) */
/* 1002 */	NdrFcShort( 0xffff ),	/* Offset= -1 (1001) */
/* 1004 */	
			0x1c,		/* FC_CVARRAY */
			0x1,		/* 1 */
/* 1006 */	NdrFcShort( 0x2 ),	/* 2 */
/* 1008 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x55,		/* FC_DIV_2 */
/* 1010 */	NdrFcShort( 0xe ),	/* 14 */
/* 1012 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x55,		/* FC_DIV_2 */
/* 1014 */	NdrFcShort( 0xc ),	/* 12 */
/* 1016 */	0x5,		/* FC_WCHAR */
			0x5b,		/* FC_END */
/* 1018 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 1020 */	NdrFcShort( 0x14 ),	/* 20 */
/* 1022 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1024 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1026 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1028 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1030 */	0x12, 0x0,	/* FC_UP */
/* 1032 */	NdrFcShort( 0xfc40 ),	/* Offset= -960 (72) */
/* 1034 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1036 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1038 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1040 */	0x12, 0x0,	/* FC_UP */
/* 1042 */	NdrFcShort( 0xffda ),	/* Offset= -38 (1004) */
/* 1044 */	
			0x5b,		/* FC_END */

			0x6,		/* FC_SHORT */
/* 1046 */	0x6,		/* FC_SHORT */
			0x8,		/* FC_LONG */
/* 1048 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 1050 */	0x6,		/* FC_SHORT */
			0x8,		/* FC_LONG */
/* 1052 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1054 */	
			0x11, 0x1,	/* FC_RP [all_nodes] */
/* 1056 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1058) */
/* 1058 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0xd,		/* FC_ENUM16 */
/* 1060 */	0x26,		/* Corr desc:  parameter, FC_SHORT */
			0x0,		/*  */
/* 1062 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1064 */	NdrFcShort( 0xffac ),	/* Offset= -84 (980) */
/* 1066 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 1068 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1070) */
/* 1070 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 1072 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1074) */
/* 1074 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0xd,		/* FC_ENUM16 */
/* 1076 */	0x26,		/* Corr desc:  parameter, FC_SHORT */
			0x0,		/*  */
/* 1078 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1080 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1082) */
/* 1082 */	NdrFcShort( 0x2c8 ),	/* 712 */
/* 1084 */	NdrFcShort( 0x7017 ),	/* 28695 */
/* 1086 */	NdrFcLong( 0x1 ),	/* 1 */
/* 1090 */	NdrFcShort( 0xa4 ),	/* Offset= 164 (1254) */
/* 1092 */	NdrFcLong( 0x2 ),	/* 2 */
/* 1096 */	NdrFcShort( 0xdc ),	/* Offset= 220 (1316) */
/* 1098 */	NdrFcLong( 0x3 ),	/* 3 */
/* 1102 */	NdrFcShort( 0x140 ),	/* Offset= 320 (1422) */
/* 1104 */	NdrFcLong( 0x4 ),	/* 4 */
/* 1108 */	NdrFcShort( 0x1d2 ),	/* Offset= 466 (1574) */
/* 1110 */	NdrFcLong( 0x5 ),	/* 5 */
/* 1114 */	NdrFcShort( 0x1fe ),	/* Offset= 510 (1624) */
/* 1116 */	NdrFcLong( 0x6 ),	/* 6 */
/* 1120 */	NdrFcShort( 0x28a ),	/* Offset= 650 (1770) */
/* 1122 */	NdrFcLong( 0x7 ),	/* 7 */
/* 1126 */	NdrFcShort( 0xfbf0 ),	/* Offset= -1040 (86) */
/* 1128 */	NdrFcLong( 0x8 ),	/* 8 */
/* 1132 */	NdrFcShort( 0xfbea ),	/* Offset= -1046 (86) */
/* 1134 */	NdrFcLong( 0x9 ),	/* 9 */
/* 1138 */	NdrFcShort( 0xff18 ),	/* Offset= -232 (906) */
/* 1140 */	NdrFcLong( 0xa ),	/* 10 */
/* 1144 */	NdrFcShort( 0x272 ),	/* Offset= 626 (1770) */
/* 1146 */	NdrFcLong( 0xb ),	/* 11 */
/* 1150 */	NdrFcShort( 0xfbd8 ),	/* Offset= -1064 (86) */
/* 1152 */	NdrFcLong( 0xc ),	/* 12 */
/* 1156 */	NdrFcShort( 0xfbd2 ),	/* Offset= -1070 (86) */
/* 1158 */	NdrFcLong( 0xd ),	/* 13 */
/* 1162 */	NdrFcShort( 0xfbcc ),	/* Offset= -1076 (86) */
/* 1164 */	NdrFcLong( 0xe ),	/* 14 */
/* 1168 */	NdrFcShort( 0xfbc6 ),	/* Offset= -1082 (86) */
/* 1170 */	NdrFcLong( 0x10 ),	/* 16 */
/* 1174 */	NdrFcShort( 0xfef4 ),	/* Offset= -268 (906) */
/* 1176 */	NdrFcLong( 0x11 ),	/* 17 */
/* 1180 */	NdrFcShort( 0xfd4e ),	/* Offset= -690 (490) */
/* 1182 */	NdrFcLong( 0x12 ),	/* 18 */
/* 1186 */	NdrFcShort( 0x28e ),	/* Offset= 654 (1840) */
/* 1188 */	NdrFcLong( 0x13 ),	/* 19 */
/* 1192 */	NdrFcShort( 0x298 ),	/* Offset= 664 (1856) */
/* 1194 */	NdrFcLong( 0x14 ),	/* 20 */
/* 1198 */	NdrFcShort( 0xfba8 ),	/* Offset= -1112 (86) */
/* 1200 */	NdrFcLong( 0x15 ),	/* 21 */
/* 1204 */	NdrFcShort( 0x340 ),	/* Offset= 832 (2036) */
/* 1206 */	NdrFcLong( 0x16 ),	/* 22 */
/* 1210 */	NdrFcShort( 0x428 ),	/* Offset= 1064 (2274) */
/* 1212 */	NdrFcLong( 0x17 ),	/* 23 */
/* 1216 */	NdrFcShort( 0x452 ),	/* Offset= 1106 (2322) */
/* 1218 */	NdrFcLong( 0x18 ),	/* 24 */
/* 1222 */	NdrFcShort( 0x53e ),	/* Offset= 1342 (2564) */
/* 1224 */	NdrFcShort( 0xffff ),	/* Offset= -1 (1223) */
/* 1226 */	
			0x1c,		/* FC_CVARRAY */
			0x1,		/* 1 */
/* 1228 */	NdrFcShort( 0x2 ),	/* 2 */
/* 1230 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x55,		/* FC_DIV_2 */
/* 1232 */	NdrFcShort( 0x16 ),	/* 22 */
/* 1234 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x55,		/* FC_DIV_2 */
/* 1236 */	NdrFcShort( 0x14 ),	/* 20 */
/* 1238 */	0x5,		/* FC_WCHAR */
			0x5b,		/* FC_END */
/* 1240 */	
			0x1c,		/* FC_CVARRAY */
			0x1,		/* 1 */
/* 1242 */	NdrFcShort( 0x2 ),	/* 2 */
/* 1244 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x55,		/* FC_DIV_2 */
/* 1246 */	NdrFcShort( 0x1e ),	/* 30 */
/* 1248 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x55,		/* FC_DIV_2 */
/* 1250 */	NdrFcShort( 0x1c ),	/* 28 */
/* 1252 */	0x5,		/* FC_WCHAR */
			0x5b,		/* FC_END */
/* 1254 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 1256 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1258 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1260 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1262 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1264 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1266 */	0x12, 0x0,	/* FC_UP */
/* 1268 */	NdrFcShort( 0xfb54 ),	/* Offset= -1196 (72) */
/* 1270 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1272 */	NdrFcShort( 0xc ),	/* 12 */
/* 1274 */	NdrFcShort( 0xc ),	/* 12 */
/* 1276 */	0x12, 0x0,	/* FC_UP */
/* 1278 */	NdrFcShort( 0xfc84 ),	/* Offset= -892 (386) */
/* 1280 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1282 */	NdrFcShort( 0x18 ),	/* 24 */
/* 1284 */	NdrFcShort( 0x18 ),	/* 24 */
/* 1286 */	0x12, 0x0,	/* FC_UP */
/* 1288 */	NdrFcShort( 0xffc2 ),	/* Offset= -62 (1226) */
/* 1290 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1292 */	NdrFcShort( 0x20 ),	/* 32 */
/* 1294 */	NdrFcShort( 0x20 ),	/* 32 */
/* 1296 */	0x12, 0x0,	/* FC_UP */
/* 1298 */	NdrFcShort( 0xffc6 ),	/* Offset= -58 (1240) */
/* 1300 */	
			0x5b,		/* FC_END */

			0x6,		/* FC_SHORT */
/* 1302 */	0x6,		/* FC_SHORT */
			0x8,		/* FC_LONG */
/* 1304 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1306 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1308 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1310 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 1312 */	0x6,		/* FC_SHORT */
			0x8,		/* FC_LONG */
/* 1314 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1316 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 1318 */	NdrFcShort( 0x14 ),	/* 20 */
/* 1320 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1322 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1324 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1326 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1328 */	0x12, 0x0,	/* FC_UP */
/* 1330 */	NdrFcShort( 0xfb16 ),	/* Offset= -1258 (72) */
/* 1332 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1334 */	NdrFcShort( 0xc ),	/* 12 */
/* 1336 */	NdrFcShort( 0xc ),	/* 12 */
/* 1338 */	0x12, 0x0,	/* FC_UP */
/* 1340 */	NdrFcShort( 0xfc46 ),	/* Offset= -954 (386) */
/* 1342 */	
			0x5b,		/* FC_END */

			0x6,		/* FC_SHORT */
/* 1344 */	0x6,		/* FC_SHORT */
			0x8,		/* FC_LONG */
/* 1346 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1348 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 1350 */	0x6,		/* FC_SHORT */
			0x5b,		/* FC_END */
/* 1352 */	
			0x1c,		/* FC_CVARRAY */
			0x1,		/* 1 */
/* 1354 */	NdrFcShort( 0x2 ),	/* 2 */
/* 1356 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x55,		/* FC_DIV_2 */
/* 1358 */	NdrFcShort( 0x22 ),	/* 34 */
/* 1360 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x55,		/* FC_DIV_2 */
/* 1362 */	NdrFcShort( 0x20 ),	/* 32 */
/* 1364 */	0x5,		/* FC_WCHAR */
			0x5b,		/* FC_END */
/* 1366 */	
			0x1c,		/* FC_CVARRAY */
			0x1,		/* 1 */
/* 1368 */	NdrFcShort( 0x2 ),	/* 2 */
/* 1370 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x55,		/* FC_DIV_2 */
/* 1372 */	NdrFcShort( 0x2a ),	/* 42 */
/* 1374 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x55,		/* FC_DIV_2 */
/* 1376 */	NdrFcShort( 0x28 ),	/* 40 */
/* 1378 */	0x5,		/* FC_WCHAR */
			0x5b,		/* FC_END */
/* 1380 */	
			0x1c,		/* FC_CVARRAY */
			0x1,		/* 1 */
/* 1382 */	NdrFcShort( 0x2 ),	/* 2 */
/* 1384 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x55,		/* FC_DIV_2 */
/* 1386 */	NdrFcShort( 0x32 ),	/* 50 */
/* 1388 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x55,		/* FC_DIV_2 */
/* 1390 */	NdrFcShort( 0x30 ),	/* 48 */
/* 1392 */	0x5,		/* FC_WCHAR */
			0x5b,		/* FC_END */
/* 1394 */	
			0x1c,		/* FC_CVARRAY */
			0x1,		/* 1 */
/* 1396 */	NdrFcShort( 0x2 ),	/* 2 */
/* 1398 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x55,		/* FC_DIV_2 */
/* 1400 */	NdrFcShort( 0x3a ),	/* 58 */
/* 1402 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x55,		/* FC_DIV_2 */
/* 1404 */	NdrFcShort( 0x38 ),	/* 56 */
/* 1406 */	0x5,		/* FC_WCHAR */
			0x5b,		/* FC_END */
/* 1408 */	
			0x1c,		/* FC_CVARRAY */
			0x0,		/* 0 */
/* 1410 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1412 */	0x40,		/* Corr desc:  constant, val=1260 */
			0x0,		/* 0 */
/* 1414 */	NdrFcShort( 0x4ec ),	/* 1260 */
/* 1416 */	0x10,		/* Corr desc:  field pointer,  */
			0x59,		/* FC_CALLBACK */
/* 1418 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1420 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 1422 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 1424 */	NdrFcShort( 0x78 ),	/* 120 */
/* 1426 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1428 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1430 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1432 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1434 */	0x12, 0x0,	/* FC_UP */
/* 1436 */	NdrFcShort( 0xfaac ),	/* Offset= -1364 (72) */
/* 1438 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1440 */	NdrFcShort( 0xc ),	/* 12 */
/* 1442 */	NdrFcShort( 0xc ),	/* 12 */
/* 1444 */	0x12, 0x0,	/* FC_UP */
/* 1446 */	NdrFcShort( 0xfbdc ),	/* Offset= -1060 (386) */
/* 1448 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1450 */	NdrFcShort( 0x1c ),	/* 28 */
/* 1452 */	NdrFcShort( 0x1c ),	/* 28 */
/* 1454 */	0x12, 0x0,	/* FC_UP */
/* 1456 */	NdrFcShort( 0xfbee ),	/* Offset= -1042 (414) */
/* 1458 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1460 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1462 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1464 */	0x12, 0x0,	/* FC_UP */
/* 1466 */	NdrFcShort( 0xff8e ),	/* Offset= -114 (1352) */
/* 1468 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1470 */	NdrFcShort( 0x2c ),	/* 44 */
/* 1472 */	NdrFcShort( 0x2c ),	/* 44 */
/* 1474 */	0x12, 0x0,	/* FC_UP */
/* 1476 */	NdrFcShort( 0xff92 ),	/* Offset= -110 (1366) */
/* 1478 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1480 */	NdrFcShort( 0x34 ),	/* 52 */
/* 1482 */	NdrFcShort( 0x34 ),	/* 52 */
/* 1484 */	0x12, 0x0,	/* FC_UP */
/* 1486 */	NdrFcShort( 0xff96 ),	/* Offset= -106 (1380) */
/* 1488 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1490 */	NdrFcShort( 0x3c ),	/* 60 */
/* 1492 */	NdrFcShort( 0x3c ),	/* 60 */
/* 1494 */	0x12, 0x0,	/* FC_UP */
/* 1496 */	NdrFcShort( 0xff9a ),	/* Offset= -102 (1394) */
/* 1498 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1500 */	NdrFcShort( 0x6c ),	/* 108 */
/* 1502 */	NdrFcShort( 0x6c ),	/* 108 */
/* 1504 */	0x12, 0x0,	/* FC_UP */
/* 1506 */	NdrFcShort( 0xff9e ),	/* Offset= -98 (1408) */
/* 1508 */	
			0x5b,		/* FC_END */

			0x6,		/* FC_SHORT */
/* 1510 */	0x6,		/* FC_SHORT */
			0x8,		/* FC_LONG */
/* 1512 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1514 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1516 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 1518 */	0x6,		/* FC_SHORT */
			0x8,		/* FC_LONG */
/* 1520 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1522 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 1524 */	0x6,		/* FC_SHORT */
			0x8,		/* FC_LONG */
/* 1526 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1528 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 1530 */	0x6,		/* FC_SHORT */
			0x8,		/* FC_LONG */
/* 1532 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1534 */	NdrFcShort( 0xfb6c ),	/* Offset= -1172 (362) */
/* 1536 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1538 */	NdrFcShort( 0xfb68 ),	/* Offset= -1176 (362) */
/* 1540 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1542 */	NdrFcShort( 0xfb64 ),	/* Offset= -1180 (362) */
/* 1544 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1546 */	NdrFcShort( 0xfb60 ),	/* Offset= -1184 (362) */
/* 1548 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1550 */	NdrFcShort( 0xfb5c ),	/* Offset= -1188 (362) */
/* 1552 */	0x6,		/* FC_SHORT */
			0x3e,		/* FC_STRUCTPAD2 */
/* 1554 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 1556 */	0x6,		/* FC_SHORT */
			0x8,		/* FC_LONG */
/* 1558 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1560 */	
			0x1c,		/* FC_CVARRAY */
			0x0,		/* 0 */
/* 1562 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1564 */	0x40,		/* Corr desc:  constant, val=1260 */
			0x0,		/* 0 */
/* 1566 */	NdrFcShort( 0x4ec ),	/* 1260 */
/* 1568 */	0x10,		/* Corr desc:  field pointer,  */
			0x59,		/* FC_CALLBACK */
/* 1570 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1572 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 1574 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 1576 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1578 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1580 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1582 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1584 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1586 */	0x12, 0x0,	/* FC_UP */
/* 1588 */	NdrFcShort( 0xffe4 ),	/* Offset= -28 (1560) */
/* 1590 */	
			0x5b,		/* FC_END */

			0x6,		/* FC_SHORT */
/* 1592 */	0x3e,		/* FC_STRUCTPAD2 */
			0x8,		/* FC_LONG */
/* 1594 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1596 */	
			0x1c,		/* FC_CVARRAY */
			0x1,		/* 1 */
/* 1598 */	NdrFcShort( 0x2 ),	/* 2 */
/* 1600 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x55,		/* FC_DIV_2 */
/* 1602 */	NdrFcShort( 0x42 ),	/* 66 */
/* 1604 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x55,		/* FC_DIV_2 */
/* 1606 */	NdrFcShort( 0x40 ),	/* 64 */
/* 1608 */	0x5,		/* FC_WCHAR */
			0x5b,		/* FC_END */
/* 1610 */	
			0x1c,		/* FC_CVARRAY */
			0x0,		/* 0 */
/* 1612 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1614 */	0x40,		/* Corr desc:  constant, val=1260 */
			0x0,		/* 0 */
/* 1616 */	NdrFcShort( 0x4ec ),	/* 1260 */
/* 1618 */	0x10,		/* Corr desc:  field pointer,  */
			0x59,		/* FC_CALLBACK */
/* 1620 */	NdrFcShort( 0x2 ),	/* 2 */
/* 1622 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 1624 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 1626 */	NdrFcShort( 0x78 ),	/* 120 */
/* 1628 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1630 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1632 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1634 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1636 */	0x12, 0x0,	/* FC_UP */
/* 1638 */	NdrFcShort( 0xf9e2 ),	/* Offset= -1566 (72) */
/* 1640 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1642 */	NdrFcShort( 0xc ),	/* 12 */
/* 1644 */	NdrFcShort( 0xc ),	/* 12 */
/* 1646 */	0x12, 0x0,	/* FC_UP */
/* 1648 */	NdrFcShort( 0xfb12 ),	/* Offset= -1262 (386) */
/* 1650 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1652 */	NdrFcShort( 0x1c ),	/* 28 */
/* 1654 */	NdrFcShort( 0x1c ),	/* 28 */
/* 1656 */	0x12, 0x0,	/* FC_UP */
/* 1658 */	NdrFcShort( 0xfb24 ),	/* Offset= -1244 (414) */
/* 1660 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1662 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1664 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1666 */	0x12, 0x0,	/* FC_UP */
/* 1668 */	NdrFcShort( 0xfec4 ),	/* Offset= -316 (1352) */
/* 1670 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1672 */	NdrFcShort( 0x2c ),	/* 44 */
/* 1674 */	NdrFcShort( 0x2c ),	/* 44 */
/* 1676 */	0x12, 0x0,	/* FC_UP */
/* 1678 */	NdrFcShort( 0xfec8 ),	/* Offset= -312 (1366) */
/* 1680 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1682 */	NdrFcShort( 0x34 ),	/* 52 */
/* 1684 */	NdrFcShort( 0x34 ),	/* 52 */
/* 1686 */	0x12, 0x0,	/* FC_UP */
/* 1688 */	NdrFcShort( 0xfecc ),	/* Offset= -308 (1380) */
/* 1690 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1692 */	NdrFcShort( 0x3c ),	/* 60 */
/* 1694 */	NdrFcShort( 0x3c ),	/* 60 */
/* 1696 */	0x12, 0x0,	/* FC_UP */
/* 1698 */	NdrFcShort( 0xfed0 ),	/* Offset= -304 (1394) */
/* 1700 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1702 */	NdrFcShort( 0x44 ),	/* 68 */
/* 1704 */	NdrFcShort( 0x44 ),	/* 68 */
/* 1706 */	0x12, 0x0,	/* FC_UP */
/* 1708 */	NdrFcShort( 0xff90 ),	/* Offset= -112 (1596) */
/* 1710 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1712 */	NdrFcShort( 0x5c ),	/* 92 */
/* 1714 */	NdrFcShort( 0x5c ),	/* 92 */
/* 1716 */	0x12, 0x0,	/* FC_UP */
/* 1718 */	NdrFcShort( 0xff94 ),	/* Offset= -108 (1610) */
/* 1720 */	
			0x5b,		/* FC_END */

			0x6,		/* FC_SHORT */
/* 1722 */	0x6,		/* FC_SHORT */
			0x8,		/* FC_LONG */
/* 1724 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1726 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1728 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 1730 */	0x6,		/* FC_SHORT */
			0x8,		/* FC_LONG */
/* 1732 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1734 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 1736 */	0x6,		/* FC_SHORT */
			0x8,		/* FC_LONG */
/* 1738 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1740 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 1742 */	0x6,		/* FC_SHORT */
			0x8,		/* FC_LONG */
/* 1744 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1746 */	0x8,		/* FC_LONG */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 1748 */	0x0,		/* 0 */
			NdrFcShort( 0xfa95 ),	/* Offset= -1387 (362) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 1752 */	0x0,		/* 0 */
			NdrFcShort( 0xfa91 ),	/* Offset= -1391 (362) */
			0x6,		/* FC_SHORT */
/* 1756 */	0x3e,		/* FC_STRUCTPAD2 */
			0x8,		/* FC_LONG */
/* 1758 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1760 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1762 */	NdrFcShort( 0xfa88 ),	/* Offset= -1400 (362) */
/* 1764 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1766 */	NdrFcShort( 0xfa84 ),	/* Offset= -1404 (362) */
/* 1768 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1770 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 1772 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1774 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1776 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1778 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1780 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1782 */	0x12, 0x0,	/* FC_UP */
/* 1784 */	NdrFcShort( 0xf950 ),	/* Offset= -1712 (72) */
/* 1786 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1788 */	NdrFcShort( 0xc ),	/* 12 */
/* 1790 */	NdrFcShort( 0xc ),	/* 12 */
/* 1792 */	0x12, 0x0,	/* FC_UP */
/* 1794 */	NdrFcShort( 0xfa80 ),	/* Offset= -1408 (386) */
/* 1796 */	
			0x5b,		/* FC_END */

			0x6,		/* FC_SHORT */
/* 1798 */	0x6,		/* FC_SHORT */
			0x8,		/* FC_LONG */
/* 1800 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1802 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1804 */	
			0x1d,		/* FC_SMFARRAY */
			0x0,		/* 0 */
/* 1806 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1808 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 1810 */	
			0x15,		/* FC_STRUCT */
			0x0,		/* 0 */
/* 1812 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1814 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1816 */	NdrFcShort( 0xfff4 ),	/* Offset= -12 (1804) */
/* 1818 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1820 */	
			0x1d,		/* FC_SMFARRAY */
			0x0,		/* 0 */
/* 1822 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1824 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1826 */	NdrFcShort( 0xfff0 ),	/* Offset= -16 (1810) */
/* 1828 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1830 */	
			0x15,		/* FC_STRUCT */
			0x0,		/* 0 */
/* 1832 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1834 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1836 */	NdrFcShort( 0xfff0 ),	/* Offset= -16 (1820) */
/* 1838 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1840 */	
			0x15,		/* FC_STRUCT */
			0x0,		/* 0 */
/* 1842 */	NdrFcShort( 0x23 ),	/* 35 */
/* 1844 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1846 */	NdrFcShort( 0xfff0 ),	/* Offset= -16 (1830) */
/* 1848 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1850 */	NdrFcShort( 0xffec ),	/* Offset= -20 (1830) */
/* 1852 */	0x2,		/* FC_CHAR */
			0x2,		/* FC_CHAR */
/* 1854 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 1856 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 1858 */	NdrFcShort( 0x18 ),	/* 24 */
/* 1860 */	0x8,		/* FC_LONG */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 1862 */	0x0,		/* 0 */
			NdrFcShort( 0xfa23 ),	/* Offset= -1501 (362) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 1866 */	0x0,		/* 0 */
			NdrFcShort( 0xfa1f ),	/* Offset= -1505 (362) */
			0x6,		/* FC_SHORT */
/* 1870 */	0x6,		/* FC_SHORT */
			0x5b,		/* FC_END */
/* 1872 */	
			0x1c,		/* FC_CVARRAY */
			0x1,		/* 1 */
/* 1874 */	NdrFcShort( 0x2 ),	/* 2 */
/* 1876 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x55,		/* FC_DIV_2 */
/* 1878 */	NdrFcShort( 0x4a ),	/* 74 */
/* 1880 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x55,		/* FC_DIV_2 */
/* 1882 */	NdrFcShort( 0x48 ),	/* 72 */
/* 1884 */	0x5,		/* FC_WCHAR */
			0x5b,		/* FC_END */
/* 1886 */	
			0x1c,		/* FC_CVARRAY */
			0x1,		/* 1 */
/* 1888 */	NdrFcShort( 0x2 ),	/* 2 */
/* 1890 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x55,		/* FC_DIV_2 */
/* 1892 */	NdrFcShort( 0x52 ),	/* 82 */
/* 1894 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x55,		/* FC_DIV_2 */
/* 1896 */	NdrFcShort( 0x50 ),	/* 80 */
/* 1898 */	0x5,		/* FC_WCHAR */
			0x5b,		/* FC_END */
/* 1900 */	
			0x1c,		/* FC_CVARRAY */
			0x1,		/* 1 */
/* 1902 */	NdrFcShort( 0x2 ),	/* 2 */
/* 1904 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x55,		/* FC_DIV_2 */
/* 1906 */	NdrFcShort( 0x5a ),	/* 90 */
/* 1908 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x55,		/* FC_DIV_2 */
/* 1910 */	NdrFcShort( 0x58 ),	/* 88 */
/* 1912 */	0x5,		/* FC_WCHAR */
			0x5b,		/* FC_END */
/* 1914 */	
			0x1c,		/* FC_CVARRAY */
			0x1,		/* 1 */
/* 1916 */	NdrFcShort( 0x2 ),	/* 2 */
/* 1918 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x55,		/* FC_DIV_2 */
/* 1920 */	NdrFcShort( 0x62 ),	/* 98 */
/* 1922 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x55,		/* FC_DIV_2 */
/* 1924 */	NdrFcShort( 0x60 ),	/* 96 */
/* 1926 */	0x5,		/* FC_WCHAR */
			0x5b,		/* FC_END */
/* 1928 */	
			0x1c,		/* FC_CVARRAY */
			0x1,		/* 1 */
/* 1930 */	NdrFcShort( 0x2 ),	/* 2 */
/* 1932 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x55,		/* FC_DIV_2 */
/* 1934 */	NdrFcShort( 0x6a ),	/* 106 */
/* 1936 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x55,		/* FC_DIV_2 */
/* 1938 */	NdrFcShort( 0x68 ),	/* 104 */
/* 1940 */	0x5,		/* FC_WCHAR */
			0x5b,		/* FC_END */
/* 1942 */	
			0x1c,		/* FC_CVARRAY */
			0x1,		/* 1 */
/* 1944 */	NdrFcShort( 0x2 ),	/* 2 */
/* 1946 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x55,		/* FC_DIV_2 */
/* 1948 */	NdrFcShort( 0x72 ),	/* 114 */
/* 1950 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x55,		/* FC_DIV_2 */
/* 1952 */	NdrFcShort( 0x70 ),	/* 112 */
/* 1954 */	0x5,		/* FC_WCHAR */
			0x5b,		/* FC_END */
/* 1956 */	
			0x1c,		/* FC_CVARRAY */
			0x1,		/* 1 */
/* 1958 */	NdrFcShort( 0x2 ),	/* 2 */
/* 1960 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x55,		/* FC_DIV_2 */
/* 1962 */	NdrFcShort( 0x7a ),	/* 122 */
/* 1964 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x55,		/* FC_DIV_2 */
/* 1966 */	NdrFcShort( 0x78 ),	/* 120 */
/* 1968 */	0x5,		/* FC_WCHAR */
			0x5b,		/* FC_END */
/* 1970 */	
			0x1c,		/* FC_CVARRAY */
			0x1,		/* 1 */
/* 1972 */	NdrFcShort( 0x2 ),	/* 2 */
/* 1974 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x55,		/* FC_DIV_2 */
/* 1976 */	NdrFcShort( 0x82 ),	/* 130 */
/* 1978 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x55,		/* FC_DIV_2 */
/* 1980 */	NdrFcShort( 0x80 ),	/* 128 */
/* 1982 */	0x5,		/* FC_WCHAR */
			0x5b,		/* FC_END */
/* 1984 */	
			0x1c,		/* FC_CVARRAY */
			0x1,		/* 1 */
/* 1986 */	NdrFcShort( 0x2 ),	/* 2 */
/* 1988 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x55,		/* FC_DIV_2 */
/* 1990 */	NdrFcShort( 0x8a ),	/* 138 */
/* 1992 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x55,		/* FC_DIV_2 */
/* 1994 */	NdrFcShort( 0x88 ),	/* 136 */
/* 1996 */	0x5,		/* FC_WCHAR */
			0x5b,		/* FC_END */
/* 1998 */	
			0x1c,		/* FC_CVARRAY */
			0x1,		/* 1 */
/* 2000 */	NdrFcShort( 0x2 ),	/* 2 */
/* 2002 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x55,		/* FC_DIV_2 */
/* 2004 */	NdrFcShort( 0x92 ),	/* 146 */
/* 2006 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x55,		/* FC_DIV_2 */
/* 2008 */	NdrFcShort( 0x90 ),	/* 144 */
/* 2010 */	0x5,		/* FC_WCHAR */
			0x5b,		/* FC_END */
/* 2012 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 2014 */	NdrFcShort( 0x1 ),	/* 1 */
/* 2016 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 2018 */	NdrFcShort( 0x98 ),	/* 152 */
/* 2020 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 2022 */	
			0x1c,		/* FC_CVARRAY */
			0x0,		/* 0 */
/* 2024 */	NdrFcShort( 0x1 ),	/* 1 */
/* 2026 */	0x40,		/* Corr desc:  constant, val=1260 */
			0x0,		/* 0 */
/* 2028 */	NdrFcShort( 0x4ec ),	/* 1260 */
/* 2030 */	0x10,		/* Corr desc:  field pointer,  */
			0x59,		/* FC_CALLBACK */
/* 2032 */	NdrFcShort( 0x3 ),	/* 3 */
/* 2034 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 2036 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 2038 */	NdrFcShort( 0xc4 ),	/* 196 */
/* 2040 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 2042 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 2044 */	NdrFcShort( 0x34 ),	/* 52 */
/* 2046 */	NdrFcShort( 0x34 ),	/* 52 */
/* 2048 */	0x12, 0x0,	/* FC_UP */
/* 2050 */	NdrFcShort( 0xfd62 ),	/* Offset= -670 (1380) */
/* 2052 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 2054 */	NdrFcShort( 0x3c ),	/* 60 */
/* 2056 */	NdrFcShort( 0x3c ),	/* 60 */
/* 2058 */	0x12, 0x0,	/* FC_UP */
/* 2060 */	NdrFcShort( 0xfd66 ),	/* Offset= -666 (1394) */
/* 2062 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 2064 */	NdrFcShort( 0x44 ),	/* 68 */
/* 2066 */	NdrFcShort( 0x44 ),	/* 68 */
/* 2068 */	0x12, 0x0,	/* FC_UP */
/* 2070 */	NdrFcShort( 0xfe26 ),	/* Offset= -474 (1596) */
/* 2072 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 2074 */	NdrFcShort( 0x4c ),	/* 76 */
/* 2076 */	NdrFcShort( 0x4c ),	/* 76 */
/* 2078 */	0x12, 0x0,	/* FC_UP */
/* 2080 */	NdrFcShort( 0xff30 ),	/* Offset= -208 (1872) */
/* 2082 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 2084 */	NdrFcShort( 0x54 ),	/* 84 */
/* 2086 */	NdrFcShort( 0x54 ),	/* 84 */
/* 2088 */	0x12, 0x0,	/* FC_UP */
/* 2090 */	NdrFcShort( 0xff34 ),	/* Offset= -204 (1886) */
/* 2092 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 2094 */	NdrFcShort( 0x5c ),	/* 92 */
/* 2096 */	NdrFcShort( 0x5c ),	/* 92 */
/* 2098 */	0x12, 0x0,	/* FC_UP */
/* 2100 */	NdrFcShort( 0xff38 ),	/* Offset= -200 (1900) */
/* 2102 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 2104 */	NdrFcShort( 0x64 ),	/* 100 */
/* 2106 */	NdrFcShort( 0x64 ),	/* 100 */
/* 2108 */	0x12, 0x0,	/* FC_UP */
/* 2110 */	NdrFcShort( 0xff3c ),	/* Offset= -196 (1914) */
/* 2112 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 2114 */	NdrFcShort( 0x6c ),	/* 108 */
/* 2116 */	NdrFcShort( 0x6c ),	/* 108 */
/* 2118 */	0x12, 0x0,	/* FC_UP */
/* 2120 */	NdrFcShort( 0xff40 ),	/* Offset= -192 (1928) */
/* 2122 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 2124 */	NdrFcShort( 0x74 ),	/* 116 */
/* 2126 */	NdrFcShort( 0x74 ),	/* 116 */
/* 2128 */	0x12, 0x0,	/* FC_UP */
/* 2130 */	NdrFcShort( 0xff44 ),	/* Offset= -188 (1942) */
/* 2132 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 2134 */	NdrFcShort( 0x7c ),	/* 124 */
/* 2136 */	NdrFcShort( 0x7c ),	/* 124 */
/* 2138 */	0x12, 0x0,	/* FC_UP */
/* 2140 */	NdrFcShort( 0xff48 ),	/* Offset= -184 (1956) */
/* 2142 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 2144 */	NdrFcShort( 0x84 ),	/* 132 */
/* 2146 */	NdrFcShort( 0x84 ),	/* 132 */
/* 2148 */	0x12, 0x0,	/* FC_UP */
/* 2150 */	NdrFcShort( 0xff4c ),	/* Offset= -180 (1970) */
/* 2152 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 2154 */	NdrFcShort( 0x8c ),	/* 140 */
/* 2156 */	NdrFcShort( 0x8c ),	/* 140 */
/* 2158 */	0x12, 0x0,	/* FC_UP */
/* 2160 */	NdrFcShort( 0xff50 ),	/* Offset= -176 (1984) */
/* 2162 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 2164 */	NdrFcShort( 0x94 ),	/* 148 */
/* 2166 */	NdrFcShort( 0x94 ),	/* 148 */
/* 2168 */	0x12, 0x0,	/* FC_UP */
/* 2170 */	NdrFcShort( 0xff54 ),	/* Offset= -172 (1998) */
/* 2172 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 2174 */	NdrFcShort( 0x9c ),	/* 156 */
/* 2176 */	NdrFcShort( 0x9c ),	/* 156 */
/* 2178 */	0x12, 0x0,	/* FC_UP */
/* 2180 */	NdrFcShort( 0xff58 ),	/* Offset= -168 (2012) */
/* 2182 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 2184 */	NdrFcShort( 0xb4 ),	/* 180 */
/* 2186 */	NdrFcShort( 0xb4 ),	/* 180 */
/* 2188 */	0x12, 0x0,	/* FC_UP */
/* 2190 */	NdrFcShort( 0xff58 ),	/* Offset= -168 (2022) */
/* 2192 */	
			0x5b,		/* FC_END */

			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 2194 */	0x0,		/* 0 */
			NdrFcShort( 0xf8d7 ),	/* Offset= -1833 (362) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 2198 */	0x0,		/* 0 */
			NdrFcShort( 0xf8d3 ),	/* Offset= -1837 (362) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 2202 */	0x0,		/* 0 */
			NdrFcShort( 0xf8cf ),	/* Offset= -1841 (362) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 2206 */	0x0,		/* 0 */
			NdrFcShort( 0xf8cb ),	/* Offset= -1845 (362) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 2210 */	0x0,		/* 0 */
			NdrFcShort( 0xf8c7 ),	/* Offset= -1849 (362) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 2214 */	0x0,		/* 0 */
			NdrFcShort( 0xf8c3 ),	/* Offset= -1853 (362) */
			0x6,		/* FC_SHORT */
/* 2218 */	0x6,		/* FC_SHORT */
			0x8,		/* FC_LONG */
/* 2220 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 2222 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 2224 */	0x6,		/* FC_SHORT */
			0x8,		/* FC_LONG */
/* 2226 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 2228 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 2230 */	0x6,		/* FC_SHORT */
			0x8,		/* FC_LONG */
/* 2232 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 2234 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 2236 */	0x6,		/* FC_SHORT */
			0x8,		/* FC_LONG */
/* 2238 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 2240 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 2242 */	0x6,		/* FC_SHORT */
			0x8,		/* FC_LONG */
/* 2244 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 2246 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 2248 */	0x6,		/* FC_SHORT */
			0x8,		/* FC_LONG */
/* 2250 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 2252 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 2254 */	0x6,		/* FC_SHORT */
			0x8,		/* FC_LONG */
/* 2256 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2258 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2260 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2262 */	0x6,		/* FC_SHORT */
			0x3e,		/* FC_STRUCTPAD2 */
/* 2264 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 2266 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 2268 */	0x6,		/* FC_SHORT */
			0x2,		/* FC_CHAR */
/* 2270 */	0x2,		/* FC_CHAR */
			0x2,		/* FC_CHAR */
/* 2272 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 2274 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x7,		/* 7 */
/* 2276 */	NdrFcShort( 0xcc ),	/* 204 */
/* 2278 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2280 */	NdrFcShort( 0x0 ),	/* Offset= 0 (2280) */
/* 2282 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 2284 */	NdrFcShort( 0xff08 ),	/* Offset= -248 (2036) */
/* 2286 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 2288 */	NdrFcShort( 0xf91c ),	/* Offset= -1764 (524) */
/* 2290 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 2292 */	
			0x1d,		/* FC_SMFARRAY */
			0x0,		/* 0 */
/* 2294 */	NdrFcShort( 0x204 ),	/* 516 */
/* 2296 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 2298 */	
			0x15,		/* FC_STRUCT */
			0x0,		/* 0 */
/* 2300 */	NdrFcShort( 0x204 ),	/* 516 */
/* 2302 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 2304 */	NdrFcShort( 0xfff4 ),	/* Offset= -12 (2292) */
/* 2306 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 2308 */	
			0x1c,		/* FC_CVARRAY */
			0x0,		/* 0 */
/* 2310 */	NdrFcShort( 0x1 ),	/* 1 */
/* 2312 */	0x40,		/* Corr desc:  constant, val=1260 */
			0x0,		/* 0 */
/* 2314 */	NdrFcShort( 0x4ec ),	/* 1260 */
/* 2316 */	0x10,		/* Corr desc:  field pointer,  */
			0x59,		/* FC_CALLBACK */
/* 2318 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2320 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 2322 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 2324 */	NdrFcShort( 0x2c8 ),	/* 712 */
/* 2326 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 2328 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 2330 */	NdrFcShort( 0x34 ),	/* 52 */
/* 2332 */	NdrFcShort( 0x34 ),	/* 52 */
/* 2334 */	0x12, 0x0,	/* FC_UP */
/* 2336 */	NdrFcShort( 0xfc44 ),	/* Offset= -956 (1380) */
/* 2338 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 2340 */	NdrFcShort( 0x3c ),	/* 60 */
/* 2342 */	NdrFcShort( 0x3c ),	/* 60 */
/* 2344 */	0x12, 0x0,	/* FC_UP */
/* 2346 */	NdrFcShort( 0xfc48 ),	/* Offset= -952 (1394) */
/* 2348 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 2350 */	NdrFcShort( 0x44 ),	/* 68 */
/* 2352 */	NdrFcShort( 0x44 ),	/* 68 */
/* 2354 */	0x12, 0x0,	/* FC_UP */
/* 2356 */	NdrFcShort( 0xfd08 ),	/* Offset= -760 (1596) */
/* 2358 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 2360 */	NdrFcShort( 0x4c ),	/* 76 */
/* 2362 */	NdrFcShort( 0x4c ),	/* 76 */
/* 2364 */	0x12, 0x0,	/* FC_UP */
/* 2366 */	NdrFcShort( 0xfe12 ),	/* Offset= -494 (1872) */
/* 2368 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 2370 */	NdrFcShort( 0x54 ),	/* 84 */
/* 2372 */	NdrFcShort( 0x54 ),	/* 84 */
/* 2374 */	0x12, 0x0,	/* FC_UP */
/* 2376 */	NdrFcShort( 0xfe16 ),	/* Offset= -490 (1886) */
/* 2378 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 2380 */	NdrFcShort( 0x5c ),	/* 92 */
/* 2382 */	NdrFcShort( 0x5c ),	/* 92 */
/* 2384 */	0x12, 0x0,	/* FC_UP */
/* 2386 */	NdrFcShort( 0xfe1a ),	/* Offset= -486 (1900) */
/* 2388 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 2390 */	NdrFcShort( 0x64 ),	/* 100 */
/* 2392 */	NdrFcShort( 0x64 ),	/* 100 */
/* 2394 */	0x12, 0x0,	/* FC_UP */
/* 2396 */	NdrFcShort( 0xfe1e ),	/* Offset= -482 (1914) */
/* 2398 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 2400 */	NdrFcShort( 0x6c ),	/* 108 */
/* 2402 */	NdrFcShort( 0x6c ),	/* 108 */
/* 2404 */	0x12, 0x0,	/* FC_UP */
/* 2406 */	NdrFcShort( 0xfe22 ),	/* Offset= -478 (1928) */
/* 2408 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 2410 */	NdrFcShort( 0x74 ),	/* 116 */
/* 2412 */	NdrFcShort( 0x74 ),	/* 116 */
/* 2414 */	0x12, 0x0,	/* FC_UP */
/* 2416 */	NdrFcShort( 0xfe26 ),	/* Offset= -474 (1942) */
/* 2418 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 2420 */	NdrFcShort( 0x7c ),	/* 124 */
/* 2422 */	NdrFcShort( 0x7c ),	/* 124 */
/* 2424 */	0x12, 0x0,	/* FC_UP */
/* 2426 */	NdrFcShort( 0xfe2a ),	/* Offset= -470 (1956) */
/* 2428 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 2430 */	NdrFcShort( 0x84 ),	/* 132 */
/* 2432 */	NdrFcShort( 0x84 ),	/* 132 */
/* 2434 */	0x12, 0x0,	/* FC_UP */
/* 2436 */	NdrFcShort( 0xfe2e ),	/* Offset= -466 (1970) */
/* 2438 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 2440 */	NdrFcShort( 0x8c ),	/* 140 */
/* 2442 */	NdrFcShort( 0x8c ),	/* 140 */
/* 2444 */	0x12, 0x0,	/* FC_UP */
/* 2446 */	NdrFcShort( 0xfe32 ),	/* Offset= -462 (1984) */
/* 2448 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 2450 */	NdrFcShort( 0x94 ),	/* 148 */
/* 2452 */	NdrFcShort( 0x94 ),	/* 148 */
/* 2454 */	0x12, 0x0,	/* FC_UP */
/* 2456 */	NdrFcShort( 0xfe36 ),	/* Offset= -458 (1998) */
/* 2458 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 2460 */	NdrFcShort( 0x9c ),	/* 156 */
/* 2462 */	NdrFcShort( 0x9c ),	/* 156 */
/* 2464 */	0x12, 0x0,	/* FC_UP */
/* 2466 */	NdrFcShort( 0xfe3a ),	/* Offset= -454 (2012) */
/* 2468 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 2470 */	NdrFcShort( 0xb4 ),	/* 180 */
/* 2472 */	NdrFcShort( 0xb4 ),	/* 180 */
/* 2474 */	0x12, 0x0,	/* FC_UP */
/* 2476 */	NdrFcShort( 0xff58 ),	/* Offset= -168 (2308) */
/* 2478 */	
			0x5b,		/* FC_END */

			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 2480 */	0x0,		/* 0 */
			NdrFcShort( 0xf7b9 ),	/* Offset= -2119 (362) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 2484 */	0x0,		/* 0 */
			NdrFcShort( 0xf7b5 ),	/* Offset= -2123 (362) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 2488 */	0x0,		/* 0 */
			NdrFcShort( 0xf7b1 ),	/* Offset= -2127 (362) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 2492 */	0x0,		/* 0 */
			NdrFcShort( 0xf7ad ),	/* Offset= -2131 (362) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 2496 */	0x0,		/* 0 */
			NdrFcShort( 0xf7a9 ),	/* Offset= -2135 (362) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 2500 */	0x0,		/* 0 */
			NdrFcShort( 0xf7a5 ),	/* Offset= -2139 (362) */
			0x6,		/* FC_SHORT */
/* 2504 */	0x6,		/* FC_SHORT */
			0x8,		/* FC_LONG */
/* 2506 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 2508 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 2510 */	0x6,		/* FC_SHORT */
			0x8,		/* FC_LONG */
/* 2512 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 2514 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 2516 */	0x6,		/* FC_SHORT */
			0x8,		/* FC_LONG */
/* 2518 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 2520 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 2522 */	0x6,		/* FC_SHORT */
			0x8,		/* FC_LONG */
/* 2524 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 2526 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 2528 */	0x6,		/* FC_SHORT */
			0x8,		/* FC_LONG */
/* 2530 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 2532 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 2534 */	0x6,		/* FC_SHORT */
			0x8,		/* FC_LONG */
/* 2536 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 2538 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 2540 */	0x6,		/* FC_SHORT */
			0x8,		/* FC_LONG */
/* 2542 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2544 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2546 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2548 */	0x6,		/* FC_SHORT */
			0x3e,		/* FC_STRUCTPAD2 */
/* 2550 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 2552 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 2554 */	0x6,		/* FC_SHORT */
			0x2,		/* FC_CHAR */
/* 2556 */	0x2,		/* FC_CHAR */
			0x2,		/* FC_CHAR */
/* 2558 */	0x2,		/* FC_CHAR */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 2560 */	0x0,		/* 0 */
			NdrFcShort( 0xfef9 ),	/* Offset= -263 (2298) */
			0x5b,		/* FC_END */
/* 2564 */	
			0x15,		/* FC_STRUCT */
			0x0,		/* 0 */
/* 2566 */	NdrFcShort( 0x205 ),	/* 517 */
/* 2568 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 2570 */	NdrFcShort( 0xfef0 ),	/* Offset= -272 (2298) */
/* 2572 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 2574 */	
			0x11, 0x1,	/* FC_RP [all_nodes] */
/* 2576 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2578) */
/* 2578 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0xd,		/* FC_ENUM16 */
/* 2580 */	0x26,		/* Corr desc:  parameter, FC_SHORT */
			0x0,		/*  */
/* 2582 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2584 */	NdrFcShort( 0xfa22 ),	/* Offset= -1502 (1082) */
/* 2586 */	
			0x12, 0x0,	/* FC_UP */
/* 2588 */	NdrFcShort( 0xfd0a ),	/* Offset= -758 (1830) */
/* 2590 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 2592 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2594) */
/* 2594 */	
			0x12, 0x0,	/* FC_UP */
/* 2596 */	NdrFcShort( 0x10 ),	/* Offset= 16 (2612) */
/* 2598 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 2600 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2602 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 2604 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2606 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 2608 */	NdrFcShort( 0xf73a ),	/* Offset= -2246 (362) */
/* 2610 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 2612 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 2614 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2616 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 2618 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 2620 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2622 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2624 */	0x12, 0x0,	/* FC_UP */
/* 2626 */	NdrFcShort( 0xffe4 ),	/* Offset= -28 (2598) */
/* 2628 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 2630 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 2632 */	
			0x11, 0x0,	/* FC_RP */
/* 2634 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2636) */
/* 2636 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0xd,		/* FC_ENUM16 */
/* 2638 */	0x26,		/* Corr desc:  parameter, FC_SHORT */
			0x0,		/*  */
/* 2640 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2642 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2644) */
/* 2644 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2646 */	NdrFcShort( 0x3005 ),	/* 12293 */
/* 2648 */	NdrFcLong( 0x1 ),	/* 1 */
/* 2652 */	NdrFcShort( 0x7e ),	/* Offset= 126 (2778) */
/* 2654 */	NdrFcLong( 0x2 ),	/* 2 */
/* 2658 */	NdrFcShort( 0xda ),	/* Offset= 218 (2876) */
/* 2660 */	NdrFcLong( 0x3 ),	/* 3 */
/* 2664 */	NdrFcShort( 0xd4 ),	/* Offset= 212 (2876) */
/* 2666 */	NdrFcLong( 0x4 ),	/* 4 */
/* 2670 */	NdrFcShort( 0x126 ),	/* Offset= 294 (2964) */
/* 2672 */	NdrFcLong( 0x5 ),	/* 5 */
/* 2676 */	NdrFcShort( 0x120 ),	/* Offset= 288 (2964) */
/* 2678 */	NdrFcShort( 0xffff ),	/* Offset= -1 (2677) */
/* 2680 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 2682 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2684 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 2686 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 2688 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2690 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2692 */	0x12, 0x0,	/* FC_UP */
/* 2694 */	NdrFcShort( 0xf966 ),	/* Offset= -1690 (1004) */
/* 2696 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 2698 */	NdrFcShort( 0x18 ),	/* 24 */
/* 2700 */	NdrFcShort( 0x18 ),	/* 24 */
/* 2702 */	0x12, 0x0,	/* FC_UP */
/* 2704 */	NdrFcShort( 0xfa3a ),	/* Offset= -1478 (1226) */
/* 2706 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 2708 */	NdrFcShort( 0x20 ),	/* 32 */
/* 2710 */	NdrFcShort( 0x20 ),	/* 32 */
/* 2712 */	0x12, 0x0,	/* FC_UP */
/* 2714 */	NdrFcShort( 0xfa3e ),	/* Offset= -1474 (1240) */
/* 2716 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 2718 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2720 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 2722 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 2724 */	0x6,		/* FC_SHORT */
			0x8,		/* FC_LONG */
/* 2726 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 2728 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 2730 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 2732 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2734 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 2736 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2738 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 2740 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 2742 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2744 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2746 */	NdrFcShort( 0x3 ),	/* 3 */
/* 2748 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2750 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2752 */	0x12, 0x0,	/* FC_UP */
/* 2754 */	NdrFcShort( 0xf92a ),	/* Offset= -1750 (1004) */
/* 2756 */	NdrFcShort( 0x18 ),	/* 24 */
/* 2758 */	NdrFcShort( 0x18 ),	/* 24 */
/* 2760 */	0x12, 0x0,	/* FC_UP */
/* 2762 */	NdrFcShort( 0xfa00 ),	/* Offset= -1536 (1226) */
/* 2764 */	NdrFcShort( 0x20 ),	/* 32 */
/* 2766 */	NdrFcShort( 0x20 ),	/* 32 */
/* 2768 */	0x12, 0x0,	/* FC_UP */
/* 2770 */	NdrFcShort( 0xfa06 ),	/* Offset= -1530 (1240) */
/* 2772 */	
			0x5b,		/* FC_END */

			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 2774 */	0x0,		/* 0 */
			NdrFcShort( 0xffa1 ),	/* Offset= -95 (2680) */
			0x5b,		/* FC_END */
/* 2778 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 2780 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2782 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 2784 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 2786 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2788 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2790 */	0x12, 0x1,	/* FC_UP [all_nodes] */
/* 2792 */	NdrFcShort( 0xffc2 ),	/* Offset= -62 (2730) */
/* 2794 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 2796 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 2798 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 2800 */	NdrFcShort( 0x1c ),	/* 28 */
/* 2802 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 2804 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 2806 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2808 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2810 */	0x12, 0x0,	/* FC_UP */
/* 2812 */	NdrFcShort( 0xf8f0 ),	/* Offset= -1808 (1004) */
/* 2814 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 2816 */	NdrFcShort( 0x18 ),	/* 24 */
/* 2818 */	NdrFcShort( 0x18 ),	/* 24 */
/* 2820 */	0x12, 0x0,	/* FC_UP */
/* 2822 */	NdrFcShort( 0xf9c4 ),	/* Offset= -1596 (1226) */
/* 2824 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 2826 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2828 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 2830 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 2832 */	0x6,		/* FC_SHORT */
			0x8,		/* FC_LONG */
/* 2834 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 2836 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 2838 */	NdrFcShort( 0x1c ),	/* 28 */
/* 2840 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 2842 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2844 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 2846 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 2848 */	NdrFcShort( 0x1c ),	/* 28 */
/* 2850 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2852 */	NdrFcShort( 0x2 ),	/* 2 */
/* 2854 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2856 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2858 */	0x12, 0x0,	/* FC_UP */
/* 2860 */	NdrFcShort( 0xf8c0 ),	/* Offset= -1856 (1004) */
/* 2862 */	NdrFcShort( 0x18 ),	/* 24 */
/* 2864 */	NdrFcShort( 0x18 ),	/* 24 */
/* 2866 */	0x12, 0x0,	/* FC_UP */
/* 2868 */	NdrFcShort( 0xf996 ),	/* Offset= -1642 (1226) */
/* 2870 */	
			0x5b,		/* FC_END */

			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 2872 */	0x0,		/* 0 */
			NdrFcShort( 0xffb5 ),	/* Offset= -75 (2798) */
			0x5b,		/* FC_END */
/* 2876 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 2878 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2880 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 2882 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 2884 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2886 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2888 */	0x12, 0x1,	/* FC_UP [all_nodes] */
/* 2890 */	NdrFcShort( 0xffca ),	/* Offset= -54 (2836) */
/* 2892 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 2894 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 2896 */	
			0x1c,		/* FC_CVARRAY */
			0x0,		/* 0 */
/* 2898 */	NdrFcShort( 0x1 ),	/* 1 */
/* 2900 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x0,		/*  */
/* 2902 */	NdrFcShort( 0x6 ),	/* 6 */
/* 2904 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x0,		/*  */
/* 2906 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2908 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 2910 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 2912 */	NdrFcShort( 0xc ),	/* 12 */
/* 2914 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 2916 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 2918 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2920 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2922 */	0x12, 0x0,	/* FC_UP */
/* 2924 */	NdrFcShort( 0xffe4 ),	/* Offset= -28 (2896) */
/* 2926 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 2928 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 2930 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 2932 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 2934 */	NdrFcShort( 0xc ),	/* 12 */
/* 2936 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 2938 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2940 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 2942 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 2944 */	NdrFcShort( 0xc ),	/* 12 */
/* 2946 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2948 */	NdrFcShort( 0x1 ),	/* 1 */
/* 2950 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2952 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2954 */	0x12, 0x0,	/* FC_UP */
/* 2956 */	NdrFcShort( 0xffc4 ),	/* Offset= -60 (2896) */
/* 2958 */	
			0x5b,		/* FC_END */

			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 2960 */	0x0,		/* 0 */
			NdrFcShort( 0xffcd ),	/* Offset= -51 (2910) */
			0x5b,		/* FC_END */
/* 2964 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 2966 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2968 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 2970 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 2972 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2974 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2976 */	0x12, 0x1,	/* FC_UP [all_nodes] */
/* 2978 */	NdrFcShort( 0xffd2 ),	/* Offset= -46 (2932) */
/* 2980 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 2982 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 2984 */	
			0x11, 0x0,	/* FC_RP */
/* 2986 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2988) */
/* 2988 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 2990 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2992 */	0x6,		/* FC_SHORT */
			0x3e,		/* FC_STRUCTPAD2 */
/* 2994 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 2996 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 2998 */	NdrFcShort( 0x2 ),	/* Offset= 2 (3000) */
/* 3000 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 3002 */	NdrFcShort( 0x2 ),	/* Offset= 2 (3004) */
/* 3004 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0xd,		/* FC_ENUM16 */
/* 3006 */	0x26,		/* Corr desc:  parameter, FC_SHORT */
			0x0,		/*  */
/* 3008 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3010 */	NdrFcShort( 0xf55a ),	/* Offset= -2726 (284) */
/* 3012 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 3014 */	NdrFcShort( 0x2 ),	/* Offset= 2 (3016) */
/* 3016 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 3018 */	NdrFcShort( 0x2 ),	/* Offset= 2 (3020) */
/* 3020 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0xd,		/* FC_ENUM16 */
/* 3022 */	0x26,		/* Corr desc:  parameter, FC_SHORT */
			0x0,		/*  */
/* 3024 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3026 */	NdrFcShort( 0xf868 ),	/* Offset= -1944 (1082) */
/* 3028 */	
			0x11, 0x0,	/* FC_RP */
/* 3030 */	NdrFcShort( 0x2 ),	/* Offset= 2 (3032) */
/* 3032 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0xd,		/* FC_ENUM16 */
/* 3034 */	0x26,		/* Corr desc:  parameter, FC_SHORT */
			0x0,		/*  */
/* 3036 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3038 */	NdrFcShort( 0xfe76 ),	/* Offset= -394 (2644) */
/* 3040 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 3042 */	NdrFcShort( 0x2 ),	/* Offset= 2 (3044) */
/* 3044 */	0x30,		/* FC_BIND_CONTEXT */
			0xa0,		/* Ctxt flags:  via ptr, out, */
/* 3046 */	0x0,		/* 0 */
			0x4,		/* 4 */
/* 3048 */	
			0x11, 0x0,	/* FC_RP */
/* 3050 */	NdrFcShort( 0x2 ),	/* Offset= 2 (3052) */
/* 3052 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0xd,		/* FC_ENUM16 */
/* 3054 */	0x26,		/* Corr desc:  parameter, FC_SHORT */
			0x0,		/*  */
/* 3056 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3058 */	NdrFcShort( 0xfe62 ),	/* Offset= -414 (2644) */
/* 3060 */	
			0x12, 0x0,	/* FC_UP */
/* 3062 */	NdrFcShort( 0x10 ),	/* Offset= 16 (3078) */
/* 3064 */	
			0x1c,		/* FC_CVARRAY */
			0x0,		/* 0 */
/* 3066 */	NdrFcShort( 0x1 ),	/* 1 */
/* 3068 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x0,		/*  */
/* 3070 */	NdrFcShort( 0x2 ),	/* 2 */
/* 3072 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x0,		/*  */
/* 3074 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3076 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 3078 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 3080 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3082 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 3084 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 3086 */	NdrFcShort( 0x4 ),	/* 4 */
/* 3088 */	NdrFcShort( 0x4 ),	/* 4 */
/* 3090 */	0x12, 0x0,	/* FC_UP */
/* 3092 */	NdrFcShort( 0xffe4 ),	/* Offset= -28 (3064) */
/* 3094 */	
			0x5b,		/* FC_END */

			0x6,		/* FC_SHORT */
/* 3096 */	0x6,		/* FC_SHORT */
			0x8,		/* FC_LONG */
/* 3098 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 3100 */	
			0x11, 0x0,	/* FC_RP */
/* 3102 */	NdrFcShort( 0xffe8 ),	/* Offset= -24 (3078) */
/* 3104 */	
			0x12, 0x0,	/* FC_UP */
/* 3106 */	NdrFcShort( 0xfcd8 ),	/* Offset= -808 (2298) */
/* 3108 */	
			0x12, 0x0,	/* FC_UP */
/* 3110 */	NdrFcShort( 0xf430 ),	/* Offset= -3024 (86) */
/* 3112 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 3114 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 3116 */	
			0x11, 0x1,	/* FC_RP [all_nodes] */
/* 3118 */	NdrFcShort( 0x2 ),	/* Offset= 2 (3120) */
/* 3120 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0xd,		/* FC_ENUM16 */
/* 3122 */	0x26,		/* Corr desc:  parameter, FC_SHORT */
			0x0,		/*  */
/* 3124 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3126 */	NdrFcShort( 0xf804 ),	/* Offset= -2044 (1082) */

			0x0
        }
    };

static const GENERIC_BINDING_ROUTINE_PAIR BindingRoutines[ GENERIC_BINDING_TABLE_SIZE ] = 
        {
        {
            (GENERIC_BINDING_ROUTINE)PSAMPR_SERVER_NAME_bind,
            (GENERIC_UNBIND_ROUTINE)PSAMPR_SERVER_NAME_unbind
         }
        
        };


static void __RPC_USER samr_SAMPR_USER_LOGON_INFORMATIONExprEval_0000( PMIDL_STUB_MESSAGE pStubMsg )
{
    SAMPR_USER_LOGON_INFORMATION *pS	=	( SAMPR_USER_LOGON_INFORMATION * )pStubMsg->StackTop;
    
    pStubMsg->Offset = 0;
    pStubMsg->MaxCount = ( unsigned long ) ( (pS->LogonHours.UnitsPerWeek + 7) / 8 );
}

static void __RPC_USER samr_SAMPR_USER_LOGON_HOURS_INFORMATIONExprEval_0001( PMIDL_STUB_MESSAGE pStubMsg )
{
    SAMPR_USER_LOGON_HOURS_INFORMATION *pS	=	( SAMPR_USER_LOGON_HOURS_INFORMATION * )pStubMsg->StackTop;
    
    pStubMsg->Offset = 0;
    pStubMsg->MaxCount = ( unsigned long ) ( (pS->LogonHours.UnitsPerWeek + 7) / 8 );
}

static void __RPC_USER samr_SAMPR_USER_ACCOUNT_INFORMATIONExprEval_0002( PMIDL_STUB_MESSAGE pStubMsg )
{
    SAMPR_USER_ACCOUNT_INFORMATION *pS	=	( SAMPR_USER_ACCOUNT_INFORMATION * )pStubMsg->StackTop;
    
    pStubMsg->Offset = 0;
    pStubMsg->MaxCount = ( unsigned long ) ( (pS->LogonHours.UnitsPerWeek + 7) / 8 );
}

static void __RPC_USER samr_SAMPR_USER_ALL_INFORMATIONExprEval_0003( PMIDL_STUB_MESSAGE pStubMsg )
{
    SAMPR_USER_ALL_INFORMATION *pS	=	( SAMPR_USER_ALL_INFORMATION * )pStubMsg->StackTop;
    
    pStubMsg->Offset = 0;
    pStubMsg->MaxCount = ( unsigned long ) ( (pS->LogonHours.UnitsPerWeek + 7) / 8 );
}

static void __RPC_USER samr_SAMPR_USER_INTERNAL4_INFORMATIONExprEval_0004( PMIDL_STUB_MESSAGE pStubMsg )
{
    SAMPR_USER_INTERNAL4_INFORMATION *pS	=	( SAMPR_USER_INTERNAL4_INFORMATION * )pStubMsg->StackTop;
    
    pStubMsg->Offset = 0;
    pStubMsg->MaxCount = ( unsigned long ) ( (pS->I1.LogonHours.UnitsPerWeek + 7) / 8 );
}

static const EXPR_EVAL ExprEvalRoutines[] = 
    {
    samr_SAMPR_USER_LOGON_INFORMATIONExprEval_0000
    ,samr_SAMPR_USER_LOGON_HOURS_INFORMATIONExprEval_0001
    ,samr_SAMPR_USER_ACCOUNT_INFORMATIONExprEval_0002
    ,samr_SAMPR_USER_ALL_INFORMATIONExprEval_0003
    ,samr_SAMPR_USER_INTERNAL4_INFORMATIONExprEval_0004
    };


static const unsigned short samr_FormatStringOffsetTable[] =
    {
    0,
    28,
    50,
    78,
    106,
    128,
    158,
    194,
    226,
    254,
    282,
    318,
    354,
    390,
    428,
    464,
    500,
    530,
    566,
    602,
    632,
    660,
    688,
    714,
    736,
    760,
    786,
    812,
    842,
    870,
    898,
    920,
    946,
    972,
    998,
    1028,
    1050,
    1078,
    1106,
    1160,
    1186,
    1228,
    1260,
    1282,
    1304,
    1330,
    1356,
    1384,
    1412,
    1454,
    1486,
    1528,
    1570,
    1596,
    1622,
    1656,
    1700,
    1726,
    1754
    };


static const MIDL_STUB_DESC samr_StubDesc = 
    {
    (void *)& samr___RpcClientInterface,
    MIDL_user_allocate,
    MIDL_user_free,
    &samcli_handle,
    0,
    BindingRoutines,
    ExprEvalRoutines,
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
/* at Fri Mar 31 14:17:00 2023
 */
/* Compiler settings for samrpc.idl, samcli.acf:
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

#include "samrpc_c.h"

#define TYPE_FORMAT_STRING_SIZE   2023                              
#define PROC_FORMAT_STRING_SIZE   3491                              
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


/* Standard interface: samr, ver. 1.0,
   GUID={0x12345778,0x1234,0xABCD,{0xEF,0x00,0x01,0x23,0x45,0x67,0x89,0xAC}} */

handle_t samcli_handle;


static const RPC_CLIENT_INTERFACE samr___RpcClientInterface =
    {
    sizeof(RPC_CLIENT_INTERFACE),
    {{0x12345778,0x1234,0xABCD,{0xEF,0x00,0x01,0x23,0x45,0x67,0x89,0xAC}},{1,0}},
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    0,
    0,
    0,
    0,
    0x00000000
    };
RPC_IF_HANDLE samr_ClientIfHandle = (RPC_IF_HANDLE)& samr___RpcClientInterface;

extern const MIDL_STUB_DESC samr_StubDesc;

static RPC_BINDING_HANDLE samr__MIDL_AutoBindHandle;


NTSTATUS SamrConnect( 
    /* [unique][in] */ PSAMPR_SERVER_NAME ServerName,
    /* [out] */ SAMPR_HANDLE *ServerHandle,
    /* [in] */ ACCESS_MASK DesiredAccess)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&samr_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[0],
                  ServerName,
                  ServerHandle,
                  DesiredAccess);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrCloseHandle( 
    /* [out][in] */ SAMPR_HANDLE *SamHandle)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&samr_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[56],
                  SamHandle);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrSetSecurityObject( 
    /* [in] */ SAMPR_HANDLE ObjectHandle,
    /* [in] */ SECURITY_INFORMATION SecurityInformation,
    /* [in] */ PSAMPR_SR_SECURITY_DESCRIPTOR SecurityDescriptor)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&samr_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[100],
                  ObjectHandle,
                  SecurityInformation,
                  SecurityDescriptor);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrQuerySecurityObject( 
    /* [in] */ SAMPR_HANDLE ObjectHandle,
    /* [in] */ SECURITY_INFORMATION SecurityInformation,
    /* [out] */ PSAMPR_SR_SECURITY_DESCRIPTOR *SecurityDescriptor)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&samr_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[156],
                  ObjectHandle,
                  SecurityInformation,
                  SecurityDescriptor);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrShutdownSamServer( 
    /* [in] */ SAMPR_HANDLE ServerHandle)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&samr_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[212],
                  ServerHandle);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrLookupDomainInSamServer( 
    /* [in] */ SAMPR_HANDLE ServerHandle,
    /* [in] */ PRPC_UNICODE_STRING Name,
    /* [out] */ PRPC_SID *DomainId)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&samr_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[256],
                  ServerHandle,
                  Name,
                  DomainId);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrEnumerateDomainsInSamServer( 
    /* [in] */ SAMPR_HANDLE ServerHandle,
    /* [out][in] */ PSAM_ENUMERATE_HANDLE EnumerationContext,
    /* [out] */ PSAMPR_ENUMERATION_BUFFER *Buffer,
    /* [in] */ ULONG PreferedMaximumLength,
    /* [out] */ PULONG CountReturned)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&samr_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[312],
                  ServerHandle,
                  EnumerationContext,
                  Buffer,
                  PreferedMaximumLength,
                  CountReturned);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrOpenDomain( 
    /* [in] */ SAMPR_HANDLE ServerHandle,
    /* [in] */ ACCESS_MASK DesiredAccess,
    /* [in] */ PRPC_SID DomainId,
    /* [out] */ SAMPR_HANDLE *DomainHandle)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&samr_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[380],
                  ServerHandle,
                  DesiredAccess,
                  DomainId,
                  DomainHandle);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrQueryInformationDomain( 
    /* [in] */ SAMPR_HANDLE DomainHandle,
    /* [in] */ DOMAIN_INFORMATION_CLASS DomainInformationClass,
    /* [switch_is][out] */ PSAMPR_DOMAIN_INFO_BUFFER *Buffer)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&samr_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[442],
                  DomainHandle,
                  DomainInformationClass,
                  Buffer);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrSetInformationDomain( 
    /* [in] */ SAMPR_HANDLE DomainHandle,
    /* [in] */ DOMAIN_INFORMATION_CLASS DomainInformationClass,
    /* [switch_is][in] */ PSAMPR_DOMAIN_INFO_BUFFER DomainInformation)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&samr_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[498],
                  DomainHandle,
                  DomainInformationClass,
                  DomainInformation);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrCreateGroupInDomain( 
    /* [in] */ SAMPR_HANDLE DomainHandle,
    /* [in] */ PRPC_UNICODE_STRING Name,
    /* [in] */ ACCESS_MASK DesiredAccess,
    /* [out] */ SAMPR_HANDLE *GroupHandle,
    /* [out] */ PULONG RelativeId)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&samr_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[554],
                  DomainHandle,
                  Name,
                  DesiredAccess,
                  GroupHandle,
                  RelativeId);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrEnumerateGroupsInDomain( 
    /* [in] */ SAMPR_HANDLE DomainHandle,
    /* [out][in] */ PSAM_ENUMERATE_HANDLE EnumerationContext,
    /* [out] */ PSAMPR_ENUMERATION_BUFFER *Buffer,
    /* [in] */ ULONG PreferedMaximumLength,
    /* [out] */ PULONG CountReturned)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&samr_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[622],
                  DomainHandle,
                  EnumerationContext,
                  Buffer,
                  PreferedMaximumLength,
                  CountReturned);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrCreateUserInDomain( 
    /* [in] */ SAMPR_HANDLE DomainHandle,
    /* [in] */ PRPC_UNICODE_STRING Name,
    /* [in] */ ACCESS_MASK DesiredAccess,
    /* [out] */ SAMPR_HANDLE *UserHandle,
    /* [out] */ PULONG RelativeId)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&samr_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[690],
                  DomainHandle,
                  Name,
                  DesiredAccess,
                  UserHandle,
                  RelativeId);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrEnumerateUsersInDomain( 
    /* [in] */ SAMPR_HANDLE DomainHandle,
    /* [out][in] */ PSAM_ENUMERATE_HANDLE EnumerationContext,
    /* [in] */ ULONG UserAccountControl,
    /* [out] */ PSAMPR_ENUMERATION_BUFFER *Buffer,
    /* [in] */ ULONG PreferedMaximumLength,
    /* [out] */ PULONG CountReturned)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&samr_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[758],
                  DomainHandle,
                  EnumerationContext,
                  UserAccountControl,
                  Buffer,
                  PreferedMaximumLength,
                  CountReturned);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrCreateAliasInDomain( 
    /* [in] */ SAMPR_HANDLE DomainHandle,
    /* [in] */ PRPC_UNICODE_STRING AccountName,
    /* [in] */ ACCESS_MASK DesiredAccess,
    /* [out] */ SAMPR_HANDLE *AliasHandle,
    /* [out] */ PULONG RelativeId)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&samr_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[832],
                  DomainHandle,
                  AccountName,
                  DesiredAccess,
                  AliasHandle,
                  RelativeId);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrEnumerateAliasesInDomain( 
    /* [in] */ SAMPR_HANDLE DomainHandle,
    /* [out][in] */ PSAM_ENUMERATE_HANDLE EnumerationContext,
    /* [out] */ PSAMPR_ENUMERATION_BUFFER *Buffer,
    /* [in] */ ULONG PreferedMaximumLength,
    /* [out] */ PULONG CountReturned)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&samr_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[900],
                  DomainHandle,
                  EnumerationContext,
                  Buffer,
                  PreferedMaximumLength,
                  CountReturned);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrGetAliasMembership( 
    /* [in] */ SAMPR_HANDLE DomainHandle,
    /* [in] */ PSAMPR_PSID_ARRAY SidArray,
    /* [out] */ PSAMPR_ULONG_ARRAY Membership)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&samr_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[968],
                  DomainHandle,
                  SidArray,
                  Membership);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrLookupNamesInDomain( 
    /* [in] */ SAMPR_HANDLE DomainHandle,
    /* [in] */ ULONG Count,
    /* [length_is][size_is][in] */ RPC_UNICODE_STRING Names[  ],
    /* [out] */ PSAMPR_ULONG_ARRAY RelativeIds,
    /* [out] */ PSAMPR_ULONG_ARRAY Use)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&samr_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1024],
                  DomainHandle,
                  Count,
                  Names,
                  RelativeIds,
                  Use);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrLookupIdsInDomain( 
    /* [in] */ SAMPR_HANDLE DomainHandle,
    /* [in] */ ULONG Count,
    /* [length_is][size_is][in] */ PULONG RelativeIds,
    /* [out] */ PSAMPR_RETURNED_USTRING_ARRAY Names,
    /* [out] */ PSAMPR_ULONG_ARRAY Use)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&samr_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1092],
                  DomainHandle,
                  Count,
                  RelativeIds,
                  Names,
                  Use);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrOpenGroup( 
    /* [in] */ SAMPR_HANDLE DomainHandle,
    /* [in] */ ACCESS_MASK DesiredAccess,
    /* [in] */ ULONG GroupId,
    /* [out] */ SAMPR_HANDLE *GroupHandle)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&samr_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1160],
                  DomainHandle,
                  DesiredAccess,
                  GroupId,
                  GroupHandle);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrQueryInformationGroup( 
    /* [in] */ SAMPR_HANDLE GroupHandle,
    /* [in] */ GROUP_INFORMATION_CLASS GroupInformationClass,
    /* [switch_is][out] */ PSAMPR_GROUP_INFO_BUFFER *Buffer)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&samr_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1222],
                  GroupHandle,
                  GroupInformationClass,
                  Buffer);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrSetInformationGroup( 
    /* [in] */ SAMPR_HANDLE GroupHandle,
    /* [in] */ GROUP_INFORMATION_CLASS GroupInformationClass,
    /* [switch_is][in] */ PSAMPR_GROUP_INFO_BUFFER Buffer)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&samr_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1278],
                  GroupHandle,
                  GroupInformationClass,
                  Buffer);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrAddMemberToGroup( 
    /* [in] */ SAMPR_HANDLE GroupHandle,
    /* [in] */ ULONG MemberId,
    /* [in] */ ULONG Attributes)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&samr_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1334],
                  GroupHandle,
                  MemberId,
                  Attributes);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrDeleteGroup( 
    /* [out][in] */ SAMPR_HANDLE *GroupHandle)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&samr_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1390],
                  GroupHandle);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrRemoveMemberFromGroup( 
    /* [in] */ SAMPR_HANDLE GroupHandle,
    /* [in] */ ULONG MemberId)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&samr_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1434],
                  GroupHandle,
                  MemberId);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrGetMembersInGroup( 
    /* [in] */ SAMPR_HANDLE GroupHandle,
    /* [out] */ PSAMPR_GET_MEMBERS_BUFFER *Members)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&samr_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1484],
                  GroupHandle,
                  Members);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrSetMemberAttributesOfGroup( 
    /* [in] */ SAMPR_HANDLE GroupHandle,
    /* [in] */ ULONG MemberId,
    /* [in] */ ULONG Attributes)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&samr_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1534],
                  GroupHandle,
                  MemberId,
                  Attributes);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrOpenAlias( 
    /* [in] */ SAMPR_HANDLE DomainHandle,
    /* [in] */ ACCESS_MASK DesiredAccess,
    /* [in] */ ULONG AliasId,
    /* [out] */ SAMPR_HANDLE *AliasHandle)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&samr_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1590],
                  DomainHandle,
                  DesiredAccess,
                  AliasId,
                  AliasHandle);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrQueryInformationAlias( 
    /* [in] */ SAMPR_HANDLE AliasHandle,
    /* [in] */ ALIAS_INFORMATION_CLASS AliasInformationClass,
    /* [switch_is][out] */ PSAMPR_ALIAS_INFO_BUFFER *Buffer)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&samr_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1652],
                  AliasHandle,
                  AliasInformationClass,
                  Buffer);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrSetInformationAlias( 
    /* [in] */ SAMPR_HANDLE AliasHandle,
    /* [in] */ ALIAS_INFORMATION_CLASS AliasInformationClass,
    /* [switch_is][in] */ PSAMPR_ALIAS_INFO_BUFFER Buffer)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&samr_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1708],
                  AliasHandle,
                  AliasInformationClass,
                  Buffer);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrDeleteAlias( 
    /* [out][in] */ SAMPR_HANDLE *AliasHandle)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&samr_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1764],
                  AliasHandle);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrAddMemberToAlias( 
    /* [in] */ SAMPR_HANDLE AliasHandle,
    /* [in] */ PRPC_SID MemberId)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&samr_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1808],
                  AliasHandle,
                  MemberId);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrRemoveMemberFromAlias( 
    /* [in] */ SAMPR_HANDLE AliasHandle,
    /* [in] */ PRPC_SID MemberId)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&samr_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1858],
                  AliasHandle,
                  MemberId);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrGetMembersInAlias( 
    /* [in] */ SAMPR_HANDLE AliasHandle,
    /* [out] */ PSAMPR_PSID_ARRAY Members)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&samr_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1908],
                  AliasHandle,
                  Members);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrOpenUser( 
    /* [in] */ SAMPR_HANDLE DomainHandle,
    /* [in] */ ACCESS_MASK DesiredAccess,
    /* [in] */ ULONG UserId,
    /* [out] */ SAMPR_HANDLE *UserHandle)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&samr_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1958],
                  DomainHandle,
                  DesiredAccess,
                  UserId,
                  UserHandle);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrDeleteUser( 
    /* [out][in] */ SAMPR_HANDLE *UserHandle)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&samr_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[2020],
                  UserHandle);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrQueryInformationUser( 
    /* [in] */ SAMPR_HANDLE UserHandle,
    /* [in] */ USER_INFORMATION_CLASS UserInformationClass,
    /* [switch_is][out] */ PSAMPR_USER_INFO_BUFFER *Buffer)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&samr_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[2064],
                  UserHandle,
                  UserInformationClass,
                  Buffer);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrSetInformationUser( 
    /* [in] */ SAMPR_HANDLE UserHandle,
    /* [in] */ USER_INFORMATION_CLASS UserInformationClass,
    /* [switch_is][in] */ PSAMPR_USER_INFO_BUFFER Buffer)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&samr_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[2120],
                  UserHandle,
                  UserInformationClass,
                  Buffer);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrChangePasswordUser( 
    /* [in] */ SAMPR_HANDLE UserHandle,
    /* [in] */ BOOLEAN LmPresent,
    /* [unique][in] */ PENCRYPTED_LM_OWF_PASSWORD LmOldEncryptedWithLmNew,
    /* [unique][in] */ PENCRYPTED_LM_OWF_PASSWORD LmNewEncryptedWithLmOld,
    /* [in] */ BOOLEAN NtPresent,
    /* [unique][in] */ PENCRYPTED_NT_OWF_PASSWORD NtOldEncryptedWithNtNew,
    /* [unique][in] */ PENCRYPTED_NT_OWF_PASSWORD NtNewEncryptedWithNtOld,
    /* [in] */ BOOLEAN NtCrossEncryptionPresent,
    /* [unique][in] */ PENCRYPTED_NT_OWF_PASSWORD NtNewEncryptedWithLmNew,
    /* [in] */ BOOLEAN LmCrossEncryptionPresent,
    /* [unique][in] */ PENCRYPTED_LM_OWF_PASSWORD LmNtNewEncryptedWithNtNew)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&samr_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[2176],
                  UserHandle,
                  LmPresent,
                  LmOldEncryptedWithLmNew,
                  LmNewEncryptedWithLmOld,
                  NtPresent,
                  NtOldEncryptedWithNtNew,
                  NtNewEncryptedWithNtOld,
                  NtCrossEncryptionPresent,
                  NtNewEncryptedWithLmNew,
                  LmCrossEncryptionPresent,
                  LmNtNewEncryptedWithNtNew);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrGetGroupsForUser( 
    /* [in] */ SAMPR_HANDLE UserHandle,
    /* [out] */ PSAMPR_GET_GROUPS_BUFFER *Groups)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&samr_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[2280],
                  UserHandle,
                  Groups);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrQueryDisplayInformation( 
    /* [in] */ SAMPR_HANDLE DomainHandle,
    /* [in] */ DOMAIN_DISPLAY_INFORMATION DisplayInformationClass,
    /* [in] */ ULONG Index,
    /* [in] */ ULONG EntryCount,
    /* [in] */ ULONG PreferredMaximumLength,
    /* [out] */ PULONG TotalAvailable,
    /* [out] */ PULONG TotalReturned,
    /* [switch_is][out] */ PSAMPR_DISPLAY_INFO_BUFFER Buffer)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&samr_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[2330],
                  DomainHandle,
                  DisplayInformationClass,
                  Index,
                  EntryCount,
                  PreferredMaximumLength,
                  TotalAvailable,
                  TotalReturned,
                  Buffer);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrGetDisplayEnumerationIndex( 
    /* [in] */ SAMPR_HANDLE DomainHandle,
    /* [in] */ DOMAIN_DISPLAY_INFORMATION DisplayInformationClass,
    /* [in] */ PRPC_UNICODE_STRING Prefix,
    /* [out] */ PULONG Index)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&samr_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[2416],
                  DomainHandle,
                  DisplayInformationClass,
                  Prefix,
                  Index);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrTestPrivateFunctionsDomain( 
    /* [in] */ SAMPR_HANDLE DomainHandle)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&samr_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[2478],
                  DomainHandle);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrTestPrivateFunctionsUser( 
    /* [in] */ SAMPR_HANDLE UserHandle)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&samr_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[2522],
                  UserHandle);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrGetUserDomainPasswordInformation( 
    /* [in] */ SAMPR_HANDLE UserHandle,
    /* [out] */ PUSER_DOMAIN_PASSWORD_INFORMATION PasswordInformation)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&samr_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[2566],
                  UserHandle,
                  PasswordInformation);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrRemoveMemberFromForeignDomain( 
    /* [in] */ SAMPR_HANDLE DomainHandle,
    /* [in] */ PRPC_SID MemberSid)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&samr_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[2616],
                  DomainHandle,
                  MemberSid);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrQueryInformationDomain2( 
    /* [in] */ SAMPR_HANDLE DomainHandle,
    /* [in] */ DOMAIN_INFORMATION_CLASS DomainInformationClass,
    /* [switch_is][out] */ PSAMPR_DOMAIN_INFO_BUFFER *Buffer)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&samr_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[2666],
                  DomainHandle,
                  DomainInformationClass,
                  Buffer);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrQueryInformationUser2( 
    /* [in] */ SAMPR_HANDLE UserHandle,
    /* [in] */ USER_INFORMATION_CLASS UserInformationClass,
    /* [switch_is][out] */ PSAMPR_USER_INFO_BUFFER *Buffer)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&samr_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[2722],
                  UserHandle,
                  UserInformationClass,
                  Buffer);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrQueryDisplayInformation2( 
    /* [in] */ SAMPR_HANDLE DomainHandle,
    /* [in] */ DOMAIN_DISPLAY_INFORMATION DisplayInformationClass,
    /* [in] */ ULONG Index,
    /* [in] */ ULONG EntryCount,
    /* [in] */ ULONG PreferredMaximumLength,
    /* [out] */ PULONG TotalAvailable,
    /* [out] */ PULONG TotalReturned,
    /* [switch_is][out] */ PSAMPR_DISPLAY_INFO_BUFFER Buffer)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&samr_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[2778],
                  DomainHandle,
                  DisplayInformationClass,
                  Index,
                  EntryCount,
                  PreferredMaximumLength,
                  TotalAvailable,
                  TotalReturned,
                  Buffer);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrGetDisplayEnumerationIndex2( 
    /* [in] */ SAMPR_HANDLE DomainHandle,
    /* [in] */ DOMAIN_DISPLAY_INFORMATION DisplayInformationClass,
    /* [in] */ PRPC_UNICODE_STRING Prefix,
    /* [out] */ PULONG Index)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&samr_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[2864],
                  DomainHandle,
                  DisplayInformationClass,
                  Prefix,
                  Index);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrCreateUser2InDomain( 
    /* [in] */ SAMPR_HANDLE DomainHandle,
    /* [in] */ PRPC_UNICODE_STRING Name,
    /* [in] */ ULONG AccountType,
    /* [in] */ ACCESS_MASK DesiredAccess,
    /* [out] */ SAMPR_HANDLE *UserHandle,
    /* [out] */ PULONG GrantedAccess,
    /* [out] */ PULONG RelativeId)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&samr_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[2926],
                  DomainHandle,
                  Name,
                  AccountType,
                  DesiredAccess,
                  UserHandle,
                  GrantedAccess,
                  RelativeId);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrQueryDisplayInformation3( 
    /* [in] */ SAMPR_HANDLE DomainHandle,
    /* [in] */ DOMAIN_DISPLAY_INFORMATION DisplayInformationClass,
    /* [in] */ ULONG Index,
    /* [in] */ ULONG EntryCount,
    /* [in] */ ULONG PreferredMaximumLength,
    /* [out] */ PULONG TotalAvailable,
    /* [out] */ PULONG TotalReturned,
    /* [switch_is][out] */ PSAMPR_DISPLAY_INFO_BUFFER Buffer)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&samr_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[3006],
                  DomainHandle,
                  DisplayInformationClass,
                  Index,
                  EntryCount,
                  PreferredMaximumLength,
                  TotalAvailable,
                  TotalReturned,
                  Buffer);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrAddMultipleMembersToAlias( 
    /* [in] */ SAMPR_HANDLE AliasHandle,
    /* [in] */ PSAMPR_PSID_ARRAY MembersBuffer)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&samr_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[3092],
                  AliasHandle,
                  MembersBuffer);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrRemoveMultipleMembersFromAlias( 
    /* [in] */ SAMPR_HANDLE AliasHandle,
    /* [in] */ PSAMPR_PSID_ARRAY MembersBuffer)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&samr_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[3142],
                  AliasHandle,
                  MembersBuffer);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrOemChangePasswordUser2( 
    /* [in] */ handle_t BindingHandle,
    /* [unique][in] */ PRPC_STRING ServerName,
    /* [in] */ PRPC_STRING UserName,
    /* [unique][in] */ PSAMPR_ENCRYPTED_USER_PASSWORD NewPasswordEncryptedWithOldLm,
    /* [unique][in] */ PENCRYPTED_LM_OWF_PASSWORD OldLmOwfPassswordEncryptedWithNewLm)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&samr_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[3192],
                  BindingHandle,
                  ServerName,
                  UserName,
                  NewPasswordEncryptedWithOldLm,
                  OldLmOwfPassswordEncryptedWithNewLm);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrUnicodeChangePasswordUser2( 
    /* [in] */ handle_t BindingHandle,
    /* [unique][in] */ PRPC_UNICODE_STRING ServerName,
    /* [in] */ PRPC_UNICODE_STRING UserName,
    /* [unique][in] */ PSAMPR_ENCRYPTED_USER_PASSWORD NewPasswordEncryptedWithOldNt,
    /* [unique][in] */ PENCRYPTED_NT_OWF_PASSWORD OldNtOwfPasswordEncryptedWithNewNt,
    /* [in] */ BOOLEAN LmPresent,
    /* [unique][in] */ PSAMPR_ENCRYPTED_USER_PASSWORD NewPasswordEncryptedWithOldLm,
    /* [unique][in] */ PENCRYPTED_LM_OWF_PASSWORD OldLmOwfPassswordEncryptedWithNewLmOrNt)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&samr_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[3252],
                  BindingHandle,
                  ServerName,
                  UserName,
                  NewPasswordEncryptedWithOldNt,
                  OldNtOwfPasswordEncryptedWithNewNt,
                  LmPresent,
                  NewPasswordEncryptedWithOldLm,
                  OldLmOwfPassswordEncryptedWithNewLmOrNt);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrGetDomainPasswordInformation( 
    /* [in] */ handle_t BindingHandle,
    /* [unique][in] */ PRPC_UNICODE_STRING ServerName,
    /* [out] */ PUSER_DOMAIN_PASSWORD_INFORMATION PasswordInformation)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&samr_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[3330],
                  BindingHandle,
                  ServerName,
                  PasswordInformation);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrConnect2( 
    /* [string][unique][in] */ PSAMPR_SERVER_NAME ServerName,
    /* [out] */ SAMPR_HANDLE *ServerHandle,
    /* [in] */ ACCESS_MASK DesiredAccess)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&samr_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[3378],
                  ServerName,
                  ServerHandle,
                  DesiredAccess);
    return ( NTSTATUS  )_RetVal.Simple;
    
}


NTSTATUS SamrSetInformationUser2( 
    /* [in] */ SAMPR_HANDLE UserHandle,
    /* [in] */ USER_INFORMATION_CLASS UserInformationClass,
    /* [switch_is][in] */ PSAMPR_USER_INFO_BUFFER Buffer)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&samr_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[3434],
                  UserHandle,
                  UserInformationClass,
                  Buffer);
    return ( NTSTATUS  )_RetVal.Simple;
    
}

extern const GENERIC_BINDING_ROUTINE_PAIR BindingRoutines[ GENERIC_BINDING_TABLE_SIZE ];
extern const EXPR_EVAL ExprEvalRoutines[];

#if !defined(__RPC_WIN64__)
#error  Invalid build platform for this stub.
#endif

static const MIDL_PROC_FORMAT_STRING __MIDL_ProcFormatString =
    {
        0,
        {

	/* Procedure SamrConnect */

			0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/*  2 */	NdrFcLong( 0x0 ),	/* 0 */
/*  6 */	NdrFcShort( 0x0 ),	/* 0 */
/*  8 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 10 */	0x31,		/* FC_BIND_GENERIC */
			0x8,		/* 8 */
/* 12 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 14 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 16 */	NdrFcShort( 0x22 ),	/* 34 */
/* 18 */	NdrFcShort( 0x40 ),	/* 64 */
/* 20 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 22 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 24 */	NdrFcShort( 0x0 ),	/* 0 */
/* 26 */	NdrFcShort( 0x0 ),	/* 0 */
/* 28 */	NdrFcShort( 0x0 ),	/* 0 */
/* 30 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ServerName */

/* 32 */	NdrFcShort( 0xa ),	/* Flags:  must free, in, */
/* 34 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 36 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter ServerHandle */

/* 38 */	NdrFcShort( 0x110 ),	/* Flags:  out, simple ref, */
/* 40 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 42 */	NdrFcShort( 0xa ),	/* Type Offset=10 */

	/* Parameter DesiredAccess */

/* 44 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 46 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 48 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 50 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 52 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 54 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SamrCloseHandle */

/* 56 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 58 */	NdrFcLong( 0x0 ),	/* 0 */
/* 62 */	NdrFcShort( 0x1 ),	/* 1 */
/* 64 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 66 */	0x30,		/* FC_BIND_CONTEXT */
			0xe0,		/* Ctxt flags:  via ptr, in, out, */
/* 68 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 70 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 72 */	NdrFcShort( 0x38 ),	/* 56 */
/* 74 */	NdrFcShort( 0x40 ),	/* 64 */
/* 76 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 78 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 80 */	NdrFcShort( 0x0 ),	/* 0 */
/* 82 */	NdrFcShort( 0x0 ),	/* 0 */
/* 84 */	NdrFcShort( 0x0 ),	/* 0 */
/* 86 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter SamHandle */

/* 88 */	NdrFcShort( 0x118 ),	/* Flags:  in, out, simple ref, */
/* 90 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 92 */	NdrFcShort( 0x12 ),	/* Type Offset=18 */

	/* Return value */

/* 94 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 96 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 98 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SamrSetSecurityObject */

/* 100 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 102 */	NdrFcLong( 0x0 ),	/* 0 */
/* 106 */	NdrFcShort( 0x2 ),	/* 2 */
/* 108 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 110 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 112 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 114 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 116 */	NdrFcShort( 0x2c ),	/* 44 */
/* 118 */	NdrFcShort( 0x8 ),	/* 8 */
/* 120 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 122 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 124 */	NdrFcShort( 0x0 ),	/* 0 */
/* 126 */	NdrFcShort( 0x1 ),	/* 1 */
/* 128 */	NdrFcShort( 0x0 ),	/* 0 */
/* 130 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ObjectHandle */

/* 132 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 134 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 136 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */

	/* Parameter SecurityInformation */

/* 138 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 140 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 142 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter SecurityDescriptor */

/* 144 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 146 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 148 */	NdrFcShort( 0x2a ),	/* Type Offset=42 */

	/* Return value */

/* 150 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 152 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 154 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SamrQuerySecurityObject */

/* 156 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 158 */	NdrFcLong( 0x0 ),	/* 0 */
/* 162 */	NdrFcShort( 0x3 ),	/* 3 */
/* 164 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 166 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 168 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 170 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 172 */	NdrFcShort( 0x2c ),	/* 44 */
/* 174 */	NdrFcShort( 0x8 ),	/* 8 */
/* 176 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x4,		/* 4 */
/* 178 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 180 */	NdrFcShort( 0x1 ),	/* 1 */
/* 182 */	NdrFcShort( 0x0 ),	/* 0 */
/* 184 */	NdrFcShort( 0x0 ),	/* 0 */
/* 186 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ObjectHandle */

/* 188 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 190 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 192 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */

	/* Parameter SecurityInformation */

/* 194 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 196 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 198 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter SecurityDescriptor */

/* 200 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 202 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 204 */	NdrFcShort( 0x3a ),	/* Type Offset=58 */

	/* Return value */

/* 206 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 208 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 210 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SamrShutdownSamServer */

/* 212 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 214 */	NdrFcLong( 0x0 ),	/* 0 */
/* 218 */	NdrFcShort( 0x4 ),	/* 4 */
/* 220 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 222 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 224 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 226 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 228 */	NdrFcShort( 0x24 ),	/* 36 */
/* 230 */	NdrFcShort( 0x8 ),	/* 8 */
/* 232 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 234 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 236 */	NdrFcShort( 0x0 ),	/* 0 */
/* 238 */	NdrFcShort( 0x0 ),	/* 0 */
/* 240 */	NdrFcShort( 0x0 ),	/* 0 */
/* 242 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ServerHandle */

/* 244 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 246 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 248 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */

	/* Return value */

/* 250 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 252 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 254 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SamrLookupDomainInSamServer */

/* 256 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 258 */	NdrFcLong( 0x0 ),	/* 0 */
/* 262 */	NdrFcShort( 0x5 ),	/* 5 */
/* 264 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 266 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 268 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 270 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 272 */	NdrFcShort( 0x24 ),	/* 36 */
/* 274 */	NdrFcShort( 0x8 ),	/* 8 */
/* 276 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 278 */	0xa,		/* 10 */
			0x7,		/* Ext Flags:  new corr desc, clt corr check, srv corr check, */
/* 280 */	NdrFcShort( 0x1 ),	/* 1 */
/* 282 */	NdrFcShort( 0x1 ),	/* 1 */
/* 284 */	NdrFcShort( 0x0 ),	/* 0 */
/* 286 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ServerHandle */

/* 288 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 290 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 292 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */

	/* Parameter Name */

/* 294 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 296 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 298 */	NdrFcShort( 0x58 ),	/* Type Offset=88 */

	/* Parameter DomainId */

/* 300 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 302 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 304 */	NdrFcShort( 0x6a ),	/* Type Offset=106 */

	/* Return value */

/* 306 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 308 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 310 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SamrEnumerateDomainsInSamServer */

/* 312 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 314 */	NdrFcLong( 0x0 ),	/* 0 */
/* 318 */	NdrFcShort( 0x6 ),	/* 6 */
/* 320 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 322 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 324 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 326 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 328 */	NdrFcShort( 0x48 ),	/* 72 */
/* 330 */	NdrFcShort( 0x40 ),	/* 64 */
/* 332 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x6,		/* 6 */
/* 334 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 336 */	NdrFcShort( 0x2 ),	/* 2 */
/* 338 */	NdrFcShort( 0x0 ),	/* 0 */
/* 340 */	NdrFcShort( 0x0 ),	/* 0 */
/* 342 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ServerHandle */

/* 344 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 346 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 348 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */

	/* Parameter EnumerationContext */

/* 350 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 352 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 354 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter Buffer */

/* 356 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 358 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 360 */	NdrFcShort( 0xa0 ),	/* Type Offset=160 */

	/* Parameter PreferedMaximumLength */

/* 362 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 364 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 366 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter CountReturned */

/* 368 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 370 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 372 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 374 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 376 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 378 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SamrOpenDomain */

/* 380 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 382 */	NdrFcLong( 0x0 ),	/* 0 */
/* 386 */	NdrFcShort( 0x7 ),	/* 7 */
/* 388 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 390 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 392 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 394 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 396 */	NdrFcShort( 0x2c ),	/* 44 */
/* 398 */	NdrFcShort( 0x40 ),	/* 64 */
/* 400 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 402 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 404 */	NdrFcShort( 0x0 ),	/* 0 */
/* 406 */	NdrFcShort( 0x1 ),	/* 1 */
/* 408 */	NdrFcShort( 0x0 ),	/* 0 */
/* 410 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ServerHandle */

/* 412 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 414 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 416 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */

	/* Parameter DesiredAccess */

/* 418 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 420 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 422 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter DomainId */

/* 424 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 426 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 428 */	NdrFcShort( 0x8e ),	/* Type Offset=142 */

	/* Parameter DomainHandle */

/* 430 */	NdrFcShort( 0x110 ),	/* Flags:  out, simple ref, */
/* 432 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 434 */	NdrFcShort( 0xea ),	/* Type Offset=234 */

	/* Return value */

/* 436 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 438 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 440 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SamrQueryInformationDomain */

/* 442 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 444 */	NdrFcLong( 0x0 ),	/* 0 */
/* 448 */	NdrFcShort( 0x8 ),	/* 8 */
/* 450 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 452 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 454 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 456 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 458 */	NdrFcShort( 0x2a ),	/* 42 */
/* 460 */	NdrFcShort( 0x8 ),	/* 8 */
/* 462 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x4,		/* 4 */
/* 464 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 466 */	NdrFcShort( 0xa ),	/* 10 */
/* 468 */	NdrFcShort( 0x0 ),	/* 0 */
/* 470 */	NdrFcShort( 0x0 ),	/* 0 */
/* 472 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter DomainHandle */

/* 474 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 476 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 478 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */

	/* Parameter DomainInformationClass */

/* 480 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 482 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 484 */	0xd,		/* FC_ENUM16 */
			0x0,		/* 0 */

	/* Parameter Buffer */

/* 486 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 488 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 490 */	NdrFcShort( 0xee ),	/* Type Offset=238 */

	/* Return value */

/* 492 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 494 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 496 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SamrSetInformationDomain */

/* 498 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 500 */	NdrFcLong( 0x0 ),	/* 0 */
/* 504 */	NdrFcShort( 0x9 ),	/* 9 */
/* 506 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 508 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 510 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 512 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 514 */	NdrFcShort( 0x2a ),	/* 42 */
/* 516 */	NdrFcShort( 0x8 ),	/* 8 */
/* 518 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 520 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 522 */	NdrFcShort( 0x0 ),	/* 0 */
/* 524 */	NdrFcShort( 0xa ),	/* 10 */
/* 526 */	NdrFcShort( 0x0 ),	/* 0 */
/* 528 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter DomainHandle */

/* 530 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 532 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 534 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */

	/* Parameter DomainInformationClass */

/* 536 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 538 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 540 */	0xd,		/* FC_ENUM16 */
			0x0,		/* 0 */

	/* Parameter DomainInformation */

/* 542 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 544 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 546 */	NdrFcShort( 0x1fe ),	/* Type Offset=510 */

	/* Return value */

/* 548 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 550 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 552 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SamrCreateGroupInDomain */

/* 554 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 556 */	NdrFcLong( 0x0 ),	/* 0 */
/* 560 */	NdrFcShort( 0xa ),	/* 10 */
/* 562 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 564 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 566 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 568 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 570 */	NdrFcShort( 0x2c ),	/* 44 */
/* 572 */	NdrFcShort( 0x5c ),	/* 92 */
/* 574 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x6,		/* 6 */
/* 576 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 578 */	NdrFcShort( 0x0 ),	/* 0 */
/* 580 */	NdrFcShort( 0x1 ),	/* 1 */
/* 582 */	NdrFcShort( 0x0 ),	/* 0 */
/* 584 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter DomainHandle */

/* 586 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 588 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 590 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */

	/* Parameter Name */

/* 592 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 594 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 596 */	NdrFcShort( 0x58 ),	/* Type Offset=88 */

	/* Parameter DesiredAccess */

/* 598 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 600 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 602 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter GroupHandle */

/* 604 */	NdrFcShort( 0x110 ),	/* Flags:  out, simple ref, */
/* 606 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 608 */	NdrFcShort( 0xea ),	/* Type Offset=234 */

	/* Parameter RelativeId */

/* 610 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 612 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 614 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 616 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 618 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 620 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SamrEnumerateGroupsInDomain */

/* 622 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 624 */	NdrFcLong( 0x0 ),	/* 0 */
/* 628 */	NdrFcShort( 0xb ),	/* 11 */
/* 630 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 632 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 634 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 636 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 638 */	NdrFcShort( 0x48 ),	/* 72 */
/* 640 */	NdrFcShort( 0x40 ),	/* 64 */
/* 642 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x6,		/* 6 */
/* 644 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 646 */	NdrFcShort( 0x2 ),	/* 2 */
/* 648 */	NdrFcShort( 0x0 ),	/* 0 */
/* 650 */	NdrFcShort( 0x0 ),	/* 0 */
/* 652 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter DomainHandle */

/* 654 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 656 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 658 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */

	/* Parameter EnumerationContext */

/* 660 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 662 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 664 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter Buffer */

/* 666 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 668 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 670 */	NdrFcShort( 0xa0 ),	/* Type Offset=160 */

	/* Parameter PreferedMaximumLength */

/* 672 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 674 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 676 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter CountReturned */

/* 678 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 680 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 682 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 684 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 686 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 688 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SamrCreateUserInDomain */

/* 690 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 692 */	NdrFcLong( 0x0 ),	/* 0 */
/* 696 */	NdrFcShort( 0xc ),	/* 12 */
/* 698 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 700 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 702 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 704 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 706 */	NdrFcShort( 0x2c ),	/* 44 */
/* 708 */	NdrFcShort( 0x5c ),	/* 92 */
/* 710 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x6,		/* 6 */
/* 712 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 714 */	NdrFcShort( 0x0 ),	/* 0 */
/* 716 */	NdrFcShort( 0x1 ),	/* 1 */
/* 718 */	NdrFcShort( 0x0 ),	/* 0 */
/* 720 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter DomainHandle */

/* 722 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 724 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 726 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */

	/* Parameter Name */

/* 728 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 730 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 732 */	NdrFcShort( 0x58 ),	/* Type Offset=88 */

	/* Parameter DesiredAccess */

/* 734 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 736 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 738 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter UserHandle */

/* 740 */	NdrFcShort( 0x110 ),	/* Flags:  out, simple ref, */
/* 742 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 744 */	NdrFcShort( 0xea ),	/* Type Offset=234 */

	/* Parameter RelativeId */

/* 746 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 748 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 750 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 752 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 754 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 756 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SamrEnumerateUsersInDomain */

/* 758 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 760 */	NdrFcLong( 0x0 ),	/* 0 */
/* 764 */	NdrFcShort( 0xd ),	/* 13 */
/* 766 */	NdrFcShort( 0x38 ),	/* ia64 Stack size/offset = 56 */
/* 768 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 770 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 772 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 774 */	NdrFcShort( 0x50 ),	/* 80 */
/* 776 */	NdrFcShort( 0x40 ),	/* 64 */
/* 778 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x7,		/* 7 */
/* 780 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 782 */	NdrFcShort( 0x2 ),	/* 2 */
/* 784 */	NdrFcShort( 0x0 ),	/* 0 */
/* 786 */	NdrFcShort( 0x0 ),	/* 0 */
/* 788 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter DomainHandle */

/* 790 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 792 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 794 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */

	/* Parameter EnumerationContext */

/* 796 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 798 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 800 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter UserAccountControl */

/* 802 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 804 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 806 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter Buffer */

/* 808 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 810 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 812 */	NdrFcShort( 0xa0 ),	/* Type Offset=160 */

	/* Parameter PreferedMaximumLength */

/* 814 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 816 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 818 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter CountReturned */

/* 820 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 822 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 824 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 826 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 828 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 830 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SamrCreateAliasInDomain */

/* 832 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 834 */	NdrFcLong( 0x0 ),	/* 0 */
/* 838 */	NdrFcShort( 0xe ),	/* 14 */
/* 840 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 842 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 844 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 846 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 848 */	NdrFcShort( 0x2c ),	/* 44 */
/* 850 */	NdrFcShort( 0x5c ),	/* 92 */
/* 852 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x6,		/* 6 */
/* 854 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 856 */	NdrFcShort( 0x0 ),	/* 0 */
/* 858 */	NdrFcShort( 0x1 ),	/* 1 */
/* 860 */	NdrFcShort( 0x0 ),	/* 0 */
/* 862 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter DomainHandle */

/* 864 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 866 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 868 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */

	/* Parameter AccountName */

/* 870 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 872 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 874 */	NdrFcShort( 0x58 ),	/* Type Offset=88 */

	/* Parameter DesiredAccess */

/* 876 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 878 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 880 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter AliasHandle */

/* 882 */	NdrFcShort( 0x110 ),	/* Flags:  out, simple ref, */
/* 884 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 886 */	NdrFcShort( 0xea ),	/* Type Offset=234 */

	/* Parameter RelativeId */

/* 888 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 890 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 892 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 894 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 896 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 898 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SamrEnumerateAliasesInDomain */

/* 900 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 902 */	NdrFcLong( 0x0 ),	/* 0 */
/* 906 */	NdrFcShort( 0xf ),	/* 15 */
/* 908 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 910 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 912 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 914 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 916 */	NdrFcShort( 0x48 ),	/* 72 */
/* 918 */	NdrFcShort( 0x40 ),	/* 64 */
/* 920 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x6,		/* 6 */
/* 922 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 924 */	NdrFcShort( 0x2 ),	/* 2 */
/* 926 */	NdrFcShort( 0x0 ),	/* 0 */
/* 928 */	NdrFcShort( 0x0 ),	/* 0 */
/* 930 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter DomainHandle */

/* 932 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 934 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 936 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */

	/* Parameter EnumerationContext */

/* 938 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 940 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 942 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter Buffer */

/* 944 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 946 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 948 */	NdrFcShort( 0xa0 ),	/* Type Offset=160 */

	/* Parameter PreferedMaximumLength */

/* 950 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 952 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 954 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter CountReturned */

/* 956 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 958 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 960 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 962 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 964 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 966 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SamrGetAliasMembership */

/* 968 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 970 */	NdrFcLong( 0x0 ),	/* 0 */
/* 974 */	NdrFcShort( 0x10 ),	/* 16 */
/* 976 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 978 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 980 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 982 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 984 */	NdrFcShort( 0x24 ),	/* 36 */
/* 986 */	NdrFcShort( 0x8 ),	/* 8 */
/* 988 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 990 */	0xa,		/* 10 */
			0x7,		/* Ext Flags:  new corr desc, clt corr check, srv corr check, */
/* 992 */	NdrFcShort( 0x1 ),	/* 1 */
/* 994 */	NdrFcShort( 0x2 ),	/* 2 */
/* 996 */	NdrFcShort( 0x0 ),	/* 0 */
/* 998 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter DomainHandle */

/* 1000 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 1002 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 1004 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */

	/* Parameter SidArray */

/* 1006 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 1008 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1010 */	NdrFcShort( 0x234 ),	/* Type Offset=564 */

	/* Parameter Membership */

/* 1012 */	NdrFcShort( 0x4113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=16 */
/* 1014 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1016 */	NdrFcShort( 0x254 ),	/* Type Offset=596 */

	/* Return value */

/* 1018 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1020 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1022 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SamrLookupNamesInDomain */

/* 1024 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1026 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1030 */	NdrFcShort( 0x11 ),	/* 17 */
/* 1032 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 1034 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 1036 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 1038 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1040 */	NdrFcShort( 0x2c ),	/* 44 */
/* 1042 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1044 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x6,		/* 6 */
/* 1046 */	0xa,		/* 10 */
			0x7,		/* Ext Flags:  new corr desc, clt corr check, srv corr check, */
/* 1048 */	NdrFcShort( 0x2 ),	/* 2 */
/* 1050 */	NdrFcShort( 0x2 ),	/* 2 */
/* 1052 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1054 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter DomainHandle */

/* 1056 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 1058 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 1060 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */

	/* Parameter Count */

/* 1062 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1064 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1066 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter Names */

/* 1068 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1070 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1072 */	NdrFcShort( 0x264 ),	/* Type Offset=612 */

	/* Parameter RelativeIds */

/* 1074 */	NdrFcShort( 0x4113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=16 */
/* 1076 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1078 */	NdrFcShort( 0x254 ),	/* Type Offset=596 */

	/* Parameter Use */

/* 1080 */	NdrFcShort( 0x4113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=16 */
/* 1082 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 1084 */	NdrFcShort( 0x254 ),	/* Type Offset=596 */

	/* Return value */

/* 1086 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1088 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 1090 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SamrLookupIdsInDomain */

/* 1092 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1094 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1098 */	NdrFcShort( 0x12 ),	/* 18 */
/* 1100 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 1102 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 1104 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 1106 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1108 */	NdrFcShort( 0x2c ),	/* 44 */
/* 1110 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1112 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x6,		/* 6 */
/* 1114 */	0xa,		/* 10 */
			0x7,		/* Ext Flags:  new corr desc, clt corr check, srv corr check, */
/* 1116 */	NdrFcShort( 0x3 ),	/* 3 */
/* 1118 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1120 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1122 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter DomainHandle */

/* 1124 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 1126 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 1128 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */

	/* Parameter Count */

/* 1130 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1132 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1134 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter RelativeIds */

/* 1136 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 1138 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1140 */	NdrFcShort( 0x27e ),	/* Type Offset=638 */

	/* Parameter Names */

/* 1142 */	NdrFcShort( 0x4113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=16 */
/* 1144 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1146 */	NdrFcShort( 0x2aa ),	/* Type Offset=682 */

	/* Parameter Use */

/* 1148 */	NdrFcShort( 0x4113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=16 */
/* 1150 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 1152 */	NdrFcShort( 0x254 ),	/* Type Offset=596 */

	/* Return value */

/* 1154 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1156 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 1158 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SamrOpenGroup */

/* 1160 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1162 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1166 */	NdrFcShort( 0x13 ),	/* 19 */
/* 1168 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 1170 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 1172 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 1174 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1176 */	NdrFcShort( 0x34 ),	/* 52 */
/* 1178 */	NdrFcShort( 0x40 ),	/* 64 */
/* 1180 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x5,		/* 5 */
/* 1182 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1184 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1186 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1188 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1190 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter DomainHandle */

/* 1192 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 1194 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 1196 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */

	/* Parameter DesiredAccess */

/* 1198 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1200 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1202 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter GroupId */

/* 1204 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1206 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1208 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter GroupHandle */

/* 1210 */	NdrFcShort( 0x110 ),	/* Flags:  out, simple ref, */
/* 1212 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1214 */	NdrFcShort( 0xea ),	/* Type Offset=234 */

	/* Return value */

/* 1216 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1218 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 1220 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SamrQueryInformationGroup */

/* 1222 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1224 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1228 */	NdrFcShort( 0x14 ),	/* 20 */
/* 1230 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 1232 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 1234 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 1236 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1238 */	NdrFcShort( 0x2a ),	/* 42 */
/* 1240 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1242 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x4,		/* 4 */
/* 1244 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 1246 */	NdrFcShort( 0x5 ),	/* 5 */
/* 1248 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1250 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1252 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter GroupHandle */

/* 1254 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 1256 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 1258 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */

	/* Parameter GroupInformationClass */

/* 1260 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1262 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1264 */	0xd,		/* FC_ENUM16 */
			0x0,		/* 0 */

	/* Parameter Buffer */

/* 1266 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 1268 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1270 */	NdrFcShort( 0x2ba ),	/* Type Offset=698 */

	/* Return value */

/* 1272 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1274 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1276 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SamrSetInformationGroup */

/* 1278 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1280 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1284 */	NdrFcShort( 0x15 ),	/* 21 */
/* 1286 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 1288 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 1290 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 1292 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1294 */	NdrFcShort( 0x2a ),	/* 42 */
/* 1296 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1298 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 1300 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 1302 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1304 */	NdrFcShort( 0x5 ),	/* 5 */
/* 1306 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1308 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter GroupHandle */

/* 1310 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 1312 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 1314 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */

	/* Parameter GroupInformationClass */

/* 1316 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1318 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1320 */	0xd,		/* FC_ENUM16 */
			0x0,		/* 0 */

	/* Parameter Buffer */

/* 1322 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1324 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1326 */	NdrFcShort( 0x304 ),	/* Type Offset=772 */

	/* Return value */

/* 1328 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1330 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1332 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SamrAddMemberToGroup */

/* 1334 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1336 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1340 */	NdrFcShort( 0x16 ),	/* 22 */
/* 1342 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 1344 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 1346 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 1348 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1350 */	NdrFcShort( 0x34 ),	/* 52 */
/* 1352 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1354 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 1356 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1358 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1360 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1362 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1364 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter GroupHandle */

/* 1366 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 1368 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 1370 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */

	/* Parameter MemberId */

/* 1372 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1374 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1376 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter Attributes */

/* 1378 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1380 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1382 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1384 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1386 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1388 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SamrDeleteGroup */

/* 1390 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1392 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1396 */	NdrFcShort( 0x17 ),	/* 23 */
/* 1398 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1400 */	0x30,		/* FC_BIND_CONTEXT */
			0xe0,		/* Ctxt flags:  via ptr, in, out, */
/* 1402 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 1404 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1406 */	NdrFcShort( 0x38 ),	/* 56 */
/* 1408 */	NdrFcShort( 0x40 ),	/* 64 */
/* 1410 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 1412 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1414 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1416 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1418 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1420 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter GroupHandle */

/* 1422 */	NdrFcShort( 0x118 ),	/* Flags:  in, out, simple ref, */
/* 1424 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 1426 */	NdrFcShort( 0x12 ),	/* Type Offset=18 */

	/* Return value */

/* 1428 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1430 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1432 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SamrRemoveMemberFromGroup */

/* 1434 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1436 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1440 */	NdrFcShort( 0x18 ),	/* 24 */
/* 1442 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1444 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 1446 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 1448 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1450 */	NdrFcShort( 0x2c ),	/* 44 */
/* 1452 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1454 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 1456 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1458 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1460 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1462 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1464 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter GroupHandle */

/* 1466 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 1468 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 1470 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */

	/* Parameter MemberId */

/* 1472 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1474 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1476 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1478 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1480 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1482 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SamrGetMembersInGroup */

/* 1484 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1486 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1490 */	NdrFcShort( 0x19 ),	/* 25 */
/* 1492 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1494 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 1496 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 1498 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1500 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1502 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1504 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 1506 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 1508 */	NdrFcShort( 0x2 ),	/* 2 */
/* 1510 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1512 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1514 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter GroupHandle */

/* 1516 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 1518 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 1520 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */

	/* Parameter Members */

/* 1522 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 1524 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1526 */	NdrFcShort( 0x312 ),	/* Type Offset=786 */

	/* Return value */

/* 1528 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1530 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1532 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SamrSetMemberAttributesOfGroup */

/* 1534 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1536 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1540 */	NdrFcShort( 0x1a ),	/* 26 */
/* 1542 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 1544 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 1546 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 1548 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1550 */	NdrFcShort( 0x34 ),	/* 52 */
/* 1552 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1554 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 1556 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1558 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1560 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1562 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1564 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter GroupHandle */

/* 1566 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 1568 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 1570 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */

	/* Parameter MemberId */

/* 1572 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1574 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1576 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter Attributes */

/* 1578 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1580 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1582 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1584 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1586 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1588 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SamrOpenAlias */

/* 1590 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1592 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1596 */	NdrFcShort( 0x1b ),	/* 27 */
/* 1598 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 1600 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 1602 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 1604 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1606 */	NdrFcShort( 0x34 ),	/* 52 */
/* 1608 */	NdrFcShort( 0x40 ),	/* 64 */
/* 1610 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x5,		/* 5 */
/* 1612 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1614 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1616 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1618 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1620 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter DomainHandle */

/* 1622 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 1624 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 1626 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */

	/* Parameter DesiredAccess */

/* 1628 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1630 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1632 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter AliasId */

/* 1634 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1636 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1638 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter AliasHandle */

/* 1640 */	NdrFcShort( 0x110 ),	/* Flags:  out, simple ref, */
/* 1642 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1644 */	NdrFcShort( 0xea ),	/* Type Offset=234 */

	/* Return value */

/* 1646 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1648 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 1650 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SamrQueryInformationAlias */

/* 1652 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1654 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1658 */	NdrFcShort( 0x1c ),	/* 28 */
/* 1660 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 1662 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 1664 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 1666 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1668 */	NdrFcShort( 0x2a ),	/* 42 */
/* 1670 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1672 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x4,		/* 4 */
/* 1674 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 1676 */	NdrFcShort( 0x5 ),	/* 5 */
/* 1678 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1680 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1682 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter AliasHandle */

/* 1684 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 1686 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 1688 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */

	/* Parameter AliasInformationClass */

/* 1690 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1692 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1694 */	0xd,		/* FC_ENUM16 */
			0x0,		/* 0 */

	/* Parameter Buffer */

/* 1696 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 1698 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1700 */	NdrFcShort( 0x330 ),	/* Type Offset=816 */

	/* Return value */

/* 1702 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1704 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1706 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SamrSetInformationAlias */

/* 1708 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1710 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1714 */	NdrFcShort( 0x1d ),	/* 29 */
/* 1716 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 1718 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 1720 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 1722 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1724 */	NdrFcShort( 0x2a ),	/* 42 */
/* 1726 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1728 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 1730 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 1732 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1734 */	NdrFcShort( 0x5 ),	/* 5 */
/* 1736 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1738 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter AliasHandle */

/* 1740 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 1742 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 1744 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */

	/* Parameter AliasInformationClass */

/* 1746 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1748 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1750 */	0xd,		/* FC_ENUM16 */
			0x0,		/* 0 */

	/* Parameter Buffer */

/* 1752 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1754 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1756 */	NdrFcShort( 0x36e ),	/* Type Offset=878 */

	/* Return value */

/* 1758 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1760 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1762 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SamrDeleteAlias */

/* 1764 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1766 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1770 */	NdrFcShort( 0x1e ),	/* 30 */
/* 1772 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1774 */	0x30,		/* FC_BIND_CONTEXT */
			0xe0,		/* Ctxt flags:  via ptr, in, out, */
/* 1776 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 1778 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1780 */	NdrFcShort( 0x38 ),	/* 56 */
/* 1782 */	NdrFcShort( 0x40 ),	/* 64 */
/* 1784 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 1786 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1788 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1790 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1792 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1794 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter AliasHandle */

/* 1796 */	NdrFcShort( 0x118 ),	/* Flags:  in, out, simple ref, */
/* 1798 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 1800 */	NdrFcShort( 0x12 ),	/* Type Offset=18 */

	/* Return value */

/* 1802 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1804 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1806 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SamrAddMemberToAlias */

/* 1808 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1810 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1814 */	NdrFcShort( 0x1f ),	/* 31 */
/* 1816 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1818 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 1820 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 1822 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1824 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1826 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1828 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 1830 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 1832 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1834 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1836 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1838 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter AliasHandle */

/* 1840 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 1842 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 1844 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */

	/* Parameter MemberId */

/* 1846 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 1848 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1850 */	NdrFcShort( 0x8e ),	/* Type Offset=142 */

	/* Return value */

/* 1852 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1854 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1856 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SamrRemoveMemberFromAlias */

/* 1858 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1860 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1864 */	NdrFcShort( 0x20 ),	/* 32 */
/* 1866 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1868 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 1870 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 1872 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1874 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1876 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1878 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 1880 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 1882 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1884 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1886 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1888 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter AliasHandle */

/* 1890 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 1892 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 1894 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */

	/* Parameter MemberId */

/* 1896 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 1898 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1900 */	NdrFcShort( 0x8e ),	/* Type Offset=142 */

	/* Return value */

/* 1902 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1904 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1906 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SamrGetMembersInAlias */

/* 1908 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1910 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1914 */	NdrFcShort( 0x21 ),	/* 33 */
/* 1916 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1918 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 1920 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 1922 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1924 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1926 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1928 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 1930 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 1932 */	NdrFcShort( 0x2 ),	/* 2 */
/* 1934 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1936 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1938 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter AliasHandle */

/* 1940 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 1942 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 1944 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */

	/* Parameter Members */

/* 1946 */	NdrFcShort( 0x4113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=16 */
/* 1948 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1950 */	NdrFcShort( 0x234 ),	/* Type Offset=564 */

	/* Return value */

/* 1952 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1954 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1956 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SamrOpenUser */

/* 1958 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1960 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1964 */	NdrFcShort( 0x22 ),	/* 34 */
/* 1966 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 1968 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 1970 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 1972 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1974 */	NdrFcShort( 0x34 ),	/* 52 */
/* 1976 */	NdrFcShort( 0x40 ),	/* 64 */
/* 1978 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x5,		/* 5 */
/* 1980 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1982 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1984 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1986 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1988 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter DomainHandle */

/* 1990 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 1992 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 1994 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */

	/* Parameter DesiredAccess */

/* 1996 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1998 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2000 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter UserId */

/* 2002 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2004 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2006 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter UserHandle */

/* 2008 */	NdrFcShort( 0x110 ),	/* Flags:  out, simple ref, */
/* 2010 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2012 */	NdrFcShort( 0xea ),	/* Type Offset=234 */

	/* Return value */

/* 2014 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2016 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 2018 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SamrDeleteUser */

/* 2020 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 2022 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2026 */	NdrFcShort( 0x23 ),	/* 35 */
/* 2028 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2030 */	0x30,		/* FC_BIND_CONTEXT */
			0xe0,		/* Ctxt flags:  via ptr, in, out, */
/* 2032 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 2034 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 2036 */	NdrFcShort( 0x38 ),	/* 56 */
/* 2038 */	NdrFcShort( 0x40 ),	/* 64 */
/* 2040 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 2042 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2044 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2046 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2048 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2050 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter UserHandle */

/* 2052 */	NdrFcShort( 0x118 ),	/* Flags:  in, out, simple ref, */
/* 2054 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 2056 */	NdrFcShort( 0x12 ),	/* Type Offset=18 */

	/* Return value */

/* 2058 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2060 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2062 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SamrQueryInformationUser */

/* 2064 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 2066 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2070 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2072 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 2074 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 2076 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 2078 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 2080 */	NdrFcShort( 0x2a ),	/* 42 */
/* 2082 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2084 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x4,		/* 4 */
/* 2086 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 2088 */	NdrFcShort( 0x51 ),	/* 81 */
/* 2090 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2092 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2094 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter UserHandle */

/* 2096 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 2098 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 2100 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */

	/* Parameter UserInformationClass */

/* 2102 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2104 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2106 */	0xd,		/* FC_ENUM16 */
			0x0,		/* 0 */

	/* Parameter Buffer */

/* 2108 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 2110 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2112 */	NdrFcShort( 0x380 ),	/* Type Offset=896 */

	/* Return value */

/* 2114 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2116 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2118 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SamrSetInformationUser */

/* 2120 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 2122 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2126 */	NdrFcShort( 0x25 ),	/* 37 */
/* 2128 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 2130 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 2132 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 2134 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 2136 */	NdrFcShort( 0x2a ),	/* 42 */
/* 2138 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2140 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 2142 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 2144 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2146 */	NdrFcShort( 0x51 ),	/* 81 */
/* 2148 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2150 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter UserHandle */

/* 2152 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 2154 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 2156 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */

	/* Parameter UserInformationClass */

/* 2158 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2160 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2162 */	0xd,		/* FC_ENUM16 */
			0x0,		/* 0 */

	/* Parameter Buffer */

/* 2164 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2166 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2168 */	NdrFcShort( 0x604 ),	/* Type Offset=1540 */

	/* Return value */

/* 2170 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2172 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2174 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SamrChangePasswordUser */

/* 2176 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 2178 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2182 */	NdrFcShort( 0x26 ),	/* 38 */
/* 2184 */	NdrFcShort( 0x60 ),	/* ia64 Stack size/offset = 96 */
/* 2186 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 2188 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 2190 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 2192 */	NdrFcShort( 0x2f0 ),	/* 752 */
/* 2194 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2196 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0xc,		/* 12 */
/* 2198 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2200 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2202 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2204 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2206 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter UserHandle */

/* 2208 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 2210 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 2212 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */

	/* Parameter LmPresent */

/* 2214 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2216 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2218 */	0x2,		/* FC_CHAR */
			0x0,		/* 0 */

	/* Parameter LmOldEncryptedWithLmNew */

/* 2220 */	NdrFcShort( 0xa ),	/* Flags:  must free, in, */
/* 2222 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2224 */	NdrFcShort( 0x612 ),	/* Type Offset=1554 */

	/* Parameter LmNewEncryptedWithLmOld */

/* 2226 */	NdrFcShort( 0xa ),	/* Flags:  must free, in, */
/* 2228 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2230 */	NdrFcShort( 0x612 ),	/* Type Offset=1554 */

	/* Parameter NtPresent */

/* 2232 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2234 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 2236 */	0x2,		/* FC_CHAR */
			0x0,		/* 0 */

	/* Parameter NtOldEncryptedWithNtNew */

/* 2238 */	NdrFcShort( 0xa ),	/* Flags:  must free, in, */
/* 2240 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 2242 */	NdrFcShort( 0x612 ),	/* Type Offset=1554 */

	/* Parameter NtNewEncryptedWithNtOld */

/* 2244 */	NdrFcShort( 0xa ),	/* Flags:  must free, in, */
/* 2246 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 2248 */	NdrFcShort( 0x612 ),	/* Type Offset=1554 */

	/* Parameter NtCrossEncryptionPresent */

/* 2250 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2252 */	NdrFcShort( 0x38 ),	/* ia64 Stack size/offset = 56 */
/* 2254 */	0x2,		/* FC_CHAR */
			0x0,		/* 0 */

	/* Parameter NtNewEncryptedWithLmNew */

/* 2256 */	NdrFcShort( 0xa ),	/* Flags:  must free, in, */
/* 2258 */	NdrFcShort( 0x40 ),	/* ia64 Stack size/offset = 64 */
/* 2260 */	NdrFcShort( 0x612 ),	/* Type Offset=1554 */

	/* Parameter LmCrossEncryptionPresent */

/* 2262 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2264 */	NdrFcShort( 0x48 ),	/* ia64 Stack size/offset = 72 */
/* 2266 */	0x2,		/* FC_CHAR */
			0x0,		/* 0 */

	/* Parameter LmNtNewEncryptedWithNtNew */

/* 2268 */	NdrFcShort( 0xa ),	/* Flags:  must free, in, */
/* 2270 */	NdrFcShort( 0x50 ),	/* ia64 Stack size/offset = 80 */
/* 2272 */	NdrFcShort( 0x612 ),	/* Type Offset=1554 */

	/* Return value */

/* 2274 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2276 */	NdrFcShort( 0x58 ),	/* ia64 Stack size/offset = 88 */
/* 2278 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SamrGetGroupsForUser */

/* 2280 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 2282 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2286 */	NdrFcShort( 0x27 ),	/* 39 */
/* 2288 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2290 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 2292 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 2294 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 2296 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2298 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2300 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 2302 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 2304 */	NdrFcShort( 0x1 ),	/* 1 */
/* 2306 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2308 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2310 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter UserHandle */

/* 2312 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 2314 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 2316 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */

	/* Parameter Groups */

/* 2318 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 2320 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2322 */	NdrFcShort( 0x616 ),	/* Type Offset=1558 */

	/* Return value */

/* 2324 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2326 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2328 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SamrQueryDisplayInformation */

/* 2330 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 2332 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2336 */	NdrFcShort( 0x28 ),	/* 40 */
/* 2338 */	NdrFcShort( 0x48 ),	/* ia64 Stack size/offset = 72 */
/* 2340 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 2342 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 2344 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 2346 */	NdrFcShort( 0x42 ),	/* 66 */
/* 2348 */	NdrFcShort( 0x40 ),	/* 64 */
/* 2350 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x9,		/* 9 */
/* 2352 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 2354 */	NdrFcShort( 0xf ),	/* 15 */
/* 2356 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2358 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2360 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter DomainHandle */

/* 2362 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 2364 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 2366 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */

	/* Parameter DisplayInformationClass */

/* 2368 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2370 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2372 */	0xd,		/* FC_ENUM16 */
			0x0,		/* 0 */

	/* Parameter Index */

/* 2374 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2376 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2378 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter EntryCount */

/* 2380 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2382 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2384 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter PreferredMaximumLength */

/* 2386 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2388 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 2390 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter TotalAvailable */

/* 2392 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 2394 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 2396 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter TotalReturned */

/* 2398 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 2400 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 2402 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter Buffer */

/* 2404 */	NdrFcShort( 0x4113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=16 */
/* 2406 */	NdrFcShort( 0x38 ),	/* ia64 Stack size/offset = 56 */
/* 2408 */	NdrFcShort( 0x642 ),	/* Type Offset=1602 */

	/* Return value */

/* 2410 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2412 */	NdrFcShort( 0x40 ),	/* ia64 Stack size/offset = 64 */
/* 2414 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SamrGetDisplayEnumerationIndex */

/* 2416 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 2418 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2422 */	NdrFcShort( 0x29 ),	/* 41 */
/* 2424 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 2426 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 2428 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 2430 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 2432 */	NdrFcShort( 0x2a ),	/* 42 */
/* 2434 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2436 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 2438 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 2440 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2442 */	NdrFcShort( 0x1 ),	/* 1 */
/* 2444 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2446 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter DomainHandle */

/* 2448 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 2450 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 2452 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */

	/* Parameter DisplayInformationClass */

/* 2454 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2456 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2458 */	0xd,		/* FC_ENUM16 */
			0x0,		/* 0 */

	/* Parameter Prefix */

/* 2460 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2462 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2464 */	NdrFcShort( 0x58 ),	/* Type Offset=88 */

	/* Parameter Index */

/* 2466 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 2468 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2470 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2472 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2474 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 2476 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SamrTestPrivateFunctionsDomain */

/* 2478 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 2480 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2484 */	NdrFcShort( 0x2a ),	/* 42 */
/* 2486 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2488 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 2490 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 2492 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 2494 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2496 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2498 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 2500 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2502 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2504 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2506 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2508 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter DomainHandle */

/* 2510 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 2512 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 2514 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */

	/* Return value */

/* 2516 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2518 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2520 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SamrTestPrivateFunctionsUser */

/* 2522 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 2524 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2528 */	NdrFcShort( 0x2b ),	/* 43 */
/* 2530 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2532 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 2534 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 2536 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 2538 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2540 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2542 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 2544 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2546 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2548 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2550 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2552 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter UserHandle */

/* 2554 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 2556 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 2558 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */

	/* Return value */

/* 2560 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2562 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2564 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SamrGetUserDomainPasswordInformation */

/* 2566 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 2568 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2572 */	NdrFcShort( 0x2c ),	/* 44 */
/* 2574 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2576 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 2578 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 2580 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 2582 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2584 */	NdrFcShort( 0x34 ),	/* 52 */
/* 2586 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 2588 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2590 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2592 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2594 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2596 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter UserHandle */

/* 2598 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 2600 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 2602 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */

	/* Parameter PasswordInformation */

/* 2604 */	NdrFcShort( 0x2112 ),	/* Flags:  must free, out, simple ref, srv alloc size=8 */
/* 2606 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2608 */	NdrFcShort( 0x76a ),	/* Type Offset=1898 */

	/* Return value */

/* 2610 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2612 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2614 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SamrRemoveMemberFromForeignDomain */

/* 2616 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 2618 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2622 */	NdrFcShort( 0x2d ),	/* 45 */
/* 2624 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2626 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 2628 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 2630 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 2632 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2634 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2636 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 2638 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 2640 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2642 */	NdrFcShort( 0x1 ),	/* 1 */
/* 2644 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2646 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter DomainHandle */

/* 2648 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 2650 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 2652 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */

	/* Parameter MemberSid */

/* 2654 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2656 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2658 */	NdrFcShort( 0x8e ),	/* Type Offset=142 */

	/* Return value */

/* 2660 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2662 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2664 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SamrQueryInformationDomain2 */

/* 2666 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 2668 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2672 */	NdrFcShort( 0x2e ),	/* 46 */
/* 2674 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 2676 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 2678 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 2680 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 2682 */	NdrFcShort( 0x2a ),	/* 42 */
/* 2684 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2686 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x4,		/* 4 */
/* 2688 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 2690 */	NdrFcShort( 0xa ),	/* 10 */
/* 2692 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2694 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2696 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter DomainHandle */

/* 2698 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 2700 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 2702 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */

	/* Parameter DomainInformationClass */

/* 2704 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2706 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2708 */	0xd,		/* FC_ENUM16 */
			0x0,		/* 0 */

	/* Parameter Buffer */

/* 2710 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 2712 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2714 */	NdrFcShort( 0x772 ),	/* Type Offset=1906 */

	/* Return value */

/* 2716 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2718 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2720 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SamrQueryInformationUser2 */

/* 2722 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 2724 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2728 */	NdrFcShort( 0x2f ),	/* 47 */
/* 2730 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 2732 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 2734 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 2736 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 2738 */	NdrFcShort( 0x2a ),	/* 42 */
/* 2740 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2742 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x4,		/* 4 */
/* 2744 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 2746 */	NdrFcShort( 0x51 ),	/* 81 */
/* 2748 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2750 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2752 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter UserHandle */

/* 2754 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 2756 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 2758 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */

	/* Parameter UserInformationClass */

/* 2760 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2762 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2764 */	0xd,		/* FC_ENUM16 */
			0x0,		/* 0 */

	/* Parameter Buffer */

/* 2766 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 2768 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2770 */	NdrFcShort( 0x784 ),	/* Type Offset=1924 */

	/* Return value */

/* 2772 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2774 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2776 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SamrQueryDisplayInformation2 */

/* 2778 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 2780 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2784 */	NdrFcShort( 0x30 ),	/* 48 */
/* 2786 */	NdrFcShort( 0x48 ),	/* ia64 Stack size/offset = 72 */
/* 2788 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 2790 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 2792 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 2794 */	NdrFcShort( 0x42 ),	/* 66 */
/* 2796 */	NdrFcShort( 0x40 ),	/* 64 */
/* 2798 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x9,		/* 9 */
/* 2800 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 2802 */	NdrFcShort( 0xf ),	/* 15 */
/* 2804 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2806 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2808 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter DomainHandle */

/* 2810 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 2812 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 2814 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */

	/* Parameter DisplayInformationClass */

/* 2816 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2818 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2820 */	0xd,		/* FC_ENUM16 */
			0x0,		/* 0 */

	/* Parameter Index */

/* 2822 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2824 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2826 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter EntryCount */

/* 2828 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2830 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2832 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter PreferredMaximumLength */

/* 2834 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2836 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 2838 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter TotalAvailable */

/* 2840 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 2842 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 2844 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter TotalReturned */

/* 2846 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 2848 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 2850 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter Buffer */

/* 2852 */	NdrFcShort( 0x4113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=16 */
/* 2854 */	NdrFcShort( 0x38 ),	/* ia64 Stack size/offset = 56 */
/* 2856 */	NdrFcShort( 0x79a ),	/* Type Offset=1946 */

	/* Return value */

/* 2858 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2860 */	NdrFcShort( 0x40 ),	/* ia64 Stack size/offset = 64 */
/* 2862 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SamrGetDisplayEnumerationIndex2 */

/* 2864 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 2866 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2870 */	NdrFcShort( 0x31 ),	/* 49 */
/* 2872 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 2874 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 2876 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 2878 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 2880 */	NdrFcShort( 0x2a ),	/* 42 */
/* 2882 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2884 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 2886 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 2888 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2890 */	NdrFcShort( 0x1 ),	/* 1 */
/* 2892 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2894 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter DomainHandle */

/* 2896 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 2898 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 2900 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */

	/* Parameter DisplayInformationClass */

/* 2902 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2904 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2906 */	0xd,		/* FC_ENUM16 */
			0x0,		/* 0 */

	/* Parameter Prefix */

/* 2908 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2910 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2912 */	NdrFcShort( 0x58 ),	/* Type Offset=88 */

	/* Parameter Index */

/* 2914 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 2916 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2918 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2920 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2922 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 2924 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SamrCreateUser2InDomain */

/* 2926 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 2928 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2932 */	NdrFcShort( 0x32 ),	/* 50 */
/* 2934 */	NdrFcShort( 0x40 ),	/* ia64 Stack size/offset = 64 */
/* 2936 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 2938 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 2940 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 2942 */	NdrFcShort( 0x34 ),	/* 52 */
/* 2944 */	NdrFcShort( 0x78 ),	/* 120 */
/* 2946 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x8,		/* 8 */
/* 2948 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 2950 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2952 */	NdrFcShort( 0x1 ),	/* 1 */
/* 2954 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2956 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter DomainHandle */

/* 2958 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 2960 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 2962 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */

	/* Parameter Name */

/* 2964 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2966 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2968 */	NdrFcShort( 0x58 ),	/* Type Offset=88 */

	/* Parameter AccountType */

/* 2970 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2972 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2974 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter DesiredAccess */

/* 2976 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2978 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2980 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter UserHandle */

/* 2982 */	NdrFcShort( 0x110 ),	/* Flags:  out, simple ref, */
/* 2984 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 2986 */	NdrFcShort( 0xea ),	/* Type Offset=234 */

	/* Parameter GrantedAccess */

/* 2988 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 2990 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 2992 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter RelativeId */

/* 2994 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 2996 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 2998 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3000 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3002 */	NdrFcShort( 0x38 ),	/* ia64 Stack size/offset = 56 */
/* 3004 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SamrQueryDisplayInformation3 */

/* 3006 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 3008 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3012 */	NdrFcShort( 0x33 ),	/* 51 */
/* 3014 */	NdrFcShort( 0x48 ),	/* ia64 Stack size/offset = 72 */
/* 3016 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 3018 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 3020 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 3022 */	NdrFcShort( 0x42 ),	/* 66 */
/* 3024 */	NdrFcShort( 0x40 ),	/* 64 */
/* 3026 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x9,		/* 9 */
/* 3028 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 3030 */	NdrFcShort( 0xf ),	/* 15 */
/* 3032 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3034 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3036 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter DomainHandle */

/* 3038 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 3040 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 3042 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */

	/* Parameter DisplayInformationClass */

/* 3044 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3046 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 3048 */	0xd,		/* FC_ENUM16 */
			0x0,		/* 0 */

	/* Parameter Index */

/* 3050 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3052 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 3054 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter EntryCount */

/* 3056 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3058 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 3060 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter PreferredMaximumLength */

/* 3062 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3064 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 3066 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter TotalAvailable */

/* 3068 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3070 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 3072 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter TotalReturned */

/* 3074 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3076 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 3078 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter Buffer */

/* 3080 */	NdrFcShort( 0x4113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=16 */
/* 3082 */	NdrFcShort( 0x38 ),	/* ia64 Stack size/offset = 56 */
/* 3084 */	NdrFcShort( 0x7a8 ),	/* Type Offset=1960 */

	/* Return value */

/* 3086 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3088 */	NdrFcShort( 0x40 ),	/* ia64 Stack size/offset = 64 */
/* 3090 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SamrAddMultipleMembersToAlias */

/* 3092 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 3094 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3098 */	NdrFcShort( 0x34 ),	/* 52 */
/* 3100 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 3102 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 3104 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 3106 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 3108 */	NdrFcShort( 0x24 ),	/* 36 */
/* 3110 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3112 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 3114 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 3116 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3118 */	NdrFcShort( 0x2 ),	/* 2 */
/* 3120 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3122 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter AliasHandle */

/* 3124 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 3126 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 3128 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */

	/* Parameter MembersBuffer */

/* 3130 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 3132 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 3134 */	NdrFcShort( 0x234 ),	/* Type Offset=564 */

	/* Return value */

/* 3136 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3138 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 3140 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SamrRemoveMultipleMembersFromAlias */

/* 3142 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 3144 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3148 */	NdrFcShort( 0x35 ),	/* 53 */
/* 3150 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 3152 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 3154 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 3156 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 3158 */	NdrFcShort( 0x24 ),	/* 36 */
/* 3160 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3162 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 3164 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 3166 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3168 */	NdrFcShort( 0x2 ),	/* 2 */
/* 3170 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3172 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter AliasHandle */

/* 3174 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 3176 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 3178 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */

	/* Parameter MembersBuffer */

/* 3180 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 3182 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 3184 */	NdrFcShort( 0x234 ),	/* Type Offset=564 */

	/* Return value */

/* 3186 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3188 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 3190 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SamrOemChangePasswordUser2 */

/* 3192 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 3194 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3198 */	NdrFcShort( 0x36 ),	/* 54 */
/* 3200 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 3202 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x0,		/* 0 */
/* 3204 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 3206 */	NdrFcShort( 0x2ac ),	/* 684 */
/* 3208 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3210 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 3212 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 3214 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3216 */	NdrFcShort( 0x2 ),	/* 2 */
/* 3218 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3220 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter BindingHandle */

/* 3222 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 3224 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 3226 */	NdrFcShort( 0x7b2 ),	/* Type Offset=1970 */

	/* Parameter ServerName */

/* 3228 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 3230 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 3232 */	NdrFcShort( 0x7b6 ),	/* Type Offset=1974 */

	/* Parameter UserName */

/* 3234 */	NdrFcShort( 0xa ),	/* Flags:  must free, in, */
/* 3236 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 3238 */	NdrFcShort( 0x7cc ),	/* Type Offset=1996 */

	/* Parameter NewPasswordEncryptedWithOldLm */

/* 3240 */	NdrFcShort( 0xa ),	/* Flags:  must free, in, */
/* 3242 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 3244 */	NdrFcShort( 0x612 ),	/* Type Offset=1554 */

	/* Parameter OldLmOwfPassswordEncryptedWithNewLm */

/* 3246 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3248 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 3250 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SamrUnicodeChangePasswordUser2 */


	/* Return value */

/* 3252 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 3254 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3258 */	NdrFcShort( 0x37 ),	/* 55 */
/* 3260 */	NdrFcShort( 0x48 ),	/* ia64 Stack size/offset = 72 */
/* 3262 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x0,		/* 0 */
/* 3264 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 3266 */	NdrFcShort( 0x55d ),	/* 1373 */
/* 3268 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3270 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x8,		/* 8 */
/* 3272 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 3274 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3276 */	NdrFcShort( 0x2 ),	/* 2 */
/* 3278 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3280 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter BindingHandle */

/* 3282 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 3284 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 3286 */	NdrFcShort( 0x7d0 ),	/* Type Offset=2000 */

	/* Parameter ServerName */

/* 3288 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 3290 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 3292 */	NdrFcShort( 0x58 ),	/* Type Offset=88 */

	/* Parameter UserName */

/* 3294 */	NdrFcShort( 0xa ),	/* Flags:  must free, in, */
/* 3296 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 3298 */	NdrFcShort( 0x7cc ),	/* Type Offset=1996 */

	/* Parameter NewPasswordEncryptedWithOldNt */

/* 3300 */	NdrFcShort( 0xa ),	/* Flags:  must free, in, */
/* 3302 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 3304 */	NdrFcShort( 0x612 ),	/* Type Offset=1554 */

	/* Parameter OldNtOwfPasswordEncryptedWithNewNt */

/* 3306 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3308 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 3310 */	0x2,		/* FC_CHAR */
			0x0,		/* 0 */

	/* Parameter LmPresent */

/* 3312 */	NdrFcShort( 0xa ),	/* Flags:  must free, in, */
/* 3314 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 3316 */	NdrFcShort( 0x7cc ),	/* Type Offset=1996 */

	/* Parameter NewPasswordEncryptedWithOldLm */

/* 3318 */	NdrFcShort( 0xa ),	/* Flags:  must free, in, */
/* 3320 */	NdrFcShort( 0x38 ),	/* ia64 Stack size/offset = 56 */
/* 3322 */	NdrFcShort( 0x612 ),	/* Type Offset=1554 */

	/* Parameter OldLmOwfPassswordEncryptedWithNewLmOrNt */

/* 3324 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3326 */	NdrFcShort( 0x40 ),	/* ia64 Stack size/offset = 64 */
/* 3328 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SamrGetDomainPasswordInformation */


	/* Return value */

/* 3330 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 3332 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3336 */	NdrFcShort( 0x38 ),	/* 56 */
/* 3338 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 3340 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x0,		/* 0 */
/* 3342 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 3344 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3346 */	NdrFcShort( 0x34 ),	/* 52 */
/* 3348 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 3350 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 3352 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3354 */	NdrFcShort( 0x1 ),	/* 1 */
/* 3356 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3358 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter BindingHandle */

/* 3360 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 3362 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 3364 */	NdrFcShort( 0x7d0 ),	/* Type Offset=2000 */

	/* Parameter ServerName */

/* 3366 */	NdrFcShort( 0x2112 ),	/* Flags:  must free, out, simple ref, srv alloc size=8 */
/* 3368 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 3370 */	NdrFcShort( 0x76a ),	/* Type Offset=1898 */

	/* Parameter PasswordInformation */

/* 3372 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3374 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 3376 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SamrConnect2 */


	/* Return value */

/* 3378 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 3380 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3384 */	NdrFcShort( 0x39 ),	/* 57 */
/* 3386 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 3388 */	0x31,		/* FC_BIND_GENERIC */
			0x8,		/* 8 */
/* 3390 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 3392 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 3394 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3396 */	NdrFcShort( 0x40 ),	/* 64 */
/* 3398 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 3400 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3402 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3404 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3406 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3408 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ServerName */

/* 3410 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 3412 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 3414 */	NdrFcShort( 0x7d4 ),	/* Type Offset=2004 */

	/* Parameter ServerHandle */

/* 3416 */	NdrFcShort( 0x110 ),	/* Flags:  out, simple ref, */
/* 3418 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 3420 */	NdrFcShort( 0xa ),	/* Type Offset=10 */

	/* Parameter DesiredAccess */

/* 3422 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3424 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 3426 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3428 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3430 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 3432 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SamrSetInformationUser2 */

/* 3434 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 3436 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3440 */	NdrFcShort( 0x3a ),	/* 58 */
/* 3442 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 3444 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 3446 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 3448 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 3450 */	NdrFcShort( 0x2a ),	/* 42 */
/* 3452 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3454 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 3456 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 3458 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3460 */	NdrFcShort( 0x51 ),	/* 81 */
/* 3462 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3464 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter UserHandle */

/* 3466 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 3468 */	NdrFcShort( 0x0 ),	/* ia64 Stack size/offset = 0 */
/* 3470 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */

	/* Parameter UserInformationClass */

/* 3472 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3474 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 3476 */	0xd,		/* FC_ENUM16 */
			0x0,		/* 0 */

	/* Parameter Buffer */

/* 3478 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 3480 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 3482 */	NdrFcShort( 0x7d8 ),	/* Type Offset=2008 */

	/* Return value */

/* 3484 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3486 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 3488 */	0x8,		/* FC_LONG */
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
/*  4 */	0x5,		/* FC_WCHAR */
			0x5c,		/* FC_PAD */
/*  6 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/*  8 */	NdrFcShort( 0x2 ),	/* Offset= 2 (10) */
/* 10 */	0x30,		/* FC_BIND_CONTEXT */
			0xa0,		/* Ctxt flags:  via ptr, out, */
/* 12 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 14 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 16 */	NdrFcShort( 0x2 ),	/* Offset= 2 (18) */
/* 18 */	0x30,		/* FC_BIND_CONTEXT */
			0xe1,		/* Ctxt flags:  via ptr, in, out, can't be null */
/* 20 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 22 */	0x30,		/* FC_BIND_CONTEXT */
			0x41,		/* Ctxt flags:  in, can't be null */
/* 24 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 26 */	
			0x11, 0x0,	/* FC_RP */
/* 28 */	NdrFcShort( 0xe ),	/* Offset= 14 (42) */
/* 30 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 32 */	NdrFcShort( 0x1 ),	/* 1 */
/* 34 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 36 */	NdrFcShort( 0x0 ),	/* 0 */
/* 38 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 40 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
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
			0x12, 0x0,	/* FC_UP */
/* 56 */	NdrFcShort( 0xffe6 ),	/* Offset= -26 (30) */
/* 58 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 60 */	NdrFcShort( 0x2 ),	/* Offset= 2 (62) */
/* 62 */	
			0x12, 0x0,	/* FC_UP */
/* 64 */	NdrFcShort( 0xffea ),	/* Offset= -22 (42) */
/* 66 */	
			0x11, 0x0,	/* FC_RP */
/* 68 */	NdrFcShort( 0x14 ),	/* Offset= 20 (88) */
/* 70 */	
			0x1c,		/* FC_CVARRAY */
			0x1,		/* 1 */
/* 72 */	NdrFcShort( 0x2 ),	/* 2 */
/* 74 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x55,		/* FC_DIV_2 */
/* 76 */	NdrFcShort( 0x2 ),	/* 2 */
/* 78 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 80 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x55,		/* FC_DIV_2 */
/* 82 */	NdrFcShort( 0x0 ),	/* 0 */
/* 84 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 86 */	0x5,		/* FC_WCHAR */
			0x5b,		/* FC_END */
/* 88 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 90 */	NdrFcShort( 0x10 ),	/* 16 */
/* 92 */	NdrFcShort( 0x0 ),	/* 0 */
/* 94 */	NdrFcShort( 0x8 ),	/* Offset= 8 (102) */
/* 96 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 98 */	0x40,		/* FC_STRUCTPAD4 */
			0x36,		/* FC_POINTER */
/* 100 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 102 */	
			0x12, 0x0,	/* FC_UP */
/* 104 */	NdrFcShort( 0xffde ),	/* Offset= -34 (70) */
/* 106 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 108 */	NdrFcShort( 0x2 ),	/* Offset= 2 (110) */
/* 110 */	
			0x12, 0x0,	/* FC_UP */
/* 112 */	NdrFcShort( 0x1e ),	/* Offset= 30 (142) */
/* 114 */	
			0x1d,		/* FC_SMFARRAY */
			0x0,		/* 0 */
/* 116 */	NdrFcShort( 0x6 ),	/* 6 */
/* 118 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 120 */	
			0x15,		/* FC_STRUCT */
			0x0,		/* 0 */
/* 122 */	NdrFcShort( 0x6 ),	/* 6 */
/* 124 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 126 */	NdrFcShort( 0xfff4 ),	/* Offset= -12 (114) */
/* 128 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 130 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 132 */	NdrFcShort( 0x4 ),	/* 4 */
/* 134 */	0x4,		/* Corr desc: FC_USMALL */
			0x0,		/*  */
/* 136 */	NdrFcShort( 0xfff9 ),	/* -7 */
/* 138 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 140 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 142 */	
			0x17,		/* FC_CSTRUCT */
			0x3,		/* 3 */
/* 144 */	NdrFcShort( 0x8 ),	/* 8 */
/* 146 */	NdrFcShort( 0xfff0 ),	/* Offset= -16 (130) */
/* 148 */	0x2,		/* FC_CHAR */
			0x2,		/* FC_CHAR */
/* 150 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 152 */	NdrFcShort( 0xffe0 ),	/* Offset= -32 (120) */
/* 154 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 156 */	
			0x11, 0x8,	/* FC_RP [simple_pointer] */
/* 158 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 160 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 162 */	NdrFcShort( 0x2 ),	/* Offset= 2 (164) */
/* 164 */	
			0x12, 0x0,	/* FC_UP */
/* 166 */	NdrFcShort( 0x28 ),	/* Offset= 40 (206) */
/* 168 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 170 */	NdrFcShort( 0x18 ),	/* 24 */
/* 172 */	NdrFcShort( 0x0 ),	/* 0 */
/* 174 */	NdrFcShort( 0x0 ),	/* Offset= 0 (174) */
/* 176 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 178 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 180 */	NdrFcShort( 0xffa4 ),	/* Offset= -92 (88) */
/* 182 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 184 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 186 */	NdrFcShort( 0x0 ),	/* 0 */
/* 188 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 190 */	NdrFcShort( 0x0 ),	/* 0 */
/* 192 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 194 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 198 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 200 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 202 */	NdrFcShort( 0xffde ),	/* Offset= -34 (168) */
/* 204 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 206 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 208 */	NdrFcShort( 0x10 ),	/* 16 */
/* 210 */	NdrFcShort( 0x0 ),	/* 0 */
/* 212 */	NdrFcShort( 0x6 ),	/* Offset= 6 (218) */
/* 214 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 216 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 218 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 220 */	NdrFcShort( 0xffdc ),	/* Offset= -36 (184) */
/* 222 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 224 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 226 */	
			0x11, 0x0,	/* FC_RP */
/* 228 */	NdrFcShort( 0xffaa ),	/* Offset= -86 (142) */
/* 230 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 232 */	NdrFcShort( 0x2 ),	/* Offset= 2 (234) */
/* 234 */	0x30,		/* FC_BIND_CONTEXT */
			0xa0,		/* Ctxt flags:  via ptr, out, */
/* 236 */	0x0,		/* 0 */
			0x1,		/* 1 */
/* 238 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 240 */	NdrFcShort( 0x2 ),	/* Offset= 2 (242) */
/* 242 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 244 */	NdrFcShort( 0x2 ),	/* Offset= 2 (246) */
/* 246 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0xd,		/* FC_ENUM16 */
/* 248 */	0x26,		/* Corr desc:  parameter, FC_SHORT */
			0x0,		/*  */
/* 250 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 252 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 254 */	NdrFcShort( 0x2 ),	/* Offset= 2 (256) */
/* 256 */	NdrFcShort( 0x70 ),	/* 112 */
/* 258 */	NdrFcShort( 0x700c ),	/* 28684 */
/* 260 */	NdrFcLong( 0x1 ),	/* 1 */
/* 264 */	NdrFcShort( 0x4e ),	/* Offset= 78 (342) */
/* 266 */	NdrFcLong( 0x2 ),	/* 2 */
/* 270 */	NdrFcShort( 0x58 ),	/* Offset= 88 (358) */
/* 272 */	NdrFcLong( 0x3 ),	/* 3 */
/* 276 */	NdrFcShort( 0x76 ),	/* Offset= 118 (394) */
/* 278 */	NdrFcLong( 0x4 ),	/* 4 */
/* 282 */	NdrFcShort( 0x7a ),	/* Offset= 122 (404) */
/* 284 */	NdrFcLong( 0x5 ),	/* 5 */
/* 288 */	NdrFcShort( 0x74 ),	/* Offset= 116 (404) */
/* 290 */	NdrFcLong( 0x7 ),	/* 7 */
/* 294 */	NdrFcShort( 0x7c ),	/* Offset= 124 (418) */
/* 296 */	NdrFcLong( 0x6 ),	/* 6 */
/* 300 */	NdrFcShort( 0x68 ),	/* Offset= 104 (404) */
/* 302 */	NdrFcLong( 0x8 ),	/* 8 */
/* 306 */	NdrFcShort( 0x7a ),	/* Offset= 122 (428) */
/* 308 */	NdrFcLong( 0x9 ),	/* 9 */
/* 312 */	NdrFcShort( 0x6a ),	/* Offset= 106 (418) */
/* 314 */	NdrFcLong( 0xb ),	/* 11 */
/* 318 */	NdrFcShort( 0x82 ),	/* Offset= 130 (448) */
/* 320 */	NdrFcLong( 0xc ),	/* 12 */
/* 324 */	NdrFcShort( 0x94 ),	/* Offset= 148 (472) */
/* 326 */	NdrFcLong( 0xd ),	/* 13 */
/* 330 */	NdrFcShort( 0xa2 ),	/* Offset= 162 (492) */
/* 332 */	NdrFcShort( 0xffff ),	/* Offset= -1 (331) */
/* 334 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 336 */	NdrFcShort( 0x8 ),	/* 8 */
/* 338 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 340 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 342 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 344 */	NdrFcShort( 0x18 ),	/* 24 */
/* 346 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 348 */	0x8,		/* FC_LONG */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 350 */	0x0,		/* 0 */
			NdrFcShort( 0xffef ),	/* Offset= -17 (334) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 354 */	0x0,		/* 0 */
			NdrFcShort( 0xffeb ),	/* Offset= -21 (334) */
			0x5b,		/* FC_END */
/* 358 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 360 */	NdrFcShort( 0x58 ),	/* 88 */
/* 362 */	NdrFcShort( 0x0 ),	/* 0 */
/* 364 */	NdrFcShort( 0x0 ),	/* Offset= 0 (364) */
/* 366 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 368 */	NdrFcShort( 0xffde ),	/* Offset= -34 (334) */
/* 370 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 372 */	NdrFcShort( 0xfee4 ),	/* Offset= -284 (88) */
/* 374 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 376 */	NdrFcShort( 0xfee0 ),	/* Offset= -288 (88) */
/* 378 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 380 */	NdrFcShort( 0xfedc ),	/* Offset= -292 (88) */
/* 382 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 384 */	NdrFcShort( 0xffce ),	/* Offset= -50 (334) */
/* 386 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 388 */	0x2,		/* FC_CHAR */
			0x3f,		/* FC_STRUCTPAD3 */
/* 390 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 392 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 394 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 396 */	NdrFcShort( 0x8 ),	/* 8 */
/* 398 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 400 */	NdrFcShort( 0xffbe ),	/* Offset= -66 (334) */
/* 402 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 404 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 406 */	NdrFcShort( 0x10 ),	/* 16 */
/* 408 */	NdrFcShort( 0x0 ),	/* 0 */
/* 410 */	NdrFcShort( 0x0 ),	/* Offset= 0 (410) */
/* 412 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 414 */	NdrFcShort( 0xfeba ),	/* Offset= -326 (88) */
/* 416 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 418 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x1,		/* 1 */
/* 420 */	NdrFcShort( 0x4 ),	/* 4 */
/* 422 */	NdrFcShort( 0x0 ),	/* 0 */
/* 424 */	NdrFcShort( 0x0 ),	/* Offset= 0 (424) */
/* 426 */	0xd,		/* FC_ENUM16 */
			0x5b,		/* FC_END */
/* 428 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 430 */	NdrFcShort( 0x10 ),	/* 16 */
/* 432 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 434 */	NdrFcShort( 0xff9c ),	/* Offset= -100 (334) */
/* 436 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 438 */	NdrFcShort( 0xff98 ),	/* Offset= -104 (334) */
/* 440 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 442 */	
			0x15,		/* FC_STRUCT */
			0x7,		/* 7 */
/* 444 */	NdrFcShort( 0x8 ),	/* 8 */
/* 446 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 448 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x7,		/* 7 */
/* 450 */	NdrFcShort( 0x6c ),	/* 108 */
/* 452 */	NdrFcShort( 0x0 ),	/* 0 */
/* 454 */	NdrFcShort( 0x0 ),	/* Offset= 0 (454) */
/* 456 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 458 */	NdrFcShort( 0xff9c ),	/* Offset= -100 (358) */
/* 460 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 462 */	NdrFcShort( 0xffec ),	/* Offset= -20 (442) */
/* 464 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 466 */	NdrFcShort( 0xffe8 ),	/* Offset= -24 (442) */
/* 468 */	0x6,		/* FC_SHORT */
			0x3e,		/* FC_STRUCTPAD2 */
/* 470 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 472 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x7,		/* 7 */
/* 474 */	NdrFcShort( 0x18 ),	/* 24 */
/* 476 */	NdrFcShort( 0x0 ),	/* 0 */
/* 478 */	NdrFcShort( 0x0 ),	/* Offset= 0 (478) */
/* 480 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 482 */	NdrFcShort( 0xffd8 ),	/* Offset= -40 (442) */
/* 484 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 486 */	NdrFcShort( 0xffd4 ),	/* Offset= -44 (442) */
/* 488 */	0x6,		/* FC_SHORT */
			0x42,		/* FC_STRUCTPAD6 */
/* 490 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 492 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 494 */	NdrFcShort( 0x18 ),	/* 24 */
/* 496 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 498 */	NdrFcShort( 0xff5c ),	/* Offset= -164 (334) */
/* 500 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 502 */	NdrFcShort( 0xff58 ),	/* Offset= -168 (334) */
/* 504 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 506 */	NdrFcShort( 0xff54 ),	/* Offset= -172 (334) */
/* 508 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 510 */	
			0x11, 0x1,	/* FC_RP [all_nodes] */
/* 512 */	NdrFcShort( 0x2 ),	/* Offset= 2 (514) */
/* 514 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0xd,		/* FC_ENUM16 */
/* 516 */	0x26,		/* Corr desc:  parameter, FC_SHORT */
			0x0,		/*  */
/* 518 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 520 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 522 */	NdrFcShort( 0xfef6 ),	/* Offset= -266 (256) */
/* 524 */	
			0x11, 0x0,	/* FC_RP */
/* 526 */	NdrFcShort( 0x26 ),	/* Offset= 38 (564) */
/* 528 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 530 */	NdrFcShort( 0x8 ),	/* 8 */
/* 532 */	NdrFcShort( 0x0 ),	/* 0 */
/* 534 */	NdrFcShort( 0x4 ),	/* Offset= 4 (538) */
/* 536 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 538 */	
			0x12, 0x0,	/* FC_UP */
/* 540 */	NdrFcShort( 0xfe72 ),	/* Offset= -398 (142) */
/* 542 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 544 */	NdrFcShort( 0x0 ),	/* 0 */
/* 546 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 548 */	NdrFcShort( 0x0 ),	/* 0 */
/* 550 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 552 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 556 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 558 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 560 */	NdrFcShort( 0xffe0 ),	/* Offset= -32 (528) */
/* 562 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 564 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 566 */	NdrFcShort( 0x10 ),	/* 16 */
/* 568 */	NdrFcShort( 0x0 ),	/* 0 */
/* 570 */	NdrFcShort( 0x6 ),	/* Offset= 6 (576) */
/* 572 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 574 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 576 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 578 */	NdrFcShort( 0xffdc ),	/* Offset= -36 (542) */
/* 580 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 582 */	NdrFcShort( 0xe ),	/* Offset= 14 (596) */
/* 584 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 586 */	NdrFcShort( 0x4 ),	/* 4 */
/* 588 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 590 */	NdrFcShort( 0x0 ),	/* 0 */
/* 592 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 594 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 596 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 598 */	NdrFcShort( 0x10 ),	/* 16 */
/* 600 */	NdrFcShort( 0x0 ),	/* 0 */
/* 602 */	NdrFcShort( 0x6 ),	/* Offset= 6 (608) */
/* 604 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 606 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 608 */	
			0x12, 0x0,	/* FC_UP */
/* 610 */	NdrFcShort( 0xffe6 ),	/* Offset= -26 (584) */
/* 612 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 614 */	NdrFcShort( 0x0 ),	/* 0 */
/* 616 */	0x40,		/* Corr desc:  constant, val=1000 */
			0x0,		/* 0 */
/* 618 */	NdrFcShort( 0x3e8 ),	/* 1000 */
/* 620 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 622 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 624 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 626 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 628 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 630 */	NdrFcShort( 0xfde2 ),	/* Offset= -542 (88) */
/* 632 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 634 */	
			0x11, 0x0,	/* FC_RP */
/* 636 */	NdrFcShort( 0x2 ),	/* Offset= 2 (638) */
/* 638 */	
			0x1c,		/* FC_CVARRAY */
			0x3,		/* 3 */
/* 640 */	NdrFcShort( 0x4 ),	/* 4 */
/* 642 */	0x40,		/* Corr desc:  constant, val=1000 */
			0x0,		/* 0 */
/* 644 */	NdrFcShort( 0x3e8 ),	/* 1000 */
/* 646 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 648 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 650 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 652 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 654 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 656 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 658 */	NdrFcShort( 0x18 ),	/* Offset= 24 (682) */
/* 660 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 662 */	NdrFcShort( 0x0 ),	/* 0 */
/* 664 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 666 */	NdrFcShort( 0x0 ),	/* 0 */
/* 668 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 670 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 674 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 676 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 678 */	NdrFcShort( 0xfdb2 ),	/* Offset= -590 (88) */
/* 680 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 682 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 684 */	NdrFcShort( 0x10 ),	/* 16 */
/* 686 */	NdrFcShort( 0x0 ),	/* 0 */
/* 688 */	NdrFcShort( 0x6 ),	/* Offset= 6 (694) */
/* 690 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 692 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 694 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 696 */	NdrFcShort( 0xffdc ),	/* Offset= -36 (660) */
/* 698 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 700 */	NdrFcShort( 0x2 ),	/* Offset= 2 (702) */
/* 702 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 704 */	NdrFcShort( 0x2 ),	/* Offset= 2 (706) */
/* 706 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0xd,		/* FC_ENUM16 */
/* 708 */	0x26,		/* Corr desc:  parameter, FC_SHORT */
			0x0,		/*  */
/* 710 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 712 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 714 */	NdrFcShort( 0x2 ),	/* Offset= 2 (716) */
/* 716 */	NdrFcShort( 0x28 ),	/* 40 */
/* 718 */	NdrFcShort( 0x3004 ),	/* 12292 */
/* 720 */	NdrFcLong( 0x1 ),	/* 1 */
/* 724 */	NdrFcShort( 0x16 ),	/* Offset= 22 (746) */
/* 726 */	NdrFcLong( 0x2 ),	/* 2 */
/* 730 */	NdrFcShort( 0xfeba ),	/* Offset= -326 (404) */
/* 732 */	NdrFcLong( 0x3 ),	/* 3 */
/* 736 */	NdrFcShort( 0x1e ),	/* Offset= 30 (766) */
/* 738 */	NdrFcLong( 0x4 ),	/* 4 */
/* 742 */	NdrFcShort( 0xfeae ),	/* Offset= -338 (404) */
/* 744 */	NdrFcShort( 0xffff ),	/* Offset= -1 (743) */
/* 746 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 748 */	NdrFcShort( 0x28 ),	/* 40 */
/* 750 */	NdrFcShort( 0x0 ),	/* 0 */
/* 752 */	NdrFcShort( 0x0 ),	/* Offset= 0 (752) */
/* 754 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 756 */	NdrFcShort( 0xfd64 ),	/* Offset= -668 (88) */
/* 758 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 760 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 762 */	NdrFcShort( 0xfd5e ),	/* Offset= -674 (88) */
/* 764 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 766 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 768 */	NdrFcShort( 0x4 ),	/* 4 */
/* 770 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 772 */	
			0x11, 0x1,	/* FC_RP [all_nodes] */
/* 774 */	NdrFcShort( 0x2 ),	/* Offset= 2 (776) */
/* 776 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0xd,		/* FC_ENUM16 */
/* 778 */	0x26,		/* Corr desc:  parameter, FC_SHORT */
			0x0,		/*  */
/* 780 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 782 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 784 */	NdrFcShort( 0xffbc ),	/* Offset= -68 (716) */
/* 786 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 788 */	NdrFcShort( 0x2 ),	/* Offset= 2 (790) */
/* 790 */	
			0x12, 0x0,	/* FC_UP */
/* 792 */	NdrFcShort( 0x2 ),	/* Offset= 2 (794) */
/* 794 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 796 */	NdrFcShort( 0x18 ),	/* 24 */
/* 798 */	NdrFcShort( 0x0 ),	/* 0 */
/* 800 */	NdrFcShort( 0x8 ),	/* Offset= 8 (808) */
/* 802 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 804 */	0x36,		/* FC_POINTER */
			0x36,		/* FC_POINTER */
/* 806 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 808 */	
			0x12, 0x0,	/* FC_UP */
/* 810 */	NdrFcShort( 0xff1e ),	/* Offset= -226 (584) */
/* 812 */	
			0x12, 0x0,	/* FC_UP */
/* 814 */	NdrFcShort( 0xff1a ),	/* Offset= -230 (584) */
/* 816 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 818 */	NdrFcShort( 0x2 ),	/* Offset= 2 (820) */
/* 820 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 822 */	NdrFcShort( 0x2 ),	/* Offset= 2 (824) */
/* 824 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0xd,		/* FC_ENUM16 */
/* 826 */	0x26,		/* Corr desc:  parameter, FC_SHORT */
			0x0,		/*  */
/* 828 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 830 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 832 */	NdrFcShort( 0x2 ),	/* Offset= 2 (834) */
/* 834 */	NdrFcShort( 0x28 ),	/* 40 */
/* 836 */	NdrFcShort( 0x3003 ),	/* 12291 */
/* 838 */	NdrFcLong( 0x1 ),	/* 1 */
/* 842 */	NdrFcShort( 0x10 ),	/* Offset= 16 (858) */
/* 844 */	NdrFcLong( 0x2 ),	/* 2 */
/* 848 */	NdrFcShort( 0xfe44 ),	/* Offset= -444 (404) */
/* 850 */	NdrFcLong( 0x3 ),	/* 3 */
/* 854 */	NdrFcShort( 0xfe3e ),	/* Offset= -450 (404) */
/* 856 */	NdrFcShort( 0xffff ),	/* Offset= -1 (855) */
/* 858 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 860 */	NdrFcShort( 0x28 ),	/* 40 */
/* 862 */	NdrFcShort( 0x0 ),	/* 0 */
/* 864 */	NdrFcShort( 0x0 ),	/* Offset= 0 (864) */
/* 866 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 868 */	NdrFcShort( 0xfcf4 ),	/* Offset= -780 (88) */
/* 870 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 872 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 874 */	NdrFcShort( 0xfcee ),	/* Offset= -786 (88) */
/* 876 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 878 */	
			0x11, 0x1,	/* FC_RP [all_nodes] */
/* 880 */	NdrFcShort( 0x2 ),	/* Offset= 2 (882) */
/* 882 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0xd,		/* FC_ENUM16 */
/* 884 */	0x26,		/* Corr desc:  parameter, FC_SHORT */
			0x0,		/*  */
/* 886 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 888 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 890 */	NdrFcShort( 0xffc8 ),	/* Offset= -56 (834) */
/* 892 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 894 */	NdrFcShort( 0xfeb6 ),	/* Offset= -330 (564) */
/* 896 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 898 */	NdrFcShort( 0x2 ),	/* Offset= 2 (900) */
/* 900 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 902 */	NdrFcShort( 0x2 ),	/* Offset= 2 (904) */
/* 904 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0xd,		/* FC_ENUM16 */
/* 906 */	0x26,		/* Corr desc:  parameter, FC_SHORT */
			0x0,		/*  */
/* 908 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 910 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 912 */	NdrFcShort( 0x2 ),	/* Offset= 2 (914) */
/* 914 */	NdrFcShort( 0x340 ),	/* 832 */
/* 916 */	NdrFcShort( 0x7017 ),	/* 28695 */
/* 918 */	NdrFcLong( 0x1 ),	/* 1 */
/* 922 */	NdrFcShort( 0x88 ),	/* Offset= 136 (1058) */
/* 924 */	NdrFcLong( 0x2 ),	/* 2 */
/* 928 */	NdrFcShort( 0x9e ),	/* Offset= 158 (1086) */
/* 930 */	NdrFcLong( 0x3 ),	/* 3 */
/* 934 */	NdrFcShort( 0xce ),	/* Offset= 206 (1140) */
/* 936 */	NdrFcLong( 0x4 ),	/* 4 */
/* 940 */	NdrFcShort( 0x10a ),	/* Offset= 266 (1206) */
/* 942 */	NdrFcLong( 0x5 ),	/* 5 */
/* 946 */	NdrFcShort( 0x112 ),	/* Offset= 274 (1220) */
/* 948 */	NdrFcLong( 0x6 ),	/* 6 */
/* 952 */	NdrFcShort( 0x14e ),	/* Offset= 334 (1286) */
/* 954 */	NdrFcLong( 0x7 ),	/* 7 */
/* 958 */	NdrFcShort( 0xfdd6 ),	/* Offset= -554 (404) */
/* 960 */	NdrFcLong( 0x8 ),	/* 8 */
/* 964 */	NdrFcShort( 0xfdd0 ),	/* Offset= -560 (404) */
/* 966 */	NdrFcLong( 0x9 ),	/* 9 */
/* 970 */	NdrFcShort( 0xff34 ),	/* Offset= -204 (766) */
/* 972 */	NdrFcLong( 0xa ),	/* 10 */
/* 976 */	NdrFcShort( 0x136 ),	/* Offset= 310 (1286) */
/* 978 */	NdrFcLong( 0xb ),	/* 11 */
/* 982 */	NdrFcShort( 0xfdbe ),	/* Offset= -578 (404) */
/* 984 */	NdrFcLong( 0xc ),	/* 12 */
/* 988 */	NdrFcShort( 0xfdb8 ),	/* Offset= -584 (404) */
/* 990 */	NdrFcLong( 0xd ),	/* 13 */
/* 994 */	NdrFcShort( 0xfdb2 ),	/* Offset= -590 (404) */
/* 996 */	NdrFcLong( 0xe ),	/* 14 */
/* 1000 */	NdrFcShort( 0xfdac ),	/* Offset= -596 (404) */
/* 1002 */	NdrFcLong( 0x10 ),	/* 16 */
/* 1006 */	NdrFcShort( 0xff10 ),	/* Offset= -240 (766) */
/* 1008 */	NdrFcLong( 0x11 ),	/* 17 */
/* 1012 */	NdrFcShort( 0xfd96 ),	/* Offset= -618 (394) */
/* 1014 */	NdrFcLong( 0x12 ),	/* 18 */
/* 1018 */	NdrFcShort( 0x142 ),	/* Offset= 322 (1340) */
/* 1020 */	NdrFcLong( 0x13 ),	/* 19 */
/* 1024 */	NdrFcShort( 0x14c ),	/* Offset= 332 (1356) */
/* 1026 */	NdrFcLong( 0x14 ),	/* 20 */
/* 1030 */	NdrFcShort( 0xfd8e ),	/* Offset= -626 (404) */
/* 1032 */	NdrFcLong( 0x15 ),	/* 21 */
/* 1036 */	NdrFcShort( 0x150 ),	/* Offset= 336 (1372) */
/* 1038 */	NdrFcLong( 0x16 ),	/* 22 */
/* 1042 */	NdrFcShort( 0x1b4 ),	/* Offset= 436 (1478) */
/* 1044 */	NdrFcLong( 0x17 ),	/* 23 */
/* 1048 */	NdrFcShort( 0x1d0 ),	/* Offset= 464 (1512) */
/* 1050 */	NdrFcLong( 0x18 ),	/* 24 */
/* 1054 */	NdrFcShort( 0x1dc ),	/* Offset= 476 (1530) */
/* 1056 */	NdrFcShort( 0xffff ),	/* Offset= -1 (1055) */
/* 1058 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1060 */	NdrFcShort( 0x48 ),	/* 72 */
/* 1062 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1064 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1064) */
/* 1066 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1068 */	NdrFcShort( 0xfc2c ),	/* Offset= -980 (88) */
/* 1070 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1072 */	NdrFcShort( 0xfc28 ),	/* Offset= -984 (88) */
/* 1074 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1076 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1078 */	NdrFcShort( 0xfc22 ),	/* Offset= -990 (88) */
/* 1080 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1082 */	NdrFcShort( 0xfc1e ),	/* Offset= -994 (88) */
/* 1084 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1086 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1088 */	NdrFcShort( 0x28 ),	/* 40 */
/* 1090 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1092 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1092) */
/* 1094 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1096 */	NdrFcShort( 0xfc10 ),	/* Offset= -1008 (88) */
/* 1098 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1100 */	NdrFcShort( 0xfc0c ),	/* Offset= -1012 (88) */
/* 1102 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1104 */	0x40,		/* FC_STRUCTPAD4 */
			0x5b,		/* FC_END */
/* 1106 */	
			0x1c,		/* FC_CVARRAY */
			0x0,		/* 0 */
/* 1108 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1110 */	0x40,		/* Corr desc:  constant, val=1260 */
			0x0,		/* 0 */
/* 1112 */	NdrFcShort( 0x4ec ),	/* 1260 */
/* 1114 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1116 */	0x10,		/* Corr desc:  field pointer,  */
			0x59,		/* FC_CALLBACK */
/* 1118 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1120 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1122 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 1124 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1126 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1128 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1130 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1136) */
/* 1132 */	0x6,		/* FC_SHORT */
			0x42,		/* FC_STRUCTPAD6 */
/* 1134 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1136 */	
			0x12, 0x0,	/* FC_UP */
/* 1138 */	NdrFcShort( 0xffe0 ),	/* Offset= -32 (1106) */
/* 1140 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1142 */	NdrFcShort( 0xb8 ),	/* 184 */
/* 1144 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1146 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1146) */
/* 1148 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1150 */	NdrFcShort( 0xfbda ),	/* Offset= -1062 (88) */
/* 1152 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1154 */	NdrFcShort( 0xfbd6 ),	/* Offset= -1066 (88) */
/* 1156 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1158 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1160 */	NdrFcShort( 0xfbd0 ),	/* Offset= -1072 (88) */
/* 1162 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1164 */	NdrFcShort( 0xfbcc ),	/* Offset= -1076 (88) */
/* 1166 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1168 */	NdrFcShort( 0xfbc8 ),	/* Offset= -1080 (88) */
/* 1170 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1172 */	NdrFcShort( 0xfbc4 ),	/* Offset= -1084 (88) */
/* 1174 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1176 */	NdrFcShort( 0xfbc0 ),	/* Offset= -1088 (88) */
/* 1178 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1180 */	NdrFcShort( 0xfcb2 ),	/* Offset= -846 (334) */
/* 1182 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1184 */	NdrFcShort( 0xfcae ),	/* Offset= -850 (334) */
/* 1186 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1188 */	NdrFcShort( 0xfcaa ),	/* Offset= -854 (334) */
/* 1190 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1192 */	NdrFcShort( 0xfca6 ),	/* Offset= -858 (334) */
/* 1194 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1196 */	NdrFcShort( 0xfca2 ),	/* Offset= -862 (334) */
/* 1198 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1200 */	NdrFcShort( 0xffb4 ),	/* Offset= -76 (1124) */
/* 1202 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1204 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1206 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1208 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1210 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1212 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1212) */
/* 1214 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1216 */	NdrFcShort( 0xffa4 ),	/* Offset= -92 (1124) */
/* 1218 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1220 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1222 */	NdrFcShort( 0xc0 ),	/* 192 */
/* 1224 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1226 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1226) */
/* 1228 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1230 */	NdrFcShort( 0xfb8a ),	/* Offset= -1142 (88) */
/* 1232 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1234 */	NdrFcShort( 0xfb86 ),	/* Offset= -1146 (88) */
/* 1236 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1238 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1240 */	NdrFcShort( 0xfb80 ),	/* Offset= -1152 (88) */
/* 1242 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1244 */	NdrFcShort( 0xfb7c ),	/* Offset= -1156 (88) */
/* 1246 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1248 */	NdrFcShort( 0xfb78 ),	/* Offset= -1160 (88) */
/* 1250 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1252 */	NdrFcShort( 0xfb74 ),	/* Offset= -1164 (88) */
/* 1254 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1256 */	NdrFcShort( 0xfb70 ),	/* Offset= -1168 (88) */
/* 1258 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1260 */	NdrFcShort( 0xfb6c ),	/* Offset= -1172 (88) */
/* 1262 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1264 */	NdrFcShort( 0xfc5e ),	/* Offset= -930 (334) */
/* 1266 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1268 */	NdrFcShort( 0xfc5a ),	/* Offset= -934 (334) */
/* 1270 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1272 */	NdrFcShort( 0xff6c ),	/* Offset= -148 (1124) */
/* 1274 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1276 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1278 */	NdrFcShort( 0xfc50 ),	/* Offset= -944 (334) */
/* 1280 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1282 */	NdrFcShort( 0xfc4c ),	/* Offset= -948 (334) */
/* 1284 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1286 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1288 */	NdrFcShort( 0x20 ),	/* 32 */
/* 1290 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1292 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1292) */
/* 1294 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1296 */	NdrFcShort( 0xfb48 ),	/* Offset= -1208 (88) */
/* 1298 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1300 */	NdrFcShort( 0xfb44 ),	/* Offset= -1212 (88) */
/* 1302 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1304 */	
			0x1d,		/* FC_SMFARRAY */
			0x0,		/* 0 */
/* 1306 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1308 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 1310 */	
			0x15,		/* FC_STRUCT */
			0x0,		/* 0 */
/* 1312 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1314 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1316 */	NdrFcShort( 0xfff4 ),	/* Offset= -12 (1304) */
/* 1318 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1320 */	
			0x1d,		/* FC_SMFARRAY */
			0x0,		/* 0 */
/* 1322 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1324 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1326 */	NdrFcShort( 0xfff0 ),	/* Offset= -16 (1310) */
/* 1328 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1330 */	
			0x15,		/* FC_STRUCT */
			0x0,		/* 0 */
/* 1332 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1334 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1336 */	NdrFcShort( 0xfff0 ),	/* Offset= -16 (1320) */
/* 1338 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1340 */	
			0x15,		/* FC_STRUCT */
			0x0,		/* 0 */
/* 1342 */	NdrFcShort( 0x23 ),	/* 35 */
/* 1344 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1346 */	NdrFcShort( 0xfff0 ),	/* Offset= -16 (1330) */
/* 1348 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1350 */	NdrFcShort( 0xffec ),	/* Offset= -20 (1330) */
/* 1352 */	0x2,		/* FC_CHAR */
			0x2,		/* FC_CHAR */
/* 1354 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 1356 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 1358 */	NdrFcShort( 0x18 ),	/* 24 */
/* 1360 */	0x8,		/* FC_LONG */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 1362 */	0x0,		/* 0 */
			NdrFcShort( 0xfbfb ),	/* Offset= -1029 (334) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 1366 */	0x0,		/* 0 */
			NdrFcShort( 0xfbf7 ),	/* Offset= -1033 (334) */
			0x6,		/* FC_SHORT */
/* 1370 */	0x6,		/* FC_SHORT */
			0x5b,		/* FC_END */
/* 1372 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1374 */	NdrFcShort( 0x13c ),	/* 316 */
/* 1376 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1378 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1378) */
/* 1380 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1382 */	NdrFcShort( 0xfbe8 ),	/* Offset= -1048 (334) */
/* 1384 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1386 */	NdrFcShort( 0xfbe4 ),	/* Offset= -1052 (334) */
/* 1388 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1390 */	NdrFcShort( 0xfbe0 ),	/* Offset= -1056 (334) */
/* 1392 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1394 */	NdrFcShort( 0xfbdc ),	/* Offset= -1060 (334) */
/* 1396 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1398 */	NdrFcShort( 0xfbd8 ),	/* Offset= -1064 (334) */
/* 1400 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1402 */	NdrFcShort( 0xfbd4 ),	/* Offset= -1068 (334) */
/* 1404 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1406 */	NdrFcShort( 0xfada ),	/* Offset= -1318 (88) */
/* 1408 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1410 */	NdrFcShort( 0xfad6 ),	/* Offset= -1322 (88) */
/* 1412 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1414 */	NdrFcShort( 0xfad2 ),	/* Offset= -1326 (88) */
/* 1416 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1418 */	NdrFcShort( 0xface ),	/* Offset= -1330 (88) */
/* 1420 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1422 */	NdrFcShort( 0xfaca ),	/* Offset= -1334 (88) */
/* 1424 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1426 */	NdrFcShort( 0xfac6 ),	/* Offset= -1338 (88) */
/* 1428 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1430 */	NdrFcShort( 0xfac2 ),	/* Offset= -1342 (88) */
/* 1432 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1434 */	NdrFcShort( 0xfabe ),	/* Offset= -1346 (88) */
/* 1436 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1438 */	NdrFcShort( 0xfaba ),	/* Offset= -1350 (88) */
/* 1440 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1442 */	NdrFcShort( 0xfab6 ),	/* Offset= -1354 (88) */
/* 1444 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1446 */	NdrFcShort( 0xfab2 ),	/* Offset= -1358 (88) */
/* 1448 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1450 */	NdrFcShort( 0xfaae ),	/* Offset= -1362 (88) */
/* 1452 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1454 */	NdrFcShort( 0xfaaa ),	/* Offset= -1366 (88) */
/* 1456 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1458 */	NdrFcShort( 0xfa78 ),	/* Offset= -1416 (42) */
/* 1460 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1462 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1464 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1466 */	NdrFcShort( 0xfeaa ),	/* Offset= -342 (1124) */
/* 1468 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1470 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1472 */	0x2,		/* FC_CHAR */
			0x2,		/* FC_CHAR */
/* 1474 */	0x2,		/* FC_CHAR */
			0x2,		/* FC_CHAR */
/* 1476 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1478 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x7,		/* 7 */
/* 1480 */	NdrFcShort( 0x144 ),	/* 324 */
/* 1482 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1484 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1484) */
/* 1486 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1488 */	NdrFcShort( 0xff8c ),	/* Offset= -116 (1372) */
/* 1490 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1492 */	NdrFcShort( 0xfbe6 ),	/* Offset= -1050 (442) */
/* 1494 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1496 */	
			0x1d,		/* FC_SMFARRAY */
			0x0,		/* 0 */
/* 1498 */	NdrFcShort( 0x204 ),	/* 516 */
/* 1500 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 1502 */	
			0x15,		/* FC_STRUCT */
			0x0,		/* 0 */
/* 1504 */	NdrFcShort( 0x204 ),	/* 516 */
/* 1506 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1508 */	NdrFcShort( 0xfff4 ),	/* Offset= -12 (1496) */
/* 1510 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1512 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1514 */	NdrFcShort( 0x340 ),	/* 832 */
/* 1516 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1518 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1518) */
/* 1520 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1522 */	NdrFcShort( 0xff6a ),	/* Offset= -150 (1372) */
/* 1524 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1526 */	NdrFcShort( 0xffe8 ),	/* Offset= -24 (1502) */
/* 1528 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1530 */	
			0x15,		/* FC_STRUCT */
			0x0,		/* 0 */
/* 1532 */	NdrFcShort( 0x205 ),	/* 517 */
/* 1534 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1536 */	NdrFcShort( 0xffde ),	/* Offset= -34 (1502) */
/* 1538 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 1540 */	
			0x11, 0x1,	/* FC_RP [all_nodes] */
/* 1542 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1544) */
/* 1544 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0xd,		/* FC_ENUM16 */
/* 1546 */	0x26,		/* Corr desc:  parameter, FC_SHORT */
			0x0,		/*  */
/* 1548 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1550 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1552 */	NdrFcShort( 0xfd82 ),	/* Offset= -638 (914) */
/* 1554 */	
			0x12, 0x0,	/* FC_UP */
/* 1556 */	NdrFcShort( 0xff1e ),	/* Offset= -226 (1330) */
/* 1558 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 1560 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1562) */
/* 1562 */	
			0x12, 0x0,	/* FC_UP */
/* 1564 */	NdrFcShort( 0x12 ),	/* Offset= 18 (1582) */
/* 1566 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 1568 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1570 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1572 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1574 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1576 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1578 */	NdrFcShort( 0xfb24 ),	/* Offset= -1244 (334) */
/* 1580 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1582 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1584 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1586 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1588 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1594) */
/* 1590 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1592 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1594 */	
			0x12, 0x0,	/* FC_UP */
/* 1596 */	NdrFcShort( 0xffe2 ),	/* Offset= -30 (1566) */
/* 1598 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 1600 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1602) */
/* 1602 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0xd,		/* FC_ENUM16 */
/* 1604 */	0x26,		/* Corr desc:  parameter, FC_SHORT */
			0x0,		/*  */
/* 1606 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1608 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1610 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1612) */
/* 1612 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1614 */	NdrFcShort( 0x3005 ),	/* 12293 */
/* 1616 */	NdrFcLong( 0x1 ),	/* 1 */
/* 1620 */	NdrFcShort( 0x4c ),	/* Offset= 76 (1696) */
/* 1622 */	NdrFcLong( 0x2 ),	/* 2 */
/* 1626 */	NdrFcShort( 0x82 ),	/* Offset= 130 (1756) */
/* 1628 */	NdrFcLong( 0x3 ),	/* 3 */
/* 1632 */	NdrFcShort( 0x8c ),	/* Offset= 140 (1772) */
/* 1634 */	NdrFcLong( 0x4 ),	/* 4 */
/* 1638 */	NdrFcShort( 0xe0 ),	/* Offset= 224 (1862) */
/* 1640 */	NdrFcLong( 0x5 ),	/* 5 */
/* 1644 */	NdrFcShort( 0xea ),	/* Offset= 234 (1878) */
/* 1646 */	NdrFcShort( 0xffff ),	/* Offset= -1 (1645) */
/* 1648 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1650 */	NdrFcShort( 0x40 ),	/* 64 */
/* 1652 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1654 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1654) */
/* 1656 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1658 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1660 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1662 */	NdrFcShort( 0xf9da ),	/* Offset= -1574 (88) */
/* 1664 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1666 */	NdrFcShort( 0xf9d6 ),	/* Offset= -1578 (88) */
/* 1668 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1670 */	NdrFcShort( 0xf9d2 ),	/* Offset= -1582 (88) */
/* 1672 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1674 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 1676 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1678 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1680 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1682 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1684 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 1688 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1690 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1692 */	NdrFcShort( 0xffd4 ),	/* Offset= -44 (1648) */
/* 1694 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1696 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1698 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1700 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1702 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1708) */
/* 1704 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1706 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1708 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 1710 */	NdrFcShort( 0xffdc ),	/* Offset= -36 (1674) */
/* 1712 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1714 */	NdrFcShort( 0x30 ),	/* 48 */
/* 1716 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1718 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1718) */
/* 1720 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1722 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1724 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1726 */	NdrFcShort( 0xf99a ),	/* Offset= -1638 (88) */
/* 1728 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1730 */	NdrFcShort( 0xf996 ),	/* Offset= -1642 (88) */
/* 1732 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1734 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 1736 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1738 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1740 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1742 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1744 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 1748 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1750 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1752 */	NdrFcShort( 0xffd8 ),	/* Offset= -40 (1712) */
/* 1754 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1756 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1758 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1760 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1762 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1768) */
/* 1764 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1766 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1768 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 1770 */	NdrFcShort( 0xffdc ),	/* Offset= -36 (1734) */
/* 1772 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1774 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1776 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1778 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1784) */
/* 1780 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1782 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1784 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 1786 */	NdrFcShort( 0xffcc ),	/* Offset= -52 (1734) */
/* 1788 */	
			0x1c,		/* FC_CVARRAY */
			0x0,		/* 0 */
/* 1790 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1792 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x0,		/*  */
/* 1794 */	NdrFcShort( 0x2 ),	/* 2 */
/* 1796 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1798 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x0,		/*  */
/* 1800 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1802 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1804 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 1806 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1808 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1810 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1812 */	NdrFcShort( 0x8 ),	/* Offset= 8 (1820) */
/* 1814 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1816 */	0x40,		/* FC_STRUCTPAD4 */
			0x36,		/* FC_POINTER */
/* 1818 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1820 */	
			0x12, 0x0,	/* FC_UP */
/* 1822 */	NdrFcShort( 0xffde ),	/* Offset= -34 (1788) */
/* 1824 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1826 */	NdrFcShort( 0x18 ),	/* 24 */
/* 1828 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1830 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1830) */
/* 1832 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1834 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1836 */	NdrFcShort( 0xffe2 ),	/* Offset= -30 (1806) */
/* 1838 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1840 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 1842 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1844 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1846 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1848 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1850 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 1854 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1856 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1858 */	NdrFcShort( 0xffde ),	/* Offset= -34 (1824) */
/* 1860 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1862 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1864 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1866 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1868 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1874) */
/* 1870 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1872 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1874 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 1876 */	NdrFcShort( 0xffdc ),	/* Offset= -36 (1840) */
/* 1878 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1880 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1882 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1884 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1890) */
/* 1886 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1888 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1890 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 1892 */	NdrFcShort( 0xffcc ),	/* Offset= -52 (1840) */
/* 1894 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 1896 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1898) */
/* 1898 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 1900 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1902 */	0x6,		/* FC_SHORT */
			0x3e,		/* FC_STRUCTPAD2 */
/* 1904 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1906 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 1908 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1910) */
/* 1910 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 1912 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1914) */
/* 1914 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0xd,		/* FC_ENUM16 */
/* 1916 */	0x26,		/* Corr desc:  parameter, FC_SHORT */
			0x0,		/*  */
/* 1918 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1920 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1922 */	NdrFcShort( 0xf97e ),	/* Offset= -1666 (256) */
/* 1924 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 1926 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1928) */
/* 1928 */	
			0x12, 0x1,	/* FC_UP [all_nodes] */
/* 1930 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1932) */
/* 1932 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0xd,		/* FC_ENUM16 */
/* 1934 */	0x26,		/* Corr desc:  parameter, FC_SHORT */
			0x0,		/*  */
/* 1936 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1938 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1940 */	NdrFcShort( 0xfbfe ),	/* Offset= -1026 (914) */
/* 1942 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 1944 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1946) */
/* 1946 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0xd,		/* FC_ENUM16 */
/* 1948 */	0x26,		/* Corr desc:  parameter, FC_SHORT */
			0x0,		/*  */
/* 1950 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1952 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1954 */	NdrFcShort( 0xfeaa ),	/* Offset= -342 (1612) */
/* 1956 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 1958 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1960) */
/* 1960 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0xd,		/* FC_ENUM16 */
/* 1962 */	0x26,		/* Corr desc:  parameter, FC_SHORT */
			0x0,		/*  */
/* 1964 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1966 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1968 */	NdrFcShort( 0xfe9c ),	/* Offset= -356 (1612) */
/* 1970 */	
			0x12, 0x0,	/* FC_UP */
/* 1972 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1974) */
/* 1974 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1976 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1978 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1980 */	NdrFcShort( 0x8 ),	/* Offset= 8 (1988) */
/* 1982 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1984 */	0x40,		/* FC_STRUCTPAD4 */
			0x36,		/* FC_POINTER */
/* 1986 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1988 */	
			0x12, 0x0,	/* FC_UP */
/* 1990 */	NdrFcShort( 0xff36 ),	/* Offset= -202 (1788) */
/* 1992 */	
			0x11, 0x0,	/* FC_RP */
/* 1994 */	NdrFcShort( 0xffec ),	/* Offset= -20 (1974) */
/* 1996 */	
			0x12, 0x0,	/* FC_UP */
/* 1998 */	NdrFcShort( 0xfe10 ),	/* Offset= -496 (1502) */
/* 2000 */	
			0x12, 0x0,	/* FC_UP */
/* 2002 */	NdrFcShort( 0xf886 ),	/* Offset= -1914 (88) */
/* 2004 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 2006 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 2008 */	
			0x11, 0x1,	/* FC_RP [all_nodes] */
/* 2010 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2012) */
/* 2012 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0xd,		/* FC_ENUM16 */
/* 2014 */	0x26,		/* Corr desc:  parameter, FC_SHORT */
			0x0,		/*  */
/* 2016 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2018 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 2020 */	NdrFcShort( 0xfbae ),	/* Offset= -1106 (914) */

			0x0
        }
    };

static const GENERIC_BINDING_ROUTINE_PAIR BindingRoutines[ GENERIC_BINDING_TABLE_SIZE ] = 
        {
        {
            (GENERIC_BINDING_ROUTINE)PSAMPR_SERVER_NAME_bind,
            (GENERIC_UNBIND_ROUTINE)PSAMPR_SERVER_NAME_unbind
         }
        
        };


static void __RPC_USER samr_SAMPR_LOGON_HOURSExprEval_0000( PMIDL_STUB_MESSAGE pStubMsg )
{
    SAMPR_LOGON_HOURS *pS	=	( SAMPR_LOGON_HOURS * )pStubMsg->StackTop;
    
    pStubMsg->Offset = 0;
    pStubMsg->MaxCount = (ULONG_PTR) ( (pS->UnitsPerWeek + 7) / 8 );
}

static const EXPR_EVAL ExprEvalRoutines[] = 
    {
    samr_SAMPR_LOGON_HOURSExprEval_0000
    };


static const unsigned short samr_FormatStringOffsetTable[] =
    {
    0,
    56,
    100,
    156,
    212,
    256,
    312,
    380,
    442,
    498,
    554,
    622,
    690,
    758,
    832,
    900,
    968,
    1024,
    1092,
    1160,
    1222,
    1278,
    1334,
    1390,
    1434,
    1484,
    1534,
    1590,
    1652,
    1708,
    1764,
    1808,
    1858,
    1908,
    1958,
    2020,
    2064,
    2120,
    2176,
    2280,
    2330,
    2416,
    2478,
    2522,
    2566,
    2616,
    2666,
    2722,
    2778,
    2864,
    2926,
    3006,
    3092,
    3142,
    3192,
    3252,
    3330,
    3378,
    3434
    };


static const MIDL_STUB_DESC samr_StubDesc = 
    {
    (void *)& samr___RpcClientInterface,
    MIDL_user_allocate,
    MIDL_user_free,
    &samcli_handle,
    0,
    BindingRoutines,
    ExprEvalRoutines,
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

