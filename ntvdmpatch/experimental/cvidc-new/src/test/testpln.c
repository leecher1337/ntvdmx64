#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <windows.h>
#include "host_def.h"
#include "insignia.h"
#include "cpu_vid.h"
#include "egacpu.h"
#include "egaports.h"
#include "../evidgen.h"
#include "../evid_c.h"
#include "ga_mark.h"
#include "cvid_if.h"
#include <stdlib.h>
#include <stdio.h>

#define MEM_START	1024
#define STDERR stdout

struct cvid_interface *Itst, *Iprd;
IU8 *EGA_planestst, *EGA_planesprd;
IU8 *video_copytst, *video_copyprd;

void dump_planes();

#define TEST_RW_FUNC(wrfunc,rdfunc,mask) \
	IU32 datatst, dataprod; \
	int ret = 0; \
	Iprd->wrfunc(MEM_START, c); \
	Itst->wrfunc(MEM_START, c); \
	if (memcmp(EGA_planestst, EGA_planesprd, 4* EGA_PLANE_SIZE)) \
	{ \
		fprintf (STDERR, #wrfunc" failed, plane buffers differ!\n"); \
		dump_planes(); \
		check_vgaglobals(#rdfunc); \
		dump_vgaglobals(); \
		exit(-1); \
		ret = -1; \
	} \
	datatst = Itst->rdfunc(MEM_START) & mask; \
	dataprod = Iprd->rdfunc(MEM_START) & mask; \
	if (datatst!=dataprod) \
	{ \
		fprintf (STDERR, #rdfunc" 0x%08X failed, dataprod=%08X, datatst=%08X\n", c, dataprod, datatst); \
		ret = -1; \
	} \
	check_states(#rdfunc, c); \
	return ret;

//#define BACK_M
#ifdef BACK_M
#define MEM_OFFS MEM_START-1
#else
#define MEM_OFFS 0
#endif

void hexdump(unsigned char *buf, int len)
{
	int i;
	for (i=0; i<len; i++) fprintf(STDERR, "%02X", buf[i]);
	fprintf(STDERR, "\n");
}

int compare_val(char *pszFunc, unsigned int prd, unsigned int tst, char *pszWhat)
{
	if (prd != tst)
	{
		fprintf (STDERR, "%s %s differ (prd:%08X tst:%08X)!\n", pszFunc, pszWhat, 
			prd, tst);
		exit(-1);
		return -1;
	}
	return 0;
}

void dump_val(unsigned int prd, unsigned int tst, char *pszWhat)
{
	printf ("%-16s:\t%08X\t%08X\n", pszWhat, prd, tst);
}

void check_vgaglobals(char *pszFunc)
{
	compare_val(pszFunc, Iprd->Video->GetVideolatches(), Itst->Video->GetVideolatches(), "video latches");
	compare_val(pszFunc, Iprd->Video->GetVideobit_prot_mask(), Itst->Video->GetVideobit_prot_mask(), "bit_prot_mask");
	compare_val(pszFunc, Iprd->Video->GetVideosr_masked_val(), Itst->Video->GetVideosr_masked_val(), "sr_masked_val");
	compare_val(pszFunc, Iprd->Video->GetVideodirty_total(), Itst->Video->GetVideodirty_total(), "dirty_total");
	compare_val(pszFunc, Iprd->Video->GetVideodirty_low(), Itst->Video->GetVideodirty_low(), "dirty_low");
	compare_val(pszFunc, Iprd->Video->GetVideodirty_high(), Itst->Video->GetVideodirty_high(), "dirty_high");
}

void dump_vgaglobals()
{
	printf ("VGA Globals:     \tPRODUCTION\tTEST\n");
	dump_val(Iprd->Video->GetVideolatches(), Itst->Video->GetVideolatches(), "video latches");
	dump_val(Iprd->Video->GetVideobit_prot_mask(), Itst->Video->GetVideobit_prot_mask(), "bit_prot_mask");
	dump_val(Iprd->Video->GetVideosr_masked_val(), Itst->Video->GetVideosr_masked_val(), "sr_masked_val");
	dump_val(Iprd->Video->GetVideosr_nmask(), Itst->Video->GetVideosr_nmask(), "sr_nmask");
	dump_val(Iprd->Video->GetVideodirty_total(), Itst->Video->GetVideodirty_total(), "dirty_total");
	dump_val(Iprd->Video->GetVideodirty_low(), Itst->Video->GetVideodirty_low(), "dirty_low");
	dump_val(Iprd->Video->GetVideodirty_high(), Itst->Video->GetVideodirty_high(), "dirty_high");
	dump_val(Iprd->Video->GetVideoplane_enable(), Itst->Video->GetVideoplane_enable(), "plane_enable");
	dump_val(Iprd->Video->GetVideoplane_enable_mask(), Itst->Video->GetVideoplane_enable_mask(), "plane_enable_mask");
}

int check_states(char *pszFunc, int c)
{
	if (memcmp(EGA_planestst, EGA_planesprd, 4* EGA_PLANE_SIZE))
	{
		fprintf (STDERR, "%s %d failed, plane buffers differ!\n", pszFunc, c);
		dump_planes();
		check_vgaglobals(pszFunc);
		dump_vgaglobals();
		exit(-1);
		return -1;
	}
	if (memcmp(video_copytst, video_copyprd, 0x20000))
	{
		fprintf (STDERR, "%s failed, videocopy buffers differ!\n", pszFunc);
		dump_planes();
		exit(-1);
		return -1;
	}
	check_vgaglobals(pszFunc);
	return 0;
}

#define TEST_FILL_FUNC(wrfunc,rdfunc,sz,offs) \
	unsigned char buftst[MEM_START], bufprd[MEM_START]; \
	int ret = 0, i; \
	Iprd->wrfunc(MEM_START, c, MEM_START/sz); \
	Itst->wrfunc(MEM_START, c, MEM_START/sz); \
	if (memcmp(EGA_planestst, EGA_planesprd, 4* EGA_PLANE_SIZE)) \
	{ \
		fprintf (STDERR, #wrfunc" %d failed, plane buffers differ!\n", c); \
		dump_planes(); \
		check_vgaglobals(#wrfunc); \
		dump_vgaglobals(); \
		exit(-1); \
		ret = -1; \
	} \
	Iprd->rdfunc(bufprd+MEM_OFFS, MEM_START+offs, MEM_START); \
	Itst->rdfunc(buftst+MEM_OFFS, MEM_START+offs, MEM_START); \
	if (memcmp(buftst, bufprd, MEM_START)) \
	{ \
		fprintf (STDERR, #rdfunc" failed:\nTestbuf:\n"); \
		hexdump(buftst, MEM_START); \
		fprintf (STDERR, "Prodbuf:\n"); \
		hexdump(bufprd, MEM_START); \
		ret = -1; \
	} \
	check_states(#rdfunc, c); \
	return ret;

void dump_ram(unsigned char *bufram, int sz)
{
	FILE *fp;

	fp=fopen("planeram.bin", "wb");
	fwrite(bufram, sz, 1, fp);
	fclose(fp);
}

void dump_planes()
{
	FILE *fp;

	fp=fopen("planetst.bin", "wb");
	fwrite(EGA_planestst, EGA_PLANE_SIZE, 4, fp);
	fclose(fp);
	fp=fopen("planeord.bin", "wb");
	fwrite(EGA_planesprd, EGA_PLANE_SIZE, 4, fp);
	fclose(fp);
}

#define TEST_MOV_FUNC_SRCRAM(wrfunc,rdfunc,sz) \
	unsigned char buftst[MEM_START], bufprd[MEM_START]; \
	int ret = 0, i; \
	Iprd->wrfunc(MEM_START, bufram+MEM_OFFS, MEM_START/sz, 1); \
	Itst->wrfunc(MEM_START, bufram+MEM_OFFS, MEM_START/sz, 1); \
	if (memcmp(EGA_planestst, EGA_planesprd, 4* EGA_PLANE_SIZE)) \
	{ \
		fprintf (STDERR, #wrfunc" failed, plane buffers differ!\n"); \
		dump_planes(); \
		dump_ram(bufram+MEM_OFFS, MEM_START/sz); \
		check_vgaglobals(#wrfunc); \
		dump_vgaglobals(); \
		exit(-1); \
		ret = -1; \
	} \
	Iprd->rdfunc(bufprd+MEM_OFFS, MEM_START, MEM_START); \
	Itst->rdfunc(buftst+MEM_OFFS, MEM_START, MEM_START); \
	if (memcmp(buftst, bufprd, MEM_START)) \
	{ \
		fprintf (STDERR, #rdfunc" failed:\nTestbuf:\n"); \
		hexdump(buftst, MEM_START); \
		fprintf (STDERR, "Prodbuf:\n"); \
		hexdump(bufprd, MEM_START); \
		ret = -1; \
	} \
	check_states(#rdfunc, 0); \
	return ret;


#define TEST_MOV_FUNC(wrfunc,rdfunc,sz) \
	unsigned char buftst[MEM_START], bufprd[MEM_START]; \
	int ret = 0, i; \
	Iprd->move_byte_fwd_ev_glue(MEM_START*2, bufram+MEM_OFFS, MEM_START, 1); \
	Iprd->wrfunc(MEM_START, MEM_START*2, MEM_START/sz, 0); \
	Iprd->rdfunc(bufprd+MEM_OFFS, MEM_START, MEM_START); \
	Itst->rdfunc(buftst+MEM_OFFS, MEM_START, MEM_START); \
	if (memcmp(buftst, bufprd, MEM_START)) \
	{ \
		fprintf (STDERR, #rdfunc" failed:\nTestbuf:\n"); \
		hexdump(buftst, MEM_START); \
		fprintf (STDERR, "Prodbuf:\n"); \
		hexdump(bufprd, MEM_START); \
		ret = -1; \
	} \
	Iprd->fill_byte_ev_glue(MEM_START, 0xEE, MEM_START); \
	Itst->wrfunc(MEM_START, MEM_START*2, MEM_START/sz, 0); \
	Iprd->rdfunc(bufprd+MEM_OFFS, MEM_START, MEM_START); \
	if (memcmp(buftst, bufprd, MEM_START)) \
	{ \
		fprintf (STDERR, #wrfunc" failed:\nTestbuf:\n"); \
		hexdump(buftst, MEM_START); \
		fprintf (STDERR, "Prodbuf:\n"); \
		hexdump(bufprd, MEM_START); \
		ret = -1; \
	} \
	return ret;


int test_b_rw(int c)
{
	TEST_RW_FUNC(write_byte_ev_glue,read_byte_ev_glue,0xFF)
}

int test_w_rw(int c)
{
	TEST_RW_FUNC(write_word_ev_glue,read_word_ev_glue,0xFFFF)
}

int test_d_rw(int c)
{
	TEST_RW_FUNC(write_dword_ev_glue,read_dword_ev_glue,0xFFFFFFFF)
}


int test_b_fill(int c)
{
	TEST_FILL_FUNC(fill_byte_ev_glue,read_str_fwd_ev_glue,sizeof(IU8),0)
}

int test_w_fill(int c)
{
	TEST_FILL_FUNC(fill_word_ev_glue,read_str_fwd_ev_glue,sizeof(IU16),0)
}

int test_d_fill(int c)
{
	TEST_FILL_FUNC(fill_dword_ev_glue,read_str_fwd_ev_glue,sizeof(IU32),0)
}


/* Backwards tests doen't work this way, as we don't know the scaling in memory */
int test_b_fill_bw(int c)
{
	TEST_FILL_FUNC(fill_byte_ev_glue,read_str_bwd_ev_glue,sizeof(IU8),(MEM_START-1))
}

int test_w_fill_bw(int c)
{
	TEST_FILL_FUNC(fill_word_ev_glue,read_str_bwd_ev_glue,sizeof(IU16),(MEM_START-1))
}

int test_d_fill_bw(int c)
{
	TEST_FILL_FUNC(fill_dword_ev_glue,read_str_bwd_ev_glue,sizeof(IU32),(MEM_START-1))
}


int test_b_mov_ram(char *bufram, char *expect)
{
	TEST_MOV_FUNC_SRCRAM(move_byte_fwd_ev_glue,read_str_fwd_ev_glue,sizeof(IU8))
}

int test_w_mov_ram(char *bufram, char *expect)
{
	TEST_MOV_FUNC_SRCRAM(move_word_fwd_ev_glue,read_str_fwd_ev_glue,sizeof(IU16))
}

int test_d_mov_ram(char *bufram, char *expect)
{
	TEST_MOV_FUNC_SRCRAM(move_dword_fwd_ev_glue,read_str_fwd_ev_glue,sizeof(IU32))
}


int test_b_mov(char *bufram)
{
	TEST_MOV_FUNC(move_byte_fwd_ev_glue,read_str_fwd_ev_glue,sizeof(IU8))
}

int test_w_mov(char *bufram)
{
	TEST_MOV_FUNC(move_word_fwd_ev_glue,read_str_fwd_ev_glue,sizeof(IU16))
}

int test_d_mov(char *bufram)
{
	/* NB: S_2147_SimpleDwordMove_Fwd_id testing fails, but imho this is due
	 * to a bug in the original CVIDC function, because fromOff never gets 
	 * incremented in the loop which fills destination area with just the 
	 * last DWORD. 
	 * This seems to be incorrect, so testing this function will fail for
	 * good reason.
	 */
	TEST_MOV_FUNC(move_dword_fwd_ev_glue,read_str_fwd_ev_glue,sizeof(IU32))
}

// -----------------------------

int test_func_b(const char *pszName, const char *pszFunc, int (*test_func)(int))
{
	IU32 data;

	printf ("Testing %s byte %s...", pszName, pszFunc);
	for (data=0; data<256; data++)
		if (test_func(data)<0) 
			return -1;
	printf ("OK\n");
	return 0;
}

int test_func_w(const char *pszName, const char *pszFunc, int (*test_func)(int))
{
	IU32 data;

	printf ("Testing %s word %s...", pszName, pszFunc);
	// Shortcut:
	for (data=0; data<256; data++)
		if (test_func(data | (0xFF-data)<<8)<0) 
			return -1;

	printf ("OK\n");
	return 0;
}

int test_func_d(const char *pszName, const char *pszFunc, int (*test_func)(int))
{
	IU32 data;

	printf ("Testing %s dword %s...", pszName, pszFunc);
	// Shortcut so that tests don't take too long, not everything tested, but approximate..
	for (data=0; data<256; data++)
		if (test_func(((0xFF-data)<<24) | ((0xEE-data)<<16) | ((0xDD-data)<<8) | (0xCC-data)) <0) 
			return -1;

	printf ("OK\n");
	return 0;
}

int test_func_mov(const char *pszName, const char *pszFunc, int (*test_func)(char *,char*), int rdmode)
{
	char bufram[MEM_START*4], expect[MEM_START], *pexpect;
	int i;

	printf ("Testing %s %s...", pszName, pszFunc);
pexpect = NULL;
	if (test_func(bufram, pexpect)<0) 
		return -1;
	printf("OK\n");
	return 0;
}

int test_func_all_rw(const char *pszName, int rdmode)
{
	int ret = 0, i;

	ret+=test_func_b(pszName, "read/write", test_b_rw);
	ret+=test_func_w(pszName, "read/write", test_w_rw);
	ret+=test_func_d(pszName, "read/write", test_d_rw);

	ret+=test_func_b(pszName, "fill", test_b_fill);
	ret+=test_func_w(pszName, "fill", test_w_fill);
	ret+=test_func_d(pszName, "fill", test_d_fill);

	ret+=test_func_mov(pszName, "byte move ram->vid", test_b_mov_ram, rdmode);
	ret+=test_func_mov(pszName, "word move ram->vid", test_w_mov_ram, rdmode);
	ret+=test_func_mov(pszName, "dword move ram->vid", test_d_mov_ram, rdmode);

	/* Now there functions are buggy in original CVID! So we cannot validate
	ret+=test_func_mov(pszName, "byte move vid->vid", test_b_mov, rdmode);
	ret+=test_func_mov(pszName, "word move vid->vid", test_w_mov, rdmode);
	ret+=test_func_mov(pszName, "dword move vid->vid", test_d_mov, rdmode);
	*/

	return ret;
}


int test_wrmode(char *pszName, int mode, int *idxs, int nIdxs, int rdmode)
{
	char szName[64],*p;
	int j, ret = 0;

	Itst->Video->SetVideowrmode(mode);
	Iprd->Video->SetVideowrmode(mode);

	p = szName + sprintf (szName, "%s WrMode %i ", pszName, mode);
	
	for (j=0; j<nIdxs; j++)
	{
		sprintf (p, "WrState %d", idxs[j]);
		Itst->Video->SetVideowrstate(idxs[j]);
		Iprd->Video->SetVideowrstate(idxs[j]);
		Itst->setWritePointers();
		Iprd->setWritePointers();

		ret += test_func_all_rw(szName, rdmode);
	}
	return ret;
}

void test_func_all_wrmodes_rw(const char *pszName, int rdmode)
{
	char szName[64];
	int idx_0[] = {0, 1, 2, 3, 16, 17, 18, 19, 22, 23},
		idx_1[] = {0},
		idx_2[] = {0, 1, 2, 3};

	test_wrmode(pszName, 0, idx_0, sizeof(idx_0)/sizeof(idx_0[0]), rdmode);
	test_wrmode(pszName, 1, idx_1, sizeof(idx_1)/sizeof(idx_1[0]), rdmode);
	test_wrmode(pszName, 2, idx_2, sizeof(idx_2)/sizeof(idx_2[0]), rdmode);
	test_wrmode(pszName, 3, idx_2, sizeof(idx_2)/sizeof(idx_2[0]), rdmode);
}

void test_chain2_rw(const char *pszName, int rdmode)
{
	int i, idx_1[] = {0};

	for (i=0; i<=4; i++)
		test_wrmode(pszName, i, idx_1, 1, rdmode);
}

void rdmode_probe(const char *pszName, int (*fpfunc)(const char *,int))
{
	int i;
	char szName[64];

	for (i=0; i<4; i++)
	{
		sprintf (szName, "%s RdMode %d", pszName, i);
		Itst->setReadPointers(i);
		Iprd->setReadPointers(i);
		fpfunc(szName, i);
	}
}

void set_chain(int chain)
{
	Itst->Video->SetVideochain(chain);
	Iprd->Video->SetVideochain(chain);
	Itst->setWritePointers();
	Iprd->setWritePointers();
}

/* For some reason, this is not set in CVID interface, but tests don't pass if
 * we don't have a fwd_str_read_addr function.
 * I wonder if this just doesn't get used in production or why it doesn't crash there...?
 *
 * NTVDM expects this to be set in setReadPointers(), but it doesn't get set there
 */
void StringReadFwd(IU8 * dest, IU32 eaOff, IU32 count, IUH destInRam)
{
	
}


int main(int argc, CHAR ** argv)
{
	int i;
	char szFunc[128];
	HMODULE hLibTest, hLibProd;
	IU8 *EGA_planes;

	if (!(hLibTest=LoadLibrary("cvidtest.dll")))
	{
		fprintf(STDERR, "Cannot load cvidtest (%d)\n", GetLastError());
		return -1;
	}
	if (!(hLibProd=LoadLibrary("cvidprod.dll")))
	{
		fprintf(STDERR, "Cannot load cvidprod (%d)\n", GetLastError());
		return -1;
	}

	if (!(Itst=(struct cvid_interface*)GetProcAddress(hLibTest, "CVID_IF")))
	{
		fprintf(STDERR, "Cannot find VCID_IF in test\n");
		return -1;
	}
	if (!(Iprd=(struct cvid_interface*)GetProcAddress(hLibProd, "CVID_IF")))
	{
		fprintf(STDERR, "Cannot find VCID_IF in prod\n");
		return -1;
	}


	EGA_planestst = malloc(4 * EGA_PLANE_SIZE);
	for (i=0; i<4 * EGA_PLANE_SIZE; i++) EGA_planestst[i]=i%0x100;
	Itst->Video->SetVideoscreen_ptr(EGA_planestst);
	Itst->Video->SetVideorplane(EGA_planestst);
	Itst->Video->SetVideowplane(EGA_planestst);

	EGA_planesprd = malloc(4 * EGA_PLANE_SIZE);
	memcpy(EGA_planesprd, EGA_planestst, 4 * EGA_PLANE_SIZE);
	Iprd->Video->SetVideoscreen_ptr(EGA_planesprd);
	Iprd->Video->SetVideorplane(EGA_planesprd);
	Iprd->Video->SetVideowplane(EGA_planesprd);

	video_copyprd = Iprd->Video->GetVideovideo_copy();
	video_copytst = Iprd->Video->GetVideovideo_copy();
	memset(video_copyprd, 0, 0x20000);
	memset(video_copytst, 0, 0x20000);

	Iprd->Video->SetVideofwd_str_read_addr(StringReadFwd);
	Iprd->Video->SetVideobwd_str_read_addr(StringReadFwd);
	Itst->Video->SetVideofwd_str_read_addr(StringReadFwd);
	Itst->Video->SetVideobwd_str_read_addr(StringReadFwd);

	
	Itst->setMarkPointers(0);
	Iprd->setMarkPointers(0);

	set_chain(SIMPLE_WRITES);
	Itst->setReadPointers(3);
	Iprd->setReadPointers(3);
	test_func_all_rw("simple_evid", 3);

	Itst->Video->SetVideorotate(1);
	Iprd->Video->SetVideorotate(1);
	Itst->setMarkPointers(2);
	Iprd->setMarkPointers(2);
	Itst->Video->SetVideoplane_enable(15);
	Iprd->Video->SetVideoplane_enable(15);
	set_chain(UNCHAINED);
	rdmode_probe("gricvid_evid",test_func_all_wrmodes_rw);


	Itst->Video->SetVideorotate(0);
	Iprd->Video->SetVideorotate(0);

	rdmode_probe("unchained_evid", test_func_all_wrmodes_rw);

	set_chain(CHAIN_4);
	Itst->setMarkPointers(3);
	Iprd->setMarkPointers(3);
	// original CVIDC is buggy (see note at C4BFLL), therefore set to 0:
	Itst->Video->SetVideoplane_enable(0);
	Iprd->Video->SetVideoplane_enable(0);
	rdmode_probe("chain4_evid", test_func_all_wrmodes_rw);

	set_chain(CHAIN_2);
	Itst->setMarkPointers(1);
	Iprd->setMarkPointers(1);
	rdmode_probe("chain2_evid", test_chain2_rw);

	return 0;
}

