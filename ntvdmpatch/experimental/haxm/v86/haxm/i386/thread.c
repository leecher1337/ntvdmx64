/*++

Copyright (c) 1992  Microsoft Corporation

Module Name:

    Thread.c

Abstract:

    This file contains functions for tracking and manipulating threads

Author:

    Dave Hastings (daveh) 18-Apr-1992

Revision History:

--*/

#include <monitorp.h>
#include <malloc.h>

//
// Local Types
//

typedef struct _MonitorThread {
    struct _MonitorThread *Previous;
    struct _MonitorThread *Next;
    PVOID Teb;
    HANDLE Thread;
} MONITORTHREAD, *PMONITORTHREAD;

//
// Local Variables
//

PMONITORTHREAD ThreadList = NULL;          // List of all threads registered

VOID
cpu_createthread(
    HANDLE Thread
    )
/*++

Routine Description:

    This routine adds a thread to the list of threads that could be executing
    in application mode.

Arguments:

    Thread -- Supplies a thread handle

Return Value:

    None.

--*/
{
    PMONITORTHREAD NewThread, CurrentThread;
    THREAD_BASIC_INFORMATION ThreadInfo;
    HANDLE MonitorThreadHandle;
    NTSTATUS Status;

    //
    // Set up a structure to keep track of the new thread
    //
    NewThread = malloc(sizeof(MONITORTHREAD));

    if (!NewThread) {
#if DBG
        DbgPrint("NTVDM: Could not allocate space for new thread\n");
        DbgBreakPoint();
#endif
        TerminateVDM();
    }

    //
    // Create a handle for the monitor to use
    //

    Status = NtDuplicateObject(
        NtCurrentProcess(),
        Thread,
        NtCurrentProcess(),
        &MonitorThreadHandle,
        0,
        0,
        DUPLICATE_SAME_ACCESS
        );

    if (!NT_SUCCESS(Status)) {
#if DBG
        DbgPrint("NTVDM: Could not duplicate thread handle\n");
        DbgBreakPoint();
#endif
        TerminateVDM();
    }

    NewThread->Thread = MonitorThreadHandle;

    Status = NtQueryInformationThread(
        MonitorThreadHandle,
        ThreadBasicInformation,
        &ThreadInfo,
        sizeof(THREAD_BASIC_INFORMATION),
        NULL
        );

    if (!NT_SUCCESS(Status)) {
#if DBG
        DbgPrint("NTVDM: Could not get thread information\n");
        DbgBreakPoint();
#endif
        TerminateVDM();
    }

    NewThread->Teb = ThreadInfo.TebBaseAddress;
    ((PTEB)(NewThread->Teb))->Vdm = &VdmTib;

    //
    // Insert the new thread in the list.  The list is sorted in ascending
    // order of Teb address
    //
    if (!ThreadList) {
        ThreadList = NewThread;
        NewThread->Next = NULL;
        NewThread->Previous = NULL;
        return;
    }

    CurrentThread = ThreadList;
    while ((CurrentThread->Next) && (CurrentThread->Teb < NewThread->Teb)) {
        CurrentThread = CurrentThread->Next;
    }

    if (NewThread->Teb > CurrentThread->Teb) {
        CurrentThread->Next = NewThread;
        NewThread->Previous = CurrentThread;
        NewThread->Next = NULL;
    } else {
        ASSERT((CurrentThread->Teb != NewThread->Teb));
        NewThread->Previous = CurrentThread->Previous;
        NewThread->Next = CurrentThread;
        CurrentThread->Previous = NewThread;
        if (NewThread->Previous) {
            NewThread->Previous->Next = NewThread;
        } else {
            ThreadList = NewThread;
        }
    }
}

VOID
cpu_exitthread(
    VOID
    )
/*++

Routine Description:

    This routine frees the thread tracking information, and closes the thread
    handle

Arguments:


Return Value:

    None.

--*/
{
    PVOID CurrentTeb;
    NTSTATUS Status;
    PMONITORTHREAD ThreadInfo;

    CurrentTeb = NtCurrentTeb();

    ThreadInfo = ThreadList;

    //
    // Find this thread in the list
    //
    while ((ThreadInfo) && (ThreadInfo->Teb != CurrentTeb)) {
        ThreadInfo = ThreadInfo->Next;
    }

    if (!ThreadInfo) {
#if DBG
        DbgPrint("NTVDM: Could not find thread in list\n");
        DbgBreakPoint();
#endif
        return;
    }

    //
    // Close our handle to this thread
    //
    Status = NtClose(ThreadInfo->Thread);
#if DBG
    if (!NT_SUCCESS(Status)) {
        DbgPrint("NTVDM: Could not close thread handle\n");
    }
#endif

    //
    // Remove this thread from the list
    //
    if (ThreadInfo->Previous) {
        ThreadInfo->Previous->Next = ThreadInfo->Next;
    } else {
        ThreadList = ThreadInfo->Next;
    }

    if (ThreadInfo->Next) {
        ThreadInfo->Next->Previous = ThreadInfo->Previous;
    }

    free(ThreadInfo);
}

HANDLE
ThreadLookUp(
    PVOID Teb
    )
/*++

Routine Description:

    This routine returns the handle for the specified thread.

Arguments:

    Teb -- Supplies the teb pointer of the thread

Return Value:

    Returns the handle of the thread, or NULL

--*/
{
    PMONITORTHREAD Thread;

    Thread = ThreadList;

    while ((Thread) && (Thread->Teb != Teb)) {
        Thread = Thread->Next;
    }

    if (Thread) {
        return Thread->Thread;
    } else {
        return NULL;
    }
}

#ifdef MONITOR
BOOL
ThreadSetDebugContext(
    PULONG pDebugRegisters
    )
/*++

Routine Description:

    This routine sets the debug registers for all the threads that the
    monitor knows about.

Arguments:

    pDebugRegisters -- Pointer to 6 dwords containing the requested debug
                       register contents.

Return Value:

    none

--*/
{
    PMONITORTHREAD Thread;
    NTSTATUS Status;

    Thread = ThreadList;
    InitialContext.ContextFlags = CONTEXT_DEBUG_REGISTERS;

    InitialContext.Dr0 = *pDebugRegisters++;
    InitialContext.Dr1 = *pDebugRegisters++;
    InitialContext.Dr2 = *pDebugRegisters++;
    InitialContext.Dr3 = *pDebugRegisters++;
    InitialContext.Dr6 = *pDebugRegisters++;
    InitialContext.Dr7 = *pDebugRegisters++;

    while (Thread) {

        Status = NtSetContextThread(
            Thread->Thread,
            &InitialContext
            );

        if (!NT_SUCCESS(Status))
            break;

        Thread = Thread->Next;
    }

    if (!NT_SUCCESS(Status))
        return (FALSE);
    else {
        DebugContextActive = ((InitialContext.Dr7 & 0x0f) != 0);
        return (TRUE);
    }

}

#endif
