/*
 * Ov002_SceneDrawPanelCounters - draw the ten counters that ride along the
 * panel's rows.
 *
 * The shared tint and the shared depth are each either a fixed value, when the
 * tween that drives them is being held, or whatever that tween is at. The tint
 * goes to the three colour slots and the depth to the list.
 *
 * Every row then samples its own pair of tweens: the one that moves it and the
 * one that fades it. A row whose tween is held contributes nothing and, in the
 * fade case, also drops the row's standing request. A row that sampled either
 * tween is marked as needing a redraw.
 *
 * Each marked row is finally drawn digit by digit, from the most significant
 * down: the digit widget takes the row's palette, is bound to the digit (or to
 * the blank glyph once the number has run out), and is placed so the whole
 * number ends at the same right edge whatever its length.
 *
 * ARM.
 */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct {
    int aWords[6];
    unsigned int pad0 : 2;
    unsigned int bHeld : 1;
} Ov002Tween;

extern int data_ov002_0207f628;

extern void Tween_Sample(void *pTween, int *pOut);
extern void NNS_G3dMdlSetMdlPolygonIDAll(int nList, int nValue);
extern void NNS_G3dMdlSetMdlAlphaAll(int nList, int nValue);
extern void Widget_SetTagWord(void *pWidget, u16 nValue);

extern int Ov002_CountDecimalDigits_2(int nValue);
extern int Ov002_ExtractDecimalDigit(int nIndex, int nValue);
extern void Ov002_BindWidgetSubNode3(void *pWidget, int nDigit);
extern void Ov002_DrawAndStepNode(void *pWidget);

void Ov002_SceneDrawPanelRowCounts(void)
{
    VecFx32 v;
    int *ctx;
    int nTint;
    int nValue;
    int i;
    int aSlide[10] = { 0 };
    Ov002Tween *pFade;
    int nDepth;
    int aFade[10] = { 0 };
    int j;
    int aDirty[10] = { 0 };
    Ov002Tween *pSlide;

    ctx = *(int **)&data_ov002_0207f628;

    if (((Ov002Tween *)((char *)ctx + 0xaf0))->bHeld != 0) {
        nTint = 0x5ec;
    } else {
        Tween_Sample((char *)ctx + 0xaf0, &nTint);
    }
    if (((Ov002Tween *)((char *)ctx + 0xb0c))->bHeld != 0) {
        nDepth = 0x1f000;
    } else {
        Tween_Sample((char *)ctx + 0xb0c, &nDepth);
    }

    nValue = nTint;
    *(int *)((char *)ctx + 0x97c) = nValue;
    *(int *)((char *)ctx + 0x980) = nValue;
    *(int *)((char *)ctx + 0x984) = nValue;
    NNS_G3dMdlSetMdlAlphaAll(*(int *)((char *)ctx + 0x944), nDepth >> 12);

    pSlide = (Ov002Tween *)((char *)ctx + 0xb28);
    pFade = (Ov002Tween *)((char *)ctx + 0xc40);
    for (i = 0; i < 10; i++) {
        if (pSlide[i].bHeld != 0) {
            aSlide[i] = 0;
        } else {
            Tween_Sample(&pSlide[i], &aSlide[i]);
            aDirty[i] = 1;
        }
        if (pFade[i].bHeld == 0) {
            Tween_Sample(&pFade[i], &aFade[i]);
            aDirty[i] = 1;
        } else {
            if (ctx[i + 0x29] != 0) {
                ctx[i + 0x29] = 0;
            }
            aFade[i] = 0;
        }
    }

    for (i = 0; i < 10; i++) {
        if (aDirty[i] != 0) {
            for (j = 0;
                 j < Ov002_CountDecimalDigits_2(ctx[i + 0x364]) + 1;
                 j++) {
                Widget_SetTagWord((char *)ctx + 0x9d4,
                              ((u16 *)ctx)[i + 0x56e]);
                if (j >= Ov002_CountDecimalDigits_2(ctx[i + 0x364])) {
                    Ov002_BindWidgetSubNode3((char *)ctx + 0x9d4, 10);
                } else {
                    Ov002_BindWidgetSubNode3(
                        (char *)ctx + 0x9d4,
                        Ov002_ExtractDecimalDigit(j, ctx[i + 0x364]));
                }
                NNS_G3dMdlSetMdlPolygonIDAll(*(int *)((char *)ctx + 0xa4c), 0x3e);
                NNS_G3dMdlSetMdlAlphaAll(*(int *)((char *)ctx + 0xa4c), aFade[i] >> 12);
                v.x = (Ov002_CountDecimalDigits_2(ctx[i + 0x364])
                       - j) * 0x44c - 0x1f40;
                v.y = (aSlide[i] >> 12) + 0x2d50;
                v.z = 0;
                *(VecFx32 *)((char *)ctx + 0xa78) = v;
                Ov002_DrawAndStepNode((char *)ctx + 0x9d4);
            }
        }
    }
}
