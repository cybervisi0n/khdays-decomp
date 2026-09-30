/* Posts update 0x177/4, stops, moves the actor to the fixed arena spot and queues action 2. */

#include "nitro/fx_types.h"

extern void func_ov107_020c0b90(int obj, int flag, VecFx32 v);
extern void Ov107_BuildAndSendUpdate(int obj, int b, int c, void *d);
extern void Ov107_MoveNodeAndRelayout(int node, VecFx32 *pos);
extern void SetIndexedSlot(int obj, int idx, int cb);
extern VecFx32 data_02041dc8;

void Ov298_AiWarpToArenaSpot(int *this)
{
    int node = this[1];
    struct {
        int field_00;
        VecFx32 pos;
    } dest;

    dest.field_00 = 0;

    func_ov107_020c0b90(*(int *)node, 0, **(VecFx32 **)(node + 8));
    Ov107_BuildAndSendUpdate(*(int *)node, 0x177, 4, (void *)(*(VecFx32 **)(node + 8)));

    dest.pos.x = 0;
    dest.pos.y = (int)0xffffda42;
    dest.pos.z = 0x2d077;
    *(int *)(node + 0x84) = 1;

    *(VecFx32 *)(node + 0x10) = data_02041dc8;
    Ov107_MoveNodeAndRelayout(*(int *)node, &dest.pos);

    *(int *)(node + 0x38) = 0;
    *(signed char *)(*(int *)node + 0x1c7) = 2;
    SetIndexedSlot((int)this, *(signed char *)((int)this + 0x20), 0);
}
