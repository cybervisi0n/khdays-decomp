/* Ov245_CrashEnter -- crash entry: raises bits 0 and 7 and clears bits 2 and 3 of the actor's
 * +0x60 high byte, raises bit 0 of +0x1ae, clears bit 0 of the +0x388 item's +8 low byte, fires
 * reactions 0x11a/6 and 0/0x48 at the state's +0xc position (020c5af8), spawns effect 0 there
 * (020c0b90), clears +0x40 and moves the node to 020d61f4. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

struct w8 { unsigned int lo : 8, rest : 24; };

extern void Ov107_BuildAndSendUpdate(int actor, int id, int kind, void *anchor);
extern void func_ov107_020c0b90(int actor, int effect, VecFx32 v, int flag);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov245_TimerAcquireTargetAngleThenAdvance(void);

void Ov245_CrashEnter(int *node) {
    int *state = (int *)node[1];

    {
        u16 hw = *(u16 *)(*state + 0x60);
        *(u16 *)(*state + 0x60) = (hw & ~0xff00) |
            ((((((unsigned int)hw << 0x10) >> 0x18) | 0x81) << 0x18) >> 0x10);
    }
    {
        u16 hw = *(u16 *)(*state + 0x60);
        *(u16 *)(*state + 0x60) = (hw & ~0xff00) |
            (((unsigned int)(unsigned short)((((unsigned int)hw << 0x10) >> 0x18) & ~0xc) << 0x18) >> 0x10);
    }
    *(u16 *)(*state + 0x100 + 0xae) |= 1;
    ((struct w8 *)(*(int *)(*state + 0x388) + 8))->lo &= ~1;
    Ov107_BuildAndSendUpdate(*state, 0x11a, 6, (void *)state[3]);
    Ov107_BuildAndSendUpdate(*state, 0, 0x48, (void *)state[3]);
    func_ov107_020c0b90(*state, 0, *(VecFx32 *)state[3], 0);
    state[0x10] = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov245_TimerAcquireTargetAngleThenAdvance);
}
