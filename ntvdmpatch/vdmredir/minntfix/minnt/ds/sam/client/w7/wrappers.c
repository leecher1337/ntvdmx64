/*++

Copyright (c) 1991  Microsoft Corporation

Module Name:

    wrappers.c

Abstract:

    This file contains all SAM rpc wrapper routines.

Author:

    Jim Kelly    (JimK)  4-July-1991

Environment:

    User Mode - Win32

Revision History:


--*/

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// Includes                                                                  //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "samclip.h"

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// Routines                                                                  //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////
 

NTSTATUS
SampMapCompletionStatus(
    IN NTSTATUS Status
    )

/*++

Routine Description:

    This service maps completion status received back from an RPC call
    into a completion status to be returned from SAM api.


Parameters:

    Status - Status value to be mapped.

Return Values:

    The mapped SAM status value.


--*/
{

    if (Status == RPC_NT_INVALID_BINDING) {
        Status =  STATUS_INVALID_HANDLE;
    }
//    if (Status == RPC_ACCESS_DENIED) {
//        Status = STATUS_ACCESS_DENIED;
//    }



    return( Status );

}

static NTSTATUS
SamiOemChangePasswordUser2_(
    PSTRING ServerName,
    PSTRING UserName,
    PSAMPR_ENCRYPTED_USER_PASSWORD NewPasswordEncryptedWithOldLm,
    PENCRYPTED_LM_OWF_PASSWORD OldLmOwfPasswordEncryptedWithNewLm
    )
/*++


Routine Description:

    Changes the password of a user account. This  can be called by OWF-aware
    clients. Password will be set to NewPassword only if OldPassword matches
    the current user password for this user and the NewPassword is not the
    same as the domain password parameter PasswordHistoryLength
    passwords.  This call allows users to change their own password if
    they have access USER_CHANGE_PASSWORD.  Password update restrictions
    apply.


Parameters:

    ServerName - The server to operate on, or NULL for this machine.

    UserName - Name of user whose password is to be changed


    NewPasswordEncryptedWithOldLm - The new cleartext password encrypted
        with the old LM OWF password.

    OldLmOwfPasswordEncryptedWithNewLm - The old LM OWF password encrypted
        with the new LM OWF password.

Return Values:

    STATUS_SUCCESS - The Service completed successfully.

    STATUS_ACCESS_DENIED - Caller does not have the appropriate
        access to complete the operation.

    STATUS_INVALID_HANDLE - The handle passed is invalid.

    STATUS_ILL_FORMED_PASSWORD - The new password is poorly formed,
        e.g. contains characters that can't be entered from the
        keyboard, etc.

    STATUS_PASSWORD_RESTRICTION - A restriction prevents the password
        from being changed.  This may be for a number of reasons,
        including time restrictions on how often a password may be
        changed or length restrictions on the provided password.

        This error might also be returned if the new password matched
        a password in the recent history log for the account.
        Security administrators indicate how many of the most
        recently used passwords may not be re-used.  These are kept
        in the password recent history log.

    STATUS_WRONG_PASSWORD - OldPassword does not contain the user's
        current password.

    STATUS_INVALID_DOMAIN_STATE - The domain server is not in the
        correct state (disabled or enabled) to perform the requested
        operation.  The domain server must be enabled for this
        operation

    STATUS_INVALID_DOMAIN_ROLE - The domain server is serving the
        incorrect role (primary or backup) to perform the requested
        operation.

--*/

