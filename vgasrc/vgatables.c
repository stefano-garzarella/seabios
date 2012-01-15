// Tables used by VGA bios
//
// Copyright (C) 2009  Kevin O'Connor <kevin@koconnor.net>
// Copyright (C) 2001-2008 the LGPL VGABios developers Team
//
// This file may be distributed under the terms of the GNU LGPLv3 license.

#include "vgabios.h" // struct VideoParamTableEntry_s
#include "biosvar.h" // GET_GLOBAL
#include "util.h" // memcpy_far
#include "stdvga.h" // struct vgamode_s


/****************************************************************
 * Video parameter table
 ****************************************************************/

// Standard Video Save Pointer Table
struct VideoSavePointer_s {
    struct segoff_s videoparam;
    struct segoff_s paramdynamicsave;
    struct segoff_s textcharset;
    struct segoff_s graphcharset;
    struct segoff_s secsavepointer;
    u8 reserved[8];
} PACKED;

struct VideoSavePointer_s video_save_pointer_table VAR16;

// standard BIOS Video Parameter Table
struct VideoParam_s {
    u8 twidth;
    u8 theightm1;
    u8 cheight;
    u16 slength;
    u8 sequ_regs[4];
    u8 miscreg;
    u8 crtc_regs[25];
    u8 actl_regs[20];
    u8 grdc_regs[9];
} PACKED;

struct VideoParam_s video_param_table[29] VAR16;

void
build_video_param(void)
{
    static u8 parammodes[ARRAY_SIZE(video_param_table)] VAR16 = {
        0, 0, 0, 0, 0x04, 0x05, 0x06, 0x07,
        0, 0, 0, 0, 0, 0x0d, 0x0e, 0,
        0, 0x0f, 0x10, 0, 0, 0, 0, 0x01,
        0x03, 0x07, 0x11, 0x12, 0x13
    };

    int i;
    for (i=0; i<ARRAY_SIZE(parammodes); i++) {
        int mode = GET_GLOBAL(parammodes[i]);
        if (! mode)
            continue;
        struct VideoParam_s *vparam_g = &video_param_table[i];
        struct vgamode_s *vmode_g = stdvga_find_mode(mode);
        if (!vmode_g)
            continue;
        int width = GET_GLOBAL(vmode_g->width);
        int height = GET_GLOBAL(vmode_g->height);
        u8 memmodel = GET_GLOBAL(vmode_g->memmodel);
        int cheight = GET_GLOBAL(vmode_g->cheight);
        if (memmodel == MM_TEXT) {
            SET_VGA(vparam_g->twidth, width);
            SET_VGA(vparam_g->theightm1, height-1);
        } else {
            int cwidth = GET_GLOBAL(vmode_g->cwidth);
            SET_VGA(vparam_g->twidth, width / cwidth);
            SET_VGA(vparam_g->theightm1, (height / cheight) - 1);
        }
        SET_VGA(vparam_g->cheight, cheight);
        SET_VGA(vparam_g->slength, calc_page_size(memmodel, width, height));
        struct stdvga_mode_s *stdmode_g = container_of(
            vmode_g, struct stdvga_mode_s, info);
        memcpy_far(get_global_seg(), vparam_g->sequ_regs
                   , get_global_seg(), GET_GLOBAL(stdmode_g->sequ_regs)
                   , ARRAY_SIZE(vparam_g->sequ_regs));
        SET_VGA(vparam_g->miscreg, GET_GLOBAL(stdmode_g->miscreg));
        memcpy_far(get_global_seg(), vparam_g->crtc_regs
                   , get_global_seg(), GET_GLOBAL(stdmode_g->crtc_regs)
                   , ARRAY_SIZE(vparam_g->crtc_regs));
        memcpy_far(get_global_seg(), vparam_g->actl_regs
                   , get_global_seg(), GET_GLOBAL(stdmode_g->actl_regs)
                   , ARRAY_SIZE(vparam_g->actl_regs));
        memcpy_far(get_global_seg(), vparam_g->grdc_regs
                   , get_global_seg(), GET_GLOBAL(stdmode_g->grdc_regs)
                   , ARRAY_SIZE(vparam_g->grdc_regs));
    }

    SET_VGA(video_save_pointer_table.videoparam
            , SEGOFF(get_global_seg(), (u32)video_param_table));
}


