/* Applies gravity and damping; lands when the animation ends and grounded (second variant). */

#include "nitro/fx_types.h"
#include "game/engine.h"

extern void ScaleVec3Fx12(int factor, int *src, int *dst);
extern void SetIndexedSlot(int *a, int i, int v);

void Ov283_AiFallTickB(int *this)
{
    int node = this[1];
    VecFx32 *v = (VecFx32 *)(node + 0x1c);
    int y;

    *(VecFx32 *)(node + 0x10) = *v;
    *(int *)(node + 0x14) = *(int *)(node + 0x58);
    *(int *)(node + 0x58) = *(int *)(node + 0x58) - 0x80;
    ScaleVec3Fx12(0xe00, (int *)v, (int *)v);

    if (*(unsigned char *)(*(int *)(node + 4) + 0xad) != 0) {
        return;
    }

    y = Rand16NextScaled(0x1922) + 0x1922;
    *(int *)(node + 0x34) = y;
    *(int *)(node + 0x7c) = (y > 0x25b3) ? 1 : 0;
    *(int *)(node + 0x3c) = 0;
    *(signed char *)(*(int *)node + 0x1c7) = 4;

    SetIndexedSlot(this, *(signed char *)((char *)this + 0x20), 0);
}
