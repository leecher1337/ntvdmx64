/*[

zfrsrvdx86.c

LOCAL CHAR SccsID[]="@(#)zfrsrvdx86.c	1.23 11/28/21";

Reserved Floating Point CPU Functions for x86 Host CPU.
Using host FPU for better accuracy
-------------------------------------------------------

]*/
#include <insignia.h>
#include <host_def.h>
#include <cfpu_def.h>
#define HOOKED_IRETS
#include <ica.h>	/* need NPX interrupt line */
#include <ccpusas4.h>   /* the cpu internal sas bits */
#include <c_main.h>     /* C CPU definitions-interfaces */
#include <c_page.h>     /* Paging Interface */
#include <c_mem.h>      /* CPU - Memory Interface */
#include <c_addr.h>
#include <c_oprnd.h>
#include <c_reg.h>
#include <c_intr.h>
#include <c_xcptn.h>	/* Definition of Int16() */
#include <intx.h>
#include <fault.h>
#ifdef SFELLOW
#include <CpuInt_c.h>
#endif	/* SFELLOW */
#include <fpieee.h>
#include <excpt.h>
#include <float.h>

typedef union
{
IU32 sng;           /* Single Part Operand */
IU32 mlt[2];        /* Multiple (two) Part Operand */
IU8 npxbuff[108];   /* Make it the maximum required size */
} OPERAND;

IMPORT IU8 *p;                        /* Pntr. to Intel Opcode Stream. */
IMPORT IU8 modRM;                     /* The modRM byte. */
IMPORT OPERAND ops[3];          /* Inst. Operands. */
IMPORT IU32 save_id[3];                /* Saved state for Inst. Operands. */
IMPORT IU32 m_off[3];          /* Memory Operand offset. */
IMPORT IU32 m_pa[3];
IMPORT IU32 m_la[3];
IMPORT ISM32   m_seg[3];          /* Memory Operand segment reg. index. */
IMPORT BOOL m_isreg[3];                /* Memory Operand Register(true)/
                           Memory(false) indicator */
IMPORT IU8 segment_override;  /* Segment Prefix for current inst. */


GLOBAL IBOOL NpxIntrNeeded = FALSE;
GLOBAL BOOL NPX_ADDRESS_SIZE_32 = 0;


LOCAL CONTEXT m_context={0};

// TODO: Check if FLDENV/FSTENV is faster than GetThreadContext()...?

// Save current Thread context, load FPU context from virtual CPU
#define INIT_FPU_CMD \
	CONTEXT context={0}; \
	IS32 WORD_PTR; \
	context.ContextFlags = CONTEXT_FLOATING_POINT; \
	GetThreadContext((HANDLE)0xFFFFFFFE, &context); \
	SetThreadContext((HANDLE)0xFFFFFFFE, &m_context); \

// After execution, restore FPU status again
#define DONE_FPU_CMD \
	GetThreadContext((HANDLE)0xFFFFFFFE, &m_context); \
	SetThreadContext((HANDLE)0xFFFFFFFE, &context);

// Stupid compiler doesn't support WORD PTR in inline asm..???
#define INIT_WORD_PTR WORD_PTR = (IS32)((IS16)ops[0].sng);


int __cdecl DoNpxException( _FPIEEE_RECORD *pieee )
{
   // user-defined ieee trap handler routine:
   // there is one handler for all 
   // IEEE exceptions

	NpxIntrNeeded = TRUE;	/* interrupt delayed until next NPX inst */

	return EXCEPTION_CONTINUE_EXECUTION;
}


/* called on NPX instr that follows faulting instr */
void TakeNpxExceptionInt()
{
	IU32 hook_address;	
	IU16 cpu_hw_interrupt_number;

	NpxIntrNeeded = FALSE;

#ifdef	SPC486
	if (GET_NE() == 0)
	{
#ifndef SFELLOW
		ica_hw_interrupt (ICA_SLAVE, CPU_AT_NPX_INT, 1);
#else	/* SFELLOW */
		c_cpu_interrupt(CPU_NPX_INT, 0);
#endif	/* SFELLOW */
	}
	else
	{
		Int16();
	}
#else	/* SPC486 */
	ica_hw_interrupt (ICA_SLAVE, CPU_AT_NPX_INT, 1);
#endif	/* SPC486 */

#ifndef SFELLOW
	/* and immediately dispatch to interrupt */
	if (GET_IF())
	{
		cpu_hw_interrupt_number = (IU16)ica_intack(&hook_address);
		EXT = EXTERNAL;
		do_intrupt(cpu_hw_interrupt_number, FALSE, FALSE, (IU16)0);
		CCPU_save_EIP = GET_EIP();   /* to reflect IP change */
	}
#endif	/*SFELLOW*/
}



LOCAL VOID npx_fabs() {
	INIT_FPU_CMD

	__asm FABS

	DONE_FPU_CMD
}


LOCAL VOID npx_fadd_f0_f0() {
	INIT_FPU_CMD

	__asm FADD st,st

	DONE_FPU_CMD
}


LOCAL VOID npx_fadd_f0_f1() {
	INIT_FPU_CMD

	__asm FADD st,st(1)

	DONE_FPU_CMD
}

LOCAL VOID npx_fadd_f0_f2() {
	INIT_FPU_CMD

	__asm FADD st,st(2)

	DONE_FPU_CMD
}

LOCAL VOID npx_fadd_f0_f3() {
	INIT_FPU_CMD

	__asm FADD st,st(3)

	DONE_FPU_CMD
}

LOCAL VOID npx_fadd_f0_f4() {
	INIT_FPU_CMD

	__asm FADD st,st(4)

	DONE_FPU_CMD
}

LOCAL VOID npx_fadd_f0_f5() {
	INIT_FPU_CMD

	__asm FADD st,st(5)

	DONE_FPU_CMD
}

LOCAL VOID npx_fadd_f0_f6() {
	INIT_FPU_CMD

	__asm FADD st,st(6)

	DONE_FPU_CMD
}

LOCAL VOID npx_fadd_f0_f7() {
	INIT_FPU_CMD

	__asm FADD st,st(7)

	DONE_FPU_CMD
}

LOCAL VOID npx_fadd_f1_f0() {
	INIT_FPU_CMD

	__asm FADD st(1),st

	DONE_FPU_CMD
}

LOCAL VOID npx_fadd_f2_f0() {
	INIT_FPU_CMD

	__asm FADD st(2),st

	DONE_FPU_CMD
}

LOCAL VOID npx_fadd_f3_f0() {
	INIT_FPU_CMD

	__asm FADD st(3),st

	DONE_FPU_CMD
}

LOCAL VOID npx_fadd_f4_f0() {
	INIT_FPU_CMD

	__asm FADD st(4),st

	DONE_FPU_CMD
}

LOCAL VOID npx_fadd_f5_f0() {
	INIT_FPU_CMD

	__asm FADD st(5),st

	DONE_FPU_CMD
}

LOCAL VOID npx_fadd_f6_f0() {
	INIT_FPU_CMD

	__asm FADD st(6),st

	DONE_FPU_CMD
}

LOCAL VOID npx_fadd_f7_f0() {
	INIT_FPU_CMD

	__asm FADD st(7),st

	DONE_FPU_CMD
}

LOCAL VOID npx_fadd_short() {
	INIT_FPU_CMD

	D_Ed(0, RO0, PG_R);
	F_Ed(0);
	__asm FADD DWORD PTR ops[0].sng

	DONE_FPU_CMD
}

LOCAL VOID npx_fadd_long() {
	INIT_FPU_CMD

	D_E08(0, RO0, PG_R);
	F_E08(0);
	__asm FADD QWORD PTR ops[0].npxbuff[0]

	DONE_FPU_CMD
}

LOCAL VOID npx_faddp_f0() {
	INIT_FPU_CMD

	__asm FADDP st(0),st

	DONE_FPU_CMD
}

LOCAL VOID npx_faddp_f1() {
	INIT_FPU_CMD

	__asm FADDP st(1),st

	DONE_FPU_CMD
}

LOCAL VOID npx_faddp_f2() {
	INIT_FPU_CMD

	__asm FADDP st(2),st

	DONE_FPU_CMD
}

LOCAL VOID npx_faddp_f3() {
	INIT_FPU_CMD

	__asm FADDP st(3),st

	DONE_FPU_CMD
}

LOCAL VOID npx_faddp_f4() {
	INIT_FPU_CMD

	__asm FADDP st(4),st

	DONE_FPU_CMD
}

LOCAL VOID npx_faddp_f5() {
	INIT_FPU_CMD

	__asm FADDP st(5),st

	DONE_FPU_CMD
}

LOCAL VOID npx_faddp_f6() {
	INIT_FPU_CMD

	__asm FADDP st(6),st

	DONE_FPU_CMD
}

LOCAL VOID npx_faddp_f7() {
	INIT_FPU_CMD

	__asm FADDP st(7),st

	DONE_FPU_CMD
}

LOCAL VOID npx_fbld() {
	INIT_FPU_CMD

	D_E0a(0, RO0, PG_R);
	F_E0a(0);
	__asm FBLD TBYTE PTR ops[0].npxbuff[0]

	DONE_FPU_CMD
}

LOCAL VOID npx_fbstp() {
	INIT_FPU_CMD

	D_E0a(0, WO0, PG_W);
	__asm FBSTP TBYTE PTR ops[0].npxbuff[0]
	P_E0a(0);


	DONE_FPU_CMD
}

LOCAL VOID npx_fchs() {
	INIT_FPU_CMD

	__asm FCHS

	DONE_FPU_CMD
}

LOCAL VOID npx_fclex() {
	INIT_FPU_CMD

	__asm FCLEX

	DONE_FPU_CMD
}

LOCAL VOID npx_fcom_f0() {
	INIT_FPU_CMD

	__asm FCOM ST(0)

	DONE_FPU_CMD
}

LOCAL VOID npx_fcom_f1() {
	INIT_FPU_CMD

	__asm FCOM ST(1)

	DONE_FPU_CMD
}

LOCAL VOID npx_fcom_f2() {
	INIT_FPU_CMD

	__asm FCOM ST(2)

	DONE_FPU_CMD
}

LOCAL VOID npx_fcom_f3() {
	INIT_FPU_CMD

	__asm FCOM ST(3)

	DONE_FPU_CMD
}

LOCAL VOID npx_fcom_f4() {
	INIT_FPU_CMD

	__asm FCOM ST(4)

	DONE_FPU_CMD
}

LOCAL VOID npx_fcom_f5() {
	INIT_FPU_CMD

	__asm FCOM ST(5)

	DONE_FPU_CMD
}

LOCAL VOID npx_fcom_f6() {
	INIT_FPU_CMD

	__asm FCOM ST(6)

	DONE_FPU_CMD
}

LOCAL VOID npx_fcom_f7() {
	INIT_FPU_CMD

	__asm FCOM ST(7)

	DONE_FPU_CMD
}

LOCAL VOID npx_fcom_short() {
	INIT_FPU_CMD

	D_Ed(0, RO0, PG_R);
	F_Ed(0);
	__asm FCOM DWORD PTR ops[0].sng

	DONE_FPU_CMD
}

LOCAL VOID npx_fcom_long() {
	INIT_FPU_CMD

	D_E08(0, RO0, PG_R);
	F_E08(0);
	__asm FCOM QWORD PTR ops[0].npxbuff[0]

	DONE_FPU_CMD
}

LOCAL VOID npx_fcomp_f0() {
	INIT_FPU_CMD

	__asm FCOMP ST(0)

	DONE_FPU_CMD
}

LOCAL VOID npx_fcomp_f1() {
	INIT_FPU_CMD

	__asm FCOMP ST(1)

	DONE_FPU_CMD
}

LOCAL VOID npx_fcomp_f2() {
	INIT_FPU_CMD

	__asm FCOMP ST(2)

	DONE_FPU_CMD
}

LOCAL VOID npx_fcomp_f3() {
	INIT_FPU_CMD

	__asm FCOMP ST(3)

	DONE_FPU_CMD
}

LOCAL VOID npx_fcomp_f4() {
	INIT_FPU_CMD

	__asm FCOMP ST(4)

	DONE_FPU_CMD
}

LOCAL VOID npx_fcomp_f5() {
	INIT_FPU_CMD

	__asm FCOMP ST(5)

	DONE_FPU_CMD
}

LOCAL VOID npx_fcomp_f6() {
	INIT_FPU_CMD

	__asm FCOMP ST(6)

	DONE_FPU_CMD
}

LOCAL VOID npx_fcomp_f7() {
	INIT_FPU_CMD

	__asm FCOMP ST(7)

	DONE_FPU_CMD
}

LOCAL VOID npx_fcomp_short() {
	INIT_FPU_CMD

	D_Ed(0, RO0, PG_R);
	F_Ed(0);
	__asm FCOMP DWORD PTR ops[0].sng

	DONE_FPU_CMD
}

LOCAL VOID npx_fcomp_long() {
	INIT_FPU_CMD

	D_E08(0, RO0, PG_R);
	F_E08(0);
	__asm FCOMP QWORD PTR ops[0].npxbuff[0]

	DONE_FPU_CMD
}

LOCAL VOID npx_fcompp() {
	INIT_FPU_CMD

	__asm FCOMPP

	DONE_FPU_CMD
}

LOCAL VOID npx_fcos() {
	INIT_FPU_CMD

	__asm FCOS

	DONE_FPU_CMD
}

LOCAL VOID npx_fdecstp() {
	INIT_FPU_CMD

	__asm FDECSTP

	DONE_FPU_CMD
}

