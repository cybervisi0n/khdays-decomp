#pragma thumb on
/* Gfx_Reset2DEngines -- reset both 2D engines to a blank state, MAIN (THUMB). Turns both displays on,
 * hides every plane, releases all VRAM banks, maps everything to LCDC and clears it, clears both
 * palettes and both OAMs (hidden objects), zeroes all BG scroll offsets, resets the four affine BG
 * matrices to identity, restores the default BG priorities (0..3), closes the windows, turns
 * blending off and sets the 3D clear colour to black at the far depth. The bank releases use the
 * SDK's GX_DisableBankFor* entry points, some of which carry other names in the symbol table. */

#include "nitro/types.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_mtx.h"
#include "nitro/gx.h"
#include "nitro/mi.h"

extern void DispCnt_ApplyPendingMode(void);                        /* GX_DispOn */


#define BG_OFFSET(h, v) ((u32)((((h) << 0) & 0x1ff) | (((v) << 16) & 0x1ff0000)))
#define BG_PRIORITY(reg, p) ((reg) = (u16)(((reg) & ~3) | ((p) << 0)))

void Gfx_Reset2DEngines(void)
{
    MtxFx22 mtx;

    DispCnt_ApplyPendingMode();
    GXS_DispOn();
    GX_SetVisiblePlane(0);
    GXS_SetVisiblePlane(0);

    GX_DisableBankForTex();
    GX_DisableBankForTexPltt();
    GX_DisableBankForBG();
    GX_DisableBankForBGExtPltt();
    GX_DisableBankForOBJ();
    GX_DisableBankForOBJExtPltt();
    GX_DisableBankForSubBG();
    GX_DisableBankForSubOBJ();
    GX_DisableBankForSubBGExtPltt();
    GX_DisableBankForSubOBJExtPltt();

    GX_SetBankForLCDC(0x1ff);
    #ifdef SDK_BUILD_ARM
    //TODO
    MIi_CpuClearFast(0, HW_LCDC_VRAM, HW_LCDC_VRAM_SIZE);
    #endif
    GX_DisableBankForLCDC();

    #ifdef SDK_BUILD_ARM
    //TODO
    MIi_CpuClearFast(0, HW_BG_PLTT, 0x400);
    MIi_CpuClearFast(0, HW_DB_BG_PLTT, 0x400);
    MIi_CpuClearFast(0xc0, HW_OAM, 0x400);
    MIi_CpuClearFast(0xc0, HW_DB_OAM, 0x400);
    #endif

    reg_G2_BG0OFS = BG_OFFSET(0, 0);
    reg_G2_BG1OFS = BG_OFFSET(0, 0);
    reg_G2_BG2OFS = BG_OFFSET(0, 0);
    reg_G2_BG3OFS = BG_OFFSET(0, 0);
    reg_G2S_DB_BG0OFS = BG_OFFSET(0, 0);
    reg_G2S_DB_BG1OFS = BG_OFFSET(0, 0);
    reg_G2S_DB_BG2OFS = BG_OFFSET(0, 0);
    reg_G2S_DB_BG3OFS = BG_OFFSET(0, 0);

    MTX_Identity22(&mtx);
    G2x_SetBGyAffine_(reg_G2_BG2PA, &mtx, 0, 0, 0, 0);
    G2x_SetBGyAffine_(reg_G2_BG3PA, &mtx, 0, 0, 0, 0);
    G2x_SetBGyAffine_(reg_G2S_DB_BG2PA, &mtx, 0, 0, 0, 0);
    G2x_SetBGyAffine_(reg_G2S_DB_BG3PA, &mtx, 0, 0, 0, 0);

    BG_PRIORITY(reg_G2_BG0CNT, 0);
    BG_PRIORITY(reg_G2_BG1CNT, 1);
    BG_PRIORITY(reg_G2_BG2CNT, 2);
    BG_PRIORITY(reg_G2_BG3CNT, 3);
    BG_PRIORITY(reg_G2S_DB_BG0CNT, 0);
    BG_PRIORITY(reg_G2S_DB_BG1CNT, 1);
    BG_PRIORITY(reg_G2S_DB_BG2CNT, 2);
    BG_PRIORITY(reg_G2S_DB_BG3CNT, 3);

    GX_SetVisibleWnd(0);
    GXS_SetVisibleWnd(0);
    reg_G2_BLDCNT = 0;
    reg_G2S_DB_BLDCNT = 0;
    G3X_SetClearColor(0, 0, 0x7fff, 0, 0);
}
#pragma thumb off
