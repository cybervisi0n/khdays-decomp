/* Slows the drift; once the ready flag is set plays the held-item anim 3 and anim 4 and installs
 * the next step. */

#include "nitro/fx_types.h"

struct Flags17a { char pad[0x17a]; unsigned char ready : 1; };

extern void ScaleVec3Fx12(int factor, void *src, void *dst);
extern void Ov297_MapHeldItemKindToAnim();
extern void Ov107_PostTagUpdate(int obj, int a, int b);
extern void SetIndexedSlot(int a, int i, int v);
extern void Ov297_CopyScaleVecSetField28ThenAdvance(int this_);

void Ov297_AiDecelerateUntilReady(int this_) {
    int node = *(int *)(this_ + 4);
    void *v = (void *)(node + 0x1c);

    *(VecFx32 *)(node + 0x10) = *(VecFx32 *)v;
    *(int *)(node + 0x14) = *(int *)(node + 0x4c);
    *(int *)(node + 0x4c) = *(int *)(node + 0x4c) - 0x80;
    ScaleVec3Fx12(0xe00, v, v);
    if (!((struct Flags17a *)(*(int *)node))->ready) return;
    Ov297_MapHeldItemKindToAnim(*(int *)node, 3);
    Ov107_PostTagUpdate(*(int *)node, 4, 0);
    SetIndexedSlot(this_, *(signed char *)(this_ + 0x20), (int)&Ov297_CopyScaleVecSetField28ThenAdvance);
}