/****************************************************************
 * Register definitions
 ****************************************************************/

/* Mono */
static u8 palette0[] VAR16 = {
  0x00,0x00,0x00, 0x00,0x00,0x00, 0x00,0x00,0x00, 0x00,0x00,0x00,
  0x00,0x00,0x00, 0x00,0x00,0x00, 0x00,0x00,0x00, 0x00,0x00,0x00,
  0x2a,0x2a,0x2a, 0x2a,0x2a,0x2a, 0x2a,0x2a,0x2a, 0x2a,0x2a,0x2a,
  0x2a,0x2a,0x2a, 0x2a,0x2a,0x2a, 0x2a,0x2a,0x2a, 0x2a,0x2a,0x2a,
  0x2a,0x2a,0x2a, 0x2a,0x2a,0x2a, 0x2a,0x2a,0x2a, 0x2a,0x2a,0x2a,
  0x2a,0x2a,0x2a, 0x2a,0x2a,0x2a, 0x2a,0x2a,0x2a, 0x2a,0x2a,0x2a,
  0x3f,0x3f,0x3f, 0x3f,0x3f,0x3f, 0x3f,0x3f,0x3f, 0x3f,0x3f,0x3f,
  0x3f,0x3f,0x3f, 0x3f,0x3f,0x3f, 0x3f,0x3f,0x3f, 0x3f,0x3f,0x3f,
  0x00,0x00,0x00, 0x00,0x00,0x00, 0x00,0x00,0x00, 0x00,0x00,0x00,
  0x00,0x00,0x00, 0x00,0x00,0x00, 0x00,0x00,0x00, 0x00,0x00,0x00,
  0x2a,0x2a,0x2a, 0x2a,0x2a,0x2a, 0x2a,0x2a,0x2a, 0x2a,0x2a,0x2a,
  0x2a,0x2a,0x2a, 0x2a,0x2a,0x2a, 0x2a,0x2a,0x2a, 0x2a,0x2a,0x2a,
  0x2a,0x2a,0x2a, 0x2a,0x2a,0x2a, 0x2a,0x2a,0x2a, 0x2a,0x2a,0x2a,
  0x2a,0x2a,0x2a, 0x2a,0x2a,0x2a, 0x2a,0x2a,0x2a, 0x2a,0x2a,0x2a,
  0x3f,0x3f,0x3f, 0x3f,0x3f,0x3f, 0x3f,0x3f,0x3f, 0x3f,0x3f,0x3f,
  0x3f,0x3f,0x3f, 0x3f,0x3f,0x3f, 0x3f,0x3f,0x3f, 0x3f,0x3f,0x3f
};