{
    handle_t BindingHandle;
    UNICODE_STRING RemoteServerName;
    ULONG Tries = 2;
    NTSTATUS NtStatus;
    USER_DOMAIN_PASSWORD_INFORMATION PasswordInformation;

    RemoteServerName.Buffer = NULL;
    RemoteServerName.Length = 0;

    if (ARGUMENT_PRESENT(ServerName)) {

        NtStatus = RtlAnsiStringToUnicodeString(
                        &RemoteServerName,
                        ServerName,
                        TRUE            // allocate destination
                        );

        if (!NT_SUCCESS(NtStatus)) {
            return(NtStatus);
        }
        ASSERT(RemoteServerName.Buffer[RemoteServerName.Length/sizeof(WCHAR)] == L'\0');
    }


    do
    {
        //
        // Try privacy level first, and if that failed with unknown authn
        // level or invalid binding try with a lower level (none).
        //

        if (Tries == 2) {
            BindingHandle = SampSecureBind(
                                RemoteServerName.Buffer,
                                RPC_C_AUTHN_LEVEL_PKT_PRIVACY
                                );


        } else if ((NtStatus == RPC_NT_UNKNOWN_AUTHN_LEVEL) ||
                   (NtStatus == RPC_NT_UNKNOWN_AUTHN_TYPE) ||
                   (NtStatus == RPC_NT_INVALID_BINDING) ||
                   (NtStatus == STATUS_ACCESS_DENIED) ) {
            SampSecureUnbind(BindingHandle);

            BindingHandle = SampSecureBind(
                                RemoteServerName.Buffer,
                                RPC_C_AUTHN_LEVEL_NONE
                                );

        } else {
            break;
        }

        if (BindingHandle != NULL) {

            RpcTryExcept{

                //
                // Get password information to make sure this operation
                // is allowed.  We do it now because we wanted to bind
                // before trying it.
                //

                NtStatus = SamrGetDomainPasswordInformation(
                               BindingHandle,
                               (PRPC_UNICODE_STRING) ServerName,
                               &PasswordInformation
                               );

                if (NtStatus == STATUS_SUCCESS) {

                    if (!( PasswordInformation.PasswordProperties &
                         DOMAIN_PASSWORD_NO_CLEAR_CHANGE) ) {

                        NtStatus = SamrOemChangePasswordUser2(
                                       BindingHandle,
                                       (PRPC_STRING) ServerName,
                                       (PRPC_STRING) UserName,
                                       NewPasswordEncryptedWithOldLm,
                                       OldLmOwfPasswordEncryptedWithNewLm
                                       );

                    } else {

                        //
                        // Set the error to indicate that we should try the
                        // downlevel way to change passwords.
                        //

                        NtStatus = STATUS_NOT_SUPPORTED;
                    }
                }



            } RpcExcept( EXCEPTION_EXECUTE_HANDLER ) {


                //
                // The mappin function doesn't handle this error so
                // special case it by hand.
                //

                if (NtStatus == RPC_S_SEC_PKG_ERROR) {
                    NtStatus = STATUS_ACCESS_DENIED;
                } else {
                    NtStatus = I_RpcMapWin32Status(RpcExceptionCode());
                }


            } RpcEndExcept;

        } else {
            NtStatus = RPC_NT_INVALID_BINDING;
        }

        Tries--;
    } while ( (Tries > 0) && (!NT_SUCCESS(NtStatus)) );

    RtlFreeUnicodeString( &RemoteServerName );

    if (BindingHandle != NULL) {
        SampSecureUnbind(BindingHandle);
    }

    //
    // Map these errors to STATUS_NOT_SUPPORTED
    //

    if ((NtStatus == RPC_NT_UNKNOWN_IF) ||
        (NtStatus == RPC_NT_PROCNUM_OUT_OF_RANGE)) {

        NtStatus = STATUS_NOT_SUPPORTED;
    }

    return(SampMapCompletionStatus(NtStatus));

}


typedef NTSTATUS (WINAPI *fpSamiOemChangePasswordUser2)(
    PSTRING ServerName,
    PSTRING UserName,
    PSAMPR_ENCRYPTED_USER_PASSWORD NewPasswordEncryptedWithOldLm,
    PENCRYPTED_LM_OWF_PASSWORD OldLmOwfPasswordEncryptedWithNewLm
    );

fpSamiOemChangePasswordUser2 pfnSamiOemChangePasswordUser2 = NULL;


NTSTATUS
SamiOemChangePasswordUser2W10(
    PSTRING ServerName,
    PSTRING UserName,
    PSAMPR_ENCRYPTED_USER_PASSWORD NewPasswordEncryptedWithOldLm,
    PENCRYPTED_LM_OWF_PASSWORD OldLmOwfPasswordEncryptedWithNewLm
    )
{
	// On Win 7 it's there, on Win 10, we have to do it ourself.
	// THANK YOU FOR SENSELESSLY RIPPING OUT A CRUCIAL FUNCTION!!
	if (!pfnSamiOemChangePasswordUser2)
	{
		HMODULE hSamLIB = GetModuleHandleW(L"SAMLIB.DLL");

		if (!(hSamLIB && (pfnSamiOemChangePasswordUser2 = (fpSamiOemChangePasswordUser2)GetProcAddress(hSamLIB, "SamiOemChangePasswordUser2"))))
			pfnSamiOemChangePasswordUser2 = SamiOemChangePasswordUser2_;
	}

	return pfnSamiOemChangePasswordUser2(ServerName, UserName, NewPasswordEncryptedWithOldLm, OldLmOwfPasswordEncryptedWithNewLm);
}