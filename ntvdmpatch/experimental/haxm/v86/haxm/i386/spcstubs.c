#include "monitorp.h"
#include "host_def.h"

static LONG stub_q_ev_count = 0;	// holder for below

/* Monitor controlled code will call quick event code immediately so the
 * following needn't be at all accurate.
 */
void host_q_ev_set_count(value)
LONG value;
{
    stub_q_ev_count = value;
}

LONG host_q_ev_get_count()
{
    return(stub_q_ev_count);
}

int host_calc_q_ev_inst_for_time(LONG time)
{
    return(time);
}

void npx_reset()
{ 
    return;
}

void initialise_npx()
{
    return;
}

void cpu_clearHwInt()
{
}

#ifndef YODA
void	check_I()
{
}

void	check_D()
{
}

void	force_yoda()
{
}

USHORT dasm_internal(
   char * txt,	/* Buffer to hold dis-assembly text (-1 means not required) */
   USHORT  seg,	/* Segment for xxxx:... text in dis-assembly */
   ULONG off,	/* ditto offset */
   BYTE default_size,/* 16BIT or 32BIT code segment */
   ULONG p,			/* linear address of start of instruction */
   PVOID byte_at,	/* like sas_hw_at() to use to read intel
				 * but will return -1 if there is an error
				 */
   char * fmt,		/* sprintf format for first line seg:offset */
   char * newline)		/* strcat text to separate lines */
{
}

BOOL	do_condition_checks = FALSE;

#endif

void SET_GLOBAL_SimulateContext(BYTE *localSimulateContext)
{
}


