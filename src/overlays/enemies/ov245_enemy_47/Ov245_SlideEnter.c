/* Ov245_SlideEnter -- slide entry: flags the actor's +0x38c, spawns effect 0 at the state's +8
 * position (020c0b90), fires reaction 0x15a of kind 4 there (020c5af8), keeps the position at
 * +0x38, clears +0x44/+0x4c/+0x24, raises bit 0 of the +0x60 high byte and bit 0 of the +0x388
 * item's +8 low byte, scales the +0x18 direction by 1.1 into the +0xc velocity and moves the
 * node to 020d31a4. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

struct w8 { unsigned int lo : 8, rest : 24; };

extern void func_ov107_020c0b90(int actor, int effect, VecFx32 v, int flag);
extern void Ov107_BuildAndSendUpdate(int actor, int id, int kind, void *anchor);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov245_Hopper_AiEnterHop(void);

void Ov245_SlideEnter(int *node) {
    int *state = (int *)node[1];

    *(int *)(*state + 0x38c) = 1;
    func_ov107_020c0b90(*state, 0, *(VecFx32 *)state[2], 0);
    Ov107_BuildAndSendUpdate(*state, 0x15a, 4, (void *)state[2]);
    *(VecFx32 *)(state + 0xe) = *(VecFx32 *)state[2];
    state[0x11] = 0;
    state[0x13] = 0;
    state[9] = 0;
    {
        u16 hw = *(u16 *)(*state + 0x60);
        *(u16 *)(*state + 0x60) = (hw & ~0xff00) |
            ((((((unsigned int)hw << 0x10) >> 0x18) | 1) << 0x18) >> 0x10);
    }
    ((struct w8 *)(*(int *)(*state + 0x388) + 8))->lo |= 1;
    ScaleVec3Fx12(0x11a0, (VecFx32 *)(state + 6), (VecFx32 *)(state + 3));
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov245_Hopper_AiEnterHop);
}
