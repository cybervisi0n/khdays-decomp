/* Flight tick of the ov208 enemy's item (x3 with ov209/ov268). Plays animation 1 (looping) once
 * the +4 animator is free and scales the +0x18 direction by 0.25 into the +0xc step. An oriented
 * box at the item's +0x74 point (axes: up x direction, up, direction; half-extents +0x80, 0.75,
 * 0.75) is swept over the owner's (+0x394) list: every entity is pushed along the flattened unit
 * direction from the item (kind 3, from the owner) and, on acceptance, the owner spawns effect 1
 * at the +8 point and reaction 0/0x53 fires there. The move from the +0x24 previous point to +8
 * is then checked against the world's (+4 -> +0x7c) collision: a wall hit, or a floor hit of
 * radius 0.1875 whose surface has no +8 owner, bursts the item (effect 1, reaction 0/0x53,
 * animation 2) and hands over to Ov208_AiStep_QueueAction0OnAnimEnd; otherwise the travelled length accumulates
 * in +0x40 and past 15.0 the item bursts silently. */

#include "nitro/fx_types.h"

struct Obb { VecFx32 center; VecFx32 axisX; VecFx32 axisY; VecFx32 axisZ; int extent[3]; };

extern void Ov107_PostTagUpdate(int owner, int anim, int mode);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void VEC_CrossProduct(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int Ov107_CollectCapsuleOverlaps(int actor, struct Obb *query, int *out);
extern void VEC_Subtract(const void *a, const void *b, VecFx32 *out);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern int Ov107_InvokeHitCallback(int hit, int a, int b, int kind, VecFx32 *push, int z);
extern void func_ov107_020c0b90(int actor, int a, VecFx32 v, int b);
extern void Ov107_BuildAndSendUpdate(int owner, int id, int mode, void *at);
extern void *Collision_CastRay(void *collision, VecFx32 *origin, VecFx32 *dir);
extern int *Collision_CastSphereEx(void *collision, VecFx32 *origin, VecFx32 *dir, int radius, void *ignore);
extern int VEC_Mag(const VecFx32 *v);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const VecFx32 data_02042264;
extern void Ov208_AiStep_QueueAction0OnAnimEnd(int *node);

void Ov208_ItemFlightTick(int *node)
{
    int *state = (int *)node[1];
    int world = *(int *)(*state + 4);
    int hits[4];
    struct Obb query;
    VecFx32 delta;
    VecFx32 push;
    int i;
    int n;
    int *floor;

    if (*(unsigned char *)(state[1] + 0xad) == 0) {
        Ov107_PostTagUpdate(*state, 1, 1);
    }
    ScaleVec3Fx12(0x400, (VecFx32 *)(state + 6), (VecFx32 *)(state + 3));
    query.center = *(VecFx32 *)(*state + 0x74);
    query.axisY = data_02042264;
    query.axisZ = *(VecFx32 *)(state + 6);
    VEC_CrossProduct(&query.axisY, &query.axisZ, &query.axisX);
    query.extent[0] = *(int *)(*state + 0x80);
    query.extent[1] = 0xc00;
    query.extent[2] = 0xc00;
    n = Ov107_CollectCapsuleOverlaps(*(int *)(*state + 0x394), &query, hits);
    i = 0;
    if (n > 0) {
        do {
            VEC_Subtract((void *)(hits[i] + 0x74), (void *)(*state + 0x74), &push);
            push.y = 0;
            VEC_Normalize(&push, &push);
            if (Ov107_InvokeHitCallback(hits[i], *state, *(int *)(*state + 0x394), 3, &push, 0) != 0) {
                func_ov107_020c0b90(*(int *)(*state + 0x394), 1, *(VecFx32 *)state[2], 0);
                Ov107_BuildAndSendUpdate(*state, 0, 0x53, (void *)state[2]);
            }
        } while (++i < n);
    }
    VEC_Subtract((void *)state[2], (void *)(state + 9), &delta);
    *(VecFx32 *)(state + 9) = *(VecFx32 *)state[2];
    if (Collision_CastRay(*(void **)(world + 0x7c), (VecFx32 *)state[2], &delta) != 0) {
        func_ov107_020c0b90(*(int *)(*state + 0x394), 1, *(VecFx32 *)state[2], 0);
        Ov107_BuildAndSendUpdate(*state, 0, 0x53, (void *)state[2]);
        Ov107_PostTagUpdate(*state, 2, 0);
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov208_AiStep_QueueAction0OnAnimEnd);
        return;
    }
    floor = Collision_CastSphereEx(*(void **)(world + 0x7c), (VecFx32 *)state[2], &delta, 0x300, 0);
    if (floor != 0 && floor[2] == 0) {
        func_ov107_020c0b90(*(int *)(*state + 0x394), 1, *(VecFx32 *)state[2], 0);
        Ov107_BuildAndSendUpdate(*state, 0, 0x53, (void *)state[2]);
        Ov107_PostTagUpdate(*state, 2, 0);
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov208_AiStep_QueueAction0OnAnimEnd);
        return;
    }
    state[0x10] += VEC_Mag(&delta);
    if (state[0x10] <= 0xf000) {
        return;
    }
    func_ov107_020c0b90(*(int *)(*state + 0x394), 1, *(VecFx32 *)state[2], 0);
    Ov107_PostTagUpdate(*state, 2, 0);
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov208_AiStep_QueueAction0OnAnimEnd);
}
