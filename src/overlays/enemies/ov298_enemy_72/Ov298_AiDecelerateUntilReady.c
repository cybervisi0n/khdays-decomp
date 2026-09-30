/* Slows the drift; once the ready flag is set plays the held-item anim 3 and anim 4 and installs
 * the next step. */

#include "nitro/fx_types.h"

struct b1 { unsigned char b:1; };

extern void ScaleVec3Fx12(int s, int dst, int src);
extern void Ov298_MapHeldItemKindToAnim();
extern void Ov107_PostTagUpdate(int obj, int a, int b);
extern void SetIndexedSlot(int obj, int a, int cb);
extern void Ov298_CopyScaleVecSetField28ThenAdvance(void);

void Ov298_AiDecelerateUntilReady(int *this)
{
    int node = this[1];
    VecFx32 *v = (VecFx32 *)(node + 0x1c);
    *(VecFx32 *)(node + 0x10) = *v;
    *(int *)(node + 0x14) = *(int *)(node + 0x4c);
    *(int *)(node + 0x4c) = *(int *)(node + 0x4c) - 0x80;
    ScaleVec3Fx12(0xe00, (int)v, (int)v);
    if (!((struct b1 *)(*(int *)node + 0x17a))->b) {
        return;
    }
    Ov298_MapHeldItemKindToAnim(*(int *)node, 3);
    Ov107_PostTagUpdate(*(int *)node, 4, 0);
    SetIndexedSlot((int)this, *(signed char *)((int)this + 0x20), (int)&Ov298_CopyScaleVecSetField28ThenAdvance);
}