static u8 palette1[] VAR16 = {
  0x00,0x00,0x00, 0x00,0x00,0x2a, 0x00,0x2a,0x00, 0x00,0x2a,0x2a,
  0x2a,0x00,0x00, 0x2a,0x00,0x2a, 0x2a,0x15,0x00, 0x2a,0x2a,0x2a,
  0x00,0x00,0x00, 0x00,0x00,0x2a, 0x00,0x2a,0x00, 0x00,0x2a,0x2a,
  0x2a,0x00,0x00, 0x2a,0x00,0x2a, 0x2a,0x15,0x00, 0x2a,0x2a,0x2a,
  0x15,0x15,0x15, 0x15,0x15,0x3f, 0x15,0x3f,0x15, 0x15,0x3f,0x3f,
  0x3f,0x15,0x15, 0x3f,0x15,0x3f, 0x3f,0x3f,0x15, 0x3f,0x3f,0x3f,
  0x15,0x15,0x15, 0x15,0x15,0x3f, 0x15,0x3f,0x15, 0x15,0x3f,0x3f,
  0x3f,0x15,0x15, 0x3f,0x15,0x3f, 0x3f,0x3f,0x15, 0x3f,0x3f,0x3f,
  0x00,0x00,0x00, 0x00,0x00,0x2a, 0x00,0x2a,0x00, 0x00,0x2a,0x2a,
  0x2a,0x00,0x00, 0x2a,0x00,0x2a, 0x2a,0x15,0x00, 0x2a,0x2a,0x2a,
  0x00,0x00,0x00, 0x00,0x00,0x2a, 0x00,0x2a,0x00, 0x00,0x2a,0x2a,
  0x2a,0x00,0x00, 0x2a,0x00,0x2a, 0x2a,0x15,0x00, 0x2a,0x2a,0x2a,
  0x15,0x15,0x15, 0x15,0x15,0x3f, 0x15,0x3f,0x15, 0x15,0x3f,0x3f,
  0x3f,0x15,0x15, 0x3f,0x15,0x3f, 0x3f,0x3f,0x15, 0x3f,0x3f,0x3f,
  0x15,0x15,0x15, 0x15,0x15,0x3f, 0x15,0x3f,0x15, 0x15,0x3f,0x3f,
  0x3f,0x15,0x15, 0x3f,0x15,0x3f, 0x3f,0x3f,0x15, 0x3f,0x3f,0x3f
};

static u8 palette2[] VAR16 = {
  0x00,0x00,0x00, 0x00,0x00,0x2a, 0x00,0x2a,0x00, 0x00,0x2a,0x2a,
  0x2a,0x00,0x00, 0x2a,0x00,0x2a, 0x2a,0x2a,0x00, 0x2a,0x2a,0x2a,
  0x00,0x00,0x15, 0x00,0x00,0x3f, 0x00,0x2a,0x15, 0x00,0x2a,0x3f,
  0x2a,0x00,0x15, 0x2a,0x00,0x3f, 0x2a,0x2a,0x15, 0x2a,0x2a,0x3f,
  0x00,0x15,0x00, 0x00,0x15,0x2a, 0x00,0x3f,0x00, 0x00,0x3f,0x2a,
  0x2a,0x15,0x00, 0x2a,0x15,0x2a, 0x2a,0x3f,0x00, 0x2a,0x3f,0x2a,
  0x00,0x15,0x15, 0x00,0x15,0x3f, 0x00,0x3f,0x15, 0x00,0x3f,0x3f,
  0x2a,0x15,0x15, 0x2a,0x15,0x3f, 0x2a,0x3f,0x15, 0x2a,0x3f,0x3f,
  0x15,0x00,0x00, 0x15,0x00,0x2a, 0x15,0x2a,0x00, 0x15,0x2a,0x2a,
  0x3f,0x00,0x00, 0x3f,0x00,0x2a, 0x3f,0x2a,0x00, 0x3f,0x2a,0x2a,
  0x15,0x00,0x15, 0x15,0x00,0x3f, 0x15,0x2a,0x15, 0x15,0x2a,0x3f,
  0x3f,0x00,0x15, 0x3f,0x00,0x3f, 0x3f,0x2a,0x15, 0x3f,0x2a,0x3f,
  0x15,0x15,0x00, 0x15,0x15,0x2a, 0x15,0x3f,0x00, 0x15,0x3f,0x2a,
  0x3f,0x15,0x00, 0x3f,0x15,0x2a, 0x3f,0x3f,0x00, 0x3f,0x3f,0x2a,
  0x15,0x15,0x15, 0x15,0x15,0x3f, 0x15,0x3f,0x15, 0x15,0x3f,0x3f,
  0x3f,0x15,0x15, 0x3f,0x15,0x3f, 0x3f,0x3f,0x15, 0x3f,0x3f,0x3f
};

