#include "sinit010.h"

void  S_0402_CiSetVideolatches_00000027 (IHPE value)
{
  Gdp->VGAGlobals.latches = value;
}

void  S_0403_CiSetVideorplane_00000028 (IHPE value)
{
  Gdp->VGAGlobals.VGA_rplane = (IU8)value;
}

void  S_0404_CiSetVideowplane_00000029 (IHPE value)
{
  Gdp->VGAGlobals.VGA_wplane = (IU8)value;
}

void  S_0405_CiSetVideoscratch_0000002a (IHPE value)
{
  Gdp->VGAGlobals.scratch = (IU8)value;
}

void  S_0406_CiSetVideosr_masked_val_0000002b (IHPE value)
{
  Gdp->VGAGlobals.sr_masked_val = value;
}

void  S_0407_CiSetVideosr_nmask_0000002c (IHPE value)
{
  Gdp->VGAGlobals.sr_nmask = value;
}

void  S_0408_CiSetVideodata_and_mask_0000002d (IHPE value)
{
  Gdp->VGAGlobals.data_and_mask = value;
}

void  S_0409_CiSetVideodata_xor_mask_0000002e (IHPE value)
{
  Gdp->VGAGlobals.data_xor_mask = value;
}

void  S_0410_CiSetVideolatch_xor_mask_0000002f (IHPE value)
{
  Gdp->VGAGlobals.latch_xor_mask = value;
}

void  S_0411_CiSetVideobit_prot_mask_00000030 (IHPE value)
{
  Gdp->VGAGlobals.bit_prot_mask = value;
}

void  S_0412_CiSetVideoplane_enable_00000031 (IHPE value)
{
  Gdp->VGAGlobals.plane_enable = value;
}

void  S_0413_CiSetVideoplane_enable_mask_00000032 (IHPE value)
{
  Gdp->VGAGlobals.plane_enable_mask = value;
}

void  S_0414_CiSetVideosr_lookup_00000033 (IHPE value)
{
  Gdp->VGAGlobals.sr_lookup = (IUH)value;
}

void  S_0415_CiSetVideofwd_str_read_addr_00000034 (IHPE value)
{
  Gdp->VGAGlobals.fwd_str_read_addr = (void (__cdecl *)())value;
}

void  S_0416_CiSetVideobwd_str_read_addr_00000035 (IHPE value)
{
  Gdp->VGAGlobals.bwd_str_read_addr = (void (__cdecl *)())value;
}

void  S_0417_CiSetVideodirty_total_00000036 (IHPE value)
{
  Gdp->VGAGlobals.dirty_total = value;
}

void  S_0418_CiSetVideodirty_low_00000037 (IHPE value)
{
  Gdp->VGAGlobals.dirty_low = value;
}

void  S_0419_CiSetVideodirty_high_00000038 (IHPE value)
{
  Gdp->VGAGlobals.dirty_high = value;
}

void  S_0420_CiSetVideovideo_copy_00000039 (IHPE value)
{
  Gdp->VGAGlobals.video_copy = (IU8)value;
}

void  S_0421_CiSetVideomark_byte_0000003a (IHPE value)
{
  Gdp->VGAGlobals.mark_byte = (void (__cdecl *)())value;
}

void  S_0422_CiSetVideomark_word_0000003b (IHPE value)
{
  Gdp->VGAGlobals.mark_word = (void (__cdecl *)())value;
}

void  S_0423_CiSetVideomark_string_0000003c (IHPE value)
{
  Gdp->VGAGlobals.mark_string = (void (__cdecl *)())value;
}

void  S_0424_CiSetVideoread_shift_count_0000003d (IHPE value)
{
  Gdp->VGAGlobals.read_shift_count = value;
}

void  S_0425_CiSetVideoread_mapped_plane_0000003e (IHPE value)
{
  Gdp->VGAGlobals.read_mapped_plane = value;
}

void  S_0426_CiSetVideocolour_comp_0000003f (IHPE value)
{
  Gdp->VGAGlobals.colour_comp = value;
}

void  S_0427_CiSetVideodont_care_00000040 (IHPE value)
{
  Gdp->VGAGlobals.dont_care = value;
}

void  S_0428_CiSetVideov7_bank_vid_copy_off_00000041 (IHPE value)
{
  Gdp->VGAGlobals.v7_bank_vid_copy_off = value;
}

void  S_0429_CiSetVideoscreen_ptr_00000042 (IHPE value)
{
  Gdp->VGAGlobals.screen_ptr = (IU8)value;
}

void  S_0430_CiSetVideorotate_00000043 (IHPE value)
{
  Gdp->VGAGlobals.rotate = value;
}

void  S_0431_CiSetVideocalc_data_xor_00000044 (IHPE value)
{
  Gdp->VGAGlobals.calc_data_xor = value;
}

void  S_0432_CiSetVideocalc_latch_xor_00000045 (IHPE value)
{
  Gdp->VGAGlobals.calc_latch_xor = value;
}

void  S_0433_CiSetVideoread_byte_addr_00000046 (IHPE value)
{
  Gdp->VGAGlobals.read_byte_addr = (IUH*)value;
}

void  S_0434_CiSetVideov7_fg_latches_00000047 (IHPE value)
{
  Gdp->VGAGlobals.v7_fg_latches = value;
}

void  S_0435_CiSetVideoGC_regs_00000048 (IHPE value)
{
  Gdp->VGAGlobals.GCRegs = (IUH**)value;
}

void  S_0436_CiSetVideolast_GC_index_00000049 (IHPE value)
{
  Gdp->VGAGlobals.lastGCindex = (IU8)value;
}

void  S_0437_CiSetVideodither_0000004a (IHPE value)
{
  Gdp->VGAGlobals.dither = (IU8)value;
}

void  S_0438_CiSetVideowrmode_0000004b (IHPE value)
{
  Gdp->VGAGlobals.wrmode = (IU8)value;
}

void  S_0439_CiSetVideochain_0000004c (IHPE value)
{
  Gdp->VGAGlobals.chain = (IU8)value;
}

void  S_0440_CiSetVideowrstate_0000004d (IHPE value)
{
  Gdp->VGAGlobals.wrstate = (IU8)value;
}

