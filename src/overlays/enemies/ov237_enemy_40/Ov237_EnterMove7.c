/* Enter move 7 of the ov237 actor: the +0x30 timer clears, the +0x54 flag is set and pose 0x12
 * plays; without a +0x4b4 hold an effect 0xf plays at the +0x38 point, then the brain waits on
 * 020cfb98. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov237_ChargeHoldTick(void);

void Ov237_EnterMove7(int *node)
{
    int *state = (int *)node[1];

    state[0xd] = 7;
    state[0xc] = 0;
    *(u8 *)(state + 0x15) = 1;
    Ov107_PostTagUpdate((Actor *)(*state), 0x12, 0);
    if (*(int *)(*state + 0x4b4) == 0) {
        func_ov107_020c0b90(*state, 0xf, *(VecFx32 *)state[0xe], 0);
    }
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov237_ChargeHoldTick);
}
