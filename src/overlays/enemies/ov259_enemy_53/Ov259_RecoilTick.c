/* Recoil tick of the ov259 actor: after 0x1540 of the +0x68 timer it restarts and the actor is
 * knocked back at its +0x10 point (mode 7); the node moves on to 020d0d28. */

#include "nitro/fx_types.h"

extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov259_SweepRecoveryTick(void);

void Ov259_RecoilTick(int *node)
{
    int *state = (int *)node[1];

    state[0x1a] += *(int *)(node[0] + 0x2c);
    if (state[0x1a] <= 0x1540) {
        return;
    }
    state[0x1a] = 0;
    func_ov107_020c0b90(*state, 7, *(VecFx32 *)state[4], 0);
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov259_SweepRecoveryTick);
}
