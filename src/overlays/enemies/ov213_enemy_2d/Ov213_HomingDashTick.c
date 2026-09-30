/* Homing dash tick: snapshots the +8 target's +0x10 position into +0x70, runs the +0x18 timer,
 * faces the target from the +4 anchor (+0x30, slerped into the +0x20 pose by 30/2 of the frame
 * step; +0x40 from data_0204227c), clamps the +0x1c range to the current distance, and sets the
 * +0xc velocity to the pose forward (data_02042258) scaled by 0.8125 of its alignment with the
 * target direction (floored at 0). Within 1.0 of the target, or once the timer passes 1.0, pose
 * request 0 is queued and the node dispatches null. */

#include "nitro/fx_types.h"
#include "game/engine.h"

static inline int FX_Mul(int a, int b) {
    return (int)(((long long)a * b + 0x800) >> 12);
}

extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern void Quat_FromTwoVectors(void *rotation, const VecFx32 *from, const VecFx32 *to);
extern void Quat_Slerp(void *a, int s, void *b, void *m);
extern int VEC_DotProduct(const VecFx32 *a, const VecFx32 *b);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const VecFx32 data_02042258;
extern const VecFx32 data_0204227c;

void Ov213_HomingDashTick(int *node) {
    int *state = (int *)node[1];
    VecFx32 dir;
    VecFx32 fwd;
    int dist;
    int align;

    *(VecFx32 *)(state + 0x1c) = *(VecFx32 *)(state[2] + 0x10);
    state[6] += *(int *)(node[0] + 0x2c);
    VEC_Subtract((VecFx32 *)(state + 0x1c), (VecFx32 *)state[1], &dir);
    VEC_Normalize(&dir, &dir);
    Quat_FromTwoVectors((void *)(state + 0xc), &data_02042258, &dir);
    Quat_Slerp(state + 8, *(int *)(node[0] + 0x2c) * 30 / 2, state + 8, state + 0xc);
    Quat_FromTwoVectors((void *)(state + 0x10), &data_0204227c, &dir);
    VEC_Subtract((VecFx32 *)(state + 0x1c), (VecFx32 *)state[1], &dir);
    dist = VEC_Normalize(&dir, &dir);
    if (dist < state[7]) state[7] = dist;
    Vec3TransformViaTempMtx(&fwd, (void *)(state + 8), &data_02042258);
    align = VEC_DotProduct(&dir, &fwd);
    if (align < 0) align = 0;
    state[7] = FX_Mul(align, 0xd00);
    ScaleVec3Fx12(state[7], &fwd, (VecFx32 *)(state + 3));
    if (dist >= 0x1000) {
        if (state[6] <= 0x1000) return;
    }
    *(unsigned char *)(*state + 0x1c7) = 0;
    SetIndexedSlot(node, *(signed char *)(node + 8), 0);
}
