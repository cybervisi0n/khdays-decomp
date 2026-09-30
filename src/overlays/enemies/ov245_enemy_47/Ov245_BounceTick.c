/* Ov245_BounceTick -- bounce tick: on the first frame (bit 7 of the +0x60 low byte, cleared
 * here) the +0xc velocity becomes (0, 0.875, 0). While the actor's +0x17a bit 0 is set the
 * horizontal velocity is the +0x24 direction scaled by 0.375 and the vertical one is zero;
 * otherwise the +0x18 direction is used and gravity (-0x80 * step / 0x88) accumulates. The
 * actor's +0x74 sphere is swept by the +0x390 item (020c8eb8) and every hit is pushed along
 * the flattened direction from the actor (020ca918, mode 1): a landing hit plays effect 0 and
 * reaction 0x15a/0xd at the +8 anchor and ends the bounce (sub-state 0). Otherwise the step
 * since the last +0x34 position is probed against the scene's +0x7c floor (01fff8e8, radius
 * 0.1875) while the +0x30 timer runs up: a solid floor hit, or 30.0 elapsed, ends the bounce
 * with effect 0. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

struct Sphere { VecFx32 centre; int radius; };
struct hw60 { unsigned short lo : 8, hi : 8; };
struct Bit0 { unsigned char b0 : 1; };

extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern int Ov107_CollectSphereOverlaps(int actor, struct Sphere *sphere, int *out);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern int Ov107_InvokeHitCallback(int hit, int a, int b, int kind, VecFx32 *push, int z);
extern void func_ov107_020c0b90(int actor, int effect, VecFx32 v, int flag);
extern void Ov107_BuildAndSendUpdate(int actor, int id, int kind, void *anchor);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern int Collision_CastSphereEx(int collision, const VecFx32 *from, const VecFx32 *step, int radius, void *ignore);

void Ov245_BounceTick(int *node) {
    int *state = (int *)node[1];
    struct Sphere sphere;
    VecFx32 v;
    int hits[4];
    VecFx32 push;
    int scene = *(int *)(*state + 4);
    int i;
    int nHits;
    int hit;
    int vy;

    if ((((struct hw60 *)(*state + 0x60))->lo & 0x80) != 0) {
        ((struct hw60 *)(*state + 0x60))->hi &= ~0x80;
        state[3] = 0;
        state[4] = 0x380;
        state[5] = 0;
    }
    if (((struct Bit0 *)(*state + 0x17a))->b0) {
        ScaleVec3Fx12(0x600, (VecFx32 *)(state + 9), &v);
        vy = 0;
    } else {
        ScaleVec3Fx12(0x600, (VecFx32 *)(state + 6), &v);
        vy = state[4] + *(int *)(node[0] + 0x2c) * -0x80 / 0x88;
    }
    state[4] = vy;
    state[3] = v.x;
    state[5] = v.z;
    sphere = *(struct Sphere *)(*state + 0x74);
    nHits = Ov107_CollectSphereOverlaps(*(int *)(*state + 0x390), &sphere, hits);
    for (i = 0; i < nHits; i++) {
        VEC_Subtract((VecFx32 *)(hits[i] + 0x74), (VecFx32 *)(*state + 0x74), &push);
        push.y = 0;
        VEC_Normalize(&push, &push);
        if (Ov107_InvokeHitCallback(hits[i], *state, *(int *)(*state + 0x390), 1, &push, 0) != 0) {
            func_ov107_020c0b90(*state, 0, *(VecFx32 *)state[2], 0);
            Ov107_BuildAndSendUpdate(*state, 0x15a, 0xd, (void *)state[2]);
            *(unsigned char *)(*state + 0x1c7) = 0;
            SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
            return;
        }
    }
    VEC_Subtract((VecFx32 *)state[2], (VecFx32 *)(state + 13), &v);
    *(VecFx32 *)(state + 13) = *(VecFx32 *)state[2];
    hit = Collision_CastSphereEx(*(int *)(scene + 0x7c), (VecFx32 *)state[2], &v, 0x300, 0);
    state[12] += *(int *)(node[0] + 0x2c);
    if (hit == 0 || *(int *)(hit + 8) != 0) {
        if (state[12] < 0x1e000) {
            return;
        }
    }
    func_ov107_020c0b90(*state, 0, *(VecFx32 *)state[2], 0);
    *(unsigned char *)(*state + 0x1c7) = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}
