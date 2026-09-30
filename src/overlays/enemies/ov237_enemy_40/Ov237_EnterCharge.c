/* Enter the charge of the ov237 actor: pose 0xc plays with effects 9 and 10 at the +0x38 point, the
 * +0x30 clock clears, +0x34 = 1, the +0x1c timer clears, the +0x54 / +0x55 flags are set and +0x57
 * cleared, +0x18 takes the +0x14 value, +0x58 clears and the brain waits on 020cf5c0. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov237_TickCharge(void);

void Ov237_EnterCharge(int *node)
{
    int *state = (int *)node[1];

    Ov107_PostTagUpdate((Actor *)(*state), 0xc, 0);
    func_ov107_020c0b90(*state, 9, *(VecFx32 *)state[0xe], 0);
    func_ov107_020c0b90(*state, 10, *(VecFx32 *)state[0xe], 0);
    state[0xc] = 0;
    state[0xd] = 1;
    state[7] = 0;
    *((u8 *)state + 0x55) = 1;
    *((u8 *)state + 0x54) = 1;
    *((u8 *)state + 0x57) = 0;
    state[6] = state[5];
    state[0x16] = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov237_TickCharge);
}