static u8 palette3[] VAR16 = {
  0x00,0x00,0x00, 0x00,0x00,0x2a, 0x00,0x2a,0x00, 0x00,0x2a,0x2a,
  0x2a,0x00,0x00, 0x2a,0x00,0x2a, 0x2a,0x15,0x00, 0x2a,0x2a,0x2a,
  0x15,0x15,0x15, 0x15,0x15,0x3f, 0x15,0x3f,0x15, 0x15,0x3f,0x3f,
  0x3f,0x15,0x15, 0x3f,0x15,0x3f, 0x3f,0x3f,0x15, 0x3f,0x3f,0x3f,
  0x00,0x00,0x00, 0x05,0x05,0x05, 0x08,0x08,0x08, 0x0b,0x0b,0x0b,
  0x0e,0x0e,0x0e, 0x11,0x11,0x11, 0x14,0x14,0x14, 0x18,0x18,0x18,
  0x1c,0x1c,0x1c, 0x20,0x20,0x20, 0x24,0x24,0x24, 0x28,0x28,0x28,
  0x2d,0x2d,0x2d, 0x32,0x32,0x32, 0x38,0x38,0x38, 0x3f,0x3f,0x3f,
  0x00,0x00,0x3f, 0x10,0x00,0x3f, 0x1f,0x00,0x3f, 0x2f,0x00,0x3f,
  0x3f,0x00,0x3f, 0x3f,0x00,0x2f, 0x3f,0x00,0x1f, 0x3f,0x00,0x10,
  0x3f,0x00,0x00, 0x3f,0x10,0x00, 0x3f,0x1f,0x00, 0x3f,0x2f,0x00,
  0x3f,0x3f,0x00, 0x2f,0x3f,0x00, 0x1f,0x3f,0x00, 0x10,0x3f,0x00,
  0x00,0x3f,0x00, 0x00,0x3f,0x10, 0x00,0x3f,0x1f, 0x00,0x3f,0x2f,
  0x00,0x3f,0x3f, 0x00,0x2f,0x3f, 0x00,0x1f,0x3f, 0x00,0x10,0x3f,
  0x1f,0x1f,0x3f, 0x27,0x1f,0x3f, 0x2f,0x1f,0x3f, 0x37,0x1f,0x3f,
  0x3f,0x1f,0x3f, 0x3f,0x1f,0x37, 0x3f,0x1f,0x2f, 0x3f,0x1f,0x27,

  0x3f,0x1f,0x1f, 0x3f,0x27,0x1f, 0x3f,0x2f,0x1f, 0x3f,0x37,0x1f,
  0x3f,0x3f,0x1f, 0x37,0x3f,0x1f, 0x2f,0x3f,0x1f, 0x27,0x3f,0x1f,
  0x1f,0x3f,0x1f, 0x1f,0x3f,0x27, 0x1f,0x3f,0x2f, 0x1f,0x3f,0x37,
  0x1f,0x3f,0x3f, 0x1f,0x37,0x3f, 0x1f,0x2f,0x3f, 0x1f,0x27,0x3f,
  0x2d,0x2d,0x3f, 0x31,0x2d,0x3f, 0x36,0x2d,0x3f, 0x3a,0x2d,0x3f,
  0x3f,0x2d,0x3f, 0x3f,0x2d,0x3a, 0x3f,0x2d,0x36, 0x3f,0x2d,0x31,
  0x3f,0x2d,0x2d, 0x3f,0x31,0x2d, 0x3f,0x36,0x2d, 0x3f,0x3a,0x2d,
  0x3f,0x3f,0x2d, 0x3a,0x3f,0x2d, 0x36,0x3f,0x2d, 0x31,0x3f,0x2d,
  0x2d,0x3f,0x2d, 0x2d,0x3f,0x31, 0x2d,0x3f,0x36, 0x2d,0x3f,0x3a,
  0x2d,0x3f,0x3f, 0x2d,0x3a,0x3f, 0x2d,0x36,0x3f, 0x2d,0x31,0x3f,
  0x00,0x00,0x1c, 0x07,0x00,0x1c, 0x0e,0x00,0x1c, 0x15,0x00,0x1c,
  0x1c,0x00,0x1c, 0x1c,0x00,0x15, 0x1c,0x00,0x0e, 0x1c,0x00,0x07,
  0x1c,0x00,0x00, 0x1c,0x07,0x00, 0x1c,0x0e,0x00, 0x1c,0x15,0x00,
  0x1c,0x1c,0x00, 0x15,0x1c,0x00, 0x0e,0x1c,0x00, 0x07,0x1c,0x00,
  0x00,0x1c,0x00, 0x00,0x1c,0x07, 0x00,0x1c,0x0e, 0x00,0x1c,0x15,
  0x00,0x1c,0x1c, 0x00,0x15,0x1c, 0x00,0x0e,0x1c, 0x00,0x07,0x1c,

  0x0e,0x0e,0x1c, 0x11,0x0e,0x1c, 0x15,0x0e,0x1c, 0x18,0x0e,0x1c,
  0x1c,0x0e,0x1c, 0x1c,0x0e,0x18, 0x1c,0x0e,0x15, 0x1c,0x0e,0x11,
  0x1c,0x0e,0x0e, 0x1c,0x11,0x0e, 0x1c,0x15,0x0e, 0x1c,0x18,0x0e,
  0x1c,0x1c,0x0e, 0x18,0x1c,0x0e, 0x15,0x1c,0x0e, 0x11,0x1c,0x0e,
  0x0e,0x1c,0x0e, 0x0e,0x1c,0x11, 0x0e,0x1c,0x15, 0x0e,0x1c,0x18,
  0x0e,0x1c,0x1c, 0x0e,0x18,0x1c, 0x0e,0x15,0x1c, 0x0e,0x11,0x1c,
  0x14,0x14,0x1c, 0x16,0x14,0x1c, 0x18,0x14,0x1c, 0x1a,0x14,0x1c,
  0x1c,0x14,0x1c, 0x1c,0x14,0x1a, 0x1c,0x14,0x18, 0x1c,0x14,0x16,
  0x1c,0x14,0x14, 0x1c,0x16,0x14, 0x1c,0x18,0x14, 0x1c,0x1a,0x14,
  0x1c,0x1c,0x14, 0x1a,0x1c,0x14, 0x18,0x1c,0x14, 0x16,0x1c,0x14,
  0x14,0x1c,0x14, 0x14,0x1c,0x16, 0x14,0x1c,0x18, 0x14,0x1c,0x1a,
  0x14,0x1c,0x1c, 0x14,0x1a,0x1c, 0x14,0x18,0x1c, 0x14,0x16,0x1c,
  0x00,0x00,0x10, 0x04,0x00,0x10, 0x08,0x00,0x10, 0x0c,0x00,0x10,
  0x10,0x00,0x10, 0x10,0x00,0x0c, 0x10,0x00,0x08, 0x10,0x00,0x04,
  0x10,0x00,0x00, 0x10,0x04,0x00, 0x10,0x08,0x00, 0x10,0x0c,0x00,
  0x10,0x10,0x00, 0x0c,0x10,0x00, 0x08,0x10,0x00, 0x04,0x10,0x00,

  0x00,0x10,0x00, 0x00,0x10,0x04, 0x00,0x10,0x08, 0x00,0x10,0x0c,
  0x00,0x10,0x10, 0x00,0x0c,0x10, 0x00,0x08,0x10, 0x00,0x04,0x10,
  0x08,0x08,0x10, 0x0a,0x08,0x10, 0x0c,0x08,0x10, 0x0e,0x08,0x10,
  0x10,0x08,0x10, 0x10,0x08,0x0e, 0x10,0x08,0x0c, 0x10,0x08,0x0a,
  0x10,0x08,0x08, 0x10,0x0a,0x08, 0x10,0x0c,0x08, 0x10,0x0e,0x08,
  0x10,0x10,0x08, 0x0e,0x10,0x08, 0x0c,0x10,0x08, 0x0a,0x10,0x08,
  0x08,0x10,0x08, 0x08,0x10,0x0a, 0x08,0x10,0x0c, 0x08,0x10,0x0e,
  0x08,0x10,0x10, 0x08,0x0e,0x10, 0x08,0x0c,0x10, 0x08,0x0a,0x10,
  0x0b,0x0b,0x10, 0x0c,0x0b,0x10, 0x0d,0x0b,0x10, 0x0f,0x0b,0x10,
  0x10,0x0b,0x10, 0x10,0x0b,0x0f, 0x10,0x0b,0x0d, 0x10,0x0b,0x0c,
  0x10,0x0b,0x0b, 0x10,0x0c,0x0b, 0x10,0x0d,0x0b, 0x10,0x0f,0x0b,
  0x10,0x10,0x0b, 0x0f,0x10,0x0b, 0x0d,0x10,0x0b, 0x0c,0x10,0x0b,
  0x0b,0x10,0x0b, 0x0b,0x10,0x0c, 0x0b,0x10,0x0d, 0x0b,0x10,0x0f,
  0x0b,0x10,0x10, 0x0b,0x0f,0x10, 0x0b,0x0d,0x10, 0x0b,0x0c,0x10,
  0x00,0x00,0x00, 0x00,0x00,0x00, 0x00,0x00,0x00, 0x00,0x00,0x00,
  0x00,0x00,0x00, 0x00,0x00,0x00, 0x00,0x00,0x00, 0x00,0x00,0x00
};

