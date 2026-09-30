/* Ov245_SlamEnter -- slam entry: flags the actor's +0x38c, clears the state's +0x24, raises
 * bit 7 then bit 0 of the +0x60 high byte and bit 0 of the +0x388 item's +8 low byte, keeps the
 * +8 position at +0x28, spawns effect 0 there (020c0b90), fires reaction 0x15a of kind 9 there
 * (020c5af8) and moves the node to 020d0fa0. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

struct w8 { unsigned int lo : 8, rest : 24; };

extern void func_ov107_020c0b90(int actor, int effect, VecFx32 v, int flag);
extern void Ov107_BuildAndSendUpdate(int actor, int id, int kind, void *anchor);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov245_RollTick(void);

void Ov245_SlamEnter(int *node) {
    int *state = (int *)node[1];

    *(int *)(*state + 0x38c) = 1;
    state[9] = 0;
    {
        u16 hw = *(u16 *)(*state + 0x60);
        *(u16 *)(*state + 0x60) = (hw & ~0xff00) |
            ((((((unsigned int)hw << 0x10) >> 0x18) | 0x80) << 0x18) >> 0x10);
    }
    {
        u16 hw = *(u16 *)(*state + 0x60);
        *(u16 *)(*state + 0x60) = (hw & ~0xff00) |
            ((((((unsigned int)hw << 0x10) >> 0x18) | 1) << 0x18) >> 0x10);
    }
    ((struct w8 *)(*(int *)(*state + 0x388) + 8))->lo |= 1;
    *(VecFx32 *)(state + 10) = *(VecFx32 *)state[2];
    func_ov107_020c0b90(*state, 0, *(VecFx32 *)state[2], 0);
    Ov107_BuildAndSendUpdate(*state, 0x15a, 9, (void *)state[2]);
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov245_RollTick);
}
