/* Entry of the ov246 enemy's ground drop: a ray from the +0x10 anchor straight down (-2.0,
 * data_ov247_020d4f28) is cast against the collision owner (+4 -> +0x7c); hitting nothing sets
 * the "airborne" flag. The +4 and +8 sub-objects are hidden (+0x5c bit 1) and the +0xc one shown;
 * its channels 0 and 2 take the flag (as a short) with a zero second argument and it is
 * reset (c7ac 0). Reaction 0x158 mode 7 fires at the anchor, the +0x28 rate becomes 0x400 with the +0x30 hit mask cleared, and
 * the drop tick Ov247_GroundDropTick runs once before it takes the slot. */

#include "nitro/fx_types.h"

extern void *Collision_CastRay(void *collision, VecFx32 *origin, VecFx32 *direction);
extern void SetSubitemState(int item, int channel, short a, int b);
extern void RefreshObjectCallbacks(int item, int a);
extern void Ov107_BuildAndSendUpdate(int owner, int id, int mode, void *at);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov247_GroundDropTick(int *node);
extern VecFx32 data_ov247_020d4f28;

void Ov247_EnterGroundDrop(int *node)
{
    int *state = (int *)node[1];
    VecFx32 dir;
    int bAir;
    int coll;

    coll = *(int *)(*state + 4);
    dir = data_ov247_020d4f28;
    bAir = 0;
    if (Collision_CastRay(*(void **)(coll + 0x7c), (VecFx32 *)(state + 4), &dir) == 0) {
        bAir = 1;
    }
    *(int *)(state[1] + 0x5c) |= 2;
    *(int *)(state[2] + 0x5c) |= 2;
    *(int *)(state[3] + 0x5c) &= ~2;
    SetSubitemState(state[3], 0, bAir, 0);
    SetSubitemState(state[3], 2, bAir, 0);
    RefreshObjectCallbacks(state[3], 0);
    Ov107_BuildAndSendUpdate(*state, 0x158, 7, state + 4);
    state[0xa] = 0x400;
    *(unsigned char *)((char *)state + 0x30) = 0;
    Ov247_GroundDropTick(node);
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov247_GroundDropTick);
}