static u8 sequ_01[] VAR16 = { 0x08, 0x03, 0x00, 0x02 };
static u8 crtc_01[] VAR16 = {
    0x2d, 0x27, 0x28, 0x90, 0x2b, 0xa0, 0xbf, 0x1f,
    0x00, 0x4f, 0x0d, 0x0e, 0x00, 0x00, 0x00, 0x00,
    0x9c, 0x8e, 0x8f, 0x14, 0x1f, 0x96, 0xb9, 0xa3,
    0xff };
static u8 actl_01[] VAR16 = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x14, 0x07,
    0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
    0x0c, 0x00, 0x0f, 0x08 };
static u8 grdc_01[] VAR16 = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x0e, 0x0f, 0xff };
static u8 sequ_03[] VAR16 = { 0x00, 0x03, 0x00, 0x02 };
static u8 crtc_03[] VAR16 = {
    0x5f, 0x4f, 0x50, 0x82, 0x55, 0x81, 0xbf, 0x1f,
    0x00, 0x4f, 0x0d, 0x0e, 0x00, 0x00, 0x00, 0x00,
    0x9c, 0x8e, 0x8f, 0x28, 0x1f, 0x96, 0xb9, 0xa3,
    0xff };
static u8 sequ_04[] VAR16 = { 0x09, 0x03, 0x00, 0x02 };
static u8 crtc_04[] VAR16 = {
    0x2d, 0x27, 0x28, 0x90, 0x2b, 0x80, 0xbf, 0x1f,
    0x00, 0xc1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x9c, 0x8e, 0x8f, 0x14, 0x00, 0x96, 0xb9, 0xa2,
    0xff };
