/* Homing-shot tick of the ov260 enemy: the +0x34 clock runs up at the owner's rate; the +0x28 step is
 * the +8 orientation's forward times the +0x38 speed, which decays by 5.5 % per 1/30 s slice and is
 * clamped to 0.375-0.625. Every entity around the owner (+0x74 sphere) is pushed by 0.5 along the
 * flattened direction away from it (kind 5); any acceptance breaks the shot: message 0 and sound 0x10
 * at the +0x18 position, sub-state 0. After 0.5 the shot turns towards the target (020cab14) while it
 * lies ahead (dot product above -0.875), slerping the orientation at 0.2 per frame. A world hit of the
 * step, or a swept sphere (half the owner's radius) from the +0x1c previous position, breaks it with
 * message 1 / 0 and sound 0x11; after 5.0 it expires with message 0. Otherwise +0x1c keeps the
 * position. */

#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct { int w[4]; } Quat;

extern int VEC_Normalize(VecFx32 *v, VecFx32 *d);
extern void ScaleVec3Fx12(int scale, VecFx32 *v, VecFx32 *d);
extern int FX_Div(int num, int den);
extern int Ov107_CollectSphereOverlaps(int owner, void *query, int *out);
extern void VEC_Subtract(void *a, void *b, VecFx32 *d);
extern int Ov107_InvokeHitCallback(int hit, int a, int b, int kind, VecFx32 *push, int z);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern void Ov260_PlaySound(int owner, int mode, int arg);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern int Ov107_FindNearestObject(int actor, int mode);
extern void Quat_FromTwoVectors(Quat *out, const VecFx32 *forward, const VecFx32 *direction);
extern int VEC_DotProduct(VecFx32 *a, VecFx32 *b);
extern void Quat_Slerp(Quat *out, int t, Quat *a, Quat *b);
extern int Collision_CastRay(int collision, VecFx32 *start, VecFx32 *ray);
extern int Collision_CastSphereEx(int collision, VecFx32 *origin, VecFx32 *dir, int radius, void *ignore);
extern const VecFx32 data_02042258;

#define FX_MUL(a, b) ((int)(((long long)(a) * (b) + 0x800) >> 12))

void Ov260_HomingShotTick(int *node)
{
    int owner;
    int *state = (int *)node[1];
    VecFx32 fwd;
    VecFx32 to;
    int hits[4];
    VecFx32 push;
    Quat q;
    int world;
    int speed;
    int hit;
    int rem;
    long i;
    long n;
    int target;

    owner = *state;
    world = *(int *)(owner + 4);
    state[0xd] += *(int *)(node[0] + 0x2c);
    hit = 0;
    Vec3TransformViaTempMtx(&fwd, state + 2, &data_02042258);
    VEC_Normalize(&fwd, &fwd);
    ScaleVec3Fx12(state[0xe], &fwd, (VecFx32 *)(state + 10));
    for (rem = *(int *)(node[0] + 0x2c); rem > 0; rem -= 0x88) {
        state[0xe] = FX_MUL(state[0xe], 0x1000 - FX_MUL(FX_Div(rem <= 0x88 ? rem : 0x88, 0x88), 0xe0));
    }
    speed = state[0xe];
    if (speed > 0xa00) {
        speed = 0xa00;
    } else if (speed < 0x600) {
        speed = 0x600;
    }
    state[0xe] = speed;
    n = Ov107_CollectSphereOverlaps(*(int *)(*state + 0x38c), (void *)(owner + 0x74), hits);
    for (i = 0; i < n; i++) {
        VEC_Subtract((void *)(hits[i] + 0x74), (void *)(owner + 0x74), &push);
        push.y = 0;
        VEC_Normalize(&push, &push);
        ScaleVec3Fx12(0x800, &push, &push);
        if (Ov107_InvokeHitCallback(hits[i], *state, *(int *)(*state + 0x38c), 5, &push, 0) != 0) {
            hit = 1;
        }
    }
    if (hit != 0) {
        func_ov107_020c0b90(*state, 0, *(VecFx32 *)state[6], 0);
        Ov260_PlaySound(*(int *)(*state + 0x38c), 0x10, state[6]);
        *(unsigned char *)(*state + 0x1c7) = 0;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
        return;
    }
    if (state[0xd] > 0x800) {
        target = Ov107_FindNearestObject(*state, 0);
        if (target != 0) {
            VEC_Subtract((void *)(target + 0x74), (void *)state[6], &to);
            VEC_Normalize(&to, &to);
            Quat_FromTwoVectors(&q, &data_02042258, &to);
            if (VEC_DotProduct(&fwd, &to) >= -0xe00) {
                Quat_Slerp((Quat *)(state + 2), FX_MUL(*(int *)(node[0] + 0x2c) * 0x14, 0x320),
                              (Quat *)(state + 2), &q);
                Vec4_Normalize((Quat *)(state + 2), (Quat *)(state + 2));
            }
        }
    }
    if (Collision_CastRay(*(int *)(world + 0x7c), (VecFx32 *)state[6], (VecFx32 *)(state + 10)) != 0) {
        func_ov107_020c0b90(*state, 1, *(VecFx32 *)state[6], 0);
        Ov260_PlaySound(*(int *)(*state + 0x38c), 0x11, state[6]);
        *(unsigned char *)(*state + 0x1c7) = 0;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
        return;
    }
    VEC_Subtract((void *)state[6], (void *)(state + 7), &to);
    if (Collision_CastSphereEx(*(int *)(world + 0x7c), (VecFx32 *)(state + 7), &to, *(int *)(owner + 0x80) / 2, 0) != 0) {
        func_ov107_020c0b90(*state, 0, *(VecFx32 *)state[6], 0);
        Ov260_PlaySound(*(int *)(*state + 0x38c), 0x11, state[6]);
        *(unsigned char *)(*state + 0x1c7) = 0;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
        return;
    }
    if (state[0xd] >= 0x5000) {
        func_ov107_020c0b90(*state, 0, *(VecFx32 *)state[6], 0);
        *(unsigned char *)(*state + 0x1c7) = 0;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
        return;
    }
    *(VecFx32 *)(state + 7) = *(VecFx32 *)state[6];
}
