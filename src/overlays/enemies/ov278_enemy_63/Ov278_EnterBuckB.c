/* Second-rider buck entry: raises bits 1-2 and 6 of the actor's +0x60 high byte and bit 0 of
 * +0x1ae, deactivates the +0x3c0 / +0x3c4 items, plays pose 5, keeps the +0xa0 pose at +0x64
 * and the +0x1c anchor's position at +0x58, throws the +0x40 velocity along a random yaw at
 * 10.0 plus a random 0..10.0 with a 40.0 rise, the +0x4c one along another random yaw at 10.0
 * plus a random 0..10.0 and flat, clears the +0x14 timer, +0x78 word and +0x7c stage and
 * moves the node to 020d528c. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"
#include "game/engine.h"

struct m4 { int w[4]; };
struct w8 { unsigned int lo : 8, rest : 24; };

static inline unsigned short FX_RadToIdx(int rad) {
    return (unsigned short)((0x28BE60DB9391LL * rad + 0x80000000000LL) >> 44);
}

extern void ScaleVec3Fx12(int scale, VecFx32 *v, VecFx32 *d);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov278_DismountLeapTick(int *node);
extern const short data_0203d210[];

void Ov278_EnterBuckB(int *node)
{
    int *state = (int *)node[1];
    unsigned short idx;
    int v;

    {
        u16 hw = *(u16 *)(*state + 0x60);
        *(u16 *)(*state + 0x60) = (hw & ~0xff00) |
            ((((((unsigned int)hw << 0x10) >> 0x18) | 0x46) << 0x18) >> 0x10);
    }
    *(u16 *)(*state + 0x100 + 0xae) |= 1;
    ((struct w8 *)(*(int *)(*state + 0x3c0) + 8))->lo &= ~1;
    ((struct w8 *)(*(int *)(*state + 0x3c4) + 8))->lo &= ~1;
    Ov107_PostTagUpdate((Actor *)(*state), 5, 0);
    *(struct m4 *)(state + 0x19) = *(struct m4 *)(*state + 0xa0);
    *(VecFx32 *)(state + 0x16) = *(VecFx32 *)state[7];
    idx = FX_RadToIdx(RandNextScaled(0x6489) + (v - v));
    state[0x10] = data_0203d210[(idx >> 4) << 1];                             /* FX_SinIdx */
    state[0x11] = 0;
    state[0x12] = data_0203d210[((idx >> 4) << 1) + 1];                       /* FX_CosIdx */
    ScaleVec3Fx12(RandNextScaled(0xa001) + 0xa000, (VecFx32 *)(state + 0x10), (VecFx32 *)(state + 0x10));
    state[0x11] = 0x28000;
    idx = FX_RadToIdx(RandNextScaled(0x6489) + (v - v));
    state[0x13] = data_0203d210[(idx >> 4) << 1];                             /* FX_SinIdx */
    state[0x14] = 0;
    state[0x15] = data_0203d210[((idx >> 4) << 1) + 1];                       /* FX_CosIdx */
    ScaleVec3Fx12(RandNextScaled(0xa001) + 0xa000, (VecFx32 *)(state + 0x13), (VecFx32 *)(state + 0x13));
    state[0x14] = 0;
    state[5] = 0;
    state[0x1e] = 0;
    *((unsigned char *)state + 0x7c) = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov278_DismountLeapTick);
}
