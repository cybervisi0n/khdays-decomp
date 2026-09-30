#pragma opt_strength_reduction off
/*
 * Ov002_SceneDrawPanelWidgets - drive the panel scene's widgets for one frame.
 *
 * The header widget is switched between its two frames as the flag at +0x50
 * changes, and only when it changes. The shared tint is either the fixed value
 * or whatever the standing tween is at, and is written to all three colour
 * slots. The two labels and the list are drawn; the one-shot widget is drawn
 * only while its flag is up, and clears the flag once its animation has passed
 * 0x1000 and nothing is holding it. The three moving widgets are then drawn in
 * turn, each with its own position, tint and element index.
 *
 * ARM.
 */

#include "nitro/fx_types.h"

typedef struct {
    unsigned int pad0 : 2;
    unsigned int bFixedTint : 1;
} Ov002PanelFlags;

extern int data_ov002_0207f628;

extern void Tween_Sample(void *pTween, int *pOut);
extern unsigned short Sequence_UpdateTracks(void *pWidget, int nTime);
extern void Scene_DrawNode(void *pWidget);
extern void NNS_G3dMdlSetMdlAlphaAll(int nList, int nValue);

extern void Ov002_RewindWidget(void *pWidget, int nFrame);
extern void Ov002_DrawAndStepNode(void *pWidget);

void Ov002_SceneDrawPanelWidgets(void)
{
    int nValue;
    int nSlot;
    int i;
    int *pBase;
    int nTint;
    int *ctx;

    ctx = *(int **)&data_ov002_0207f628;
    pBase = ctx;

    if (*(int *)((char *)ctx + 0x50) != 0 && *(int *)((char *)ctx + 0xe4) != 1) {
        Ov002_RewindWidget((char *)ctx + 0xe8, 1);
        *(int *)((char *)ctx + 0xe4) = 1;
    } else if (ctx[0x14] == 0
               && *(int *)((char *)ctx + 0xe4) != 0) {
        Ov002_RewindWidget((char *)ctx + 0xe8, 0);
        *(int *)((char *)ctx + 0xe4) = 0;
    }

    if (((Ov002PanelFlags *)((char *)ctx + 0x76c))->bFixedTint != 0) {
        nTint = 0xd71;
    } else {
        Tween_Sample((char *)ctx + 0x754, &nTint);
    }
    nValue = nTint;
    *(int *)((char *)ctx + 0x1a0) = nValue;
    *(int *)((char *)ctx + 0x19c) = nValue;
    *(int *)((char *)ctx + 0x198) = nValue;

    Ov002_DrawAndStepNode((char *)ctx + 0xe8);
    Ov002_DrawAndStepNode((char *)ctx + 0x508);

    if (*(int *)((char *)ctx + 0x48) != 0) {
        if (Sequence_UpdateTracks((char *)ctx + 0x1f0, 0x1000) != 0
            && *(int *)((char *)ctx + 0xd58) == 0) {
            *(int *)((char *)ctx + 0x48) = 0;
        }
        Scene_DrawNode((char *)ctx + 0x1f0);
    }
    Ov002_DrawAndStepNode((char *)ctx + 0x2f8);

    i = 0;
    do {
        *(VecFx32 *)((char *)pBase + 0x4a4) =
            *(VecFx32 *)((char *)ctx + 0x718);
        nSlot = *(int *)((char *)pBase + i * 4 + 0x73c);
        *(int *)((char *)pBase + 0x4b8) = nSlot;
        *(int *)((char *)pBase + 0x4b4) = nSlot;
        *(int *)((char *)pBase + 0x4b0) = nSlot;
        NNS_G3dMdlSetMdlAlphaAll(*(int *)((char *)pBase + 0x478),
                      *(int *)((char *)pBase + i * 4 + 0x748));
        Ov002_DrawAndStepNode((char *)pBase + 0x400);
        i++;
        ctx = (int *)((char *)ctx + 0xc);
    } while (i < 3);
}
