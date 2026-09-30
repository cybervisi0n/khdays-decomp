/* Carry-dash tick of the ov171 enemy (and its byte-identical twins). With a target in reach the
 * +0x20 orientation quaternion is slerped (frame-time * 300 / 100) towards the quaternion turning
 * the shared forward vector onto the direction from the +8 position to the target's +0x190 point,
 * and the +0x30 velocity becomes (0, 0, 0x500) rotated by it. The +0x38c item's own sphere is
 * swept: the first victim that accepts a flat push (mode 0, strength 0x200) gets the item's +0xb0
 * point published (mode 2) and reaction 0x140 mode 6; its +0x74 sphere is copied to +0x10 and its
 * +0x18c rider becomes the +0xc carried target (grabbed with Ov022_ToggleBit13ByMode mode 1, +0x4c
 * timer reset, hand-off to the carry-release tick) or, without a rider, the state ends with
 * sub-state 0. Otherwise the step from the +0x3c origin to the position is cast against the
 * scene (ray, then the second ray cast, then a sphere of the actor's +0x80 radius) unless the
 * +0x17a flags bits 0/1/3 already report a wall; a hit publishes the position (mode 1) with
 * reaction 0x140 mode 7 and ends; else the +0x48 travel accumulates the step length and ends the
 * dash past 0x15000. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/engine.h"

struct Vecx32_4 { int x, y, z, w; };
struct Quat { int a, b, c, d; };
struct Flags17a { u8 b0 : 1, b1 : 1, b2 : 1, b3 : 1; };

extern int Ov107_FindNearestObject(int actor, int mode);
extern void VEC_Subtract(void *a, void *b, void *d);
extern int VEC_Normalize(void *a, void *d);
extern void Quat_FromTwoVectors(struct Quat *dst, const VecFx32 *from, const VecFx32 *to);
extern void Quat_Slerp(struct Quat *dst, int t, const struct Quat *a, const struct Quat *b);
extern int Ov107_CollectSphereOverlaps(void *item, void *sphere, void *out);
extern int Ov107_InvokeHitCallback(void *hit, int actor, void *item, int mode, void *push, int z);
extern void func_ov107_020c0b90(void *item, int a, VecFx32 v, int b);
extern void Ov107_BuildAndSendUpdate(int actor, int a, int id, void *at);
extern void Ov022_ToggleBit13ByMode(int target, int a);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void *Collision_CastRayEx(void *world, void *from, void *step, void *z);
extern void *Collision_CastSimple(void *world, void *from, void *step, void *z);
extern void *Collision_CastSphereEx(void *world, void *from, void *step, int radius, void *z);
extern int VEC_Mag(const VecFx32 *v);
extern const VecFx32 data_02042258;
extern void Ov171_CarryReleaseTick(int *node);

void Ov171_CarryDashTick(int *node)
{
    int actor;
    int *state = (int *)node[1];
    VecFx32 step;
    void *hits[4];
    struct Quat turn;
    VecFx32 push;
    int i;
    int n;
    int target;
    int scene;
    int wall;
    int hit;

    actor = *state;
    target = Ov107_FindNearestObject(actor, 0);
    if (target != 0) {
        VEC_Subtract((void *)(target + 0x190), (void *)state[2], &step);
        VEC_Normalize(&step, &step);
        Quat_FromTwoVectors(&turn, &data_02042258, &step);
        Quat_Slerp((struct Quat *)(state + 8), *(int *)(*node + 0x2c) * 300 / 100, (struct Quat *)(state + 8), &turn);
        state[0xc] = 0;
        state[0xd] = 0;
        state[0xe] = 0x500;
        Vec3TransformViaTempMtx(state + 0xc, state + 8, state + 0xc);
    }
    n = Ov107_CollectSphereOverlaps(*(void **)(*state + 0x38c), (void *)(actor + 0x74), hits);
    i = 0;
    if (n > 0) {
        do {
            VEC_Subtract((char *)hits[i] + 0x74, (void *)(actor + 0x74), &push);
            push.y = 0;
            VEC_Normalize(&push, &push);
            if (Ov107_InvokeHitCallback(hits[i], *state, *(void **)(*state + 0x38c), 0, &push, 0x200) != 0) {
                char *item = *(char **)(*state + 0x38c);
                func_ov107_020c0b90(item, 2, *(VecFx32 *)(item + 0xb0), 0);
                Ov107_BuildAndSendUpdate(*state, 0x140, 6, (void *)state[2]);
                *(struct Vecx32_4 *)(state + 4) = *(struct Vecx32_4 *)((char *)hits[i] + 0x74);
                target = *(int *)((char *)hits[i] + 0x18c);
                state[3] = target;
                if (target != 0) {
                    Ov022_ToggleBit13ByMode(target, 1);
                    state[0x13] = 0;
                    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov171_CarryReleaseTick);
                    return;
                }
                *(u8 *)(*state + 0x1c7) = 0;
                SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
                return;
            }
            i++;
        } while (i < n);
    }
    VEC_Subtract((void *)state[2], state + 0xf, &step);
    actor = *state;
    wall = hit = 1;
    if (((struct Flags17a *)(actor + 0x17a))->b0 == 0 && ((struct Flags17a *)(actor + 0x17a))->b1 == 0) {
        wall = 0;
    }
    if (wall == 0 && ((struct Flags17a *)(actor + 0x17a))->b3 == 0) {
        hit = 0;
    }
    if (state[0x12] == 0) {
        scene = *(int *)(actor + 4);
        if (hit == 0) {
            hit = (int)Collision_CastRayEx(*(void **)(scene + 0x7c), state + 0xf, &step, 0);
        }
        if (hit == 0) {
            hit = (int)Collision_CastSimple(*(void **)(scene + 0x7c), state + 0xf, &step, 0);
        }
        if (hit == 0) {
            hit = (int)Collision_CastSphereEx(*(void **)(scene + 0x7c), state + 0xf, &step, *(int *)(*state + 0x80), 0);
        }
    }
    *(VecFx32 *)(state + 0xf) = *(VecFx32 *)state[2];
    if (hit != 0) {
        func_ov107_020c0b90(*(void **)(*state + 0x38c), 1, *(VecFx32 *)state[2], 0);
        Ov107_BuildAndSendUpdate(*state, 0x140, 7, (void *)state[2]);
        *(u8 *)(*state + 0x1c7) = 0;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
        return;
    }
    state[0x12] += VEC_Mag(&step);
    if (state[0x12] < 0x15000) {
        return;
    }
    *(u8 *)(*state + 0x1c7) = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}
