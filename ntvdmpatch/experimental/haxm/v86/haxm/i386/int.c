/*++

Copyright (c) 1992  Microsoft Corporation

Module Name:

    int.c

Abstract:

    This file contains interrupt support routines for the monitor

Author:

    Dave Hastings (daveh) 18-Apr-1992

Notes:

    The code in this file split out from monitor.c (18-Apr-1992)

Revision History:

--*/

#include <monitorp.h>

/* Seems like CPU_HW_INT is 2 or 3, depending of CPU-style. We use CPU_40_STYLE
 * and this includes CCPU's cpuint_c.h, which in turn defines it as 2,
 * but the stuff we include here defines it as 3...
 */
#define MY_CPU_HW_INT 2

typedef struct _KIDTENTRY {
   USHORT Offset;
   USHORT Selector;
   USHORT Access;
   USHORT ExtendedOffset;
} KIDTENTRY;

BOOL
DpmiHwIntHandler(
    ULONG IntNumber
    );

VOID
IRQ13_Eoi(
    int IrqLine,
    int CallCount
    );

extern HANDLE hVCPU;
extern host_addr Start_of_M_area;
#undef pNtVDMState
#define pNtVDMState   ((ULONG *)(Start_of_M_area + FIXED_NTVDMSTATE_LINEAR))

#ifdef MINNT
#define DPMIFLAGS VdmTib.DpmiInfo.Flags
#else
#define DPMIFLAGS VdmTib.PmStackInfo.Flags
#endif

BOOLEAN IRQ13BeingHandled;  // true until IRQ13 eoi'ed


VOID
InterruptInit(
    VOID
)
/*++

Routine Description:

    This routine initializes the interrupt code for the monitor.

Arguments:


Return Value:

    None.

--*/
{
    BOOL Bool;



    Bool = RegisterEOIHook( 13, IRQ13_Eoi);
    if (!Bool) {
#if DBG
        DbgPrint("NtVdm : Could not register IRQ 13 Eoi handler\n");
        DbgBreakPoint();
#endif
        TerminateVDM();
    }
}

VOID
InterruptTerminate(
    VOID
    )
/*++

Routine Description:

    This routine frees the resoures allocated by InterruptInit

Arguments:


Return Value:

    None.

--*/
{
}


VOID
cpu_interrupt(
    IN int Type,
    IN int Number	/* Always 0 :( */
    )
/*++

Routine Description:

    This routine causes an interrupt of the specified type to be raised
    at the appropriate time.

Arguments:

    Type -- indicates the type of the interrupt.  One of HARDWARE, TIMER, YODA,
            or RESET

            YODA and RESET are ignored

Return Value:

    None.

Notes:

--*/
{  
	host_ica_lock();

    if (Type == CPU_TIMER_TICK) {

        //
        // Set the VDM State for timer tick int pending
        //
		*pNtVDMState |= VDM_INT_TIMER;
    } else if (Type == MY_CPU_HW_INT) {

        if (*pNtVDMState & VDM_INT_HARDWARE) {
            goto EarlyExit;
            }

        //
        // Set the VDM State for Hardware Int pending
        //
		*pNtVDMState |= VDM_INT_HARDWARE;
    } else {
#if DBG
        DbgPrint("Monitor: Invalid Interrupt Type=%ld\n",Type);
#endif
        goto EarlyExit;
    }


EarlyExit:

    host_ica_unlock();
}




VOID
DispatchInterrupts(
    )
/*++

Routine Description:

    This routine dispatches interrupts to their appropriate handler routine
    in priority order. The order is YODA, RESET, TIMER, HARDWARE. however
    the YODA and RESET interrupts do nothing. Hardware interrupts are not
    simulated unless the virtual interrupt enable flag is set.  Flags
    indicating which interrupts are pending appear in the pNtVDMState.


Arguments:

    None.

Return Value:

    None.

Notes:

--*/
{
	DWORD bytes;

    host_ica_lock();

       // If any delayed interrupts have expired
       // call the ica to restart interrupts
    if (UndelayIrqLine) {
        ica_RestartInterrupts(UndelayIrqLine);
        }


    if (*pNtVDMState & VDM_INT_TIMER) {
        *pNtVDMState &= ~VDM_INT_TIMER;
        host_ica_unlock();      // maybe don't need to unlock ? Jonle
        host_timer_event();
        host_ica_lock();
    }

    if ( getIF() && *pNtVDMState & VDM_INT_HARDWARE) {

		if (getMSW() & MSW_PE)
			DispatchHwInterrupt();
		else {
			ULONG IretHookAddress = 0L;
		    int InterruptNumber = ica_intack(&IretHookAddress);

			if (InterruptNumber != -1)
				DeviceIoControl(hVCPU, HAX_VCPU_IOCTL_INTERRUPT, &InterruptNumber, sizeof(InterruptNumber), NULL, 0, &bytes, NULL);
		}
		*pNtVDMState &= ~VDM_INT_HARDWARE;
    }

    host_ica_unlock();
}

