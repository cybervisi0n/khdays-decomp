/* Ov245_RollTick -- roll tick: on the first frame (bit 7 of the +0x60 low byte, cleared
 * here) effect 1 plays at the +8 anchor. The actor's +0x74 sphere is swept (020c8eb8) and every
 * hit is pushed 0.5 along the flattened direction from the actor through the +0x390 item
 * (020ca918, mode 0): a landing hit plays effect 2 and reaction 0x15a/0xa at the anchor and ends
 * the roll (sub-state 0). Otherwise the step since the last +0x28 position is probed against
 * the scene's +0x7c collision: a wall (01fff920) or a floor probe (01fff8e8, radius 0.1875)
 * that hits nothing solid fires reaction 0x15a/0x10 and ends the roll; else the travelled
 * distance accumulates in +0x24 and past 32.0 the roll ends with effect 2. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

struct Sphere { VecFx32 centre; int radius; };
struct hw60 { unsigned short lo : 8, hi : 8; };

extern void func_ov107_020c0b90(int actor, int effect, VecFx32 v, int flag);
extern int Ov107_CollectSphereOverlaps(int actor, struct Sphere *sphere, int *out);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern int Ov107_InvokeHitCallback(int hit, int a, int b, int kind, VecFx32 *push, int z);
extern void Ov107_BuildAndSendUpdate(int actor, int id, int kind, void *anchor);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern int Collision_CastRay(int collision, const VecFx32 *from, const VecFx32 *step);
extern int Collision_CastSphereEx(int collision, const VecFx32 *from, const VecFx32 *step, int radius, void *ignore);
extern int VEC_Mag(const VecFx32 *v);

void Ov245_RollTick(int *node) {
    int *state = (int *)node[1];
    struct Sphere sphere;
    VecFx32 step;
    int hits[4];
    VecFx32 push;
    int scene = *(int *)(*state + 4);
    int i;
    int nHits;
    int hit;

    if ((((struct hw60 *)(*state + 0x60))->lo & 0x80) != 0) {
        ((struct hw60 *)(*state + 0x60))->hi &= ~0x80;
        func_ov107_020c0b90(*state, 1, *(VecFx32 *)state[2], 0);
    }
    sphere = *(struct Sphere *)(*state + 0x74);
    nHits = Ov107_CollectSphereOverlaps(*state, &sphere, hits);
    for (i = 0; i < nHits; i++) {
        VEC_Subtract((VecFx32 *)(hits[i] + 0x74), (VecFx32 *)(*state + 0x74), &push);
        push.y = 0;
        VEC_Normalize(&push, &push);
        ScaleVec3Fx12(0x800, &push, &push);
        if (Ov107_InvokeHitCallback(hits[i], *state, *(int *)(*state + 0x390), 0, &push, 0) != 0) {
            func_ov107_020c0b90(*state, 2, *(VecFx32 *)state[2], 0);
            Ov107_BuildAndSendUpdate(*state, 0x15a, 0xa, (void *)state[2]);
            *(unsigned char *)(*state + 0x1c7) = 0;
            SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
            return;
        }
    }
    VEC_Subtract((VecFx32 *)state[2], (VecFx32 *)(state + 10), &step);
    *(VecFx32 *)(state + 10) = *(VecFx32 *)state[2];
    if (Collision_CastRay(*(int *)(scene + 0x7c), (VecFx32 *)state[2], &step) != 0) {
        Ov107_BuildAndSendUpdate(*state, 0x15a, 0x10, (void *)state[2]);
    } else {
        hit = Collision_CastSphereEx(*(int *)(scene + 0x7c), (VecFx32 *)state[2], &step, 0x300, 0);
        if (hit != 0 && *(int *)(hit + 8) == 0) {
            Ov107_BuildAndSendUpdate(*state, 0x15a, 0x10, (void *)state[2]);
        } else {
            state[9] += VEC_Mag(&step);
            if (state[9] < 0x20000) {
                return;
            }
        }
    }
    func_ov107_020c0b90(*state, 2, *(VecFx32 *)state[2], 0);
    *(unsigned char *)(*state + 0x1c7) = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}
