/*
 * Ov002_SceneStepPanelFlash - run the three-step flash the panel plays over its
 * counter row.
 *
 * The step counter is clamped back to 0 whenever it leaves 1..3, and nothing
 * happens while it sits at 0. Otherwise each of the three tweens that is done
 * is handed the leg of its own track for the current step: the level track
 * fades the row in and back out, the tint track brightens and drops it, and the
 * slide track carries it across. Only the level tween advances the step, so the
 * other two follow it.
 *
 * The three tweens are sampled every call, whether or not a step started, and
 * their values become the row's middle tint, its list level and its X offset.
 *
 * ARM.
 */

#include "nitro/fx_types.h"

typedef struct {
    int nMode;
    int nDuration;
    int nFrom;
    int nTo;
    int aStart[2];
    unsigned int pad0 : 2;
    unsigned int bDone : 1;
} Ov002Tween;

typedef struct {
    int nFrom;
    int nTo;
    short nDuration;
} Ov002FxStep;

typedef struct {
    Ov002FxStep aStep[3];
} Ov002FxTrack;

typedef struct {
    char pad000[0x370];
    int nRowList;
    char pad374[0x28];
    VecFx32 vRowPos;
    int aRowTint[3];
    char pad3b4[0x3a0];
    Ov002Tween aFlash[4];
    int nFlashStep;
} Ov002FlashScene;

extern int data_ov002_0207f628;
extern const Ov002FxTrack data_ov002_0207e1a8;
extern const Ov002FxTrack data_ov002_0207e1cc;
extern const Ov002FxTrack data_ov002_0207e1f0;

extern void Tween_Configure(Ov002Tween *pTween, int nMode, int nFrom, int nTo,
                          int nDuration);
extern void Tween_Start(Ov002Tween *pTween);
extern void Tween_Sample(Ov002Tween *pTween, int *pOut);
extern void NNS_G3dMdlSetMdlAlphaAll(int nList, int nValue);

void Ov002_SceneStepPanelFlash(void)
{
    VecFx32 v;
    Ov002FxTrack level = data_ov002_0207e1a8;
    Ov002FxTrack tint = data_ov002_0207e1cc;
    Ov002FxTrack slide = data_ov002_0207e1f0;
    int nTint = 0;
    int nLevel = 0;
    int nSlide = 0;
    int nStep;
    int nValue;
    Ov002FlashScene *s;

    s = *(Ov002FlashScene **)&data_ov002_0207f628;
    nStep = s->nFlashStep - 1;
    if ((unsigned int)nStep >= 3) {
        s->nFlashStep = 0;
    }

    if (s->nFlashStep > 0) {
        if (s->aFlash[3].bDone != 0) {
            Tween_Configure(&s->aFlash[3], 0, level.aStep[nStep].nFrom,
                          level.aStep[nStep].nTo,
                          level.aStep[nStep].nDuration);
            Tween_Start(&s->aFlash[3]);
            s->nFlashStep++;
        }
        if (s->aFlash[2].bDone != 0) {
            Tween_Configure(&s->aFlash[2], 0, tint.aStep[nStep].nFrom,
                          tint.aStep[nStep].nTo, tint.aStep[nStep].nDuration);
            Tween_Start(&s->aFlash[2]);
        }
        if (s->aFlash[1].bDone != 0) {
            Tween_Configure(&s->aFlash[1], 0, slide.aStep[nStep].nFrom,
                          slide.aStep[nStep].nTo,
                          slide.aStep[nStep].nDuration);
            Tween_Start(&s->aFlash[1]);
        }
    }

    Tween_Sample(&s->aFlash[2], &nTint);
    Tween_Sample(&s->aFlash[3], &nLevel);
    Tween_Sample(&s->aFlash[1], &nSlide);

    nValue = nTint;
    s->aRowTint[0] = 0xe66;
    s->aRowTint[1] = nValue;
    s->aRowTint[2] = 0xe66;
    NNS_G3dMdlSetMdlAlphaAll(s->nRowList, nLevel >> 12);

    v.x = nSlide >> 12;
    v.y = 0x3520;
    v.z = -0x100000;
    s->vRowPos = v;
}
