/* Shot flight tick: the shot's own sphere (+0x74, radius +0x80) sweeps the actor list on behalf of
 * the +0x38c owner. The first entity that accepts a 0.25 horizontal push away from it (kind 0) ends
 * the shot: effect 0 at the +4 point, the owner's reaction 0x173 mode 5 there and pose 0. Otherwise
 * the step since the last tick (+0x20) is tested against the owner's +4 +0x7c stage grid (01fff920)
 * and a 0.19 sweep (01fff8e8, solid hits only); a wall ends the shot with reaction mode 6. The
 * +0x1c distance accumulates the step length; past 21.0 the shot fizzles (effect 0, pose 0). */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { VecFx32 center; int nRadius; } Sphere;

extern int Ov107_CollectSphereOverlaps(int owner, Sphere *sphere, int *hits);
extern void VEC_Subtract(const void *a, const void *b, VecFx32 *out);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern int Ov107_InvokeHitCallback(int hit, int owner, int item, int kind, VecFx32 *push, int z);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern void Ov283_PostItemUpdate(int owner, int id, int mode, void *at);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern int Collision_CastRay(int grid, void *pos, VecFx32 *step);
extern int Collision_CastSphereEx(int grid, void *pos, VecFx32 *step, int radius, int flags);
extern int VEC_Mag(const VecFx32 *v);

void Ov283_ShotFlightTick(int *node)
{
    int *state = (int *)node[1];
    int owner = *(int *)(*state + 4);
    Sphere sphere;
    VecFx32 step;
    int hits[4];
    VecFx32 push;
    int hit;
    int i;
    int n;

    sphere = *(Sphere *)(*state + 0x74);
    n = Ov107_CollectSphereOverlaps(*state, &sphere, hits);
    for (i = 0; i < n; i++) {
        VEC_Subtract((void *)(hits[i] + 0x74), (void *)(*state + 0x74), &push);
        push.y = 0;
        VEC_Normalize(&push, &push);
        ScaleVec3Fx12(0x400, &push, &push);
        if (Ov107_InvokeHitCallback(hits[i], *state, *(int *)(*state + 0x38c), 0, &push, 0) == 0) {
            continue;
        }
        func_ov107_020c0b90(*state, 0, *(VecFx32 *)state[1], 0);
        Ov283_PostItemUpdate(*(int *)(state[0] + 0x38c), 0x173, 5, (void *)state[1]);
        *(u8 *)(*state + 0x1c7) = 0;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
        return;
    }
    VEC_Subtract((void *)state[1], state + 8, &step);
    *(VecFx32 *)(state + 8) = *(VecFx32 *)state[1];
    if (Collision_CastRay(*(int *)(owner + 0x7c), (void *)state[1], &step) != 0) {
        func_ov107_020c0b90(*state, 0, *(VecFx32 *)state[1], 0);
        *(u8 *)(*state + 0x1c7) = 0;
        Ov283_PostItemUpdate(*(int *)(state[0] + 0x38c), 0x173, 6, (void *)state[1]);
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
        return;
    }
    hit = Collision_CastSphereEx(*(int *)(owner + 0x7c), (void *)state[1], &step, 0x300, 0);
    if (hit != 0 && *(int *)(hit + 8) == 0) {
        func_ov107_020c0b90(*state, 0, *(VecFx32 *)state[1], 0);
        *(u8 *)(*state + 0x1c7) = 0;
        Ov283_PostItemUpdate(*(int *)(state[0] + 0x38c), 0x173, 6, (void *)state[1]);
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
        return;
    }
    state[7] += VEC_Mag(&step);
    if (state[7] < 0x15000) {
        return;
    }
    func_ov107_020c0b90(*state, 0, *(VecFx32 *)state[1], 0);
    *(u8 *)(*state + 0x1c7) = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}
