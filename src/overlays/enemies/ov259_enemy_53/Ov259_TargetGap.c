/* Gap between the ov259 actor and its +8 target: the distance from the actor's +0xb0 position to
 * the target's +0x190 point minus both +0x80 radii. Without a target the actor goes to move 2 and
 * the node ends (the result is then left undefined, as in the original). */

#include "nitro/fx_types.h"

extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int VEC_Normalize(VecFx32 *v, VecFx32 *out);

int Ov259_TargetGap(int *node)
{
    int *state = (int *)node[1];
    VecFx32 d;

    if (state[2] == 0) {
        *(signed char *)(*state + 0x1c7) = 2;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
    } else {
        VEC_Subtract((VecFx32 *)(state[2] + 0x190), (VecFx32 *)(*state + 0xb0), &d);
        return VEC_Normalize(&d, &d) - (*(int *)(state[2] + 0x80) + *(int *)(*state + 0x80));
    }
}
