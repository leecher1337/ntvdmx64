/*++

Copyright (c) 2019  leecher@dose.0wnz.at

Module Name:

    debug.c
Abstract:

    This module is provides some debug functions to yoda

Author:

    leecher@dose.0wnz.at (leecher1337) 10 Mar 2019

Environment:

    User mode only

Revision History:

--*/


#include "monitorp.h"
#include <stdlib.h>
#include "insignia.h"
#include "host_def.h"
#include "../../../softpc.new/base/cvidc/gdpvar.h"
#include "debug.h"

extern FILE *in_stream;

extern yoda_suspend_filter_process(void);

static char *outerPrompts[32]={0};

void FatalError(char *msg)
{
	fprintf (stdout, "FatalError: %s\n", msg);
	// if (fmDebugAfterError) FmDebug(0);
	exit(1);
}

void EnterDebug(char *pDbg)
{
	if (GLOBAL_InsideDebugger < sizeof(outerPrompts)/sizeof(outerPrompts[0]))
	{
		outerPrompts[GLOBAL_InsideDebugger] = pDbg;
		GLOBAL_InsideDebugger++;
#ifdef YODA
		yoda_suspend_filter_process();
#endif
	}
	else
	{
		GLOBAL_InsideDebugger-=2;
		FatalError("Too deep debug nesting");
	}
}

void LeaveDebug()
{
	if (GLOBAL_InsideDebugger<1)
		FatalError("LeaveDebug called when not in debugger");
	GLOBAL_InsideDebugger--;
	outerPrompts[GLOBAL_InsideDebugger] = 0;
}


void AddToTraceXBuffer(DWORD dwAddr, char *str)
{
	// Currently not implemented, I think we don't need it
}

BOOL GetCpuCmdLine(char *buff,int size,FILE *stream, char *who)
{ 
	char prompt[264], *p="";
	int i;
  
	prompt[0]=0;
	if (stream == stdin)
	{
		if (GLOBAL_InsideDebugger > 0)
		{
			for (i=0; i<GLOBAL_InsideDebugger; i++)
			{
				strcat(prompt, p);
				strcat(prompt, outerPrompts[i]);
				p=":";
			}
		}
		if (*who)
		{
			strcat(prompt,p);
			strcat(prompt,who);
		}
		if (p = strrchr(who, '?'))
		{
			p++;
			while (*p && *p<=' ') p++;
		}
		if (!p || *p)
			strcat(prompt, "> ");
		fprintf(stdout, prompt);
		fflush(stdout);
#ifdef YODA
		fflush(trace_file);
#endif
	}
	if (fgets(buff, size, stream) != NULL)
	{
		for (p=buff; *p && *p<=' '; p++);
		if (buff < p)
		{
			if (!*p) *buff=0; else
			{
				memmove(buff, p, p-buff);
				*p = 0;
			}
		}
		p = strchr(buff, '\n');
		if (!p) p = strchr(buff, '\r');
		if (p != NULL)
			*p = '\0';
		fprintf(stdout, "\n");
		return TRUE;
	}
	else
	{
#ifdef YODA
		if (stream == in_stream && in_stream != stdin)
		{
			fclose(in_stream);
			in_stream = stdin;
			if (trace_file == stdout)
				fprintf(stdout,"(eof)");
			return GetCpuCmdLine(buff,size,stream,who);
		}
#endif
		return FALSE;
	}
}

const char *name_vmx_exit(int value)
{
#define CASE(x) \
    case x: return #x

    switch (value) {
    CASE(VMX_EXIT_INT_EXCEPTION_NMI);
    CASE(VMX_EXIT_EXT_INTERRUPT);
    CASE(VMX_EXIT_TRIPLE_FAULT);
    CASE(VMX_EXIT_INIT_EVENT);
    CASE(VMX_EXIT_SIPI_EVENT);
    CASE(VMX_EXIT_SMI_IO_EVENT);
    CASE(VMX_EXIT_SMI_OTHER_EVENT);
    CASE(VMX_EXIT_PENDING_INTERRUPT);
    CASE(VMX_EXIT_PENDING_NMI);
    CASE(VMX_EXIT_TASK_SWITCH);
    CASE(VMX_EXIT_CPUID);
    CASE(VMX_EXIT_GETSEC);
    CASE(VMX_EXIT_HLT);
    CASE(VMX_EXIT_INVD);
    CASE(VMX_EXIT_INVLPG);
    CASE(VMX_EXIT_RDPMC);
    CASE(VMX_EXIT_RDTSC);
    CASE(VMX_EXIT_RSM);
    CASE(VMX_EXIT_VMCALL);
    CASE(VMX_EXIT_VMCLEAR);
    CASE(VMX_EXIT_VMLAUNCH);
    CASE(VMX_EXIT_VMPTRLD);
    CASE(VMX_EXIT_VMPTRST);
    CASE(VMX_EXIT_VMREAD);
    CASE(VMX_EXIT_VMRESUME);
    CASE(VMX_EXIT_VMWRITE);
    CASE(VMX_EXIT_VMXOFF);
    CASE(VMX_EXIT_VMXON);
    CASE(VMX_EXIT_CR_ACCESS);
    CASE(VMX_EXIT_DR_ACCESS);
    CASE(VMX_EXIT_IO);
    CASE(VMX_EXIT_MSR_READ);
    CASE(VMX_EXIT_MSR_WRITE);
    CASE(VMX_EXIT_FAILED_VMENTER_GS);
    CASE(VMX_EXIT_FAILED_VMENTER_MSR);
    CASE(VMX_EXIT_MWAIT);
    CASE(VMX_EXIT_MTF_EXIT);
    CASE(VMX_EXIT_MONITOR);
    CASE(VMX_EXIT_PAUSE);
    CASE(VMX_EXIT_MACHINE_CHECK);
    CASE(VMX_EXIT_TPR_BELOW_THRESHOLD);
    CASE(VMX_EXIT_APIC_ACCESS);
    CASE(VMX_EXIT_GDT_IDT_ACCESS);
    CASE(VMX_EXIT_LDT_TR_ACCESS);
    CASE(VMX_EXIT_EPT_VIOLATION);
    CASE(VMX_EXIT_EPT_MISCONFIG);
    CASE(VMX_EXIT_INVEPT);
    CASE(VMX_EXIT_RDTSCP);
    CASE(VMX_EXIT_VMX_TIMER_EXIT);
    CASE(VMX_EXIT_INVVPID);
    CASE(VMX_EXIT_WBINVD);
    CASE(VMX_EXIT_XSETBV);
    CASE(VMX_EXIT_APIC_WRITE);
    CASE(VMX_EXIT_RDRAND);
    CASE(VMX_EXIT_INVPCID);
    CASE(VMX_EXIT_VMFUNC);
    CASE(VMX_EXIT_ENCLS);
    CASE(VMX_EXIT_RDSEED);
    CASE(VMX_EXIT_XSAVES);
    CASE(VMX_EXIT_XRSTORS);
    default:
        return "";
    }
#undef CASE
}
