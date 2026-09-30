/* Ov245_LandingEnter -- landing entry: raises bit 1 of the actor's +0x60 high byte, plays pose 2,
 * spawns effect 1 at the state's +8 position (020c0b90), fires reaction 0x15a of kind 0xc there
 * (020c5af8), clears the actor's +0x3b0 byte, the state's +0x10 byte and +0xc, and moves the
 * node to 020d52b8. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern void func_ov107_020c0b90(int actor, int effect, VecFx32 v, int flag);
extern void Ov107_BuildAndSendUpdate(int actor, int id, int kind, void *anchor);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov245_SweepTick(void);

void Ov245_LandingEnter(int *node) {
    int *state = (int *)node[1];

    {
        u16 hw = *(u16 *)(*state + 0x60);
        *(u16 *)(*state + 0x60) = (hw & ~0xff00) |
            ((((((unsigned int)hw << 0x10) >> 0x18) | 2) << 0x18) >> 0x10);
    }
    Ov107_PostTagUpdate((Actor *)(*state), 2, 0);
    func_ov107_020c0b90(*state, 1, *(VecFx32 *)state[2], 0);
    Ov107_BuildAndSendUpdate(*state, 0x15a, 0xc, (void *)state[2]);
    *(unsigned char *)(*state + 0x3b0) = 0;
    *(unsigned char *)((char *)state + 0x10) = 0;
    state[3] = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov245_SweepTick);
}
