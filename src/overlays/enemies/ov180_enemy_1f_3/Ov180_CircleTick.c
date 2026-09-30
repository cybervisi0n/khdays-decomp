/* Circle tick of the ov178 enemy (x3: ov178/179/180): acquires the target -- none sends it to
 * sub-state 2 -- then steps towards it (the target direction from 020ccaf4 scaled by its
 * distance, capped at 0x200) plus a tangential component (up x direction, normalised, scaled by
 * the +0x60 orbit sense times 0x180); the hover height (+0x24) tracks the target's +0x78 by 0x80
 * per tick outside a 0x80 dead band. The +0x5c timer runs down by the node's +0x2c speed and,
 * while no target is free (020ccb8c), keeps being re-armed to a random value between the actor's
 * +0x224 and +0x228; at zero a roll picks sub-state 9 (40 %) or 8 (60 %) when a target is free,
 * else 5, and the slot is cleared unless the sub-state stayed idle.
 * `+ (dist - dist)` is the documented copy artifact of RandNextScaled (`add r6,r0,#0`). */

#include "nitro/fx_types.h"
#include "game/engine.h"

extern int Ov107_FindNearestObject(int obj, int b);
extern void SetIndexedSlot(int node, int slot, void *cb);
extern int Ov180_FaceTargetGetClearance(int node, VecFx32 *out);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void VEC_CrossProduct(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int Ov180_IsChildInactive(int node);
extern int data_02042264;

void Ov180_CircleTick(int node)
{
    int *state = *(int **)(node + 4);
    VecFx32 dir;
    VecFx32 side;
    int dist;
    int a;
    int b;
    int diff;
    int lo;
    int roll;

    state[3] = Ov107_FindNearestObject(*state, 0);
    if (state[3] == 0) {
        *(unsigned char *)(*state + 0x1c7) = 2;
        SetIndexedSlot(node, *(signed char *)(node + 0x20), 0);
        return;
    }
    dist = Ov180_FaceTargetGetClearance(node, &dir);
    if (dist > 0x100) {
        dist = 0x200;
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
            state[9] -= 0x80;
        } else {
            state[9] += 0x80;
        }
    }
    state[0x17] -= *(int *)(*(int *)node + 0x2c);
    if (Ov180_IsChildInactive(node) == 0) {
        lo = *(int *)(*state + 0x224);
        diff = *(int *)(*state + 0x228) - lo;
        if (diff < 0) {
            diff = -diff;
        }
        state[0x17] = lo + RandNextScaled(diff + 1);
    }
    if (state[0x17] > 0) {
        return;
    }
    roll = RandNextScaled(0x65) + (dist - dist);
    state[0x17] = 0;
    if (roll < 0x28 && Ov180_IsChildInactive(node) != 0) {
        *(unsigned char *)(*state + 0x1c7) = 9;
    } else if (roll < 0x3c && Ov180_IsChildInactive(node) != 0) {
        *(unsigned char *)(*state + 0x1c7) = 8;
    } else {
        *(unsigned char *)(*state + 0x1c7) = 5;
    }
    if (*(signed char *)(*state + 0x1c7) != -1) {
        SetIndexedSlot(node, *(signed char *)(node + 0x20), 0);
    }
}
