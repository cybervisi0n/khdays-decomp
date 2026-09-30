/* Wind-up tick of the ov259 actor: the +0x68 clock runs up at the frame rate; past 0.5 it resets,
 * effects 0xf and 8 spawn at the +0x10 point, the +0x384 rig opens (020d1764) and brain slot +0x20
 * runs 020cf8e0. */

#include "nitro/fx_types.h"

extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern void Ov259_SwapShells(int rig, int open);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov259_BurrowBurstTick(void);

void Ov259_TickWindUp(int *node)
{
    int *state = (int *)node[1];

    state[0x1a] += *(int *)(node[0] + 0x2c);
    if (!(state[0x1a] <= 0x7f8)) {
        state[0x1a] = 0;
        func_ov107_020c0b90(*state, 0xf, *(VecFx32 *)state[4], 0);
        func_ov107_020c0b90(*state, 8, *(VecFx32 *)state[4], 0);
        Ov259_SwapShells(*(int *)(*state + 0x384), 1);
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov259_BurrowBurstTick);
        return;
    }
}
