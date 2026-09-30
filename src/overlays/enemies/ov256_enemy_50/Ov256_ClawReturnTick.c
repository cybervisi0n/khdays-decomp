/* Return tick of an ov256 claw: the +0x60 timer accumulates the frame rate and the claw moves
 * (020d1400 1, 2); after 0x330 its +0x10 velocity is the +0x1c spin vector scaled by 1 + the owner's
 * +0x3ac part's +0x45c boost. Once the +0x390 part's animation ends (or the +0x39c hold is gone) the
 * hold clears, the owner snaps back to its hand bone (+0x418, or +0x424 for the second claw), the next
 * move is 1 and the node ends. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

extern void Ov256_AttackHitTestB(int *node, int a, int b);
extern void ScaleVec3Fx12(int scale, VecFx32 *v, VecFx32 *d);
extern void Ov107_MoveNodeAndRelayout(int actor, VecFx32 *at);
extern void SetIndexedSlot(int *node, int slot, void *cb);

void Ov256_ClawReturnTick(int *node)
{
    int *state = (int *)node[1];

    state[0x18] += *(int *)(node[0] + 0x2c);
    Ov256_AttackHitTestB(node, 1, 2);
    if (state[0x18] > 0x330) {
        *(VecFx32 *)(state + 4) = *(VecFx32 *)(state + 7);
        ScaleVec3Fx12((*(int *)(*(int *)(*state + 0x3ac) + 0x45c) << 9) + 0x1000, (VecFx32 *)(state + 4), (VecFx32 *)(state + 4));
    }
    {
        int owner = *state;

        if (*(u8 *)(*(int *)(*(int *)(owner + 0x390) + 0x3c) + 0xad) != 0 && *(int *)(owner + 0x39c) != 0) {
            return;
        }
        *(int *)(owner + 0x39c) = 0;
    }
    if (*(u8 *)(*state + 0x394) == 0) {
        Ov107_MoveNodeAndRelayout(*state, (VecFx32 *)(*(int *)(*(int *)(*state + 0x3ac) + 0x418) + 0x14));
    } else {
        Ov107_MoveNodeAndRelayout(*state, (VecFx32 *)(*(int *)(*(int *)(*state + 0x3ac) + 0x424) + 0x14));
    }
    *(signed char *)(*state + 0x1c7) = 1;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}