static u8 actl_04[] VAR16 = {
    0x00, 0x13, 0x15, 0x17, 0x02, 0x04, 0x06, 0x07,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x01, 0x00, 0x03, 0x00 };
static u8 grdc_04[] VAR16 = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x0f, 0x0f, 0xff };
static u8 sequ_06[] VAR16 = { 0x01, 0x01, 0x00, 0x06 };
static u8 crtc_06[] VAR16 = {
    0x5f, 0x4f, 0x50, 0x82, 0x54, 0x80, 0xbf, 0x1f,
    0x00, 0xc1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x9c, 0x8e, 0x8f, 0x28, 0x00, 0x96, 0xb9, 0xc2,
    0xff };
static u8 actl_06[] VAR16 = {
    0x00, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17,
    0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17,
    0x01, 0x00, 0x01, 0x00 };
static u8 grdc_06[] VAR16 = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0d, 0x0f, 0xff };
static u8 crtc_07[] VAR16 = {
    0x5f, 0x4f, 0x50, 0x82, 0x55, 0x81, 0xbf, 0x1f,
    0x00, 0x4f, 0x0d, 0x0e, 0x00, 0x00, 0x00, 0x00,
    0x9c, 0x8e, 0x8f, 0x28, 0x0f, 0x96, 0xb9, 0xa3,
    0xff };
