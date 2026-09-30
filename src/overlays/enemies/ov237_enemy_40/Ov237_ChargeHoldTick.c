/* Charge-hold tick of the ov237 actor: the +0x30 clock runs up at the frame rate and past 0x3b8 the
 * pending +0x54 flare (effect 0x15 at the +0x38 point) fires once. When the +4 rig is idle a charge
 * step is spent (+0x34), the clock and the +0x55 / +0x57 flags clear, pose 0x13 plays (effect 0x10
 * without a +0x4b4 hold) and the brain waits on 020cfc88. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov237_ChargeHitTick(void);

void Ov237_ChargeHoldTick(int *node)
{
    int *state = (int *)node[1];

    state[0xc] += *(int *)(node[0] + 0x2c);
    if (state[0xc] >= 0x3b8 && *((u8 *)state + 0x54) != 0) {
        *((u8 *)state + 0x54) = 0;
        func_ov107_020c0b90(*state, 0x15, *(VecFx32 *)state[0xe], 0);
    }
    if (*(u8 *)(state[1] + 0xad) != 0) {
        return;
    }
    state[0xd]--;
    state[0xc] = 0;
    *((u8 *)state + 0x57) = 0;
    *((u8 *)state + 0x55) = 0;
    Ov107_PostTagUpdate((Actor *)(*state), 0x13, 0);
    if (*(int *)(*state + 0x4b4) == 0) {
        func_ov107_020c0b90(*state, 0x10, *(VecFx32 *)state[0xe], 0);
    }
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov237_ChargeHitTick);
}
