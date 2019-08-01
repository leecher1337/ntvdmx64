#include "sinit010.h"

IHPE  S_0363_CiGetVideolatches_00000000 ()
{
  return Gdp->VGAGlobals.latches;
}

IHPE  S_0364_CiGetVideorplane_00000001 ()
{
  return Gdp->VGAGlobals.VGA_rplane;
}

IHPE  S_0365_CiGetVideowplane_00000002 ()
{
  return Gdp->VGAGlobals.VGA_wplane;
}

IHPE  S_0366_CiGetVideoscratch_00000003 ()
{
  return Gdp->VGAGlobals.scratch;
}

IHPE  S_0367_CiGetVideosr_masked_val_00000004 ()
{
  return Gdp->VGAGlobals.sr_masked_val;
}

IHPE  S_0368_CiGetVideosr_nmask_00000005 ()
{
  return Gdp->VGAGlobals.sr_nmask;
}

IHPE  S_0369_CiGetVideodata_and_mask_00000006 ()
{
  return Gdp->VGAGlobals.data_and_mask;
}

IHPE  S_0370_CiGetVideodata_xor_mask_00000007 ()
{
  return Gdp->VGAGlobals.data_xor_mask;
}

IHPE  S_0371_CiGetVideolatch_xor_mask_00000008 ()
{
  return Gdp->VGAGlobals.latch_xor_mask;
}

IHPE  S_0372_CiGetVideobit_prot_mask_00000009 ()
{
  return Gdp->VGAGlobals.bit_prot_mask;
}

IHPE  S_0373_CiGetVideoplane_enable_0000000a ()
{
  return Gdp->VGAGlobals.plane_enable;
}

IHPE  S_0374_CiGetVideoplane_enable_mask_0000000b ()
{
  return Gdp->VGAGlobals.plane_enable_mask;
}

IHPE  S_0375_CiGetVideosr_lookup_0000000c ()
{
  return Gdp->VGAGlobals.sr_lookup;
}

IHPE  S_0376_CiGetVideofwd_str_read_addr_0000000d ()
{
  return Gdp->VGAGlobals.fwd_str_read_addr;
}

IHPE  S_0377_CiGetVideobwd_str_read_addr_0000000e ()
{
  return Gdp->VGAGlobals.bwd_str_read_addr;
}

IHPE  S_0378_CiGetVideodirty_total_0000000f ()
{
  return Gdp->VGAGlobals.dirty_total;
}

IHPE  S_0379_CiGetVideodirty_low_00000010 ()
{
  return Gdp->VGAGlobals.dirty_low;
}

IHPE  S_0380_CiGetVideodirty_high_00000011 ()
{
  return Gdp->VGAGlobals.dirty_high;
}

IHPE  S_0381_CiGetVideovideo_copy_00000012 ()
{
  return Gdp->VGAGlobals.video_copy;
}

IHPE  S_0382_CiGetVideomark_byte_00000013 ()
{
  return Gdp->VGAGlobals.mark_byte;
}

IHPE  S_0383_CiGetVideomark_word_00000014 ()
{
  return Gdp->VGAGlobals.mark_word;
}

IHPE  S_0384_CiGetVideomark_string_00000015 ()
{
  return Gdp->VGAGlobals.mark_string;
}

IHPE  S_0385_CiGetVideoread_shift_count_00000016 ()
{
  return Gdp->VGAGlobals.read_shift_count;
}

IHPE  S_0386_CiGetVideoread_mapped_plane_00000017 ()
{
  return Gdp->VGAGlobals.read_mapped_plane;
}

IHPE  S_0387_CiGetVideocolour_comp_00000018 ()
{
  return Gdp->VGAGlobals.colour_comp;
}

IHPE  S_0388_CiGetVideodont_care_00000019 ()
{
  return Gdp->VGAGlobals.dont_care;
}

IHPE  S_0389_CiGetVideov7_bank_vid_copy_off_0000001a ()
{
  return Gdp->VGAGlobals.v7_bank_vid_copy_off;
}

IHPE  S_0390_CiGetVideoscreen_ptr_0000001b ()
{
  return Gdp->VGAGlobals.screen_ptr;
}

IHPE  S_0391_CiGetVideorotate_0000001c ()
{
  return Gdp->VGAGlobals.rotate;
}

IHPE  S_0392_CiGetVideocalc_data_xor_0000001d ()
{
  return Gdp->VGAGlobals.calc_data_xor;
}

IHPE  S_0393_CiGetVideocalc_latch_xor_0000001e ()
{
  return Gdp->VGAGlobals.calc_latch_xor;
}

IHPE  S_0394_CiGetVideoread_byte_addr_0000001f ()
{
  return Gdp->VGAGlobals.read_byte_addr;
}

IHPE  S_0395_CiGetVideov7_fg_latches_00000020 ()
{
  return Gdp->VGAGlobals.v7_fg_latches;
}

IHPE  S_0396_CiGetVideoGC_regs_00000021 ()
{
  return Gdp->VGAGlobals.GCRegs;
}

IHPE  S_0397_CiGetVideolast_GC_index_00000022 ()
{
  return Gdp->VGAGlobals.lastGCindex;
}

IHPE  S_0398_CiGetVideodither_00000023 ()
{
  return Gdp->VGAGlobals.dither;
}

IHPE  S_0399_CiGetVideowrmode_00000024 ()
{
  return Gdp->VGAGlobals.wrmode;
}

IHPE  S_0400_CiGetVideochain_00000025 ()
{
  return Gdp->VGAGlobals.chain;
}

IHPE  S_0401_CiGetVideowrstate_00000026 ()
{
  return Gdp->VGAGlobals.wrstate;
}