LOCAL VOID npx_fdiv_f0_f0() {
	INIT_FPU_CMD

	__asm FDIV ST, ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fdiv_f0_f1() {
	INIT_FPU_CMD

	__asm FDIV ST, ST(1)

	DONE_FPU_CMD
}

LOCAL VOID npx_fdiv_f0_f2() {
	INIT_FPU_CMD

	__asm FDIV ST, ST(2)

	DONE_FPU_CMD
}

LOCAL VOID npx_fdiv_f0_f3() {
	INIT_FPU_CMD

	__asm FDIV ST, ST(3)

	DONE_FPU_CMD
}

LOCAL VOID npx_fdiv_f0_f4() {
	INIT_FPU_CMD

	__asm FDIV ST, ST(4)

	DONE_FPU_CMD
}

LOCAL VOID npx_fdiv_f0_f5() {
	INIT_FPU_CMD

	__asm FDIV ST, ST(5)

	DONE_FPU_CMD
}

LOCAL VOID npx_fdiv_f0_f6() {
	INIT_FPU_CMD

	__asm FDIV ST, ST(6)

	DONE_FPU_CMD
}

LOCAL VOID npx_fdiv_f0_f7() {
	INIT_FPU_CMD

	__asm FDIV ST, ST(7)

	DONE_FPU_CMD
}

LOCAL VOID npx_fdiv_f1_f0() {
	INIT_FPU_CMD

	__asm FDIV ST(1), ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fdiv_f2_f0() {
	INIT_FPU_CMD

	__asm FDIV ST(2), ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fdiv_f3_f0() {
	INIT_FPU_CMD

	__asm FDIV ST(3), ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fdiv_f4_f0() {
	INIT_FPU_CMD

	__asm FDIV ST(4), ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fdiv_f5_f0() {
	INIT_FPU_CMD

	__asm FDIV ST(5), ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fdiv_f6_f0() {
	INIT_FPU_CMD

	__asm FDIV ST(6), ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fdiv_f7_f0() {
	INIT_FPU_CMD

	__asm FDIV ST(7), ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fdiv_short() {
	INIT_FPU_CMD

	D_Ed(0, RO0, PG_R);
	F_Ed(0);
	__asm FDIV DWORD PTR ops[0].sng

	DONE_FPU_CMD
}

LOCAL VOID npx_fdiv_long() {
	INIT_FPU_CMD

	D_E08(0, RO0, PG_R);
	F_E08(0);
	__asm FDIV QWORD PTR ops[0].npxbuff[0]

	DONE_FPU_CMD
}

LOCAL VOID npx_fdivp_f0() {
	INIT_FPU_CMD

	__asm FDIVP ST(0), ST

	DONE_FPU_CMD
}

// ---

LOCAL VOID npx_fdivp_f1() {
	INIT_FPU_CMD

	__asm FDIVP	ST(1),ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fdivp_f2() {
	INIT_FPU_CMD

	__asm FDIVP	ST(2),ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fdivp_f3() {
	INIT_FPU_CMD

	__asm FDIVP	ST(3),ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fdivp_f4() {
	INIT_FPU_CMD

	__asm FDIVP	ST(4),ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fdivp_f5() {
	INIT_FPU_CMD

	__asm FDIVP	ST(5),ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fdivp_f6() {
	INIT_FPU_CMD

	__asm FDIVP	ST(6),ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fdivp_f7() {
	INIT_FPU_CMD

	__asm FDIVP	ST(7),ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fdivr_f0_f0() {
	INIT_FPU_CMD

	__asm FDIVR	ST,ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fdivr_f0_f1() {
	INIT_FPU_CMD

	__asm FDIVR	ST,ST(1)

	DONE_FPU_CMD
}

LOCAL VOID npx_fdivr_f0_f2() {
	INIT_FPU_CMD

	__asm FDIVR	ST,ST(2)

	DONE_FPU_CMD
}

LOCAL VOID npx_fdivr_f0_f3() {
	INIT_FPU_CMD

	__asm FDIVR	ST,ST(3)

	DONE_FPU_CMD
}

LOCAL VOID npx_fdivr_f0_f4() {
	INIT_FPU_CMD

	__asm FDIVR	ST,ST(4)

	DONE_FPU_CMD
}

LOCAL VOID npx_fdivr_f0_f5() {
	INIT_FPU_CMD

	__asm FDIVR	ST,ST(5)

	DONE_FPU_CMD
}

LOCAL VOID npx_fdivr_f0_f6() {
	INIT_FPU_CMD

	__asm FDIVR	ST,ST(6)

	DONE_FPU_CMD
}

LOCAL VOID npx_fdivr_f0_f7() {
	INIT_FPU_CMD

	__asm FDIVR	ST,ST(7)

	DONE_FPU_CMD
}

LOCAL VOID npx_fdivr_f1_f0() {
	INIT_FPU_CMD

	__asm FDIVR	ST(1),ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fdivr_f2_f0() {
	INIT_FPU_CMD

	__asm FDIVR	ST(2),ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fdivr_f3_f0() {
	INIT_FPU_CMD

	__asm FDIVR	ST(3),ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fdivr_f4_f0() {
	INIT_FPU_CMD

	__asm FDIVR	ST(4),ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fdivr_f5_f0() {
	INIT_FPU_CMD

	__asm FDIVR	ST(5),ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fdivr_f6_f0() {
	INIT_FPU_CMD

	__asm FDIVR	ST(6),ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fdivr_f7_f0() {
	INIT_FPU_CMD

	__asm FDIVR	ST(7),ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fdivr_short() {
	INIT_FPU_CMD

	D_Ed(0, RO0, PG_R);
	F_Ed(0);
	__asm FDIVR	DWORD PTR ops[0].sng

	DONE_FPU_CMD
}

LOCAL VOID npx_fdivr_long() {
	INIT_FPU_CMD

	D_E08(0, RO0, PG_R);
	F_E08(0);
	__asm FDIVR	QWORD PTR ops[0].npxbuff[0]

	DONE_FPU_CMD
}

LOCAL VOID npx_fdivrp_f0() {
	INIT_FPU_CMD

	__asm FDIVRP	ST(0),ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fdivrp_f1() {
	INIT_FPU_CMD

	__asm FDIVRP	ST(1),ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fdivrp_f2() {
	INIT_FPU_CMD

	__asm FDIVRP	ST(2),ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fdivrp_f3() {
	INIT_FPU_CMD

	__asm FDIVRP	ST(3),ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fdivrp_f4() {
	INIT_FPU_CMD

	__asm FDIVRP	ST(4),ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fdivrp_f5() {
	INIT_FPU_CMD

	__asm FDIVRP	ST(5),ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fdivrp_f6() {
	INIT_FPU_CMD

	__asm FDIVRP	ST(6),ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fdivrp_f7() {
	INIT_FPU_CMD

	__asm FDIVRP	ST(7),ST

	DONE_FPU_CMD
}

LOCAL VOID npx_ffree_f0() {
	INIT_FPU_CMD

	__asm FFREE	ST(0)

	DONE_FPU_CMD
}

LOCAL VOID npx_ffree_f1() {
	INIT_FPU_CMD

	__asm FFREE	ST(1)

	DONE_FPU_CMD
}

LOCAL VOID npx_ffree_f2() {
	INIT_FPU_CMD

	__asm FFREE	ST(2)

	DONE_FPU_CMD
}

LOCAL VOID npx_ffree_f3() {
	INIT_FPU_CMD

	__asm FFREE	ST(3)

	DONE_FPU_CMD
}

LOCAL VOID npx_ffree_f4() {
	INIT_FPU_CMD

	__asm FFREE	ST(4)

	DONE_FPU_CMD
}

LOCAL VOID npx_ffree_f5() {
	INIT_FPU_CMD

	__asm FFREE	ST(5)

	DONE_FPU_CMD
}

LOCAL VOID npx_ffree_f6() {
	INIT_FPU_CMD

	__asm FFREE	ST(6)

	DONE_FPU_CMD
}

LOCAL VOID npx_ffree_f7() {
	INIT_FPU_CMD

	__asm FFREE	ST(7)

	DONE_FPU_CMD
}

LOCAL VOID npx_ffreep_f0() {
	INIT_FPU_CMD

	__asm FFREEP	ST(0)

	DONE_FPU_CMD
}

LOCAL VOID npx_ffreep_f1() {
	INIT_FPU_CMD

	__asm FFREEP	ST(1)

	DONE_FPU_CMD
}

LOCAL VOID npx_ffreep_f2() {
	INIT_FPU_CMD

	__asm FFREEP	ST(2)

	DONE_FPU_CMD
}

LOCAL VOID npx_ffreep_f3() {
	INIT_FPU_CMD

	__asm FFREEP	ST(3)

	DONE_FPU_CMD
}

LOCAL VOID npx_ffreep_f4() {
	INIT_FPU_CMD

	__asm FFREEP	ST(4)

	DONE_FPU_CMD
}

LOCAL VOID npx_ffreep_f5() {
	INIT_FPU_CMD

	__asm FFREEP	ST(5)

	DONE_FPU_CMD
}

LOCAL VOID npx_ffreep_f6() {
	INIT_FPU_CMD

	__asm FFREEP	ST(6)

	DONE_FPU_CMD
}

LOCAL VOID npx_ffreep_f7() {
	INIT_FPU_CMD

	__asm FFREEP	ST(7)

	DONE_FPU_CMD
}

LOCAL VOID npx_fiadd_word() {
	INIT_FPU_CMD

	D_Ew(0, RO0, PG_R);
	F_Ew(0);
	INIT_WORD_PTR
	__asm FIADD	DWORD PTR WORD_PTR

	DONE_FPU_CMD
}

LOCAL VOID npx_fiadd_short() {
	INIT_FPU_CMD

	D_Ed(0, RO0, PG_R);
	F_Ed(0);
	__asm FIADD	DWORD PTR ops[0].sng

	DONE_FPU_CMD
}

LOCAL VOID npx_ficom_word() {
	INIT_FPU_CMD

	D_Ew(0, RO0, PG_R);
	F_Ew(0);
	INIT_WORD_PTR
	__asm FICOM	DWORD PTR WORD_PTR

	DONE_FPU_CMD
}

LOCAL VOID npx_ficom_short() {
	INIT_FPU_CMD

	D_Ed(0, RO0, PG_R);
	F_Ed(0);
	__asm FICOM	DWORD PTR ops[0].sng

	DONE_FPU_CMD
}

LOCAL VOID npx_ficomp_word() {
	INIT_FPU_CMD

	D_Ew(0, RO0, PG_R);
	F_Ew(0);
	INIT_WORD_PTR
	__asm FICOMP	DWORD PTR WORD_PTR

	DONE_FPU_CMD
}

LOCAL VOID npx_ficomp_short() {
	INIT_FPU_CMD

	D_Ed(0, RO0, PG_R);
	F_Ed(0);
	__asm FICOMP	DWORD PTR ops[0].sng

	DONE_FPU_CMD
}

LOCAL VOID npx_fidiv_word() {
	INIT_FPU_CMD

	D_Ew(0, RO0, PG_R);
	F_Ew(0);
	INIT_WORD_PTR
	__asm FIDIV	DWORD PTR WORD_PTR

	DONE_FPU_CMD
}

LOCAL VOID npx_fidiv_short() {
	INIT_FPU_CMD

	D_Ed(0, RO0, PG_R);
	F_Ed(0);
	__asm FIDIV	DWORD PTR ops[0].sng

	DONE_FPU_CMD
}

LOCAL VOID npx_fidivr_word() {
	INIT_FPU_CMD

	D_Ew(0, RO0, PG_R);
	F_Ew(0);
	INIT_WORD_PTR
	__asm FIDIVR	DWORD PTR WORD_PTR

	DONE_FPU_CMD
}

LOCAL VOID npx_fidivr_short() {
	INIT_FPU_CMD

	D_Ed(0, RO0, PG_R);
	F_Ed(0);
	__asm FIDIVR	DWORD PTR ops[0].sng

	DONE_FPU_CMD
}

LOCAL VOID npx_fild_word() {
	INIT_FPU_CMD

	D_Ew(0, RO0, PG_R);
	F_Ew(0);
	INIT_WORD_PTR
	__asm FILD	DWORD PTR WORD_PTR

	DONE_FPU_CMD
}

LOCAL VOID npx_fild_short() {
	INIT_FPU_CMD

	D_Ed(0, RO0, PG_R);
	F_Ed(0);
	__asm FILD	DWORD PTR ops[0].sng

	DONE_FPU_CMD
}

LOCAL VOID npx_fild_long() {
	INIT_FPU_CMD

	D_E08(0, RO0, PG_R);
	F_E08(0);
	__asm FILD	QWORD PTR ops[0].npxbuff[0]

	DONE_FPU_CMD
}

LOCAL VOID npx_fimul_word() {
	INIT_FPU_CMD

	D_Ew(0, RO0, PG_R);
	F_Ew(0);
	INIT_WORD_PTR
	__asm FIMUL	DWORD PTR WORD_PTR

	DONE_FPU_CMD
}

LOCAL VOID npx_fimul_short() {
	INIT_FPU_CMD

	D_Ed(0, RO0, PG_R);
	F_Ed(0);
	__asm FIMUL	DWORD PTR ops[0].sng

	DONE_FPU_CMD
}

LOCAL VOID npx_fincstp() {
	INIT_FPU_CMD

	__asm FINCSTP

	DONE_FPU_CMD
}


LOCAL VOID npx_fist_word() {
	INIT_FPU_CMD

	D_Ew(0, WO0, PG_W);
	INIT_WORD_PTR
	__asm FIST	DWORD PTR WORD_PTR
	P_Ew(0);

	DONE_FPU_CMD
}

LOCAL VOID npx_fist_short() {
	INIT_FPU_CMD

	D_Ed(0, WO0, PG_W);
	__asm FIST	DWORD PTR ops[0].sng
	P_Ed(0);

	DONE_FPU_CMD
}

LOCAL VOID npx_fistp_word() {
	INIT_FPU_CMD

	D_Ew(0, WO0, PG_W);
	INIT_WORD_PTR
	__asm FISTP	DWORD PTR WORD_PTR
	P_Ew(0);

	DONE_FPU_CMD
}

LOCAL VOID npx_fistp_short() {
	INIT_FPU_CMD

	D_Ed(0, WO0, PG_W);
	__asm FISTP	DWORD PTR ops[0].sng
	P_Ed(0);

	DONE_FPU_CMD
}

LOCAL VOID npx_fistp_long() {
	INIT_FPU_CMD

	D_E08(0, WO0, PG_W);
	__asm FISTP	QWORD PTR ops[0].npxbuff[0]
	P_E08(0);

	DONE_FPU_CMD
}

LOCAL VOID npx_fisub_word() {
	INIT_FPU_CMD

	D_Ew(0, RO0, PG_R);
	F_Ew(0);
	INIT_WORD_PTR
	__asm FISUB	DWORD PTR WORD_PTR

	DONE_FPU_CMD
}

LOCAL VOID npx_fisub_short() {
	INIT_FPU_CMD

	D_Ed(0, RO0, PG_R);
	F_Ed(0);
	__asm FISUB	DWORD PTR ops[0].sng

	DONE_FPU_CMD
}

LOCAL VOID npx_fisubr_word() {
	INIT_FPU_CMD

	D_Ew(0, RO0, PG_R);
	F_Ew(0);
	INIT_WORD_PTR
	__asm FISUBR	DWORD PTR WORD_PTR

	DONE_FPU_CMD
}

LOCAL VOID npx_fisubr_short() {
	INIT_FPU_CMD

	D_Ed(0, RO0, PG_R);
	F_Ed(0);
	__asm FISUBR	DWORD PTR ops[0].sng

	DONE_FPU_CMD
}

LOCAL VOID npx_fld_f0() {
	INIT_FPU_CMD

	__asm FLD	ST(0)

	DONE_FPU_CMD
}

LOCAL VOID npx_fld_f1() {
	INIT_FPU_CMD

	__asm FLD	ST(1)

	DONE_FPU_CMD
}

LOCAL VOID npx_fld_f2() {
	INIT_FPU_CMD

	__asm FLD	ST(2)

	DONE_FPU_CMD
}

LOCAL VOID npx_fld_f3() {
	INIT_FPU_CMD

	__asm FLD	ST(3)

	DONE_FPU_CMD
}

LOCAL VOID npx_fld_f4() {
	INIT_FPU_CMD

	__asm FLD	ST(4)

	DONE_FPU_CMD
}

LOCAL VOID npx_fld_f5() {
	INIT_FPU_CMD

	__asm FLD	ST(5)

	DONE_FPU_CMD
}

LOCAL VOID npx_fld_f6() {
	INIT_FPU_CMD

	__asm FLD	ST(6)

	DONE_FPU_CMD
}

LOCAL VOID npx_fld_f7() {
	INIT_FPU_CMD

	__asm FLD	ST(7)

	DONE_FPU_CMD
}

LOCAL VOID npx_fld_short() {
	INIT_FPU_CMD

	D_Ed(0, RO0, PG_R);
	F_Ed(0);
	__asm FLD	DWORD PTR ops[0].sng

	DONE_FPU_CMD
}

LOCAL VOID npx_fld_long() {
	INIT_FPU_CMD

	D_E08(0, RO0, PG_R);
	F_E08(0);
	__asm FLD	QWORD PTR ops[0].npxbuff[0]

	DONE_FPU_CMD
}

LOCAL VOID npx_fld_temp() {
	INIT_FPU_CMD

	D_E0a(0, RO0, PG_R);
	F_E0a(0);
	__asm FLD	TBYTE PTR ops[0].npxbuff[0]

	DONE_FPU_CMD
}

LOCAL VOID npx_fldcw() {
	INIT_FPU_CMD

	D_Ew(0, RO0, PG_R);
	F_Ew(0);
	__asm FLDCW ops[0].sng

	DONE_FPU_CMD
}

LOCAL VOID npx_fldenv() {
	INIT_FPU_CMD

	NPX_ADDRESS_SIZE_32 = (GET_OPERAND_SIZE()==USE16)?FALSE:TRUE;
	D_E0e(0, RO0, PG_R);
	F_E0e(0);
	__asm FLDENV ops[0].npxbuff[0]

	DONE_FPU_CMD
}

LOCAL VOID npx_fldlg2() {
	INIT_FPU_CMD

	__asm FLDLG2

	DONE_FPU_CMD
}

LOCAL VOID npx_fldln2() {
	INIT_FPU_CMD

	__asm FLDLN2

	DONE_FPU_CMD
}

LOCAL VOID npx_fldl2e() {
	INIT_FPU_CMD

	__asm FLDL2E

	DONE_FPU_CMD
}

LOCAL VOID npx_fldl2t() {
	INIT_FPU_CMD

	__asm FLDL2T

	DONE_FPU_CMD
}

LOCAL VOID npx_fldpi() {
	INIT_FPU_CMD

	__asm FLDPI

	DONE_FPU_CMD
}

LOCAL VOID npx_fldz() {
	INIT_FPU_CMD

	__asm FLDZ

	DONE_FPU_CMD
}

LOCAL VOID npx_fld1() {
	INIT_FPU_CMD

	__asm FLD1

	DONE_FPU_CMD
}

LOCAL VOID npx_fmul_f0_f0() {
	INIT_FPU_CMD

	__asm FMUL	ST,ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fmul_f0_f1() {
	INIT_FPU_CMD

	__asm FMUL	ST,ST(1)

	DONE_FPU_CMD
}

LOCAL VOID npx_fmul_f0_f2() {
	INIT_FPU_CMD

	__asm FMUL	ST,ST(2)

	DONE_FPU_CMD
}

LOCAL VOID npx_fmul_f0_f3() {
	INIT_FPU_CMD

	__asm FMUL	ST,ST(3)

	DONE_FPU_CMD
}

LOCAL VOID npx_fmul_f0_f4() {
	INIT_FPU_CMD

	__asm FMUL	ST,ST(4)

	DONE_FPU_CMD
}

LOCAL VOID npx_fmul_f0_f5() {
	INIT_FPU_CMD

	__asm FMUL	ST,ST(5)

	DONE_FPU_CMD
}

LOCAL VOID npx_fmul_f0_f6() {
	INIT_FPU_CMD

	__asm FMUL	ST,ST(6)

	DONE_FPU_CMD
}

LOCAL VOID npx_fmul_f0_f7() {
	INIT_FPU_CMD

	__asm FMUL	ST,ST(7)

	DONE_FPU_CMD
}

LOCAL VOID npx_fmul_f1_f0() {
	INIT_FPU_CMD

	__asm FMUL	ST(1),ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fmul_f2_f0() {
	INIT_FPU_CMD

	__asm FMUL	ST(2),ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fmul_f3_f0() {
	INIT_FPU_CMD

	__asm FMUL	ST(3),ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fmul_f4_f0() {
	INIT_FPU_CMD

	__asm FMUL	ST(4),ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fmul_f5_f0() {
	INIT_FPU_CMD

	__asm FMUL	ST(5),ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fmul_f6_f0() {
	INIT_FPU_CMD

	__asm FMUL	ST(6),ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fmul_f7_f0() {
	INIT_FPU_CMD

	__asm FMUL	ST(7),ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fmul_short() {
	INIT_FPU_CMD

	D_Ed(0, RO0, PG_R);
	F_Ed(0);
	__asm FMUL	DWORD PTR ops[0].sng

	DONE_FPU_CMD
}

LOCAL VOID npx_fmul_long() {
	INIT_FPU_CMD

	D_E08(0, RO0, PG_R);
	F_E08(0);
	__asm FMUL	QWORD PTR ops[0].npxbuff[0]

	DONE_FPU_CMD
}

LOCAL VOID npx_fmulp_f0() {
	INIT_FPU_CMD

	__asm FMULP	ST(0),ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fmulp_f1() {
	INIT_FPU_CMD

	__asm FMULP	ST(1),ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fmulp_f2() {
	INIT_FPU_CMD

	__asm FMULP	ST(2),ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fmulp_f3() {
	INIT_FPU_CMD

	__asm FMULP	ST(3),ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fmulp_f4() {
	INIT_FPU_CMD

	__asm FMULP	ST(4),ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fmulp_f5() {
	INIT_FPU_CMD

	__asm FMULP	ST(5),ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fmulp_f6() {
	INIT_FPU_CMD

	__asm FMULP	ST(6),ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fmulp_f7() {
	INIT_FPU_CMD

	__asm FMULP	ST(7),ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fnop() {
	INIT_FPU_CMD

	__asm FNOP

	DONE_FPU_CMD
}

LOCAL VOID npx_fpatan() {
	INIT_FPU_CMD

	__asm FPATAN

	DONE_FPU_CMD
}

LOCAL VOID npx_fprem() {
	INIT_FPU_CMD

	__asm FPREM

	DONE_FPU_CMD
}

LOCAL VOID npx_fprem1() {
	INIT_FPU_CMD

	__asm FPREM1

	DONE_FPU_CMD
}

LOCAL VOID npx_fptan() {
	INIT_FPU_CMD

	__asm FPTAN

	DONE_FPU_CMD
}

LOCAL VOID npx_frndint() {
	INIT_FPU_CMD

	__asm FRNDINT

	DONE_FPU_CMD
}

LOCAL VOID npx_fscale() {
	INIT_FPU_CMD

	__asm FSCALE

	DONE_FPU_CMD
}

LOCAL VOID npx_fsin() {
	INIT_FPU_CMD

	__asm FSIN

	DONE_FPU_CMD
}

LOCAL VOID npx_fsincos() {
	INIT_FPU_CMD

	__asm FSINCOS

	DONE_FPU_CMD
}

LOCAL VOID npx_fsqrt() {
	INIT_FPU_CMD

	__asm FSQRT

	DONE_FPU_CMD
}

LOCAL VOID npx_frstor() {
	INIT_FPU_CMD

	NPX_ADDRESS_SIZE_32 = (GET_OPERAND_SIZE()==USE16)?FALSE:TRUE;
	D_E5e(0, RO0, PG_R);
	F_E5e(0);
	__asm FRSTOR ops[0].npxbuff[0]

	DONE_FPU_CMD
}

LOCAL VOID npx_fsave() {
	INIT_FPU_CMD

	NPX_ADDRESS_SIZE_32 = (GET_OPERAND_SIZE()==USE16)?FALSE:TRUE;
	D_E5e(0, WO0, PG_W);
	__asm FSAVE ops[0].npxbuff[0]
	P_E5e(0);

	DONE_FPU_CMD
}

LOCAL VOID npx_fst_f0() {
	INIT_FPU_CMD

	__asm FST	ST(0)

	DONE_FPU_CMD
}

LOCAL VOID npx_fst_f1() {
	INIT_FPU_CMD

	__asm FST	ST(1)

	DONE_FPU_CMD
}

LOCAL VOID npx_fst_f2() {
	INIT_FPU_CMD

	__asm FST	ST(2)

	DONE_FPU_CMD
}

LOCAL VOID npx_fst_f3() {
	INIT_FPU_CMD

	__asm FST	ST(3)

	DONE_FPU_CMD
}

LOCAL VOID npx_fst_f4() {
	INIT_FPU_CMD

	__asm FST	ST(4)

	DONE_FPU_CMD
}

LOCAL VOID npx_fst_f5() {
	INIT_FPU_CMD

	__asm FST	ST(5)

	DONE_FPU_CMD
}

LOCAL VOID npx_fst_f6() {
	INIT_FPU_CMD

	__asm FST	ST(6)

	DONE_FPU_CMD
}

LOCAL VOID npx_fst_f7() {
	INIT_FPU_CMD

	__asm FST	ST(7)

	DONE_FPU_CMD
}

LOCAL VOID npx_fst_short() {
	INIT_FPU_CMD

	D_Ed(0, WO0, PG_W);
	__asm FST	DWORD PTR ops[0].sng
	P_Ed(0);

	DONE_FPU_CMD
}

LOCAL VOID npx_fst_long() {
	INIT_FPU_CMD

	D_E08(0, WO0, PG_W);
	__asm FST	QWORD PTR ops[0].npxbuff[0]
	P_E08(0);

	DONE_FPU_CMD
}

LOCAL VOID npx_fstcw() {
	INIT_FPU_CMD

	D_Ew(0, WO0, PG_W);
	__asm FSTCW ops[0].sng
	P_Ew(0);

	DONE_FPU_CMD
}

LOCAL VOID npx_fstenv() {
	INIT_FPU_CMD

	NPX_ADDRESS_SIZE_32 = (GET_OPERAND_SIZE()==USE16)?FALSE:TRUE;
	D_E0e(0, WO0, PG_W);
	__asm FSTENV ops[0].npxbuff[0]
	P_E0e(0);

	DONE_FPU_CMD
}

LOCAL VOID npx_fstp_f0() {
	INIT_FPU_CMD

	__asm FSTP	ST(0)

	DONE_FPU_CMD
}

LOCAL VOID npx_fstp_f1() {
	INIT_FPU_CMD

	__asm FSTP	ST(1)

	DONE_FPU_CMD
}

LOCAL VOID npx_fstp_f2() {
	INIT_FPU_CMD

	__asm FSTP	ST(2)

	DONE_FPU_CMD
}

LOCAL VOID npx_fstp_f3() {
	INIT_FPU_CMD

	__asm FSTP	ST(3)

	DONE_FPU_CMD
}

LOCAL VOID npx_fstp_f4() {
	INIT_FPU_CMD

	__asm FSTP	ST(4)

	DONE_FPU_CMD
}

LOCAL VOID npx_fstp_f5() {
	INIT_FPU_CMD

	__asm FSTP	ST(5)

	DONE_FPU_CMD
}

LOCAL VOID npx_fstp_f6() {
	INIT_FPU_CMD

	__asm FSTP	ST(6)

	DONE_FPU_CMD
}

LOCAL VOID npx_fstp_f7() {
	INIT_FPU_CMD

	__asm FSTP	ST(7)

	DONE_FPU_CMD
}

LOCAL VOID npx_fstp_short() {
	INIT_FPU_CMD

	D_Ed(0, WO0, PG_W);
	__asm FSTP	DWORD PTR ops[0].sng
	P_Ed(0);

	DONE_FPU_CMD
}

LOCAL VOID npx_fstp_long() {
	INIT_FPU_CMD

	D_E08(0, WO0, PG_W);
	__asm FSTP	QWORD PTR ops[0].npxbuff[0]
	P_E08(0);

	DONE_FPU_CMD
}

LOCAL VOID npx_fstp_temp() {
	INIT_FPU_CMD

	D_E0a(0, WO0, PG_W);
	__asm FSTP	TBYTE PTR ops[0].npxbuff[0]
	P_E0a(0);

	DONE_FPU_CMD
}

LOCAL VOID npx_fstsw() {
	INIT_FPU_CMD

	D_Ew(0, WO0, PG_W);
	__asm FSTSW ops[0].sng
	P_Ew(0);

	DONE_FPU_CMD
}

LOCAL VOID npx_fstswax() {
	IU16 Ax_regptr;
	INIT_FPU_CMD

	__asm FSTSW ax
	__asm MOV Ax_regptr, ax
	SET_AX(Ax_regptr);

	DONE_FPU_CMD
}

LOCAL VOID npx_fsub_f0_f0() {
	INIT_FPU_CMD

	__asm FSUB	ST,ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fsub_f0_f1() {
	INIT_FPU_CMD

	__asm FSUB	ST,ST(1)

	DONE_FPU_CMD
}

LOCAL VOID npx_fsub_f0_f2() {
	INIT_FPU_CMD

	__asm FSUB	ST,ST(2)

	DONE_FPU_CMD
}

LOCAL VOID npx_fsub_f0_f3() {
	INIT_FPU_CMD

	__asm FSUB	ST,ST(3)

	DONE_FPU_CMD
}

LOCAL VOID npx_fsub_f0_f4() {
	INIT_FPU_CMD

	__asm FSUB	ST,ST(4)

	DONE_FPU_CMD
}

LOCAL VOID npx_fsub_f0_f5() {
	INIT_FPU_CMD

	__asm FSUB	ST,ST(5)

	DONE_FPU_CMD
}

LOCAL VOID npx_fsub_f0_f6() {
	INIT_FPU_CMD

	__asm FSUB	ST,ST(6)

	DONE_FPU_CMD
}

LOCAL VOID npx_fsub_f0_f7() {
	INIT_FPU_CMD

	__asm FSUB	ST,ST(7)

	DONE_FPU_CMD
}

LOCAL VOID npx_fsub_f1_f0() {
	INIT_FPU_CMD

	__asm FSUB	ST(1),ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fsub_f2_f0() {
	INIT_FPU_CMD

	__asm FSUB	ST(2),ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fsub_f3_f0() {
	INIT_FPU_CMD

	__asm FSUB	ST(3),ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fsub_f4_f0() {
	INIT_FPU_CMD

	__asm FSUB	ST(4),ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fsub_f5_f0() {
	INIT_FPU_CMD

	__asm FSUB	ST(5),ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fsub_f6_f0() {
	INIT_FPU_CMD

	__asm FSUB	ST(6),ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fsub_f7_f0() {
	INIT_FPU_CMD

	__asm FSUB	ST(7),ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fsub_short() {
	INIT_FPU_CMD

	D_Ed(0, RO0, PG_R);
	F_Ed(0);
	__asm FSUB	DWORD PTR ops[0].sng

	DONE_FPU_CMD
}

LOCAL VOID npx_fsub_long() {
	INIT_FPU_CMD

	D_E08(0, RO0, PG_R);
	F_E08(0);
	__asm FSUB	QWORD PTR ops[0].npxbuff[0]

	DONE_FPU_CMD
}

LOCAL VOID npx_fsubp_f0() {
	INIT_FPU_CMD

	__asm FSUBP	ST(0),ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fsubp_f1() {
	INIT_FPU_CMD

	__asm FSUBP	ST(1),ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fsubp_f2() {
	INIT_FPU_CMD

	__asm FSUBP	ST(2),ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fsubp_f3() {
	INIT_FPU_CMD

	__asm FSUBP	ST(3),ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fsubp_f4() {
	INIT_FPU_CMD

	__asm FSUBP	ST(4),ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fsubp_f5() {
	INIT_FPU_CMD

	__asm FSUBP	ST(5),ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fsubp_f6() {
	INIT_FPU_CMD

	__asm FSUBP	ST(6),ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fsubp_f7() {
	INIT_FPU_CMD

	__asm FSUBP	ST(7),ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fsubr_f0_f0() {
	INIT_FPU_CMD

	__asm FSUBR	ST,ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fsubr_f0_f1() {
	INIT_FPU_CMD

	__asm FSUBR	ST,ST(1)

	DONE_FPU_CMD
}

LOCAL VOID npx_fsubr_f0_f2() {
	INIT_FPU_CMD

	__asm FSUBR	ST,ST(2)

	DONE_FPU_CMD
}

LOCAL VOID npx_fsubr_f0_f3() {
	INIT_FPU_CMD

	__asm FSUBR	ST,ST(3)

	DONE_FPU_CMD
}

LOCAL VOID npx_fsubr_f0_f4() {
	INIT_FPU_CMD

	__asm FSUBR	ST,ST(4)

	DONE_FPU_CMD
}

LOCAL VOID npx_fsubr_f0_f5() {
	INIT_FPU_CMD

	__asm FSUBR	ST,ST(5)

	DONE_FPU_CMD
}

LOCAL VOID npx_fsubr_f0_f6() {
	INIT_FPU_CMD

	__asm FSUBR	ST,ST(6)

	DONE_FPU_CMD
}

LOCAL VOID npx_fsubr_f0_f7() {
	INIT_FPU_CMD

	__asm FSUBR	ST,ST(7)

	DONE_FPU_CMD
}

LOCAL VOID npx_fsubr_f1_f0() {
	INIT_FPU_CMD

	__asm FSUBR	ST(1),ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fsubr_f2_f0() {
	INIT_FPU_CMD

	__asm FSUBR	ST(2),ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fsubr_f3_f0() {
	INIT_FPU_CMD

	__asm FSUBR	ST(3),ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fsubr_f4_f0() {
	INIT_FPU_CMD

	__asm FSUBR	ST(4),ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fsubr_f5_f0() {
	INIT_FPU_CMD

	__asm FSUBR	ST(5),ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fsubr_f6_f0() {
	INIT_FPU_CMD

	__asm FSUBR	ST(6),ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fsubr_f7_f0() {
	INIT_FPU_CMD

	__asm FSUBR	ST(7),ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fsubr_short() {
	INIT_FPU_CMD

	D_Ed(0, RO0, PG_R);
	F_Ed(0);
	__asm FSUBR	DWORD PTR ops[0].sng

	DONE_FPU_CMD
}

LOCAL VOID npx_fsubr_long() {
	INIT_FPU_CMD

	D_E08(0, RO0, PG_R);
	F_E08(0);
	__asm FSUBR	QWORD PTR ops[0].npxbuff[0]

	DONE_FPU_CMD
}

LOCAL VOID npx_fsubrp_f0() {
	INIT_FPU_CMD

	__asm FSUBRP	ST(0),ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fsubrp_f1() {
	INIT_FPU_CMD

	__asm FSUBRP	ST(1),ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fsubrp_f2() {
	INIT_FPU_CMD

	__asm FSUBRP	ST(2),ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fsubrp_f3() {
	INIT_FPU_CMD

	__asm FSUBRP	ST(3),ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fsubrp_f4() {
	INIT_FPU_CMD

	__asm FSUBRP	ST(4),ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fsubrp_f5() {
	INIT_FPU_CMD

	__asm FSUBRP	ST(5),ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fsubrp_f6() {
	INIT_FPU_CMD

	__asm FSUBRP	ST(6),ST

	DONE_FPU_CMD
}

LOCAL VOID npx_fsubrp_f7() {
	INIT_FPU_CMD

	__asm FSUBRP	ST(7),ST

	DONE_FPU_CMD
}

LOCAL VOID npx_ftst() {
	INIT_FPU_CMD

	__asm FTST

	DONE_FPU_CMD
}

LOCAL VOID npx_fucom_f0() {
	INIT_FPU_CMD

	__asm FUCOM	ST(0)

	DONE_FPU_CMD
}

LOCAL VOID npx_fucom_f1() {
	INIT_FPU_CMD

	__asm FUCOM	ST(1)

	DONE_FPU_CMD
}

LOCAL VOID npx_fucom_f2() {
	INIT_FPU_CMD

	__asm FUCOM	ST(2)

	DONE_FPU_CMD
}

LOCAL VOID npx_fucom_f3() {
	INIT_FPU_CMD

	__asm FUCOM	ST(3)

	DONE_FPU_CMD
}

LOCAL VOID npx_fucom_f4() {
	INIT_FPU_CMD

	__asm FUCOM	ST(4)

	DONE_FPU_CMD
}

LOCAL VOID npx_fucom_f5() {
	INIT_FPU_CMD

	__asm FUCOM	ST(5)

	DONE_FPU_CMD
}

LOCAL VOID npx_fucom_f6() {
	INIT_FPU_CMD

	__asm FUCOM	ST(6)

	DONE_FPU_CMD
}

LOCAL VOID npx_fucom_f7() {
	INIT_FPU_CMD

	__asm FUCOM	ST(7)

	DONE_FPU_CMD
}

LOCAL VOID npx_fucomp_f0() {
	INIT_FPU_CMD

	__asm FUCOMP	ST(0)

	DONE_FPU_CMD
}

LOCAL VOID npx_fucomp_f1() {
	INIT_FPU_CMD

	__asm FUCOMP	ST(1)

	DONE_FPU_CMD
}

LOCAL VOID npx_fucomp_f2() {
	INIT_FPU_CMD

	__asm FUCOMP	ST(2)

	DONE_FPU_CMD
}

LOCAL VOID npx_fucomp_f3() {
	INIT_FPU_CMD

	__asm FUCOMP	ST(3)

	DONE_FPU_CMD
}

LOCAL VOID npx_fucomp_f4() {
	INIT_FPU_CMD

	__asm FUCOMP	ST(4)

	DONE_FPU_CMD
}

LOCAL VOID npx_fucomp_f5() {
	INIT_FPU_CMD

	__asm FUCOMP	ST(5)

	DONE_FPU_CMD
}

LOCAL VOID npx_fucomp_f6() {
	INIT_FPU_CMD

	__asm FUCOMP	ST(6)

	DONE_FPU_CMD
}

LOCAL VOID npx_fucomp_f7() {
	INIT_FPU_CMD

	__asm FUCOMP	ST(7)

	DONE_FPU_CMD
}

LOCAL VOID npx_fucompp() {
	INIT_FPU_CMD

	__asm FUCOMPP

	DONE_FPU_CMD
}

LOCAL VOID npx_fxam() {
	INIT_FPU_CMD

	__asm FXAM

	DONE_FPU_CMD
}

LOCAL VOID npx_fxch_f0() {
	INIT_FPU_CMD

	__asm FXCH	ST(0)

	DONE_FPU_CMD
}

LOCAL VOID npx_fxch_f1() {
	INIT_FPU_CMD

	__asm FXCH	ST(1)

	DONE_FPU_CMD
}

LOCAL VOID npx_fxch_f2() {
	INIT_FPU_CMD

	__asm FXCH	ST(2)

	DONE_FPU_CMD
}

LOCAL VOID npx_fxch_f3() {
	INIT_FPU_CMD

	__asm FXCH	ST(3)

	DONE_FPU_CMD
}

LOCAL VOID npx_fxch_f4() {
	INIT_FPU_CMD

	__asm FXCH	ST(4)

	DONE_FPU_CMD
}

LOCAL VOID npx_fxch_f5() {
	INIT_FPU_CMD

	__asm FXCH	ST(5)

	DONE_FPU_CMD
}

LOCAL VOID npx_fxch_f6() {
	INIT_FPU_CMD

	__asm FXCH	ST(6)

	DONE_FPU_CMD
}

LOCAL VOID npx_fxch_f7() {
	INIT_FPU_CMD

	__asm FXCH	ST(7)

	DONE_FPU_CMD
}

LOCAL VOID npx_fxtract() {
	INIT_FPU_CMD

	__asm FXTRACT

	DONE_FPU_CMD
}

LOCAL VOID npx_fyl2x() {
	INIT_FPU_CMD

	__asm FYL2X

	DONE_FPU_CMD
}

LOCAL VOID npx_fyl2xp1() {
	INIT_FPU_CMD

	__asm FYL2XP1

	DONE_FPU_CMD
}

LOCAL VOID npx_f2xm1() {
	INIT_FPU_CMD

	__asm F2XM1

	DONE_FPU_CMD
}

LOCAL VOID npx_funimp() {
	Int6();
}

LOCAL VOID npx_finit() {
	CONTEXT context;
	IU32 NpxControl = 0x037f;

	m_context.ContextFlags = CONTEXT_FLOATING_POINT;
	GetThreadContext(GetCurrentThread(), &m_context);
	memcpy(&context, &m_context, sizeof(context));

	//_controlfp(0, 0x037f);
	__asm FINIT
	__asm FSTCW NpxControl

	DONE_FPU_CMD
}

LOCAL VOID (*inst_table[])() = {
npx_fadd_short,		/* d8 00 */
npx_fadd_short,		/* d8 01 */
npx_fadd_short,		/* d8 02 */
npx_fadd_short,		/* d8 03 */
npx_fadd_short,		/* d8 04 */
npx_fadd_short,		/* d8 05 */
npx_fadd_short,		/* d8 06 */
npx_fadd_short,		/* d8 07 */
npx_fmul_short,		/* d8 08 */
npx_fmul_short,		/* d8 09 */
npx_fmul_short,		/* d8 0a */
npx_fmul_short,		/* d8 0b */
npx_fmul_short,		/* d8 0c */
npx_fmul_short,		/* d8 0d */
npx_fmul_short,		/* d8 0e */
npx_fmul_short,		/* d8 0f */
npx_fcom_short,		/* d8 10 */
npx_fcom_short,		/* d8 11 */
npx_fcom_short,		/* d8 12 */
npx_fcom_short,		/* d8 13 */
npx_fcom_short,		/* d8 14 */
npx_fcom_short,		/* d8 15 */
npx_fcom_short,		/* d8 16 */
npx_fcom_short,		/* d8 17 */
npx_fcomp_short,	/* d8 18 */
npx_fcomp_short,	/* d8 19 */
npx_fcomp_short,	/* d8 1a */
npx_fcomp_short,	/* d8 1b */
npx_fcomp_short,	/* d8 1c */
npx_fcomp_short,	/* d8 1d */
npx_fcomp_short,	/* d8 1e */
npx_fcomp_short,	/* d8 1f */
npx_fsub_short,		/* d8 20 */
npx_fsub_short,		/* d8 21 */
npx_fsub_short,		/* d8 22 */
npx_fsub_short,		/* d8 23 */
npx_fsub_short,		/* d8 24 */
npx_fsub_short,		/* d8 25 */
npx_fsub_short,		/* d8 26 */
npx_fsub_short,		/* d8 27 */
npx_fsubr_short,	/* d8 28 */
npx_fsubr_short,	/* d8 29 */
npx_fsubr_short,	/* d8 2a */
npx_fsubr_short,	/* d8 2b */
npx_fsubr_short,	/* d8 2c */
npx_fsubr_short,	/* d8 2d */
npx_fsubr_short,	/* d8 2e */
npx_fsubr_short,	/* d8 2f */
npx_fdiv_short,		/* d8 30 */
npx_fdiv_short,		/* d8 31 */
npx_fdiv_short,		/* d8 32 */
npx_fdiv_short,		/* d8 33 */
npx_fdiv_short,		/* d8 34 */
npx_fdiv_short,		/* d8 35 */
npx_fdiv_short,		/* d8 36 */
npx_fdiv_short,		/* d8 37 */
npx_fdivr_short,	/* d8 38 */
npx_fdivr_short,	/* d8 39 */
npx_fdivr_short,	/* d8 3a */
npx_fdivr_short,	/* d8 3b */
npx_fdivr_short,	/* d8 3c */
npx_fdivr_short,	/* d8 3d */
npx_fdivr_short,	/* d8 3e */
npx_fdivr_short,	/* d8 3f */
npx_fadd_short,		/* d8 40 */
npx_fadd_short,		/* d8 41 */
npx_fadd_short,		/* d8 42 */
npx_fadd_short,		/* d8 43 */
npx_fadd_short,		/* d8 44 */
npx_fadd_short,		/* d8 45 */
npx_fadd_short,		/* d8 46 */
npx_fadd_short,		/* d8 47 */
npx_fmul_short,		/* d8 48 */
npx_fmul_short,		/* d8 49 */
npx_fmul_short,		/* d8 4a */
npx_fmul_short,		/* d8 4b */
npx_fmul_short,		/* d8 4c */
npx_fmul_short,		/* d8 4d */
npx_fmul_short,		/* d8 4e */
npx_fmul_short,		/* d8 4f */
npx_fcom_short,		/* d8 50 */
npx_fcom_short,		/* d8 51 */
npx_fcom_short,		/* d8 52 */
npx_fcom_short,		/* d8 53 */
npx_fcom_short,		/* d8 54 */
npx_fcom_short,		/* d8 55 */
npx_fcom_short,		/* d8 56 */
npx_fcom_short,		/* d8 57 */
npx_fcomp_short,	/* d8 58 */
npx_fcomp_short,	/* d8 59 */
npx_fcomp_short,	/* d8 5a */
npx_fcomp_short,	/* d8 5b */
npx_fcomp_short,	/* d8 5c */
npx_fcomp_short,	/* d8 5d */
npx_fcomp_short,	/* d8 5e */
npx_fcomp_short,	/* d8 5f */
npx_fsub_short,		/* d8 60 */
npx_fsub_short,		/* d8 61 */
npx_fsub_short,		/* d8 62 */
npx_fsub_short,		/* d8 63 */
npx_fsub_short,		/* d8 64 */
npx_fsub_short,		/* d8 65 */
npx_fsub_short,		/* d8 66 */
npx_fsub_short,		/* d8 67 */
npx_fsubr_short,	/* d8 68 */
npx_fsubr_short,	/* d8 69 */
npx_fsubr_short,	/* d8 6a */
npx_fsubr_short,	/* d8 6b */
npx_fsubr_short,	/* d8 6c */
npx_fsubr_short,	/* d8 6d */
npx_fsubr_short,	/* d8 6e */
npx_fsubr_short,	/* d8 6f */
npx_fdiv_short,		/* d8 70 */
npx_fdiv_short,		/* d8 71 */
npx_fdiv_short,		/* d8 72 */
npx_fdiv_short,		/* d8 73 */
npx_fdiv_short,		/* d8 74 */
npx_fdiv_short,		/* d8 75 */
npx_fdiv_short,		/* d8 76 */
npx_fdiv_short,		/* d8 77 */
npx_fdivr_short,	/* d8 78 */
npx_fdivr_short,	/* d8 79 */
npx_fdivr_short,	/* d8 7a */
npx_fdivr_short,	/* d8 7b */
npx_fdivr_short,	/* d8 7c */
npx_fdivr_short,	/* d8 7d */
npx_fdivr_short,	/* d8 7e */
npx_fdivr_short,	/* d8 7f */
npx_fadd_short,		/* d8 80 */
npx_fadd_short,		/* d8 81 */
npx_fadd_short,		/* d8 82 */
npx_fadd_short,		/* d8 83 */
npx_fadd_short,		/* d8 84 */
npx_fadd_short,		/* d8 85 */
npx_fadd_short,		/* d8 86 */
npx_fadd_short,		/* d8 87 */
npx_fmul_short,		/* d8 88 */
npx_fmul_short,		/* d8 89 */
npx_fmul_short,		/* d8 8a */
npx_fmul_short,		/* d8 8b */
npx_fmul_short,		/* d8 8c */
npx_fmul_short,		/* d8 8d */
npx_fmul_short,		/* d8 8e */
npx_fmul_short,		/* d8 8f */
npx_fcom_short,		/* d8 90 */
npx_fcom_short,		/* d8 91 */
npx_fcom_short,		/* d8 92 */
npx_fcom_short,		/* d8 93 */
npx_fcom_short,		/* d8 94 */
npx_fcom_short,		/* d8 95 */
npx_fcom_short,		/* d8 96 */
npx_fcom_short,		/* d8 97 */
npx_fcomp_short,	/* d8 98 */
npx_fcomp_short,	/* d8 99 */
npx_fcomp_short,	/* d8 9a */
npx_fcomp_short,	/* d8 9b */
npx_fcomp_short,	/* d8 9c */
npx_fcomp_short,	/* d8 9d */
npx_fcomp_short,	/* d8 9e */
npx_fcomp_short,	/* d8 9f */
npx_fsub_short,		/* d8 a0 */
npx_fsub_short,		/* d8 a1 */
npx_fsub_short,		/* d8 a2 */
npx_fsub_short,		/* d8 a3 */
npx_fsub_short,		/* d8 a4 */
npx_fsub_short,		/* d8 a5 */
npx_fsub_short,		/* d8 a6 */
npx_fsub_short,		/* d8 a7 */
npx_fsubr_short,	/* d8 a8 */
npx_fsubr_short,	/* d8 a9 */
npx_fsubr_short,	/* d8 aa */
npx_fsubr_short,	/* d8 ab */
npx_fsubr_short,	/* d8 ac */
npx_fsubr_short,	/* d8 ad */
npx_fsubr_short,	/* d8 ae */
npx_fsubr_short,	/* d8 af */
npx_fdiv_short,		/* d8 b0 */
npx_fdiv_short,		/* d8 b1 */
npx_fdiv_short,		/* d8 b2 */
npx_fdiv_short,		/* d8 b3 */
npx_fdiv_short,		/* d8 b4 */
npx_fdiv_short,		/* d8 b5 */
npx_fdiv_short,		/* d8 b6 */
npx_fdiv_short,		/* d8 b7 */
npx_fdivr_short,	/* d8 b8 */
npx_fdivr_short,	/* d8 b9 */
npx_fdivr_short,	/* d8 ba */
npx_fdivr_short,	/* d8 bb */
npx_fdivr_short,	/* d8 bc */
npx_fdivr_short,	/* d8 bd */
npx_fdivr_short,	/* d8 be */
npx_fdivr_short,	/* d8 bf */
npx_fadd_f0_f0,		/* d8 c0 */
npx_fadd_f0_f1,
npx_fadd_f0_f2,
npx_fadd_f0_f3,
npx_fadd_f0_f4,
npx_fadd_f0_f5,
npx_fadd_f0_f6,
npx_fadd_f0_f7,
npx_fmul_f0_f0,		/* d8 c7 */
npx_fmul_f0_f1,
npx_fmul_f0_f2,
npx_fmul_f0_f3,
npx_fmul_f0_f4,
npx_fmul_f0_f5,
npx_fmul_f0_f6,
npx_fmul_f0_f7,
npx_fcom_f0,		/* d8 d0 */
npx_fcom_f1,	
npx_fcom_f2,	
npx_fcom_f3,	
npx_fcom_f4,	
npx_fcom_f5,	
npx_fcom_f6,	
npx_fcom_f7,	
npx_fcomp_f0,	
npx_fcomp_f1,	
npx_fcomp_f2,	
npx_fcomp_f3,	
npx_fcomp_f4,	
npx_fcomp_f5,	
npx_fcomp_f6,	
npx_fcomp_f7,	
npx_fsub_f0_f0,		/* d8 e0 */
npx_fsub_f0_f1,
npx_fsub_f0_f2,
npx_fsub_f0_f3,
npx_fsub_f0_f4,
npx_fsub_f0_f5,
npx_fsub_f0_f6,
npx_fsub_f0_f7,
npx_fsubr_f0_f0,
npx_fsubr_f0_f1,
npx_fsubr_f0_f2,
npx_fsubr_f0_f3,
npx_fsubr_f0_f4,
npx_fsubr_f0_f5,
npx_fsubr_f0_f6,
npx_fsubr_f0_f7,
npx_fdiv_f0_f0,		/* d8 f0 */
npx_fdiv_f0_f1,
npx_fdiv_f0_f2,
npx_fdiv_f0_f3,
npx_fdiv_f0_f4,
npx_fdiv_f0_f5,
npx_fdiv_f0_f6,
npx_fdiv_f0_f7,
npx_fdivr_f0_f0,
npx_fdivr_f0_f1,
npx_fdivr_f0_f2,
npx_fdivr_f0_f3,
npx_fdivr_f0_f4,
npx_fdivr_f0_f5,
npx_fdivr_f0_f6,
npx_fdivr_f0_f7,
npx_fld_short,		/* d9 00 */
npx_fld_short,		/* d9 01 */
npx_fld_short,		/* d9 02 */
npx_fld_short,		/* d9 03 */
npx_fld_short,		/* d9 04 */
npx_fld_short,		/* d9 05 */
npx_fld_short,		/* d9 06 */
npx_fld_short,		/* d9 07 */
npx_funimp,		/* d9 08 */
npx_funimp,		/* d9 09 */
npx_funimp,		/* d9 0a */
npx_funimp,		/* d9 0b */
npx_funimp,		/* d9 0c */
npx_funimp,		/* d9 0d */
npx_funimp,		/* d9 0e */
npx_funimp,		/* d9 0f */
npx_fst_short,		/* d9 10 */
npx_fst_short,		/* d9 11 */
npx_fst_short,		/* d9 12 */
npx_fst_short,		/* d9 13 */
npx_fst_short,		/* d9 14 */
npx_fst_short,		/* d9 15 */
npx_fst_short,		/* d9 16 */
npx_fst_short,		/* d9 17 */
npx_fstp_short,		/* d9 18 */
npx_fstp_short,		/* d9 19 */
npx_fstp_short,		/* d9 1a */
npx_fstp_short,		/* d9 1b */
npx_fstp_short,		/* d9 1c */
npx_fstp_short,		/* d9 1d */
npx_fstp_short,		/* d9 1e */
npx_fstp_short,		/* d9 1f */
npx_fldenv,		/* d9 20 */
npx_fldenv,		/* d9 21 */
npx_fldenv,		/* d9 22 */
npx_fldenv,		/* d9 23 */
npx_fldenv,		/* d9 24 */
npx_fldenv,		/* d9 25 */
npx_fldenv,		/* d9 26 */
npx_fldenv,		/* d9 27 */
npx_fldcw,		/* d9 28 */
npx_fldcw,		/* d9 29 */
npx_fldcw,		/* d9 2a */
npx_fldcw,		/* d9 2b */
npx_fldcw,		/* d9 2c */
npx_fldcw,		/* d9 2d */
npx_fldcw,		/* d9 2e */
npx_fldcw,		/* d9 2f */
npx_fstenv,		/* d9 30 */
npx_fstenv,		/* d9 31 */
npx_fstenv,		/* d9 32 */
npx_fstenv,		/* d9 33 */
npx_fstenv,		/* d9 34 */
npx_fstenv,		/* d9 35 */
npx_fstenv,		/* d9 36 */
npx_fstenv,		/* d9 37 */
npx_fstcw,		/* d9 38 */
npx_fstcw,		/* d9 39 */
npx_fstcw,		/* d9 3a */
npx_fstcw,		/* d9 3b */
npx_fstcw,		/* d9 3c */
npx_fstcw,		/* d9 3d */
npx_fstcw,		/* d9 3e */
npx_fstcw,		/* d9 3f */
npx_fld_short,		/* d9 40 */
npx_fld_short,		/* d9 41 */
npx_fld_short,		/* d9 42 */
npx_fld_short,		/* d9 43 */
npx_fld_short,		/* d9 44 */
npx_fld_short,		/* d9 45 */
npx_fld_short,		/* d9 46 */
npx_fld_short,		/* d9 47 */
npx_funimp,		/* d9 48 */
npx_funimp,		/* d9 49 */
npx_funimp,		/* d9 4a */
npx_funimp,		/* d9 4b */
npx_funimp,		/* d9 4c */
npx_funimp,		/* d9 4d */
npx_funimp,		/* d9 4e */
npx_funimp,		/* d9 4f */
npx_fst_short,		/* d9 50 */
npx_fst_short,		/* d9 51 */
npx_fst_short,		/* d9 52 */
npx_fst_short,		/* d9 53 */
npx_fst_short,		/* d9 54 */
npx_fst_short,		/* d9 55 */
npx_fst_short,		/* d9 56 */
npx_fst_short,		/* d9 57 */
npx_fstp_short,		/* d9 58 */
npx_fstp_short,		/* d9 59 */
npx_fstp_short,		/* d9 5a */
npx_fstp_short,		/* d9 5b */
npx_fstp_short,		/* d9 5c */
npx_fstp_short,		/* d9 5d */
npx_fstp_short,		/* d9 5e */
npx_fstp_short,		/* d9 5f */
npx_fldenv,		/* d9 60 */
npx_fldenv,		/* d9 61 */
npx_fldenv,		/* d9 62 */
npx_fldenv,		/* d9 63 */
npx_fldenv,		/* d9 64 */
npx_fldenv,		/* d9 65 */
npx_fldenv,		/* d9 66 */
npx_fldenv,		/* d9 67 */
npx_fldcw,		/* d9 68 */
npx_fldcw,		/* d9 69 */
npx_fldcw,		/* d9 6a */
npx_fldcw,		/* d9 6b */
npx_fldcw,		/* d9 6c */
npx_fldcw,		/* d9 6d */
npx_fldcw,		/* d9 6e */
npx_fldcw,		/* d9 6f */
npx_fstenv,		/* d9 70 */
npx_fstenv,		/* d9 71 */
npx_fstenv,		/* d9 72 */
npx_fstenv,		/* d9 73 */
npx_fstenv,		/* d9 74 */
npx_fstenv,		/* d9 75 */
npx_fstenv,		/* d9 76 */
npx_fstenv,		/* d9 77 */
npx_fstcw,		/* d9 78 */
npx_fstcw,		/* d9 79 */
npx_fstcw,		/* d9 7a */
npx_fstcw,		/* d9 7b */
npx_fstcw,		/* d9 7c */
npx_fstcw,		/* d9 7d */
npx_fstcw,		/* d9 7e */
npx_fstcw,		/* d9 7f */
npx_fld_short,		/* d9 80 */
npx_fld_short,		/* d9 81 */
npx_fld_short,		/* d9 82 */
npx_fld_short,		/* d9 83 */
npx_fld_short,		/* d9 84 */
npx_fld_short,		/* d9 85 */
npx_fld_short,		/* d9 86 */
npx_fld_short,		/* d9 87 */
npx_funimp,		/* d9 88 */
npx_funimp,		/* d9 89 */
npx_funimp,		/* d9 8a */
npx_funimp,		/* d9 8b */
npx_funimp,		/* d9 8c */
npx_funimp,		/* d9 8d */
npx_funimp,		/* d9 8e */
npx_funimp,		/* d9 8f */
npx_fst_short,		/* d9 90 */
npx_fst_short,		/* d9 91 */
npx_fst_short,		/* d9 92 */
npx_fst_short,		/* d9 93 */
npx_fst_short,		/* d9 94 */
npx_fst_short,		/* d9 95 */
npx_fst_short,		/* d9 96 */
npx_fst_short,		/* d9 97 */
npx_fstp_short,		/* d9 98 */
npx_fstp_short,		/* d9 99 */
npx_fstp_short,		/* d9 9a */
npx_fstp_short,		/* d9 9b */
npx_fstp_short,		/* d9 9c */
npx_fstp_short,		/* d9 9d */
npx_fstp_short,		/* d9 9e */
npx_fstp_short,		/* d9 9f */
npx_fldenv,		/* d9 a0 */
npx_fldenv,		/* d9 a1 */
npx_fldenv,		/* d9 a2 */
npx_fldenv,		/* d9 a3 */
npx_fldenv,		/* d9 a4 */
npx_fldenv,		/* d9 a5 */
npx_fldenv,		/* d9 a6 */
npx_fldenv,		/* d9 a7 */
npx_fldcw,		/* d9 a8 */
npx_fldcw,		/* d9 a9 */
npx_fldcw,		/* d9 aa */
npx_fldcw,		/* d9 ab */
npx_fldcw,		/* d9 ac */
npx_fldcw,		/* d9 ad */
npx_fldcw,		/* d9 ae */
npx_fldcw,		/* d9 af */
npx_fstenv,		/* d9 b0 */
npx_fstenv,		/* d9 b1 */
npx_fstenv,		/* d9 b2 */
npx_fstenv,		/* d9 b3 */
npx_fstenv,		/* d9 b4 */
npx_fstenv,		/* d9 b5 */
npx_fstenv,		/* d9 b6 */
npx_fstenv,		/* d9 b7 */
npx_fstcw,		/* d9 b8 */
npx_fstcw,		/* d9 b9 */
npx_fstcw,		/* d9 ba */
npx_fstcw,		/* d9 bb */
npx_fstcw,		/* d9 bc */
npx_fstcw,		/* d9 bd */
npx_fstcw,		/* d9 be */
npx_fstcw,		/* d9 bf */
npx_fld_f0,		/* d9 c0 */
npx_fld_f1,	
npx_fld_f2,	
npx_fld_f3,	
npx_fld_f4,	
npx_fld_f5,	
npx_fld_f6,	
npx_fld_f7,
npx_fxch_f0,
npx_fxch_f1,
npx_fxch_f2,
npx_fxch_f3,
npx_fxch_f4,
npx_fxch_f5,
npx_fxch_f6,
npx_fxch_f7,
npx_fnop,		/* d9 d0 */
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_fstp_f0,
npx_fstp_f1,
npx_fstp_f2,
npx_fstp_f3,
npx_fstp_f4,
npx_fstp_f5,
npx_fstp_f6,
npx_fstp_f7,
npx_fchs,		/* d9 e0 */
npx_fabs,		/* d9 e1 */
npx_funimp,
npx_funimp,
npx_ftst,		/* d9 e4 */
npx_fxam,		/* d9 e5 */
npx_funimp,
npx_funimp,
npx_fld1,		/* d9 e8 */
npx_fldl2t,		/* d9 e9 */
npx_fldl2e,		/* d9 ea */
npx_fldpi,		/* d9 eb */
npx_fldlg2,		/* d9 ec */
npx_fldln2,		/* d9 ed */
npx_fldz,		/* d9 ee */
npx_funimp,
npx_f2xm1,		/* d9 f0 */
npx_fyl2x,		/* d9 f1 */
npx_fptan,		/* d9 f2 */
npx_fpatan,		/* d9 f3 */
npx_fxtract,		/* d9 f4 */
npx_fprem1,		/* d9 f5 */
npx_fdecstp,		/* d9 f6 */
npx_fincstp,		/* d9 f7 */
npx_fprem,		/* d9 f8 */
npx_fyl2xp1,		/* d9 f9 */
npx_fsqrt,		/* d9 fa */
npx_fsincos,		/* d9 fb */
npx_frndint,		/* d9 fc */
npx_fscale,		/* d9 fd */
npx_fsin,		/* d9 fe */
npx_fcos,		/* d9 ff */
npx_fiadd_short,	/* da 00 */
npx_fiadd_short,	/* da 01 */
npx_fiadd_short,	/* da 02 */
npx_fiadd_short,	/* da 03 */
npx_fiadd_short,	/* da 04 */
npx_fiadd_short,	/* da 05 */
npx_fiadd_short,	/* da 06 */
npx_fiadd_short,	/* da 07 */
npx_fimul_short,	/* da 08 */
npx_fimul_short,	/* da 09 */
npx_fimul_short,	/* da 0a */
npx_fimul_short,	/* da 0b */
npx_fimul_short,	/* da 0c */
npx_fimul_short,	/* da 0d */
npx_fimul_short,	/* da 0e */
npx_fimul_short,	/* da 0f */
npx_ficom_short,	/* da 10 */
npx_ficom_short,	/* da 11 */
npx_ficom_short,	/* da 12 */
npx_ficom_short,	/* da 13 */
npx_ficom_short,	/* da 14 */
npx_ficom_short,	/* da 15 */
npx_ficom_short,	/* da 16 */
npx_ficom_short,	/* da 17 */
npx_ficomp_short,	/* da 18 */
npx_ficomp_short,	/* da 19 */
npx_ficomp_short,	/* da 1a */
npx_ficomp_short,	/* da 1b */
npx_ficomp_short,	/* da 1c */
npx_ficomp_short,	/* da 1d */
npx_ficomp_short,	/* da 1e */
npx_ficomp_short,	/* da 1f */
npx_fisub_short,	/* da 20 */
npx_fisub_short,	/* da 21 */
npx_fisub_short,	/* da 22 */
npx_fisub_short,	/* da 23 */
npx_fisub_short,	/* da 24 */
npx_fisub_short,	/* da 25 */
npx_fisub_short,	/* da 26 */
npx_fisub_short,	/* da 27 */
npx_fisubr_short,	/* da 28 */
npx_fisubr_short,	/* da 29 */
npx_fisubr_short,	/* da 2a */
npx_fisubr_short,	/* da 2b */
npx_fisubr_short,	/* da 2c */
npx_fisubr_short,	/* da 2d */
npx_fisubr_short,	/* da 2e */
npx_fisubr_short,	/* da 2f */
npx_fidiv_short,	/* da 30 */
npx_fidiv_short,	/* da 31 */
npx_fidiv_short,	/* da 32 */
npx_fidiv_short,	/* da 33 */
npx_fidiv_short,	/* da 34 */
npx_fidiv_short,	/* da 35 */
npx_fidiv_short,	/* da 36 */
npx_fidiv_short,	/* da 37 */
npx_fidivr_short,	/* da 38 */
npx_fidivr_short,	/* da 39 */
npx_fidivr_short,	/* da 3a */
npx_fidivr_short,	/* da 3b */
npx_fidivr_short,	/* da 3c */
npx_fidivr_short,	/* da 3d */
npx_fidivr_short,	/* da 3e */
npx_fidivr_short,	/* da 3f */
npx_fiadd_short,	/* da 40 */
npx_fiadd_short,	/* da 41 */
npx_fiadd_short,	/* da 42 */
npx_fiadd_short,	/* da 43 */
npx_fiadd_short,	/* da 44 */
npx_fiadd_short,	/* da 45 */
npx_fiadd_short,	/* da 46 */
npx_fiadd_short,	/* da 47 */
npx_fimul_short,	/* da 48 */
npx_fimul_short,	/* da 49 */
npx_fimul_short,	/* da 4a */
npx_fimul_short,	/* da 4b */
npx_fimul_short,	/* da 4c */
npx_fimul_short,	/* da 4d */
npx_fimul_short,	/* da 4e */
npx_fimul_short,	/* da 4f */
npx_ficom_short,	/* da 50 */
npx_ficom_short,	/* da 51 */
npx_ficom_short,	/* da 52 */
npx_ficom_short,	/* da 53 */
npx_ficom_short,	/* da 54 */
npx_ficom_short,	/* da 55 */
npx_ficom_short,	/* da 56 */
npx_ficom_short,	/* da 57 */
npx_ficomp_short,	/* da 58 */
npx_ficomp_short,	/* da 59 */
npx_ficomp_short,	/* da 5a */
npx_ficomp_short,	/* da 5b */
npx_ficomp_short,	/* da 5c */
npx_ficomp_short,	/* da 5d */
npx_ficomp_short,	/* da 5e */
npx_ficomp_short,	/* da 5f */
npx_fisub_short,	/* da 60 */
npx_fisub_short,	/* da 61 */
npx_fisub_short,	/* da 62 */
npx_fisub_short,	/* da 63 */
npx_fisub_short,	/* da 64 */
npx_fisub_short,	/* da 65 */
npx_fisub_short,	/* da 66 */
npx_fisub_short,	/* da 67 */
npx_fisubr_short,	/* da 68 */
npx_fisubr_short,	/* da 69 */
npx_fisubr_short,	/* da 6a */
npx_fisubr_short,	/* da 6b */
npx_fisubr_short,	/* da 6c */
npx_fisubr_short,	/* da 6d */
npx_fisubr_short,	/* da 6e */
npx_fisubr_short,	/* da 6f */
npx_fidiv_short,	/* da 70 */
npx_fidiv_short,	/* da 71 */
npx_fidiv_short,	/* da 72 */
npx_fidiv_short,	/* da 73 */
npx_fidiv_short,	/* da 74 */
npx_fidiv_short,	/* da 75 */
npx_fidiv_short,	/* da 76 */
npx_fidiv_short,	/* da 77 */
npx_fidivr_short,	/* da 78 */
npx_fidivr_short,	/* da 79 */
npx_fidivr_short,	/* da 7a */
npx_fidivr_short,	/* da 7b */
npx_fidivr_short,	/* da 7c */
npx_fidivr_short,	/* da 7d */
npx_fidivr_short,	/* da 7e */
npx_fidivr_short,	/* da 7f */
npx_fiadd_short,	/* da 80 */
npx_fiadd_short,	/* da 81 */
npx_fiadd_short,	/* da 82 */
npx_fiadd_short,	/* da 83 */
npx_fiadd_short,	/* da 84 */
npx_fiadd_short,	/* da 85 */
npx_fiadd_short,	/* da 86 */
npx_fiadd_short,	/* da 87 */
npx_fimul_short,	/* da 88 */
npx_fimul_short,	/* da 89 */
npx_fimul_short,	/* da 8a */
npx_fimul_short,	/* da 8b */
npx_fimul_short,	/* da 8c */
npx_fimul_short,	/* da 8d */
npx_fimul_short,	/* da 8e */
npx_fimul_short,	/* da 8f */
npx_ficom_short,	/* da 90 */
npx_ficom_short,	/* da 91 */
npx_ficom_short,	/* da 92 */
npx_ficom_short,	/* da 93 */
npx_ficom_short,	/* da 94 */
npx_ficom_short,	/* da 95 */
npx_ficom_short,	/* da 96 */
npx_ficom_short,	/* da 97 */
npx_ficomp_short,	/* da 98 */
npx_ficomp_short,	/* da 99 */
npx_ficomp_short,	/* da 9a */
npx_ficomp_short,	/* da 9b */
npx_ficomp_short,	/* da 9c */
npx_ficomp_short,	/* da 9d */
npx_ficomp_short,	/* da 9e */
npx_ficomp_short,	/* da 9f */
npx_fisub_short,	/* da a0 */
npx_fisub_short,	/* da a1 */
npx_fisub_short,	/* da a2 */
npx_fisub_short,	/* da a3 */
npx_fisub_short,	/* da a4 */
npx_fisub_short,	/* da a5 */
npx_fisub_short,	/* da a6 */
npx_fisub_short,	/* da a7 */
npx_fisubr_short,	/* da a8 */
npx_fisubr_short,	/* da a9 */
npx_fisubr_short,	/* da aa */
npx_fisubr_short,	/* da ab */
npx_fisubr_short,	/* da ac */
npx_fisubr_short,	/* da ad */
npx_fisubr_short,	/* da ae */
npx_fisubr_short,	/* da af */
npx_fidiv_short,	/* da b0 */
npx_fidiv_short,	/* da b1 */
npx_fidiv_short,	/* da b2 */
npx_fidiv_short,	/* da b3 */
npx_fidiv_short,	/* da b4 */
npx_fidiv_short,	/* da b5 */
npx_fidiv_short,	/* da b6 */
npx_fidiv_short,	/* da b7 */
npx_fidivr_short,	/* da b8 */
npx_fidivr_short,	/* da b9 */
npx_fidivr_short,	/* da ba */
npx_fidivr_short,	/* da bb */
npx_fidivr_short,	/* da bc */
npx_fidivr_short,	/* da bd */
npx_fidivr_short,	/* da be */
npx_fidivr_short,	/* da bf */
npx_funimp,		/* da c0 */
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,	
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,		/* da d0 */
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,	
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,		/* da e0 */
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_fucompp,		/* da e9 */
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,		/* da f0 */
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,	
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_fild_short,		/* db 00 */
npx_fild_short,		/* db 01 */
npx_fild_short,		/* db 02 */
npx_fild_short,		/* db 03 */
npx_fild_short,		/* db 04 */
npx_fild_short,		/* db 05 */
npx_fild_short,		/* db 06 */
npx_fild_short,		/* db 07 */
npx_funimp,		/* db 08 */
npx_funimp,		/* db 09 */
npx_funimp,		/* db 0a */
npx_funimp,		/* db 0b */
npx_funimp,		/* db 0c */
npx_funimp,		/* db 0d */
npx_funimp,		/* db 0e */
npx_funimp,		/* db 0f */
npx_fist_short,		/* db 10 */
npx_fist_short,		/* db 11 */
npx_fist_short,		/* db 12 */
npx_fist_short,		/* db 13 */
npx_fist_short,		/* db 14 */
npx_fist_short,		/* db 15 */
npx_fist_short,		/* db 16 */
npx_fist_short,		/* db 17 */
npx_fistp_short,	/* db 18 */
npx_fistp_short,	/* db 19 */
npx_fistp_short,	/* db 1a */
npx_fistp_short,	/* db 1b */
npx_fistp_short,	/* db 1c */
npx_fistp_short,	/* db 1d */
npx_fistp_short,	/* db 1e */
npx_fistp_short,	/* db 1f */
npx_funimp,		/* db 20 */
npx_funimp,		/* db 21 */
npx_funimp,		/* db 22 */
npx_funimp,		/* db 23 */
npx_funimp,		/* db 24 */
npx_funimp,		/* db 25 */
npx_funimp,		/* db 26 */
npx_funimp,		/* db 27 */
npx_fld_temp,		/* db 28 */
npx_fld_temp,		/* db 29 */
npx_fld_temp,		/* db 2a */
npx_fld_temp,		/* db 2b */
npx_fld_temp,		/* db 2c */
npx_fld_temp,		/* db 2d */
npx_fld_temp,		/* db 2e */
npx_fld_temp,		/* db 2f */
npx_funimp,		/* db 30 */
npx_funimp,		/* db 31 */
npx_funimp,		/* db 32 */
npx_funimp,		/* db 33 */
npx_funimp,		/* db 34 */
npx_funimp,		/* db 35 */
npx_funimp,		/* db 36 */
npx_funimp,		/* db 37 */
npx_fstp_temp,		/* db 38 */
npx_fstp_temp,		/* db 39 */
npx_fstp_temp,		/* db 3a */
npx_fstp_temp,		/* db 3b */
npx_fstp_temp,		/* db 3c */
npx_fstp_temp,		/* db 3d */
npx_fstp_temp,		/* db 3e */
npx_fstp_temp,		/* db 3f */
npx_fild_short,		/* db 40 */
npx_fild_short,		/* db 41 */
npx_fild_short,		/* db 42 */
npx_fild_short,		/* db 43 */
npx_fild_short,		/* db 44 */
npx_fild_short,		/* db 45 */
npx_fild_short,		/* db 46 */
npx_fild_short,		/* db 47 */
npx_funimp,		/* db 48 */
npx_funimp,		/* db 49 */
npx_funimp,		/* db 4a */
npx_funimp,		/* db 4b */
npx_funimp,		/* db 4c */
npx_funimp,		/* db 4d */
npx_funimp,		/* db 4e */
npx_funimp,		/* db 4f */
npx_fist_short,		/* db 50 */
npx_fist_short,		/* db 51 */
npx_fist_short,		/* db 52 */
npx_fist_short,		/* db 53 */
npx_fist_short,		/* db 54 */
npx_fist_short,		/* db 55 */
npx_fist_short,		/* db 56 */
npx_fist_short,		/* db 57 */
npx_fistp_short,	/* db 58 */
npx_fistp_short,	/* db 59 */
npx_fistp_short,	/* db 5a */
npx_fistp_short,	/* db 5b */
npx_fistp_short,	/* db 5c */
npx_fistp_short,	/* db 5d */
npx_fistp_short,	/* db 5e */
npx_fistp_short,	/* db 5f */
npx_funimp,		/* db 60 */
npx_funimp,		/* db 61 */
npx_funimp,		/* db 62 */
npx_funimp,		/* db 63 */
npx_funimp,		/* db 64 */
npx_funimp,		/* db 65 */
npx_funimp,		/* db 66 */
npx_funimp,		/* db 67 */
npx_fld_temp,		/* db 68 */
npx_fld_temp,		/* db 69 */
npx_fld_temp,		/* db 6a */
npx_fld_temp,		/* db 6b */
npx_fld_temp,		/* db 6c */
npx_fld_temp,		/* db 6d */
npx_fld_temp,		/* db 6e */
npx_fld_temp,		/* db 6f */
npx_funimp,		/* db 70 */
npx_funimp,		/* db 71 */
npx_funimp,		/* db 72 */
npx_funimp,		/* db 73 */
npx_funimp,		/* db 74 */
npx_funimp,		/* db 75 */
npx_funimp,		/* db 76 */
npx_funimp,		/* db 77 */
npx_fstp_temp,		/* db 78 */
npx_fstp_temp,		/* db 79 */
npx_fstp_temp,		/* db 7a */
npx_fstp_temp,		/* db 7b */
npx_fstp_temp,		/* db 7c */
npx_fstp_temp,		/* db 7d */
npx_fstp_temp,		/* db 7e */
npx_fstp_temp,		/* db 7f */
npx_fild_short,		/* db 80 */
npx_fild_short,		/* db 81 */
npx_fild_short,		/* db 82 */
npx_fild_short,		/* db 83 */
npx_fild_short,		/* db 84 */
npx_fild_short,		/* db 85 */
npx_fild_short,		/* db 86 */
npx_fild_short,		/* db 87 */
npx_funimp,		/* db 88 */
npx_funimp,		/* db 89 */
npx_funimp,		/* db 8a */
npx_funimp,		/* db 8b */
npx_funimp,		/* db 8c */
npx_funimp,		/* db 8d */
npx_funimp,		/* db 8e */
npx_funimp,		/* db 8f */
npx_fist_short,		/* db 90 */
npx_fist_short,		/* db 91 */
npx_fist_short,		/* db 92 */
npx_fist_short,		/* db 93 */
npx_fist_short,		/* db 94 */
npx_fist_short,		/* db 95 */
npx_fist_short,		/* db 96 */
npx_fist_short,		/* db 97 */
npx_fistp_short,	/* db 98 */
npx_fistp_short,	/* db 99 */
npx_fistp_short,	/* db 9a */
npx_fistp_short,	/* db 9b */
npx_fistp_short,	/* db 9c */
npx_fistp_short,	/* db 9d */
npx_fistp_short,	/* db 9e */
npx_fistp_short,	/* db 9f */
npx_funimp,		/* db a0 */
npx_funimp,		/* db a1 */
npx_funimp,		/* db a2 */
npx_funimp,		/* db a3 */
npx_funimp,		/* db a4 */
npx_funimp,		/* db a5 */
npx_funimp,		/* db a6 */
npx_funimp,		/* db a7 */
npx_fld_temp,		/* db a8 */
npx_fld_temp,		/* db a9 */
npx_fld_temp,		/* db aa */
npx_fld_temp,		/* db ab */
npx_fld_temp,		/* db ac */
npx_fld_temp,		/* db ad */
npx_fld_temp,		/* db ae */
npx_fld_temp,		/* db af */
npx_funimp,		/* db b0 */
npx_funimp,		/* db b1 */
npx_funimp,		/* db b2 */
npx_funimp,		/* db b3 */
npx_funimp,		/* db b4 */
npx_funimp,		/* db b5 */
npx_funimp,		/* db b6 */
npx_funimp,		/* db b7 */
npx_fstp_temp,		/* db b8 */
npx_fstp_temp,		/* db b9 */
npx_fstp_temp,		/* db ba */
npx_fstp_temp,		/* db bb */
npx_fstp_temp,		/* db bc */
npx_fstp_temp,		/* db bd */
npx_fstp_temp,		/* db be */
npx_fstp_temp,		/* db bf */
npx_funimp,		/* db c0 */
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,		/* db d0 */
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_fnop,		/* db e0 */
npx_fnop,
npx_fclex,		/* db e2 */
npx_finit,		/* db e3 */
npx_fnop,		/* db e4 - used to be fsetpm */
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,		/* db f0 */
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_fadd_long,		/* dc 00 */
npx_fadd_long,		/* dc 01 */
npx_fadd_long,		/* dc 02 */
npx_fadd_long,		/* dc 03 */
npx_fadd_long,		/* dc 04 */
npx_fadd_long,		/* dc 05 */
npx_fadd_long,		/* dc 06 */
npx_fadd_long,		/* dc 07 */
npx_fmul_long,		/* dc 08 */
npx_fmul_long,		/* dc 09 */
npx_fmul_long,		/* dc 0a */
npx_fmul_long,		/* dc 0b */
npx_fmul_long,		/* dc 0c */
npx_fmul_long,		/* dc 0d */
npx_fmul_long,		/* dc 0e */
npx_fmul_long,		/* dc 0f */
npx_fcom_long,		/* dc 10 */
npx_fcom_long,		/* dc 11 */
npx_fcom_long,		/* dc 12 */
npx_fcom_long,		/* dc 13 */
npx_fcom_long,		/* dc 14 */
npx_fcom_long,		/* dc 15 */
npx_fcom_long,		/* dc 16 */
npx_fcom_long,		/* dc 17 */
npx_fcomp_long,		/* dc 18 */
npx_fcomp_long,		/* dc 19 */
npx_fcomp_long,		/* dc 1a */
npx_fcomp_long,		/* dc 1b */
npx_fcomp_long,		/* dc 1c */
npx_fcomp_long,		/* dc 1d */
npx_fcomp_long,		/* dc 1e */
npx_fcomp_long,		/* dc 1f */
npx_fsub_long,		/* dc 20 */
npx_fsub_long,		/* dc 21 */
npx_fsub_long,		/* dc 22 */
npx_fsub_long,		/* dc 23 */
npx_fsub_long,		/* dc 24 */
npx_fsub_long,		/* dc 25 */
npx_fsub_long,		/* dc 26 */
npx_fsub_long,		/* dc 27 */
npx_fsubr_long,		/* dc 28 */
npx_fsubr_long,		/* dc 29 */
npx_fsubr_long,		/* dc 2a */
npx_fsubr_long,		/* dc 2b */
npx_fsubr_long,		/* dc 2c */
npx_fsubr_long,		/* dc 2d */
npx_fsubr_long,		/* dc 2e */
npx_fsubr_long,		/* dc 2f */
npx_fdiv_long,		/* dc 30 */
npx_fdiv_long,		/* dc 31 */
npx_fdiv_long,		/* dc 32 */
npx_fdiv_long,		/* dc 33 */
npx_fdiv_long,		/* dc 34 */
npx_fdiv_long,		/* dc 35 */
npx_fdiv_long,		/* dc 36 */
npx_fdiv_long,		/* dc 37 */
npx_fdivr_long,		/* dc 38 */
npx_fdivr_long,		/* dc 39 */
npx_fdivr_long,		/* dc 3a */
npx_fdivr_long,		/* dc 3b */
npx_fdivr_long,		/* dc 3c */
npx_fdivr_long,		/* dc 3d */
npx_fdivr_long,		/* dc 3e */
npx_fdivr_long,		/* dc 3f */
npx_fadd_long,		/* dc 40 */
npx_fadd_long,		/* dc 41 */
npx_fadd_long,		/* dc 42 */
npx_fadd_long,		/* dc 43 */
npx_fadd_long,		/* dc 44 */
npx_fadd_long,		/* dc 45 */
npx_fadd_long,		/* dc 46 */
npx_fadd_long,		/* dc 47 */
npx_fmul_long,		/* dc 48 */
npx_fmul_long,		/* dc 49 */
npx_fmul_long,		/* dc 4a */
npx_fmul_long,		/* dc 4b */
npx_fmul_long,		/* dc 4c */
npx_fmul_long,		/* dc 4d */
npx_fmul_long,		/* dc 4e */
npx_fmul_long,		/* dc 4f */
npx_fcom_long,		/* dc 50 */
npx_fcom_long,		/* dc 51 */
npx_fcom_long,		/* dc 52 */
npx_fcom_long,		/* dc 53 */
npx_fcom_long,		/* dc 54 */
npx_fcom_long,		/* dc 55 */
npx_fcom_long,		/* dc 56 */
npx_fcom_long,		/* dc 57 */
npx_fcomp_long,		/* dc 58 */
npx_fcomp_long,		/* dc 59 */
npx_fcomp_long,		/* dc 5a */
npx_fcomp_long,		/* dc 5b */
npx_fcomp_long,		/* dc 5c */
npx_fcomp_long,		/* dc 5d */
npx_fcomp_long,		/* dc 5e */
npx_fcomp_long,		/* dc 5f */
npx_fsub_long,		/* dc 60 */
npx_fsub_long,		/* dc 61 */
npx_fsub_long,		/* dc 62 */
npx_fsub_long,		/* dc 63 */
npx_fsub_long,		/* dc 64 */
npx_fsub_long,		/* dc 65 */
npx_fsub_long,		/* dc 66 */
npx_fsub_long,		/* dc 67 */
npx_fsubr_long,		/* dc 68 */
npx_fsubr_long,		/* dc 69 */
npx_fsubr_long,		/* dc 6a */
npx_fsubr_long,		/* dc 6b */
npx_fsubr_long,		/* dc 6c */
npx_fsubr_long,		/* dc 6d */
npx_fsubr_long,		/* dc 6e */
npx_fsubr_long,		/* dc 6f */
npx_fdiv_long,		/* dc 70 */
npx_fdiv_long,		/* dc 71 */
npx_fdiv_long,		/* dc 72 */
npx_fdiv_long,		/* dc 73 */
npx_fdiv_long,		/* dc 74 */
npx_fdiv_long,		/* dc 75 */
npx_fdiv_long,		/* dc 76 */
npx_fdiv_long,		/* dc 77 */
npx_fdivr_long,		/* dc 78 */
npx_fdivr_long,		/* dc 79 */
npx_fdivr_long,		/* dc 7a */
npx_fdivr_long,		/* dc 7b */
npx_fdivr_long,		/* dc 7c */
npx_fdivr_long,		/* dc 7d */
npx_fdivr_long,		/* dc 7e */
npx_fdivr_long,		/* dc 7f */
npx_fadd_long,		/* dc 80 */
npx_fadd_long,		/* dc 81 */
npx_fadd_long,		/* dc 82 */
npx_fadd_long,		/* dc 83 */
npx_fadd_long,		/* dc 84 */
npx_fadd_long,		/* dc 85 */
npx_fadd_long,		/* dc 86 */
npx_fadd_long,		/* dc 87 */
npx_fmul_long,		/* dc 88 */
npx_fmul_long,		/* dc 89 */
npx_fmul_long,		/* dc 8a */
npx_fmul_long,		/* dc 8b */
npx_fmul_long,		/* dc 8c */
npx_fmul_long,		/* dc 8d */
npx_fmul_long,		/* dc 8e */
npx_fmul_long,		/* dc 8f */
npx_fcom_long,		/* dc 90 */
npx_fcom_long,		/* dc 91 */
npx_fcom_long,		/* dc 92 */
npx_fcom_long,		/* dc 93 */
npx_fcom_long,		/* dc 94 */
npx_fcom_long,		/* dc 95 */
npx_fcom_long,		/* dc 96 */
npx_fcom_long,		/* dc 97 */
npx_fcomp_long,		/* dc 98 */
npx_fcomp_long,		/* dc 99 */
npx_fcomp_long,		/* dc 9a */
npx_fcomp_long,		/* dc 9b */
npx_fcomp_long,		/* dc 9c */
npx_fcomp_long,		/* dc 9d */
npx_fcomp_long,		/* dc 9e */
npx_fcomp_long,		/* dc 9f */
npx_fsub_long,		/* dc a0 */
npx_fsub_long,		/* dc a1 */
npx_fsub_long,		/* dc a2 */
npx_fsub_long,		/* dc a3 */
npx_fsub_long,		/* dc a4 */
npx_fsub_long,		/* dc a5 */
npx_fsub_long,		/* dc a6 */
npx_fsub_long,		/* dc a7 */
npx_fsubr_long,		/* dc a8 */
npx_fsubr_long,		/* dc a9 */
npx_fsubr_long,		/* dc aa */
npx_fsubr_long,		/* dc ab */
npx_fsubr_long,		/* dc ac */
npx_fsubr_long,		/* dc ad */
npx_fsubr_long,		/* dc ae */
npx_fsubr_long,		/* dc af */
npx_fdiv_long,		/* dc b0 */
npx_fdiv_long,		/* dc b1 */
npx_fdiv_long,		/* dc b2 */
npx_fdiv_long,		/* dc b3 */
npx_fdiv_long,		/* dc b4 */
npx_fdiv_long,		/* dc b5 */
npx_fdiv_long,		/* dc b6 */
npx_fdiv_long,		/* dc b7 */
npx_fdivr_long,		/* dc b8 */
npx_fdivr_long,		/* dc b9 */
npx_fdivr_long,		/* dc 3a */
npx_fdivr_long,		/* dc bb */
npx_fdivr_long,		/* dc bc */
npx_fdivr_long,		/* dc bd */
npx_fdivr_long,		/* dc be */
npx_fdivr_long,		/* dc bf */
npx_fadd_f0_f0,		/* dc c0 */
npx_fadd_f1_f0,
npx_fadd_f2_f0,
npx_fadd_f3_f0,
npx_fadd_f4_f0,
npx_fadd_f5_f0,
npx_fadd_f6_f0,
npx_fadd_f7_f0,
npx_fmul_f0_f0,		/* dc c8 */
npx_fmul_f1_f0,
npx_fmul_f2_f0,
npx_fmul_f3_f0,
npx_fmul_f4_f0,
npx_fmul_f5_f0,
npx_fmul_f6_f0,
npx_fmul_f7_f0,
npx_fcom_f0,		/* dc d0 */
npx_fcom_f1,
npx_fcom_f2,
npx_fcom_f3,
npx_fcom_f4,
npx_fcom_f5,
npx_fcom_f6,
npx_fcom_f7,
npx_fcomp_f0,
npx_fcomp_f1,
npx_fcomp_f2,
npx_fcomp_f3,
npx_fcomp_f4,
npx_fcomp_f5,
npx_fcomp_f6,
npx_fcomp_f7,
npx_fsubr_f0_f0,	/* dc e0 */
npx_fsubr_f1_f0,
npx_fsubr_f2_f0,
npx_fsubr_f3_f0,
npx_fsubr_f4_f0,
npx_fsubr_f5_f0,
npx_fsubr_f6_f0,
npx_fsubr_f7_f0,
npx_fsub_f0_f0,		/* dc e8 */
npx_fsub_f1_f0,
npx_fsub_f2_f0,
npx_fsub_f3_f0,
npx_fsub_f4_f0,
npx_fsub_f5_f0,
npx_fsub_f6_f0,
npx_fsub_f7_f0,
npx_fdivr_f0_f0,	/* dc f0 */
npx_fdivr_f1_f0,
npx_fdivr_f2_f0,
npx_fdivr_f3_f0,
npx_fdivr_f4_f0,
npx_fdivr_f5_f0,
npx_fdivr_f6_f0,
npx_fdivr_f7_f0,
npx_fdiv_f0_f0,		/* dc f8 */
npx_fdiv_f1_f0,
npx_fdiv_f2_f0,
npx_fdiv_f3_f0,
npx_fdiv_f4_f0,
npx_fdiv_f5_f0,
npx_fdiv_f6_f0,
npx_fdiv_f7_f0,
npx_fld_long,		/* dd 00 */
npx_fld_long,		/* dd 01 */
npx_fld_long,		/* dd 02 */
npx_fld_long,		/* dd 03 */
npx_fld_long,		/* dd 04 */
npx_fld_long,		/* dd 05 */
npx_fld_long,		/* dd 06 */
npx_fld_long,		/* dd 07 */
npx_funimp,		/* dd 08 */
npx_funimp,		/* dd 09 */
npx_funimp,		/* dd 0a */
npx_funimp,		/* dd 0b */
npx_funimp,		/* dd 0c */
npx_funimp,		/* dd 0d */
npx_funimp,		/* dd 0e */
npx_funimp,		/* dd 0f */
npx_fst_long,		/* dd 10 */
npx_fst_long,		/* dd 11 */
npx_fst_long,		/* dd 12 */
npx_fst_long,		/* dd 13 */
npx_fst_long,		/* dd 14 */
npx_fst_long,		/* dd 15 */
npx_fst_long,		/* dd 16 */
npx_fst_long,		/* dd 17 */
npx_fstp_long,		/* dd 18 */
npx_fstp_long,		/* dd 19 */
npx_fstp_long,		/* dd 1a */
npx_fstp_long,		/* dd 1b */
npx_fstp_long,		/* dd 1c */
npx_fstp_long,		/* dd 1d */
npx_fstp_long,		/* dd 1e */
npx_fstp_long,		/* dd 1f */
npx_frstor,		/* dd 20 */
npx_frstor,		/* dd 21 */
npx_frstor,		/* dd 22 */
npx_frstor,		/* dd 23 */
npx_frstor,		/* dd 24 */
npx_frstor,		/* dd 25 */
npx_frstor,		/* dd 26 */
npx_frstor,		/* dd 27 */
npx_funimp,		/* dd 28 */
npx_funimp,		/* dd 29 */
npx_funimp,		/* dd 2a */
npx_funimp,		/* dd 2b */
npx_funimp,		/* dd 2c */
npx_funimp,		/* dd 2d */
npx_funimp,		/* dd 2e */
npx_funimp,		/* dd 2f */
npx_fsave,		/* dd 30 */
npx_fsave,		/* dd 31 */
npx_fsave,		/* dd 32 */
npx_fsave,		/* dd 33 */
npx_fsave,		/* dd 34 */
npx_fsave,		/* dd 35 */
npx_fsave,		/* dd 36 */
npx_fsave,		/* dd 37 */
npx_fstsw,		/* dd 38 */
npx_fstsw,		/* dd 39 */
npx_fstsw,		/* dd 3a */
npx_fstsw,		/* dd 3b */
npx_fstsw,		/* dd 3c */
npx_fstsw,		/* dd 3d */
npx_fstsw,		/* dd 3e */
npx_fstsw,		/* dd 3f */
npx_fld_long,		/* dd 40 */
npx_fld_long,		/* dd 41 */
npx_fld_long,		/* dd 42 */
npx_fld_long,		/* dd 43 */
npx_fld_long,		/* dd 44 */
npx_fld_long,		/* dd 45 */
npx_fld_long,		/* dd 46 */
npx_fld_long,		/* dd 47 */
npx_funimp,		/* dd 48 */
npx_funimp,		/* dd 49 */
npx_funimp,		/* dd 4a */
npx_funimp,		/* dd 4b */
npx_funimp,		/* dd 4c */
npx_funimp,		/* dd 4d */
npx_funimp,		/* dd 4e */
npx_funimp,		/* dd 4f */
npx_fst_long,		/* dd 50 */
npx_fst_long,		/* dd 51 */
npx_fst_long,		/* dd 52 */
npx_fst_long,		/* dd 53 */
npx_fst_long,		/* dd 54 */
npx_fst_long,		/* dd 55 */
npx_fst_long,		/* dd 56 */
npx_fst_long,		/* dd 57 */
npx_fstp_long,		/* dd 58 */
npx_fstp_long,		/* dd 59 */
npx_fstp_long,		/* dd 5a */
npx_fstp_long,		/* dd 5b */
npx_fstp_long,		/* dd 5c */
npx_fstp_long,		/* dd 5d */
npx_fstp_long,		/* dd 5e */
npx_fstp_long,		/* dd 5f */
npx_frstor,		/* dd 60 */
npx_frstor,		/* dd 61 */
npx_frstor,		/* dd 62 */
npx_frstor,		/* dd 63 */
npx_frstor,		/* dd 64 */
npx_frstor,		/* dd 65 */
npx_frstor,		/* dd 66 */
npx_frstor,		/* dd 67 */
npx_funimp,		/* dd 68 */
npx_funimp,		/* dd 69 */
npx_funimp,		/* dd 6a */
npx_funimp,		/* dd 6b */
npx_funimp,		/* dd 6c */
npx_funimp,		/* dd 6d */
npx_funimp,		/* dd 6e */
npx_funimp,		/* dd 6f */
npx_fsave,		/* dd 70 */
npx_fsave,		/* dd 71 */
npx_fsave,		/* dd 72 */
npx_fsave,		/* dd 73 */
npx_fsave,		/* dd 74 */
npx_fsave,		/* dd 75 */
npx_fsave,		/* dd 76 */
npx_fsave,		/* dd 77 */
npx_fstsw,		/* dd 78 */
npx_fstsw,		/* dd 79 */
npx_fstsw,		/* dd 7a */
npx_fstsw,		/* dd 7b */
npx_fstsw,		/* dd 7c */
npx_fstsw,		/* dd 7d */
npx_fstsw,		/* dd 7e */
npx_fstsw,		/* dd 7f */
npx_fld_long,		/* dd 80 */
npx_fld_long,		/* dd 81 */
npx_fld_long,		/* dd 82 */
npx_fld_long,		/* dd 83 */
npx_fld_long,		/* dd 84 */
npx_fld_long,		/* dd 85 */
npx_fld_long,		/* dd 86 */
npx_fld_long,		/* dd 87 */
npx_funimp,		/* dd 88 */
npx_funimp,		/* dd 89 */
npx_funimp,		/* dd 8a */
npx_funimp,		/* dd 8b */
npx_funimp,		/* dd 8c */
npx_funimp,		/* dd 8d */
npx_funimp,		/* dd 8e */
npx_funimp,		/* dd 8f */
npx_fst_long,		/* dd 90 */
npx_fst_long,		/* dd 91 */
npx_fst_long,		/* dd 92 */
npx_fst_long,		/* dd 93 */
npx_fst_long,		/* dd 94 */
npx_fst_long,		/* dd 95 */
npx_fst_long,		/* dd 96 */
npx_fst_long,		/* dd 97 */
npx_fstp_long,		/* dd 98 */
npx_fstp_long,		/* dd 99 */
npx_fstp_long,		/* dd 9a */
npx_fstp_long,		/* dd 9b */
npx_fstp_long,		/* dd 9c */
npx_fstp_long,		/* dd 9d */
npx_fstp_long,		/* dd 9e */
npx_fstp_long,		/* dd 9f */
npx_frstor,		/* dd a0 */
npx_frstor,		/* dd a1 */
npx_frstor,		/* dd a2 */
npx_frstor,		/* dd a3 */
npx_frstor,		/* dd a4 */
npx_frstor,		/* dd a5 */
npx_frstor,		/* dd a6 */
npx_frstor,		/* dd a7 */
npx_funimp,		/* dd a8 */
npx_funimp,		/* dd a9 */
npx_funimp,		/* dd aa */
npx_funimp,		/* dd ab */
npx_funimp,		/* dd ac */
npx_funimp,		/* dd ad */
npx_funimp,		/* dd ae */
npx_funimp,		/* dd af */
npx_fsave,		/* dd b0 */
npx_fsave,		/* dd b1 */
npx_fsave,		/* dd b2 */
npx_fsave,		/* dd b3 */
npx_fsave,		/* dd b4 */
npx_fsave,		/* dd b5 */
npx_fsave,		/* dd b6 */
npx_fsave,		/* dd b7 */
npx_fstsw,		/* dd b8 */
npx_fstsw,		/* dd b9 */
npx_fstsw,		/* dd ba */
npx_fstsw,		/* dd bb */
npx_fstsw,		/* dd bc */
npx_fstsw,		/* dd bd */
npx_fstsw,		/* dd be */
npx_fstsw,		/* dd bf */
npx_ffree_f0,		/* dd c0 */
npx_ffree_f1,
npx_ffree_f2,
npx_ffree_f3,
npx_ffree_f4,
npx_ffree_f5,
npx_ffree_f6,
npx_ffree_f7,
npx_fxch_f0,		/* dd c8 */
npx_fxch_f1,
npx_fxch_f2,
npx_fxch_f3,
npx_fxch_f4,
npx_fxch_f5,
npx_fxch_f6,
npx_fxch_f7,
npx_fst_f0,		/* dd d0 */
npx_fst_f1,
npx_fst_f2,
npx_fst_f3,
npx_fst_f4,
npx_fst_f5,
npx_fst_f6,
npx_fst_f7,
npx_fstp_f0,		/* dd d8 */
npx_fstp_f1,
npx_fstp_f2,
npx_fstp_f3,
npx_fstp_f4,
npx_fstp_f5,
npx_fstp_f6,
npx_fstp_f7,
npx_fucom_f0,		/* dd e0 */
npx_fucom_f1,
npx_fucom_f2,
npx_fucom_f3,
npx_fucom_f4,
npx_fucom_f5,
npx_fucom_f6,
npx_fucom_f7,
npx_fucomp_f0,		/* dd e8 */
npx_fucomp_f1,
npx_fucomp_f2,
npx_fucomp_f3,
npx_fucomp_f4,
npx_fucomp_f5,
npx_fucomp_f6,
npx_fucomp_f7,
npx_funimp,		/* dd f0 */
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_fiadd_word,		/* de 00 */
npx_fiadd_word,		/* de 01 */
npx_fiadd_word,		/* de 02 */
npx_fiadd_word,		/* de 03 */
npx_fiadd_word,		/* de 04 */
npx_fiadd_word,		/* de 05 */
npx_fiadd_word,		/* de 06 */
npx_fiadd_word,		/* de 07 */
npx_fimul_word,		/* de 08 */
npx_fimul_word,		/* de 09 */
npx_fimul_word,		/* de 0a */
npx_fimul_word,		/* de 0b */
npx_fimul_word,		/* de 0c */
npx_fimul_word,		/* de 0d */
npx_fimul_word,		/* de 0e */
npx_fimul_word,		/* de 0f */
npx_ficom_word,		/* de 10 */
npx_ficom_word,		/* de 11 */
npx_ficom_word,		/* de 12 */
npx_ficom_word,		/* de 13 */
npx_ficom_word,		/* de 14 */
npx_ficom_word,		/* de 15 */
npx_ficom_word,		/* de 16 */
npx_ficom_word,		/* de 17 */
npx_ficomp_word,	/* de 18 */
npx_ficomp_word,	/* de 19 */
npx_ficomp_word,	/* de 1a */
npx_ficomp_word,	/* de 1b */
npx_ficomp_word,	/* de 1c */
npx_ficomp_word,	/* de 1d */
npx_ficomp_word,	/* de 1e */
npx_ficomp_word,	/* de 1f */
npx_fisub_word,		/* de 20 */
npx_fisub_word,		/* de 21 */
npx_fisub_word,		/* de 22 */
npx_fisub_word,		/* de 23 */
npx_fisub_word,		/* de 24 */
npx_fisub_word,		/* de 25 */
npx_fisub_word,		/* de 26 */
npx_fisub_word,		/* de 27 */
npx_fisubr_word,	/* de 28 */
npx_fisubr_word,	/* de 29 */
npx_fisubr_word,	/* de 2a */
npx_fisubr_word,	/* de 2b */
npx_fisubr_word,	/* de 2c */
npx_fisubr_word,	/* de 2d */
npx_fisubr_word,	/* de 2e */
npx_fisubr_word,	/* de 2f */
npx_fidiv_word,		/* de 30 */
npx_fidiv_word,		/* de 31 */
npx_fidiv_word,		/* de 32 */
npx_fidiv_word,		/* de 33 */
npx_fidiv_word,		/* de 34 */
npx_fidiv_word,		/* de 35 */
npx_fidiv_word,		/* de 36 */
npx_fidiv_word,		/* de 37 */
npx_fidivr_word,	/* de 38 */
npx_fidivr_word,	/* de 39 */
npx_fidivr_word,	/* de 3a */
npx_fidivr_word,	/* de 3b */
npx_fidivr_word,	/* de 3c */
npx_fidivr_word,	/* de 3d */
npx_fidivr_word,	/* de 3e */
npx_fidivr_word,	/* de 3f */
npx_fiadd_word,		/* de 40 */
npx_fiadd_word,		/* de 41 */
npx_fiadd_word,		/* de 42 */
npx_fiadd_word,		/* de 43 */
npx_fiadd_word,		/* de 44 */
npx_fiadd_word,		/* de 45 */
npx_fiadd_word,		/* de 46 */
npx_fiadd_word,		/* de 47 */
npx_fimul_word,		/* de 48 */
npx_fimul_word,		/* de 49 */
npx_fimul_word,		/* de 4a */
npx_fimul_word,		/* de 4b */
npx_fimul_word,		/* de 4c */
npx_fimul_word,		/* de 4d */
npx_fimul_word,		/* de 4e */
npx_fimul_word,		/* de 4f */
npx_ficom_word,		/* de 50 */
npx_ficom_word,		/* de 51 */
npx_ficom_word,		/* de 52 */
npx_ficom_word,		/* de 53 */
npx_ficom_word,		/* de 54 */
npx_ficom_word,		/* de 55 */
npx_ficom_word,		/* de 56 */
npx_ficom_word,		/* de 57 */
npx_ficomp_word,	/* de 58 */
npx_ficomp_word,	/* de 59 */
npx_ficomp_word,	/* de 5a */
npx_ficomp_word,	/* de 5b */
npx_ficomp_word,	/* de 5c */
npx_ficomp_word,	/* de 5d */
npx_ficomp_word,	/* de 5e */
npx_ficomp_word,	/* de 5f */
npx_fisub_word,		/* de 60 */
npx_fisub_word,		/* de 61 */
npx_fisub_word,		/* de 62 */
npx_fisub_word,		/* de 63 */
npx_fisub_word,		/* de 64 */
npx_fisub_word,		/* de 65 */
npx_fisub_word,		/* de 66 */
npx_fisub_word,		/* de 67 */
npx_fisubr_word,	/* de 68 */
npx_fisubr_word,	/* de 69 */
npx_fisubr_word,	/* de 6a */
npx_fisubr_word,	/* de 6b */
npx_fisubr_word,	/* de 6c */
npx_fisubr_word,	/* de 6d */
npx_fisubr_word,	/* de 6e */
npx_fisubr_word,	/* de 6f */
npx_fidiv_word,		/* de 70 */
npx_fidiv_word,		/* de 71 */
npx_fidiv_word,		/* de 72 */
npx_fidiv_word,		/* de 73 */
npx_fidiv_word,		/* de 74 */
npx_fidiv_word,		/* de 75 */
npx_fidiv_word,		/* de 76 */
npx_fidiv_word,		/* de 77 */
npx_fidivr_word,	/* de 78 */
npx_fidivr_word,	/* de 79 */
npx_fidivr_word,	/* de 7a */
npx_fidivr_word,	/* de 7b */
npx_fidivr_word,	/* de 7c */
npx_fidivr_word,	/* de 7d */
npx_fidivr_word,	/* de 7e */
npx_fidivr_word,	/* de 7f */
npx_fiadd_word,		/* de 80 */
npx_fiadd_word,		/* de 81 */
npx_fiadd_word,		/* de 82 */
npx_fiadd_word,		/* de 83 */
npx_fiadd_word,		/* de 84 */
npx_fiadd_word,		/* de 85 */
npx_fiadd_word,		/* de 86 */
npx_fiadd_word,		/* de 87 */
npx_fimul_word,		/* de 88 */
npx_fimul_word,		/* de 89 */
npx_fimul_word,		/* de 8a */
npx_fimul_word,		/* de 8b */
npx_fimul_word,		/* de 8c */
npx_fimul_word,		/* de 8d */
npx_fimul_word,		/* de 8e */
npx_fimul_word,		/* de 8f */
npx_ficom_word,		/* de 90 */
npx_ficom_word,		/* de 91 */
npx_ficom_word,		/* de 92 */
npx_ficom_word,		/* de 93 */
npx_ficom_word,		/* de 94 */
npx_ficom_word,		/* de 95 */
npx_ficom_word,		/* de 96 */
npx_ficom_word,		/* de 97 */
npx_ficomp_word,	/* de 98 */
npx_ficomp_word,	/* de 99 */
npx_ficomp_word,	/* de 9a */
npx_ficomp_word,	/* de 9b */
npx_ficomp_word,	/* de 9c */
npx_ficomp_word,	/* de 9d */
npx_ficomp_word,	/* de 9e */
npx_ficomp_word,	/* de 9f */
npx_fisub_word,		/* de a0 */
npx_fisub_word,		/* de a1 */
npx_fisub_word,		/* de a2 */
npx_fisub_word,		/* de a3 */
npx_fisub_word,		/* de a4 */
npx_fisub_word,		/* de a5 */
npx_fisub_word,		/* de a6 */
npx_fisub_word,		/* de a7 */
npx_fisubr_word,	/* de a8 */
npx_fisubr_word,	/* de a9 */
npx_fisubr_word,	/* de aa */
npx_fisubr_word,	/* de ab */
npx_fisubr_word,	/* de ac */
npx_fisubr_word,	/* de ad */
npx_fisubr_word,	/* de ae */
npx_fisubr_word,	/* de af */
npx_fidiv_word,		/* de b0 */
npx_fidiv_word,		/* de b1 */
npx_fidiv_word,		/* de b2 */
npx_fidiv_word,		/* de b3 */
npx_fidiv_word,		/* de b4 */
npx_fidiv_word,		/* de b5 */
npx_fidiv_word,		/* de b6 */
npx_fidiv_word,		/* de b7 */
npx_fidivr_word,	/* de b8 */
npx_fidivr_word,	/* de b9 */
npx_fidivr_word,	/* de ba */
npx_fidivr_word,	/* de bb */
npx_fidivr_word,	/* de bc */
npx_fidivr_word,	/* de bd */
npx_fidivr_word,	/* de be */
npx_fidivr_word,	/* de bf */
npx_faddp_f0,		/* de c0 */
npx_faddp_f1,
npx_faddp_f2,
npx_faddp_f3,
npx_faddp_f4,
npx_faddp_f5,
npx_faddp_f6,
npx_faddp_f7,
npx_fmulp_f0,		/* de c8 */
npx_fmulp_f1,
npx_fmulp_f2,
npx_fmulp_f3,
npx_fmulp_f4,
npx_fmulp_f5,
npx_fmulp_f6,
npx_fmulp_f7,
npx_fcomp_f0,		/* de d0 */
npx_fcomp_f1,
npx_fcomp_f2,
npx_fcomp_f3,
npx_fcomp_f4,
npx_fcomp_f5,
npx_fcomp_f6,
npx_fcomp_f7,
npx_funimp,
npx_fcompp,		/* de d9 */
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_fsubrp_f0,		/* de e0 */
npx_fsubrp_f1,
npx_fsubrp_f2,
npx_fsubrp_f3,
npx_fsubrp_f4,
npx_fsubrp_f5,
npx_fsubrp_f6,
npx_fsubrp_f7,
npx_fsubp_f0,		/* de e8 */
npx_fsubp_f1,
npx_fsubp_f2,
npx_fsubp_f3,
npx_fsubp_f4,
npx_fsubp_f5,
npx_fsubp_f6,
npx_fsubp_f7,
npx_fdivrp_f0,		/* de f0 */
npx_fdivrp_f1,
npx_fdivrp_f2,
npx_fdivrp_f3,
npx_fdivrp_f4,
npx_fdivrp_f5,
npx_fdivrp_f6,
npx_fdivrp_f7,
npx_fdivp_f0,		/* de f8 */
npx_fdivp_f1,
npx_fdivp_f2,
npx_fdivp_f3,
npx_fdivp_f4,
npx_fdivp_f5,
npx_fdivp_f6,
npx_fdivp_f7,
npx_fild_word,		/* df 00 */
npx_fild_word,		/* df 01 */
npx_fild_word,		/* df 02 */
npx_fild_word,		/* df 03 */
npx_fild_word,		/* df 04 */
npx_fild_word,		/* df 05 */
npx_fild_word,		/* df 06 */
npx_fild_word,		/* df 07 */
npx_funimp,		/* df 08 */
npx_funimp,		/* df 09 */
npx_funimp,		/* df 0a */
npx_funimp,		/* df 0b */
npx_funimp,		/* df 0c */
npx_funimp,		/* df 0d */
npx_funimp,		/* df 0e */
npx_funimp,		/* df 0f */
npx_fist_word,		/* df 10 */
npx_fist_word,		/* df 11 */
npx_fist_word,		/* df 12 */
npx_fist_word,		/* df 13 */
npx_fist_word,		/* df 14 */
npx_fist_word,		/* df 15 */
npx_fist_word,		/* df 16 */
npx_fist_word,		/* df 17 */
npx_fistp_word,		/* df 18 */
npx_fistp_word,		/* df 19 */
npx_fistp_word,		/* df 1a */
npx_fistp_word,		/* df 1b */
npx_fistp_word,		/* df 1c */
npx_fistp_word,		/* df 1d */
npx_fistp_word,		/* df 1e */
npx_fistp_word,		/* df 1f */
npx_fbld,		/* df 20 */
npx_fbld,		/* df 21 */
npx_fbld,		/* df 22 */
npx_fbld,		/* df 23 */
npx_fbld,		/* df 24 */
npx_fbld,		/* df 25 */
npx_fbld,		/* df 26 */
npx_fbld,		/* df 27 */
npx_fild_long,		/* df 28 */
npx_fild_long,		/* df 29 */
npx_fild_long,		/* df 2a */
npx_fild_long,		/* df 2b */
npx_fild_long,		/* df 2c */
npx_fild_long,		/* df 2d */
npx_fild_long,		/* df 2e */
npx_fild_long,		/* df 2f */
npx_fbstp,		/* df 30 */
npx_fbstp,		/* df 31 */
npx_fbstp,		/* df 32 */
npx_fbstp,		/* df 33 */
npx_fbstp,		/* df 34 */
npx_fbstp,		/* df 35 */
npx_fbstp,		/* df 36 */
npx_fbstp,		/* df 37 */
npx_fistp_long,		/* df 38 */
npx_fistp_long,		/* df 39 */
npx_fistp_long,		/* df 3a */
npx_fistp_long,		/* df 3b */
npx_fistp_long,		/* df 3c */
npx_fistp_long,		/* df 3d */
npx_fistp_long,		/* df 3e */
npx_fistp_long,		/* df 3f */
npx_fild_word,		/* df 40 */
npx_fild_word,		/* df 41 */
npx_fild_word,		/* df 42 */
npx_fild_word,		/* df 43 */
npx_fild_word,		/* df 44 */
npx_fild_word,		/* df 45 */
npx_fild_word,		/* df 46 */
npx_fild_word,		/* df 47 */
npx_funimp,		/* df 48 */
npx_funimp,		/* df 49 */
npx_funimp,		/* df 4a */
npx_funimp,		/* df 4b */
npx_funimp,		/* df 4c */
npx_funimp,		/* df 4d */
npx_funimp,		/* df 4e */
npx_funimp,		/* df 4f */
npx_fist_word,		/* df 50 */
npx_fist_word,		/* df 51 */
npx_fist_word,		/* df 52 */
npx_fist_word,		/* df 53 */
npx_fist_word,		/* df 54 */
npx_fist_word,		/* df 55 */
npx_fist_word,		/* df 56 */
npx_fist_word,		/* df 57 */
npx_fistp_word,		/* df 58 */
npx_fistp_word,		/* df 59 */
npx_fistp_word,		/* df 5a */
npx_fistp_word,		/* df 5b */
npx_fistp_word,		/* df 5c */
npx_fistp_word,		/* df 5d */
npx_fistp_word,		/* df 5e */
npx_fistp_word,		/* df 5f */
npx_fbld,		/* df 60 */
npx_fbld,		/* df 61 */
npx_fbld,		/* df 62 */
npx_fbld,		/* df 63 */
npx_fbld,		/* df 64 */
npx_fbld,		/* df 65 */
npx_fbld,		/* df 66 */
npx_fbld,		/* df 67 */
npx_fild_long,		/* df 68 */
npx_fild_long,		/* df 69 */
npx_fild_long,		/* df 6a */
npx_fild_long,		/* df 6b */
npx_fild_long,		/* df 6c */
npx_fild_long,		/* df 6d */
npx_fild_long,		/* df 6e */
npx_fild_long,		/* df 6f */
npx_fbstp,		/* df 70 */
npx_fbstp,		/* df 71 */
npx_fbstp,		/* df 72 */
npx_fbstp,		/* df 73 */
npx_fbstp,		/* df 34 */
npx_fbstp,		/* df 75 */
npx_fbstp,		/* df 76 */
npx_fbstp,		/* df 77 */
npx_fistp_long,		/* df 78 */
npx_fistp_long,		/* df 79 */
npx_fistp_long,		/* df 7a */
npx_fistp_long,		/* df 7b */
npx_fistp_long,		/* df 7c */
npx_fistp_long,		/* df 7d */
npx_fistp_long,		/* df 7e */
npx_fistp_long,		/* df 7f */
npx_fild_word,		/* df 80 */
npx_fild_word,		/* df 81 */
npx_fild_word,		/* df 82 */
npx_fild_word,		/* df 83 */
npx_fild_word,		/* df 84 */
npx_fild_word,		/* df 85 */
npx_fild_word,		/* df 86 */
npx_fild_word,		/* df 87 */
npx_funimp,		/* df 88 */
npx_funimp,		/* df 89 */
npx_funimp,		/* df 8a */
npx_funimp,		/* df 8b */
npx_funimp,		/* df 8c */
npx_funimp,		/* df 8d */
npx_funimp,		/* df 8e */
npx_funimp,		/* df 8f */
npx_fist_word,		/* df 90 */
npx_fist_word,		/* df 91 */
npx_fist_word,		/* df 92 */
npx_fist_word,		/* df 93 */
npx_fist_word,		/* df 94 */
npx_fist_word,		/* df 95 */
npx_fist_word,		/* df 96 */
npx_fist_word,		/* df 97 */
npx_fistp_word,		/* df 98 */
npx_fistp_word,		/* df 99 */
npx_fistp_word,		/* df 9a */
npx_fistp_word,		/* df 9b */
npx_fistp_word,		/* df 9c */
npx_fistp_word,		/* df 9d */
npx_fistp_word,		/* df 9e */
npx_fistp_word,		/* df 9f */
npx_fbld,		/* df a0 */
npx_fbld,		/* df a1 */
npx_fbld,		/* df a2 */
npx_fbld,		/* df a3 */
npx_fbld,		/* df a4 */
npx_fbld,		/* df a5 */
npx_fbld,		/* df a6 */
npx_fbld,		/* df a7 */
npx_fild_long,		/* df a8 */
npx_fild_long,		/* df a9 */
npx_fild_long,		/* df aa */
npx_fild_long,		/* df ab */
npx_fild_long,		/* df ac */
npx_fild_long,		/* df ad */
npx_fild_long,		/* df ae */
npx_fild_long,		/* df af */
npx_fbstp,		/* df b0 */
npx_fbstp,		/* df b1 */
npx_fbstp,		/* df b2 */
npx_fbstp,		/* df b3 */
npx_fbstp,		/* df b4 */
npx_fbstp,		/* df b5 */
npx_fbstp,		/* df b6 */
npx_fbstp,		/* df b7 */
npx_fistp_long,		/* df b8 */
npx_fistp_long,		/* df b9 */
npx_fistp_long,		/* df ba */
npx_fistp_long,		/* df bb */
npx_fistp_long,		/* df bc */
npx_fistp_long,		/* df bd */
npx_fistp_long,		/* df be */
npx_fistp_long,		/* df bf */
npx_ffreep_f0,		/* df c0 */
npx_ffreep_f1,
npx_ffreep_f2,
npx_ffreep_f3,
npx_ffreep_f4,
npx_ffreep_f5,
npx_ffreep_f6,
npx_ffreep_f7,
npx_fxch_f0,		/* df c8 */
npx_fxch_f1,
npx_fxch_f2,
npx_fxch_f3,
npx_fxch_f4,
npx_fxch_f5,
npx_fxch_f6,
npx_fxch_f7,
npx_fstp_f0,		/* df d0 */
npx_fstp_f1,
npx_fstp_f2,
npx_fstp_f3,
npx_fstp_f4,
npx_fstp_f5,
npx_fstp_f6,
npx_fstp_f7,
npx_fstp_f0,		/* df d8 */
npx_fstp_f1,
npx_fstp_f2,
npx_fstp_f3,
npx_fstp_f4,
npx_fstp_f5,
npx_fstp_f6,
npx_fstp_f7,
npx_fstswax,		/* df e0 */
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,		/* df f0 */
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp,
npx_funimp
};

LOCAL BOOL DoNpxPrologue() {
	if (GET_EM() || GET_TS()) {
		INTx(0x7);
		return(FALSE);
	} else {
		return(TRUE);
	}
}

VOID ZFRSRVD(npx_instr)
IU32 npx_instr;
{
	__try
	{
		if (DoNpxPrologue())
			(*inst_table[npx_instr])();
	}
	__except ( _fpieee_flt( GetExceptionCode(), GetExceptionInformation(), DoNpxException ) )
	{ 
	} 
}


GLOBAL IU32 getNpxControlReg IFN0()
{
	return m_context.FloatSave.ControlWord;
}

GLOBAL VOID setNpxControlReg IFN1(IU32, newControl)
{
	m_context.FloatSave.ControlWord = newControl;
}

GLOBAL IU32 getNpxStatusReg IFN0()
{
	return m_context.FloatSave.StatusWord;
}

GLOBAL VOID setNpxStatusReg IFN1( IU32, newStatus)
{
	m_context.FloatSave.StatusWord = newStatus;
}

GLOBAL IU32 getNpxTagwordReg IFN0()
{
	return m_context.FloatSave.TagWord;
}

GLOBAL VOID setNpxTagwordReg IFN1(IU32, newTag)
{
	/* Don't do it!! */
	// m_context.FloatSave.TagWord = newTag;
}

GLOBAL void getNpxStackRegs IFN1(FPSTACKENTRY *, dumpPtr)
{
	memcpy((char *)dumpPtr, m_context.FloatSave.RegisterArea, 8 * sizeof(FPSTACKENTRY));
}

GLOBAL void setNpxStackRegs IFN1(FPSTACKENTRY *, loadPtr)
{
	memcpy(m_context.FloatSave.RegisterArea, (char *)loadPtr, 8 * sizeof(FPSTACKENTRY));
}


/* And finally some stubs */
GLOBAL void initialise_npx IFN0()
{
}

GLOBAL void npx_reset IFN0()
{
  
}
/* Called from cpu_init and cpu_reset */

GLOBAL VOID InitNpx IFN1(IBOOL, disabled)
{
}
