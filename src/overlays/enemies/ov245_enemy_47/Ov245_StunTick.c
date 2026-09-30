/* Ov245_StunTick -- stun/knockdown tick: counts the +0x34 timer down by the scene's +0x2c
 * frame step; while it lasts it keeps bits 1, 2 and 7 of the actor's +0x60 high byte raised.
 * When it runs out it clears bits 1-2 and raises bit 0 there, restores pose 0 (020c9264), raises
 * bit 0 of +0x1ae, picks the +0x38 follow-up (2 for a random draw below 40 of 100, else 1),
 * spawns effect 0 and fires reaction 0/0x48 at the actor's +0x74 position, and moves the node
 * to 020cff94. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"
#include "game/engine.h"

extern void func_ov107_020c0b90(int actor, int effect, VecFx32 v, int flag);
extern void Ov107_BuildAndSendUpdate(int actor, int id, int kind, void *anchor);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov245_Carrier_AiStep_QueueAction2OnAnimEnd(void);

void Ov245_StunTick(int *node) {
    int *state = (int *)node[1];

    state[0xd] -= *(int *)(node[0] + 0x2c);
    if (state[0xd] > 0) {
        u16 hw = *(u16 *)(*state + 0x60);
        *(u16 *)(*state + 0x60) = (hw & ~0xff00) |
            ((((((unsigned int)hw << 0x10) >> 0x18) | 0x86) << 0x18) >> 0x10);
        return;
    }
    {
        u16 hw = *(u16 *)(*state + 0x60);
        *(u16 *)(*state + 0x60) = (hw & ~0xff00) |
            (((unsigned int)(unsigned short)((((unsigned int)hw << 0x10) >> 0x18) & ~6) << 0x18) >> 0x10);
    }
    {
        u16 hw = *(u16 *)(*state + 0x60);
        *(u16 *)(*state + 0x60) = (hw & ~0xff00) |
            ((((((unsigned int)hw << 0x10) >> 0x18) | 1) << 0x18) >> 0x10);
    }
    Ov107_PostTagUpdate((Actor *)(*state), 0, 0);
    *(u16 *)(*state + 0x100 + 0xae) |= 1;
    state[0xe] = (unsigned int)RandNextScaled(0x64) < 0x28 ? 2 : 1;
    func_ov107_020c0b90(*state, 0, *(VecFx32 *)(*state + 0x74), 0);
    Ov107_BuildAndSendUpdate(*state, 0, 0x48, (void *)(*state + 0x74));
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov245_Carrier_AiStep_QueueAction2OnAnimEnd);
}
