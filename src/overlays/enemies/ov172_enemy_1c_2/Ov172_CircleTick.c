/* Circle tick of the ov171 enemy (and its byte-identical twins), the Ov178_CircleTick shape: acquires
 * a target (+0xc; none requests sub-state 2), steps towards it along the direction from
 * 020cca08 (0x200 when the distance exceeds 0x100) plus a sideways component (world Y x
 * direction, normalised, times the +0x60 orbit sense x 0x180) into +0x20; the +0x24 height
 * tracks the target's +0x78 by 0x80 per tick outside a 0x80 dead band. The +0x5c timer runs
 * down by the node's +0x2c speed; at zero a roll under 60 (of 101) with a free target
 * (020ccaa0) requests sub-state 8, else 5, and the slot is cleared unless the sub-state stayed
 * idle. `+ (dist - dist)` is the documented copy artifact of RandNextScaled. */

#include "nitro/fx_types.h"
#include "game/engine.h"

extern int Ov107_FindNearestObject(int obj, int b);
extern void SetIndexedSlot(int node, int slot, void *cb);
extern int Ov172_FaceTargetGetClearance(int node, VecFx32 *out);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void VEC_CrossProduct(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int Ov172_IsChildInactive(int node);
extern int data_02042264;

void Ov172_CircleTick(int node)
{
    int *state = *(int **)(node + 4);
    VecFx32 dir;
    VecFx32 side;
    int dist;
    int a;
    int b;
    int diff;
    int roll;

    state[3] = Ov107_FindNearestObject(*state, 0);
    if (state[3] == 0) {
        *(unsigned char *)(*state + 0x1c7) = 2;
        SetIndexedSlot(node, *(signed char *)(node + 0x20), 0);
        return;
    }
    dist = Ov172_FaceTargetGetClearance(node, &dir);
    if (dist > 0x100) {
        dist = 0x100;
    }
    ScaleVec3Fx12(dist, &dir, (VecFx32 *)(state + 8));
    VEC_CrossProduct((VecFx32 *)&data_02042264, &dir, &side);
    VEC_Normalize(&side, &side);
    ScaleVec3Fx12(state[0x18] * 0x180, &side, &side);
    VEC_Add((VecFx32 *)(state + 8), &side, (VecFx32 *)(state + 8));
    a = *(int *)(state[3] + 0x78);
    b = *(int *)(state[2] + 4);
    diff = a - b;
    if (diff < 0) {
        diff = -diff;
    }
    if (diff > 0x80) {
        if (a < b) {
            state[9] -= 0x40;
        } else {
            state[9] += 0x40;
        }
    }
    state[0x17] -= *(int *)(*(int *)node + 0x2c);
    if (state[0x17] > 0) {
        return;
    }
    roll = RandNextScaled(0x65) + (dist - dist);
    state[0x17] = 0;
    *(unsigned char *)(*state + 0x1c7) = (roll < 0x3c && Ov172_IsChildInactive(node) != 0) ? 8 : 5;
    if (*(signed char *)(*state + 0x1c7) != -1) {
        SetIndexedSlot(node, *(signed char *)(node + 0x20), 0);
    }
}