VOID HaxmSetProtectedModeInterrupt(USHORT IntNumber)
{
	ULONG IDTbase = getIDT_BASE();
	KIDTENTRY *pIDT;

	// HACKHACK: Remove and use VMX_EXIT_GDT_IDT_ACCESS in HAXM to get proper address
	if (!IDTbase) IDTbase = getES()<<16;
	pIDT = (KIDTENTRY*)Sim32GetVDMPointer(IDTbase, 0, 
		(UCHAR)(getMSW() & MSW_PE));

printf ("HaxmSetProtectedModeInterrupt(%d) @%08X\n", IntNumber, &pIDT[IntNumber]);
	// Mask off present-Bit to let it fault
	pIDT[IntNumber].Access &= ~(1<<15);
}


VOID
DispatchHwInterrupt(
    )
/*++

Routine Description:

    This routine dispatches hardware interrupts to the vdm in Protect Mode.
    It calls the ICA to get the vector number and sets up the VDM stack
    appropriately. Real Mode interrupt dispatching has been moved to the
    kernel.

Arguments:

    None.

Return Value:

    None.

--*/
{
    int InterruptNumber;
    ULONG IretHookAddress = 0L;

    InterruptNumber = ica_intack(&IretHookAddress);
    if (InterruptNumber == -1) { // skip spurious ints
        return;
        }

	/* host_swint_hook() in CCPU -> Software INT hooks --> VdmInstallSoftwareIntHandler(DpmiSwIntHandler); in DPMI32
	 * Dieser handler ersetzt die normale Interrupbehandlung, wenn ausgeführt!
	 * Hierzu gibt es kein Äquivalent in MONITOR, in Monitor werden dafür IRET hooks verwendet über
	 * den Instruction-emulator, u.A. für INT, POPF, STI, ...
	 * und der DpmiHwIntHandler wofür es wiederum keine Ensprechung in der CCPU gibt.
	 * Hooks werden generell via SetProtectedModeInterrupt BOP (Handler DpmiSetProtectedmodeInterrupt) gesetzt.
	 * Dafür wird durch  host_iret_bop_table_addr() in ica_intack ein hook zurückgegeben.
	 * Diese Funktionen simulieren einen INT, statt einen echten auf der CPU auszuführen. Eventuell sollten wir
	 * das einfach auch so machen. Problem: Im V86 Modus gibt es auch emulierte Instruktionen (PUSHF), welche
	 * unsere INT-Dispatcher aufrufen, das haben wir in HAXm so nicht.
	 *
	 * How should we hook INTs in HAXM?
	 * Idea #1: http://hypervsir.blogspot.com/2014/10/monitortrap-software-interrupt-int-08h.html
	 * Disable present bit of hooked INT in IDT, then we get a #NP fault on INT, we trigger this and call hooker:
	 * BOP SetDescriptorTableEntries (handler DpmiSetDescriptorEntry) --> calls DpmiSetX86Descriptor on Monitor -->
	 * Sets LDT on Monitor. We have to check for already active hooks there and remove bit on their entries
	 * Advantage:		Only hooked INTs will fault back to Hypervisor
	 * Disadvantage:	We have to modify IDT entries so that guest sees that there is no present-bit and may
	 *					screw us up by modifying it on its own.
	 *					Under the assumption that DPMI is used, there shouldn't be any LIDTs, but ARB_DATA0NP
	 *					actually gets used! INT 31h/0B returns descriptor which could confuse caller if it 
	 *					discovers that we killed the Present-bit!
	 *					Hooks don't work in real mode (no present-bit in IVT)
	 *
	 * Idea #2: Do it like WineVDM, setup our own LDT in memory area outside guest memory and let it point
	 * to a previously allocated mem block. As memory is not assigned to guest, we get HAX_EXIT_MMIO fault.
	 * If CS:IP is inside our memory block, calculate INT#, check for hooker, call either hooker or set CPU
	 * to real handler and continue execution.
	 * Advantage:		Hooking of all INTs without LDT entry modification; Gets trapped on every R/W to
	 *					it, so we can give guest the entries that it wants to see, even though in the real
	 *					world, the table just consists of entries that point to our Int-hook.
	 * Disadvantage:	Fires on every INT, whether hooked or not (slow!), IDT cannot be maintained by guest,
	 *					as it has to point to our mem area.
	 *					Hooks don't work in real mode (IVT isn't 1 4k page in size which is limit)
	 * How to implement:DpmiPassTableAddress gets to know Ldt pointer by DPMI. As soon as we get it, we can
	 *					exclude it from assigned memory so that it falls back into being MMIO memory space.
	 *					We pipe through every R/W on MMIO, but must check if MMIO was caused by INT, and if
	 *					so, call hooker, if any. Don't forget to readd old Ldt memory area if LDT changes.
	 *
	 * Fortunately, DPMI hooks just reflect V86 INTs anyway, so it's not a real problem that we don't support
	 * hooking IVT.
	 * Also, the whole IVT gets hooked via the Wow32IntrRefl table anyway, so the advantage of #1 isn't really
	 * that big. 
	 * 
	*/
    DpmiHwIntHandler(InterruptNumber);

    if (IretHookAddress) {
		BOOL Frame32 = (BOOL) DPMIFLAGS;
        BOOL Stack32;
        USHORT SegSs, VdmCs;
        ULONG VdmSp, VdmEip;
        PUCHAR VdmStackPointer;
        ULONG StackOffset;

        SegSs = getSS();
        VdmStackPointer = Sim32GetVDMPointer(((ULONG)SegSs) << 16, 1, TRUE);
    
        //
        // Figure out how many bits of sp to use
        //
    
        if (Ldt[(SegSs & ~0x7)/sizeof(LDT_ENTRY)].HighWord.Bits.Default_Big) {
            VdmSp = getESP();
            StackOffset = 12;
        } else {
            VdmSp = getSP();
            StackOffset = 6;
        }

        (PCHAR)VdmStackPointer += VdmSp;

        //
        // BUGBUG need to add stack limit checking 15-Nov-1993 Jonle
        //
        setESP(VdmSp - StackOffset);

        //
        // Push info for Iret hook handler
        //
        VdmCs = (USHORT) ((IretHookAddress & 0xFFFF0000) >> 16);
        VdmEip = (IretHookAddress & 0xFFFF);

        if (Frame32) {
            *(PULONG)(VdmStackPointer - 4) = getEFLAGS();
            *(PULONG)(VdmStackPointer - 8) = (ULONG) VdmCs;
            *(PULONG)(VdmStackPointer - 12) = VdmEip;
        } else {
            *(PUSHORT)(VdmStackPointer - 2) = (USHORT) getEFLAGS();
            *(PUSHORT)(VdmStackPointer - 4) = VdmCs;
            *(PUSHORT)(VdmStackPointer - 6) = (USHORT) VdmEip;
        }
    }

}


