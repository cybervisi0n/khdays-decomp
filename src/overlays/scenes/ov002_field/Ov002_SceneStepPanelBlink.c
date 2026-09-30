/*
 * Ov002_SceneStepPanelBlink - run the blink the panel plays over its alert
 * widget.
 *
 * While blinks are still owed and the interval since the last one has passed, a
 * fresh blink is armed: the row flash and each of the three moving widgets are
 * nudged back to step 1 if they had run out, one blink is taken off the tally
 * and the interval is re-armed from this tick.
 *
 * The two blink tweens are then sampled - a held tween contributes its resting
 * value instead - and the widget is only drawn if at least one of them was
 * still running. It takes the sampled level, the sampled tint in all three
 * colour slots, and a fixed position.
 *
 * ARM.
 */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct {
    char pad000[0x78];
    int nList;
    char pad07c[0x28];
    VecFx32 vPos;
    int aTint[3];
    char pad0bc[0x4c];
} Ov002Widget;

typedef struct {
    int nMode;
    int nDuration;
    int nFrom;
    int nTo;
    int aStart[2];
    unsigned int pad0 : 2;
    unsigned int bHeld : 1;
} Ov002Tween;

typedef struct {
    char pad000[0x7c4];
    int nFlashStep;
    char pad7c8[0xfc];
    short aFxStep[3];
    char pad8ca[0x5fe];
    Ov002Widget alertWidget;
    char padfd0[0x20];
    Ov002Tween blinkLevel;
    Ov002Tween blinkTint;
    int nBlinkLeft;
    char pad102c[0x20];
    u64 llBlinkStamp;
    u64 llBlinkInterval;
} Ov002BlinkScene;

extern int data_ov002_0207f628;

extern u64 OS_GetTick(void);
extern void Tween_Sample(Ov002Tween *pTween, int *pOut);
extern void NNS_G3dMdlSetMdlPolygonIDAll(int nList, int nValue);
extern void NNS_G3dMdlSetMdlAlphaAll(int nList, int nValue);

extern void Ov002_SceneStartPanelBlink(void);
extern void Ov002_DrawAndStepNode(void *pWidget);

void Ov002_SceneStepPanelBlink(void)
{
    VecFx32 v;
    int nTint = 0;
    int nLevel = 0;
    Ov002BlinkScene *s;
    int bDrawn;
    u64 llNow;
    u64 llDelta;
    int i;

    s = *(Ov002BlinkScene **)&data_ov002_0207f628;
    bDrawn = 0;
    llNow = OS_GetTick();
    llDelta = llNow - s->llBlinkStamp;

    if (s->nBlinkLeft != 0) {
        if (llDelta >= s->llBlinkInterval) {
            Ov002_SceneStartPanelBlink();
            if (s->nFlashStep <= 0) {
                s->nFlashStep = 1;
            }
            for (i = 0; i < 3; i++) {
                if (s->aFxStep[i] <= 0) {
                    s->aFxStep[i] = 1;
                }
            }
            s->nBlinkLeft--;
            s->llBlinkInterval = 0x4cb51;
            s->llBlinkStamp = llNow;
        }
    }

    if (s->blinkTint.bHeld != 0) {
        nTint = 0x5ec;
    } else {
        Tween_Sample(&s->blinkTint, &nTint);
        bDrawn = 1;
    }
    if (s->blinkLevel.bHeld != 0) {
        nLevel = 0;
    } else {
        Tween_Sample(&s->blinkLevel, &nLevel);
        bDrawn = 1;
    }
    if (bDrawn == 0) {
        return;
    }

    NNS_G3dMdlSetMdlPolygonIDAll(s->alertWidget.nList, 0x38);
    NNS_G3dMdlSetMdlAlphaAll(s->alertWidget.nList, nLevel >> 12);

    v.x = -0x5dc;
    v.y = 0x1770;
    v.z = 0;
    s->alertWidget.aTint[0] = s->alertWidget.aTint[1]
        = s->alertWidget.aTint[2] = nTint;
    s->alertWidget.vPos = v;
    Ov002_DrawAndStepNode(&s->alertWidget);
}
