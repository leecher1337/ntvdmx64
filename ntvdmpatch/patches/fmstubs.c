#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <windows.h>

#include "insignia.h"
#include "host_def.h"

#include "timestmp.h"
#include "nt_timer.h"


#if !defined(PROD) && defined(CPU_40_STYLE) 
void FmDebugBop()
{
	illegal_bop();
}

void	FmDebug(ULONG hook)
{
	__asm int 3
}
#endif

VOID ActivityCheckAfterTimeSlice(VOID)
{
}

VOID ClearInstanceDataMarking(VOID)
{
	// No idea what this does, stub it...
}

VOID VdmSetPhysRecStructs(ULONG HostAddress, ULONG IntelAddress, ULONG Size)
{
	// TODO: Shoud set PhysicalPageREC.translation entries?
}

VOID EDL_fast_bop(ULONG immed)
{
	// Whatever this it... Better break on it and see if it gets used
	__asm int 3
}

ULONG c_VirtualiseInstruction (ULONG eipInRom, UINT size, ULONG linearAddrOrPort, ULONG dataIn)
{
	// Hopefully this is never called...
	__asm int 3
	return 0;
}

PBYTE c_sas_touch (ULONG addr, ULONG length)
{
	// Hopefully this is never called...
	__asm int 3
	return NULL;
}

IUH host_q_timestamp_diff IPT2(QTIMESTAMP_PTR, tbegin, QTIMESTAMP_PTR, tend)
{
	LARGE_INTEGER DiffTime = *((PLARGE_INTEGER)tend);
	DiffTime.QuadPart -= ((PLARGE_INTEGER)tbegin)->QuadPart;
	return DiffTime.LowPart;
}

void host_q_write_timestamp IPT1(QTIMESTAMP_PTR, stamp)
{
	GetPerfCounterUsecs(NULL, (PLARGE_INTEGER)&stamp->data);
}

/* Not supported */
IUH host_get_q_calib_val	IFN0()
{
	return 0;
}

/* Stolen from video */
IUH host_get_jump_restart	IFN0()
{
	return 100;
}
