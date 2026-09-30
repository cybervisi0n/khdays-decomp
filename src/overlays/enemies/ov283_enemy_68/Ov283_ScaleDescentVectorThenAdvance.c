
#include "nitro/fx_types.h"

extern void ScaleVec3Fx12(int s, int dst, int src);
extern void Ov107_PostTagUpdate(int obj, int a, int b);
extern void SetIndexedSlot(int obj, int a, int cb);
extern void Ov283_AiFallTick(void);

// Snapshot the working vector (node+0x1c) into node+0x10, override its Y with the
// descent value (node[0x13]) and decrement that by 0x80, then scale the vector by
// 0xe00. Once the linked object is ready (node[1][0xad]==0), switch to mode 3.
void Ov283_ScaleDescentVectorThenAdvance(int *this)
{
    int node = this[1];
    VecFx32 *v = (VecFx32 *)(node + 0x1c);
    *(VecFx32 *)(node + 0x10) = *v;
    *(int *)(node + 0x14) = *(int *)(node + 0x58);
    *(int *)(node + 0x58) = *(int *)(node + 0x58) - 0x80;
    ScaleVec3Fx12(0xe00, (int)v, (int)v);
    if (*(unsigned char *)(*(int *)(node + 4) + 0xad) != 0) {
        return;
    }
    Ov107_PostTagUpdate(*(int *)node, 3, 0);
    SetIndexedSlot((int)this, *(signed char *)((int)this + 0x20), (int)&Ov283_AiFallTick);
}