static u8 actl_07[] VAR16 = {
    0x00, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
    0x10, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
    0x0e, 0x00, 0x0f, 0x08 };
static u8 grdc_07[] VAR16 = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x0a, 0x0f, 0xff };
static u8 sequ_0d[] VAR16 = { 0x09, 0x0f, 0x00, 0x06 };
static u8 crtc_0d[] VAR16 = {
    0x2d, 0x27, 0x28, 0x90, 0x2b, 0x80, 0xbf, 0x1f,
    0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x9c, 0x8e, 0x8f, 0x14, 0x00, 0x96, 0xb9, 0xe3,
    0xff };
static u8 actl_0d[] VAR16 = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x01, 0x00, 0x0f, 0x00 };
static u8 grdc_0d[] VAR16 = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x0f, 0xff };
static u8 sequ_0e[] VAR16 = { 0x01, 0x0f, 0x00, 0x06 };
static u8 crtc_0e[] VAR16 = {
    0x5f, 0x4f, 0x50, 0x82, 0x54, 0x80, 0xbf, 0x1f,
    0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x9c, 0x8e, 0x8f, 0x28, 0x00, 0x96, 0xb9, 0xe3,
    0xff };
static u8 crtc_0f[] VAR16 = {
    0x5f, 0x4f, 0x50, 0x82, 0x54, 0x80, 0xbf, 0x1f,
    0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x83, 0x85, 0x5d, 0x28, 0x0f, 0x63, 0xba, 0xe3,
    0xff };
static u8 actl_0f[] VAR16 = {
    0x00, 0x08, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00,
    0x00, 0x08, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00,
    0x01, 0x00, 0x01, 0x00 };
static u8 actl_10[] VAR16 = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x14, 0x07,
    0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
    0x01, 0x00, 0x0f, 0x00 };
static u8 crtc_11[] VAR16 = {
    0x5f, 0x4f, 0x50, 0x82, 0x54, 0x80, 0x0b, 0x3e,
    0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xea, 0x8c, 0xdf, 0x28, 0x00, 0xe7, 0x04, 0xe3,
    0xff };
static u8 actl_11[] VAR16 = {
    0x00, 0x3f, 0x00, 0x3f, 0x00, 0x3f, 0x00, 0x3f,
    0x00, 0x3f, 0x00, 0x3f, 0x00, 0x3f, 0x00, 0x3f,
    0x01, 0x00, 0x0f, 0x00 };
static u8 sequ_13[] VAR16 = { 0x01, 0x0f, 0x00, 0x0e };
static u8 crtc_13[] VAR16 = {
    0x5f, 0x4f, 0x50, 0x82, 0x54, 0x80, 0xbf, 0x1f,
    0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x9c, 0x8e, 0x8f, 0x28, 0x40, 0x96, 0xb9, 0xa3,
    0xff };
static u8 actl_13[] VAR16 = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x41, 0x00, 0x0f, 0x00 };
static u8 grdc_13[] VAR16 = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0f, 0xff };
static u8 crtc_6A[] VAR16 = {
    0x7f, 0x63, 0x63, 0x83, 0x6b, 0x1b, 0x72, 0xf0,
    0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x59, 0x8d, 0x57, 0x32, 0x00, 0x57, 0x73, 0xe3,
    0xff };


/****************************************************************
 * Video mode list
 ****************************************************************/

#define PAL(x) x, sizeof(x)
#define VPARAM(x) &video_param_table[x]

