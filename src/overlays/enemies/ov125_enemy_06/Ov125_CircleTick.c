/* Circle tick of the ov125 enemy: reseeds the +0x3c counter with 3 times the owner's rate
 * (30/10) and acquires the target -- none sends it to sub-state 2 -- then steps towards it (the
 * normalised owner-to-target direction scaled by the gap beyond both radii plus 0x800, capped
 * at 0x400) plus a tangential component (up x direction, normalised, scaled by the +0x78 orbit
 * sense times 0x400); the look-at at +0x68 is rebuilt from the two +0x74 positions and the
 * hover height (+0xc) tracks the target's +0x78 by 0x40 per tick outside a 0x80 dead band. The
 * +0x2c timer accumulates the rate; while the +0x54 count is spent a 0x65 roll re-arms it to a
 * random value between the actor's +0x224 and +0x228 and picks sub-state 6 (under 20 with an
 * idle aim node) or 7; otherwise past 0x2000 sub-state 4 is queued.
 * `+ (dist - dist)` is the documented copy artifact of RandNextScaled (`add r5,r0,#0`). */

#include "nitro/fx_types.h"

extern int Ov107_FindNearestObject(int obj, int b);
extern void SetIndexedSlot(int node, int slot, void *cb);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern void Mtx33_LookAt(void *out, void *a, void *b, void *c);
extern void Quat_FromMtx33(void *a, void *b);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void VEC_CrossProduct(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int RandNextScaled(int bound);
extern int Ov125_IsField34Nibble1(int node);
extern int data_02042264;

void Ov125_CircleTick(int *node)
{
    int actor;
    int target;
    int *state = (int *)node[1];
    int dist;
    int len;
    int sum;
    int buf[9];
    VecFx32 dir;
    VecFx32 side;
    int b;
    int a;
    int diff;
    int lo;
    int roll;

    state[0xf] = *(int *)(node[0] + 0x2c) * 30 / 10;
    target = state[1] = Ov107_FindNearestObject(*state, 0);
    if (target == 0) {
        *(unsigned char *)(*state + 0x1c7) = 2;
        SetIndexedSlot((int)node, *(signed char *)((int)node + 0x20), 0);
        return;
    }
    actor = *state;
    VEC_Subtract((VecFx32 *)(target + 0x74), (VecFx32 *)(actor + 0x74), &dir);
    len = VEC_Normalize(&dir, &dir);
    sum = *(int *)(target + 0x80) + *(int *)(actor + 0x80) + 0x800;
    dist = len > sum ? len - sum : 0;
    Mtx33_LookAt(buf, (void *)(target + 0x74), (void *)(actor + 0x74), &data_02042264);
    Quat_FromMtx33(state + 0x1a, buf);
    if (dist > 0x400) {
        dist = 0x400;
    }
    ScaleVec3Fx12(dist, &dir, (VecFx32 *)(state + 2));
    VEC_CrossProduct((VecFx32 *)&data_02042264, &dir, &side);
    VEC_Normalize(&side, &side);
    ScaleVec3Fx12(state[0x1e] << 10, &side, &side);
    VEC_Add((VecFx32 *)(state + 2), &side, (VecFx32 *)(state + 2));
    b = *(int *)(actor + 0x78);
    a = *(int *)(target + 0x78);
    diff = a - b;
    if (diff < 0) {
        diff = -diff;
    }
    if (diff > 0x80) {
        if (a < b) {
            state[3] -= 0x40;
        } else {
            state[3] += 0x40;
        }
    }
    state[0xb] += *(int *)(node[0] + 0x2c);
    if (state[0x15] <= 0) {
        roll = RandNextScaled(0x65) + (dist - dist);
        lo = *(int *)(*state + 0x224);
        diff = *(int *)(*state + 0x228) - lo;
        if (diff < 0) {
            diff = -diff;
        }
        state[0x15] = lo + RandNextScaled(diff + 1);
        if (roll < 0x14 && Ov125_IsField34Nibble1(*(int *)(*state + 0x390)) == 0) {
            *(unsigned char *)(*state + 0x1c7) = 6;
            SetIndexedSlot((int)node, *(signed char *)((int)node + 0x20), 0);
            return;
        }
        *(unsigned char *)(*state + 0x1c7) = 7;
        SetIndexedSlot((int)node, *(signed char *)((int)node + 0x20), 0);
        return;
    }
    if (state[0xb] <= 0x2000) {
        return;
    }
    *(unsigned char *)(*state + 0x1c7) = 4;
    SetIndexedSlot((int)node, *(signed char *)((int)node + 0x20), 0);
}
