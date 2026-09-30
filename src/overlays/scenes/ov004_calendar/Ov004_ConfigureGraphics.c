/* Initializes ov004 display banks, BG priorities and maps, blend planes, 3D control and display
 * selection; clears three BG screen buffers and restores brightness. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct {
    vu16 nBg0Control;
    vu16 nBg1Control;
    vu16 nBg2Control;
    vu16 nBg3Control;
} BgControlRegisters;

typedef struct {
    vu16 nDisplay3dControl;
    u8 pad0002[0x2a2];
    vu16 nGeometryStatus;
} GeometryRegisters;

typedef struct {
    unsigned int nBg1;
    unsigned int nBg2;
    unsigned int nBg3;
} BgControlValues;

typedef struct {
    vu16 nControl;
} Bg3ControlRegister;

typedef union {
    u16 raw;
    struct {
        u16 nPriority : 2;
        u16 nCharBase : 4;
        u16 bMosaic : 1;
        u16 nColorMode : 1;
        u16 nScreenBase : 5;
        u16 nExtPalette : 1;
        u16 nScreenSize : 2;
    } bits;
} BgControlRegister;

typedef struct {
    BgControlRegister bg1;
    BgControlRegister bg2;
    BgControlRegister bg3;
} BgLayerRegisters;

typedef struct {
    int nFirstTarget;
    int nSecondTarget;
    int nZero;
} BlendArguments;

typedef struct {
    unsigned int nBg3Control;
    BgLayerRegisters *pBgControls;
    BgControlValues bgControls;
    vu32 *pDisplayControl;
    u32 *pBlendControl;
    u32 nDisplayControl;
    int nFirstTarget;
    int nSecondTarget;
    int nZero;
} BackgroundSetupState;

typedef union {
    u16 *pBgControls;
    u32 nDisplayControl;
} HardwareRegisterScratch;

typedef enum {
    GX_BLEND_PLANEMASK_BG0 = 1,
    GX_BLEND_PLANEMASK_BG1 = 2,
    GX_BLEND_PLANEMASK_BG2 = 4,
    GX_BLEND_PLANEMASK_BG3 = 8,
    GX_BLEND_PLANEMASK_OBJ = 0x10,
    GX_BLEND_PLANEMASK_BD = 0x20
} GXBlendPlaneMask;

extern void GX_SetBankForTex(int nBank);
extern void GX_SetBankForTexPltt(int nOffset);
extern void GX_SetBankForBG(int nBank);
extern void GX_SetBankForOBJ(int nBank);
extern void GX_SetGraphicsMode(int nDisplayMode, int nBgMode, int bUse3d);
extern void NNS_GfdResetFrmTexVramState(void);
extern void NNS_GfdInitFrmTexVramManager(int nMode, int bEnable);
extern void NNS_GfdResetFrmPlttVramState(void);
extern void NNS_GfdInitFrmPlttVramManager(int nValue, int bInstallCallbacks);
extern void *G2_GetBG1ScrPtr(void);
extern void *G2_GetBG2ScrPtr(void);
extern void *G2_GetBG3ScrPtr(void);
extern void MIi_CpuClearFast(int nValue, void *pDestination, int nSize);
extern void G2x_SetBlendAlpha_(u32 *pRegister,
                              GXBlendPlaneMask nFirstTarget,
                              GXBlendPlaneMask nSecondTarget,
                              fx32 nEva, fx32 nEvb);

static inline void G2_SetBlendAlpha(GXBlendPlaneMask nFirstTarget,
                                    GXBlendPlaneMask nSecondTarget,
                                    int nEva, int nEvb)
{
    G2x_SetBlendAlpha_((u32 *)0x04000050, nFirstTarget, nSecondTarget,
                       nEva, nEvb);
}

static inline void G2_SetBG0Priority(int nPriority)
{
    vu16 *pRegister = (vu16 *)0x04000008;
    *pRegister = (*pRegister & ~3) | nPriority;
}

static inline void G2_SetBG1Priority(int nPriority)
{
    vu16 *pRegister = (vu16 *)0x0400000a;
    *pRegister = (*pRegister & ~3) | nPriority;
}

static inline void G2_SetBG2Priority(int nPriority)
{
    vu16 *pRegister = (vu16 *)0x0400000c;
    *pRegister = (*pRegister & ~3) | nPriority;
}

static inline void G2_SetBG3Priority(int nPriority)
{
    vu16 *pRegister = (vu16 *)0x0400000e;
    *pRegister = (*pRegister & ~3) | nPriority;
}

static inline void G2_SetBG1Control(int nScreenSize, int nColorMode,
                                    int nScreenBase, int nCharBase,
                                    int nExtPalette)
{
    vu16 *pRegister = (vu16 *)0x0400000a;
    *pRegister = (*pRegister & 0x43) | (nScreenSize << 14) |
                 (nColorMode << 7) | (nScreenBase << 8) |
                 (nCharBase << 2) | (nExtPalette << 13);
}

static inline void G2_SetBG2ControlText(int nScreenSize, int nColorMode,
                                        int nScreenBase, int nCharBase)
{
    vu16 *pRegister = (vu16 *)0x0400000c;
    *pRegister = (*pRegister & 0x43) | (nScreenSize << 14) |
                 (nColorMode << 7) | (nScreenBase << 8) |
                 (nCharBase << 2);
}

static inline void G2_SetBG3ControlText(int nScreenSize, int nColorMode,
                                        int nScreenBase, int nCharBase)
{
    vu16 *pRegister = (vu16 *)0x0400000e;
    *pRegister = (*pRegister & 0x43) | (nScreenSize << 14) |
                 (nColorMode << 7) | (nScreenBase << 8) |
                 (nCharBase << 2);
}

static inline void GX_SetVisiblePlane(int nPlaneMask)
{
    vu32 *pDisplayControl = (vu32 *)0x04000000;
    *pDisplayControl = (*pDisplayControl & ~0x1f00) | (nPlaneMask << 8);
}

static inline void GX_SetDispSelect(int nSelection)
{
    vu16 *pPowerControl = (vu16 *)0x04000304;
    *pPowerControl = (*pPowerControl & ~0x8000) | (nSelection << 15);
}

static inline u32 *ConfigureBackgroundLayers(void)
{
    BackgroundSetupState setup;
    BgControlRegister *pControl;
    int nIndex;

    pControl = (BgControlRegister *)0x0400000a;
    nIndex = 0;
    pControl[nIndex].raw = setup.bgControls.nBg1 =
        (u16)((pControl[nIndex].raw & 0x43) | 0x104);
    nIndex++;
    setup.pDisplayControl = (vu32 *)0x04000000;
    setup.pBlendControl =
        (u32 *)((u8 *)setup.pDisplayControl + 0x50);
    pControl[nIndex].raw = setup.bgControls.nBg2 =
        (u16)((pControl[nIndex].raw & 0x43) | 0x208);
    nIndex++;
    pControl[nIndex].raw = setup.nBg3Control =
        (u16)((pControl[nIndex].raw & 0x43) | 0x30c);
    setup.nDisplayControl = *setup.pDisplayControl;
    setup.nDisplayControl =
        (setup.nDisplayControl & ~0x1f00) | 0x1900;
    *setup.pDisplayControl = setup.nDisplayControl;
    return setup.pBlendControl;
}

static inline void SetBlendAt(u32 *pRegister,
                              GXBlendPlaneMask nFirstTarget,
                              GXBlendPlaneMask nSecondTarget,
                              fx32 nEva, fx32 nEvb)
{
    G2x_SetBlendAlpha_(pRegister, nFirstTarget, nSecondTarget, nEva, nEvb);
}

void Ov004_ConfigureGraphics(void)
{
    GXBlendPlaneMask nFirstTarget = (GXBlendPlaneMask)0x1f;
    GXBlendPlaneMask nSecondTarget = (GXBlendPlaneMask)0x3f;
    fx32 nZero = 0;
    Gfx_Reset2DEngines();
    SetMasterBrightnessMain(-16);
    SetMasterBrightnessSub(-16);
    GX_SetBankForTex(7);
    GX_SetBankForTexPltt(0x60);
    GX_SetBankForBG(8);
    GX_SetBankForOBJ(0x10);
    GX_SetGraphicsMode(1, 0, 1);
    NNS_GfdResetFrmTexVramState();
    NNS_GfdInitFrmTexVramManager(3, 1);
    NNS_GfdResetFrmPlttVramState();
    NNS_GfdInitFrmPlttVramManager(0x8000, 1);

    G2_SetBG0Priority(2);
    G2_SetBG1Priority(3);
    G2_SetBG2Priority(1);
    G2_SetBG3Priority(0);

    MIi_CpuClearFast(0, G2_GetBG1ScrPtr(), 0x800);
    MIi_CpuClearFast(0, G2_GetBG2ScrPtr(), 0x800);
    MIi_CpuClearFast(0, G2_GetBG3ScrPtr(), 0x800);

    SetBlendAt(ConfigureBackgroundLayers(), nFirstTarget, nSecondTarget,
               nZero, nZero);

    GeometryRegisters *pGeometry = (GeometryRegisters *)0x04000060;
    pGeometry->nDisplay3dControl =
        (pGeometry->nDisplay3dControl & ~0x3000) | 8;
    GX_SetDispSelect(1);

    SetMasterBrightnessMain(0);
    SetMasterBrightnessSub(0);
}
