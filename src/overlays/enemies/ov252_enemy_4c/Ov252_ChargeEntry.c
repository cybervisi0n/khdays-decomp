/* Charge entry of the ov252 actor: sound 0x148/5 plays at its +8 point, pose 0x1a starts (0x2e with the
 * +0xac guard up); unguarded and below the ground it gets a 1.3125 upward +0x10 velocity and bits 3-4
 * of the +0x60 high byte are set. The owner plays effect 2 at its +0x560 model's +0x14 point, the
 * +0x60/+0x6c/+0x64 timers clear, +0x88 = 1 and the node moves on to 020cfdd8. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern void Ov107_BuildAndSendUpdate(int actor, int bank, int variant, void *at);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov252_ChargeTick(void);

void Ov252_ChargeEntry(int *node)
{
    int *state = (int *)node[1];

    Ov107_BuildAndSendUpdate(*state, 0x148, 5, (void *)state[2]);
    if (state[0x2b] == 0) {
        Ov107_PostTagUpdate((Actor *)(*state), 0x1a, 0);
    } else {
        Ov107_PostTagUpdate((Actor *)(*state), 0x2e, 0);
    }
    if (state[0x2b] == 0 && *(int *)(state[2] + 4) < 0) {
        state[4] = 0x1500;
        {
            u16 hw = *(u16 *)(*state + 0x60);
            *(u16 *)(*state + 0x60) = (hw & ~0xff00) |
                ((((((unsigned int)hw << 0x10) >> 0x18) | 0x18) << 0x18) >> 0x10);
        }
    }
    func_ov107_020c0b90(*state, 2, *(VecFx32 *)(*(int *)(*state + 0x560) + 0x14), 0);
    state[0x18] = 0;
    state[0x1b] = 0;
    state[0x19] = 0;
    *((unsigned char *)state + 0x88) = 1;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov252_ChargeTick);
}