static struct stdvga_mode_s vga_modes[] VAR16 = {
    //mode { model       tx   ty bpp cw ch  sstart    }
    // pelm  dac            sequ     misc  crtc     actl     grdc
    {0x00, { MM_TEXT,    40,  25, 4, 9, 16, SEG_CTEXT }
     , 0xFF, PAL(palette2), sequ_01, 0x67, crtc_01, actl_01, grdc_01},
    {0x01, { MM_TEXT,    40,  25, 4, 9, 16, SEG_CTEXT }
     , 0xFF, PAL(palette2), sequ_01, 0x67, crtc_01, actl_01, grdc_01},
    {0x02, { MM_TEXT,    80,  25, 4, 9, 16, SEG_CTEXT }
     , 0xFF, PAL(palette2), sequ_03, 0x67, crtc_03, actl_01, grdc_01},
    {0x03, { MM_TEXT,    80,  25, 4, 9, 16, SEG_CTEXT }
     , 0xFF, PAL(palette2), sequ_03, 0x67, crtc_03, actl_01, grdc_01},
    {0x04, { MM_CGA,    320, 200, 2, 8,  8, SEG_CTEXT }
     , 0xFF, PAL(palette1), sequ_04, 0x63, crtc_04, actl_04, grdc_04},
    {0x05, { MM_CGA,    320, 200, 2, 8,  8, SEG_CTEXT }
     , 0xFF, PAL(palette1), sequ_04, 0x63, crtc_04, actl_04, grdc_04},
    {0x06, { MM_CGA,    640, 200, 1, 8,  8, SEG_CTEXT }
     , 0xFF, PAL(palette1), sequ_06, 0x63, crtc_06, actl_06, grdc_06},
    {0x07, { MM_TEXT,    80,  25, 4, 9, 16, SEG_MTEXT }
     , 0xFF, PAL(palette0), sequ_03, 0x66, crtc_07, actl_07, grdc_07},
    {0x0D, { MM_PLANAR, 320, 200, 4, 8,  8, SEG_GRAPH }
     , 0xFF, PAL(palette1), sequ_0d, 0x63, crtc_0d, actl_0d, grdc_0d},
    {0x0E, { MM_PLANAR, 640, 200, 4, 8,  8, SEG_GRAPH }
     , 0xFF, PAL(palette1), sequ_0e, 0x63, crtc_0e, actl_0d, grdc_0d},
    {0x0F, { MM_PLANAR, 640, 350, 1, 8, 14, SEG_GRAPH }
     , 0xFF, PAL(palette0), sequ_0e, 0xa3, crtc_0f, actl_0f, grdc_0d},
    {0x10, { MM_PLANAR, 640, 350, 4, 8, 14, SEG_GRAPH }
     , 0xFF, PAL(palette2), sequ_0e, 0xa3, crtc_0f, actl_10, grdc_0d},
    {0x11, { MM_PLANAR, 640, 480, 1, 8, 16, SEG_GRAPH }
     , 0xFF, PAL(palette2), sequ_0e, 0xe3, crtc_11, actl_11, grdc_0d},
    {0x12, { MM_PLANAR, 640, 480, 4, 8, 16, SEG_GRAPH }
     , 0xFF, PAL(palette2), sequ_0e, 0xe3, crtc_11, actl_10, grdc_0d},
    {0x13, { MM_PACKED, 320, 200, 8, 8,  8, SEG_GRAPH }
     , 0xFF, PAL(palette3), sequ_13, 0x63, crtc_13, actl_13, grdc_13},
    {0x6A, { MM_PLANAR, 800, 600, 4, 8, 16, SEG_GRAPH }
     , 0xFF, PAL(palette2), sequ_0e, 0xe3, crtc_6A, actl_10, grdc_0d},
};

struct vgamode_s *
stdvga_find_mode(int mode)
{
    int i;
    for (i = 0; i < ARRAY_SIZE(vga_modes); i++) {
        struct stdvga_mode_s *stdmode_g = &vga_modes[i];
        if (GET_GLOBAL(stdmode_g->mode) == mode)
            return &stdmode_g->info;
    }
    return NULL;
}