VOID
IRQ13_Eoi(
    int IrqLine,
    int CallCount
    )
{
    UNREFERENCED_PARAMETER(IrqLine);
    UNREFERENCED_PARAMETER(CallCount);

       //
       //  if CallCount is less than Zero, then the interrupt request
       //  is being canceled.
       //
    if (CallCount < 0) {
        return;
        }

    IRQ13BeingHandled = FALSE;
}





#ifdef MONITOR

// spckbd.asm: iret_bop_table:  IRET_HOOK
// void MS_bop_F(void): AddrIretBopTable = ( ((ULONG)getDS() << 16) | (ULONG)getDI() );
// used i.e. by host_iret_bop_table_addr, which gets used to determine hooks in ica_intack()

VOID
MonitorEndIretHook(
    VOID
    )
/*++

Routine Description:


Arguments:

    None.

Return Value:

    None.

--*/
{

    PVOID VdmStackPointer;

    if (getMSW() & MSW_PE) {
        BOOL Frame32 = (BOOL) DPMIFLAGS;
        ULONG FrameSize;

        if (Frame32) {
            FrameSize = 12;
        } else { 
            FrameSize = 6;
        }

        VdmStackPointer = Sim32GetVDMPointer(((ULONG)getSS() << 16),2,TRUE);

        if (Ldt[(getSS() & ~0x7)/sizeof(LDT_ENTRY)].HighWord.Bits.Default_Big) {
            (PCHAR)VdmStackPointer += getESP();
            setESP(getESP() + FrameSize);
        } else {
            (PCHAR)VdmStackPointer += getSP();
            setSP((USHORT) (getSP() + FrameSize));
        }

        if (Frame32) {

            VdmTib.VdmContext.EFlags = *(PULONG)((PCHAR)VdmStackPointer + 8);
            setCS(*(PUSHORT)((PCHAR)VdmStackPointer + 4));
            VdmTib.VdmContext.Eip = *((PULONG)VdmStackPointer);

        } else {

            VdmTib.VdmContext.EFlags = (VdmTib.VdmContext.EFlags & 0xFFFF0000) |
                                        ((ULONG) *(PUSHORT)((PCHAR)VdmStackPointer + 4));
            setCS(*(PUSHORT)((PCHAR)VdmStackPointer + 2));
            VdmTib.VdmContext.Eip = (VdmTib.VdmContext.Eip & 0xFFFF0000) | 
                                        ((ULONG) *(PUSHORT)((PCHAR)VdmStackPointer));

        }

    } else {

        VdmStackPointer = Sim32GetVDMPointer(((ULONG)getSS() << 16) | getSP(),2,FALSE);

        setSP((USHORT) (getSP() + 6));

        (USHORT)(VdmTib.VdmContext.EFlags) = *((PUSHORT)((PCHAR)VdmStackPointer + 4));
        setCS(*((PUSHORT)((PCHAR)VdmStackPointer + 2)));
        setIP(*((PUSHORT)VdmStackPointer));

    }


}

#endif

VOID
host_clear_hw_int()
/*++

Routine Description:

    This routine "forgets" a previously requested hardware interrupt.

Arguments:

    None.

Return Value:

    None.

--*/
{
   /*
    *  We do nothing here to save a kernel call, because the
    *  interrupt if it hasn't been intacked yet or dispatched,
    *  will produce a harmless spurious int, which is dropped
    *  in the i386 interrupt dispatching code anyhow.
    */
}
