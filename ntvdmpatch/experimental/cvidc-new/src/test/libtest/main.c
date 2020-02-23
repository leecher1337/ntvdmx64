#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <windows.h>
#include <malloc.h>
#include "host_def.h"
#include "insignia.h"
#include "cpu_vid.h"
#include "egacpu.h"
#include "egaports.h"
#include "ga_mark.h"
#include "../../evidgen.h"
#include "../cvid_if.h"

sys_addr gvi_pc_low_regen = 0;
GLOBAL VOID (*bios_ch2_byte_wrt_fn)();
GLOBAL VOID (*bios_ch2_word_wrt_fn)();
GLOBAL IHP  __stdcall setup_global_data_ptr ();
GLOBAL void __stdcall setup_vga_globals ();

struct	EGA_CPU_GLOBALS	EGA_CPU;
IU8 *video_copy, *EGA_planes;

BOOL WINAPI _CRT_INIT(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved);

extern void __stdcall SetBiosWrites (IUH markset);
extern void __stdcall setReadPointers (IUH readset);
extern void __stdcall setWritePointers ();
extern void __stdcall setMarkPointers (IUH markset);

extern void  __stdcall write_byte_ev_glue (IU32 eaOff, IU8 eaVal);
extern void  __stdcall write_word_ev_glue (IU32 eaOff, IU16 eaVal);
extern void  __stdcall write_dword_ev_glue (IU32 eaOff, IU32 eaVal);
extern void  __stdcall fill_byte_ev_glue (IU32 eaOff, IU8 eaVal, IU32 count);
extern void  __stdcall fill_word_ev_glue (IU32 eaOff, IU8 eaVal, IU32 count);
extern void  __stdcall fill_dword_ev_glue (IU32 eaOff, IU32 eaVal, IU32 count);
extern void  __stdcall move_byte_fwd_ev_glue (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM);
extern void  __stdcall move_byte_bwd_ev_glue (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM);
extern void  __stdcall move_word_fwd_ev_glue (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM);
extern void  __stdcall move_word_bwd_ev_glue (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM);
extern void  __stdcall move_dword_fwd_ev_glue (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM);
extern void  __stdcall move_dword_bwd_ev_glue (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM);
extern IU32 __stdcall read_byte_ev_glue (IU32 eaOff);
extern IU32 __stdcall read_word_ev_glue (IU32 eaOff);
extern IU32 __stdcall read_dword_ev_glue (IU32 eaOff);
extern void __stdcall read_str_fwd_ev_glue (IU8 * dest, IU32 eaOff, IU32 count);
extern void __stdcall read_str_bwd_ev_glue (IU8 * dest, IU32 eaOff, IU32 count);

extern struct VideoVector Video;
extern EVID_WRT_POINTERS simple_evid;
extern EVID_WRT_POINTERS gricvid_evid;
extern EVID_WRT_POINTERS dith_evid;
extern EVID_WRT_POINTERS chain2_evid;
extern EVID_WRT_POINTERS unchained_evid;
extern EVID_WRT_POINTERS chain4_evid;
extern EVID_READ_POINTERS simple_read_evid;
extern EVID_READ_POINTERS ram_dsbld_read_evid;
extern EVID_READ_POINTERS read_mode0_evid;
extern EVID_READ_POINTERS read_mode1_evid;
extern EVID_MARK_POINTERS simple_mark_evid;
extern EVID_MARK_POINTERS cga_mark_evid;
extern EVID_MARK_POINTERS unchained_mark_evid;
extern EVID_MARK_POINTERS chain4_mark_evid;


struct cvid_interface CVID_IF =
{
	&Video,
	SetBiosWrites,
	setReadPointers,
	setWritePointers,
	setMarkPointers,

	write_byte_ev_glue,
	write_word_ev_glue,
	write_dword_ev_glue,
	fill_byte_ev_glue,
	fill_word_ev_glue,
	fill_dword_ev_glue,
	move_byte_fwd_ev_glue,
	move_byte_bwd_ev_glue,
	move_word_fwd_ev_glue,
	move_word_bwd_ev_glue,
	move_dword_fwd_ev_glue,
	move_dword_bwd_ev_glue,
	read_byte_ev_glue,
	read_word_ev_glue,
	read_dword_ev_glue,
	read_str_fwd_ev_glue,
	read_str_bwd_ev_glue,

	&simple_evid,
	&gricvid_evid,
	&dith_evid,
	&chain2_evid,
	&unchained_evid,
	&chain4_evid,
	&simple_read_evid,
	&ram_dsbld_read_evid,
	&read_mode0_evid,
	&read_mode1_evid,
	&simple_mark_evid,
	&cga_mark_evid,
	&unchained_mark_evid,
	&chain4_mark_evid
};


ULONG sr_lookup[16] =
{
#ifdef LITTLEND
	0x00000000,0x000000ff,0x0000ff00,0x0000ffff,
	0x00ff0000,0x00ff00ff,0x00ffff00,0x00ffffff,
	0xff000000,0xff0000ff,0xff00ff00,0xff00ffff,
	0xffff0000,0xffff00ff,0xffffff00,0xffffffff
#endif
#ifdef BIGEND
	0x00000000,0xff000000,0x00ff0000,0xffff0000,
	0x0000ff00,0xff00ff00,0x00ffff00,0xffffff00,
	0x000000ff,0xff0000ff,0x00ff00ff,0xffff00ff,
	0x0000ffff,0xff00ffff,0x00ffffff,0xffffffff
#endif
};


GLOBAL VOID _simple_mark_lge()
{
}

GLOBAL VOID _simple_mark_sml()
{
}


VOID
init_vga_globals IFN0()
{
	video_copy = malloc (0x20000);
	//EGA_planes = malloc(4*EGA_PLANE_SIZE);
	Video.SetVideov7_bank_vid_copy_off(0);
	Video.SetVideosr_lookup(sr_lookup);
	Video.SetVideovideo_copy(&video_copy[0]);
	Video.SetVideoscratch(malloc(0x10000));
	//Video.SetVideoscreen_ptr(EGA_planes);
	Video.SetVideorotate(0);
	Video.SetVideomark_byte(_simple_mark_sml);
	Video.SetVideomark_word(_simple_mark_sml);
	Video.SetVideomark_string(_simple_mark_lge);
	//Video.SetVideorplane(EGA_planes);
	//Video.SetVideowplane(EGA_planes);
}


int __stdcall
InitializeDLL (
    IN void * DllHandle,
    IN ULONG Reason,
    IN PCONTEXT Context OPTIONAL
    ) 
{
	if (Reason == DLL_PROCESS_ATTACH)
	{
		_CRT_INIT(DllHandle, Reason, Context);
		setup_global_data_ptr();
		setup_vga_globals();
		init_vga_globals();
	}
	return TRUE;
}

int main(int argc,char **argv)
{
	return 0;
}
