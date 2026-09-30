/* Hop entry of the ov283 actor: +0x74 clears, the +0x60 timer starts at 5.98, bit 6 of the +0x60 high
 * byte is set, pose 2 plays, the +0x1c drift points along the +0x38 heading at 0.875, +0x58 is 0.3125
 * and the node moves on to 020cd9e0. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov283_ScaleDescentVectorThenAdvance(void);
extern const short data_0203d210[];

#define ANG2IDX(a) ((unsigned short)(((long long)(a) * 0x28be60db9391LL + 0x80000000000LL) >> 44) >> 4)

void Ov283_HopEntry(int *node)
{
    int *state = (int *)node[1];

    state[0x1d] = 0;
    state[0x18] = 0x5fa0;
    {
        u16 hw = *(u16 *)(*state + 0x60);
        *(u16 *)(*state + 0x60) = (hw & ~0xff00) |
            ((((((unsigned int)hw << 0x10) >> 0x18) | 0x40) << 0x18) >> 0x10);
    }
    Ov107_PostTagUpdate((Actor *)(*state), 2, 0);
    {
        int idx = ANG2IDX(state[0xe]) * 2;

        state[7] = data_0203d210[idx];
        state[8] = 0;
        state[9] = data_0203d210[idx + 1];
    }
    ScaleVec3Fx12(0xe00, (VecFx32 *)(state + 7), (VecFx32 *)(state + 7));
    state[0x16] = 0x500;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov283_ScaleDescentVectorThenAdvance);
}
