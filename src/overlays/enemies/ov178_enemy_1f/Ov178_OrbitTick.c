/* Orbit tick of the ov178 enemy (x3: ov178/179/180): acquires the target -- none returns at
 * once. Beyond the actor's +0x2d8 range the step is zeroed; within it the step is the tangent
 * (up x direction, normalised) scaled by the +0x4c speed times the +0x60 orbit sense, the
 * +0x50 bob phase advances by 30x the node's +0x2c speed (wrapping at +0x54 seconds, re-rolled
 * to 40 + rand(1)), and the hover height eases 1/10 of the way to the target's +0x78 + 0x1c00
 * (lowered by the excess of +0x44 over 0x1c00, or the +0x18 override) plus half a sine of the
 * phase, scaled by 0x800. Closer than 0x2000 the flattened direction pushes 0x100 back, beyond
 * 0x3000 it pulls 0x100 in. Then: no free target (020ccb8c) or a target beyond half the range
 * ends in sub-state 4; inside the range the +0x5c timer runs down and at zero a roll picks:
 * 1-in-120 (or a target beyond 0x4000) -> 4, 1-in-20 -> 5, else with a free target 70 % -> 8 and
 * 30 % -> 9. */

#include "nitro/fx_types.h"
#include "game/engine.h"

extern int Ov107_FindNearestObject(int obj, int b);
extern int Ov178_FaceTargetGetClearance(int node, VecFx32 *out);
extern void VEC_CrossProduct(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern int func_02020400(int a, int b);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern void SetIndexedSlot(int node, int slot, void *cb);
extern int Ov178_IsChildInactive(int node);
extern short data_0203d210[];
extern VecFx32 data_02041dc8;
extern int data_02042264;

void Ov178_OrbitTick(int node)
{
    int *state = *(int **)(node + 4);
    VecFx32 dir;
    VecFx32 side;
    VecFx32 pull;
    int dist;
    int h;
    int t;
    int idx;
    int s;
    int diff;
    int obj;
    int mode;

    state[3] = Ov107_FindNearestObject(*state, 0);
    if (state[3] == 0) {
        return;
    }
    dist = Ov178_FaceTargetGetClearance(node, &dir);
    if (dist > *(int *)(*state + 0x2d8)) {
        *(VecFx32 *)(state + 8) = data_02041dc8;
    } else {
        VEC_CrossProduct((VecFx32 *)&data_02042264, &dir, &side);
        VEC_Normalize(&side, &side);
        ScaleVec3Fx12((int)(((long long)(state[0x13] * state[0x18]) * 0x2000 + 0x800) >> 12), &side, &side);
        state[8] = side.x;
        state[9] = 0;
        state[10] = side.z;
        state[0x14] += *(int *)(*(int *)node + 0x2c) * 30;
        if (state[0x14] >= state[0x15] << 12) {
            state[0x14] = 0;
            state[0x15] = RandNextScaled(1) + 0x28;
        }
        if (state[0x22] == 0) {
            h = *(int *)(state[3] + 0x78) + 0x1c00;
            if (state[0x11] > 0x1c00) {
                h -= state[0x11] - 0x1c00;
            }
        } else {
            h = state[6];
        }
        t = func_02020400((int)(((long long)state[0x14] * 0x6488 + 0x800) >> 12), state[0x15]);
        idx = (unsigned short)((0x28BE60DB9391LL * t + 0x80000000000LL) >> 44);          /* FX_RAD_TO_IDX */
        s = data_0203d210[(idx >> 4) << 1] / 2;                                            /* FX_SinIdx / 2 */
        diff = h + (int)(((long long)s * 0x800 + 0x800) >> 12) - *(int *)(state[2] + 4);
        state[9] += (int)(((long long)(diff / 10) * 0x2000 + 0x800) >> 12);
    }
    if (dist < 0x2000) {
        dir.y = 0;
        VEC_Normalize(&dir, &dir);
        ScaleVec3Fx12(-0x100, &dir, &pull);
        VEC_Add((VecFx32 *)(state + 8), &pull, (VecFx32 *)(state + 8));
    } else if (dist > 0x3000) {
        dir.y = 0;
        VEC_Normalize(&dir, &dir);
        ScaleVec3Fx12(0x100, &dir, &pull);
        VEC_Add((VecFx32 *)(state + 8), &pull, (VecFx32 *)(state + 8));
    }
    if (Ov178_IsChildInactive(node) == 0) {
        *(unsigned char *)(*state + 0x1c7) = 4;
        SetIndexedSlot(node, *(signed char *)(node + 0x20), 0);
        return;
    }
    obj = *state;
    if (dist > *(int *)(obj + 0x2d8) / 2) {
        *(unsigned char *)(obj + 0x1c7) = 4;
        SetIndexedSlot(node, *(signed char *)(node + 0x20), 0);
        return;
    }
    if (dist >= *(int *)(obj + 0x2d8)) {
        return;
    }
    state[0x17] -= *(int *)(*(int *)node + 0x2c);
    if (state[0x17] > 0) {
        return;
    }
    state[0x17] = 0;
    if (RandNextScaled(0x78) == 0 || dist > 0x4000) {
        *(unsigned char *)(*state + 0x1c7) = 4;
        SetIndexedSlot(node, *(signed char *)(node + 0x20), 0);
        return;
    }
    if (RandNextScaled(0x14) == 0) {
        *(unsigned char *)(*state + 0x1c7) = 5;
        SetIndexedSlot(node, *(signed char *)(node + 0x20), 0);
        return;
    }
    if (Ov178_IsChildInactive(node) == 0) {
        return;
    }
    mode = ((unsigned int)RandNextScaled(100) < 0x46) ? 8 : 9;
    *(unsigned char *)(*state + 0x1c7) = mode;
    SetIndexedSlot(node, *(signed char *)(node + 0x20), 0);
}
