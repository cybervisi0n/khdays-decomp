/*
 * Ov002_SceneStepPanelBackdrop - hold the backdrop up for its dwell time and
 * then fade it away.
 *
 * A pending refresh is applied first. The rest only runs while the backdrop is
 * in state 1, the one state where it is on screen.
 *
 * Its tween is sampled, a held tween contributing nothing. Once the last third
 * of the dwell time is reached - or something is holding the backdrop down -
 * the fade is started, once, and restarted whenever the tween runs out while
 * the fade is still owed. The widget is then drawn at whatever level the tween
 * reads.
 *
 * When the whole dwell time has passed the backdrop drops back to state 0 and
 * both flags are cleared.
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
    char pad000[0xcc];
    int bDirty;
    int bFading;
    int bHeldDown;
    char pad0d8[0xce8];
    Ov002Widget backdropWidget;
    char padec8[0x108];
    Ov002Tween tweenBackdrop;
    int nBackdropState;
    char padff0[0x6c];
    u64 llHoldStamp;
    u32 nHoldMs;
} Ov002BackdropScene;

extern int data_ov002_0207f628;

extern u64 OS_GetTick(void);
extern void Tween_Configure(Ov002Tween *pTween, int nMode, int nFrom, int nTo,
                          int nDuration);
extern void Tween_Start(Ov002Tween *pTween);
extern void Tween_Sample(Ov002Tween *pTween, int *pOut);
extern void NNS_G3dMdlSetMdlAlphaAll(int nList, int nValue);

extern void Ov002_HoldTweenOpen(void);
extern void Ov002_DrawAndStepNode(void *pWidget);

void Ov002_SceneStepPanelBackdrop(void)
{
    int nLevel;
    u64 llNow;
    Ov002BackdropScene *s;
    u64 llDelta;
    u64 llFade;

    s = *(Ov002BackdropScene **)&data_ov002_0207f628;
    llNow = OS_GetTick();
    llDelta = llNow - s->llHoldStamp;
    llFade = ((u64)s->nHoldMs * 0x82ea >> 6)
             - ((u64)(s->nHoldMs / 3) * 0x82ea >> 6);

    if (s->bDirty != 0) {
        Ov002_HoldTweenOpen();
        s->bDirty = 0;
    }
    if (s->nBackdropState == 0) {
        return;
    }
    if (s->nBackdropState != 1) {
        return;
    }

    if (s->tweenBackdrop.bHeld != 0) {
        nLevel = 0;
    } else {
        Tween_Sample(&s->tweenBackdrop, &nLevel);
    }

    if (llDelta > llFade || s->bHeldDown != 0) {
        if (s->bFading == 0) {
            Tween_Configure(&s->tweenBackdrop, 0, 0x1f000, 0, 300);
            Tween_Start(&s->tweenBackdrop);
            s->bFading = 1;
        }
        if (s->bFading != 0) {
            if (s->tweenBackdrop.bHeld != 0) {
                Tween_Configure(&s->tweenBackdrop, 0, 0x1f000, 0, 300);
                Tween_Start(&s->tweenBackdrop);
            }
        }
    }

    NNS_G3dMdlSetMdlAlphaAll(s->backdropWidget.nList, nLevel >> 12);
    Ov002_DrawAndStepNode(&s->backdropWidget);

    if (llDelta > (u64)s->nHoldMs * 0x82ea >> 6) {
        s->bFading = 0;
        s->bHeldDown = 0;
        s->nBackdropState = 0;
    }
}
