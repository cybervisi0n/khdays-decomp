/* Shot flight tick: the part's +0x5c bit 1 clears, the +0x20 timer accumulates the frame rate and
 * the +8 position advances by the +0x14 velocity. While the owner (+4) is active (+0x50 == 1) a 0.75
 * sphere there sweeps the actor list: every entity in it is pushed 0.5 away horizontally (kind 0);
 * on acceptance reaction 0x148 mode 7 fires at the shot and the owner's +0x57a bit of this shot's
 * slot (+0x24) toggles. Hitting the owner's +4 +0x7c grid (01fff920) or its 0.75 sweep (01fff948)
 * toggles it too. The +0x7c part follows the shot (0203ca30). Past 1.0, or once its bit is set,
 * the bit toggles, the owner spawns effect 0 at the shot, the slot's +0x640 entry clears and the
 * node is released (0203c640). */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct { VecFx32 center; int nRadius; } Sphere;

extern void VEC_Add(const void *a, const void *b, void *out);
extern int Ov107_CollectSphereOverlaps(int owner, Sphere *sphere, int *hits);
extern void VEC_Subtract(const void *a, const void *b, VecFx32 *out);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern int Ov107_InvokeHitCallback(int hit, int owner, int item, int kind, VecFx32 *push, int z);
extern void Ov107_BuildAndSendUpdate(int owner, int id, int mode, void *at);
extern int Collision_CastRay(int grid, void *pos, void *vel);
extern int Collision_CastSphere(int grid, void *pos, void *vel, int radius);
extern void Srt_SetTranslation(int srt, void *pos);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);

void Ov252_ShotFlightTick(int *node)
{
    int *state = (int *)node[1];
    int hits[4];
    Sphere sphere;
    VecFx32 push;
    int owner;
    signed char i;
    int n;

    *(int *)(*state + 0x5c) &= ~2;
    state[8] += *(int *)(node[0] + 0x2c);
    VEC_Add(state + 2, state + 5, state + 2);
    if (*(int *)(state[1] + 0x50) == 1) {
        sphere.center = *(VecFx32 *)(state + 2);
        sphere.nRadius = 0xc00;
        n = Ov107_CollectSphereOverlaps(state[1], &sphere, hits);
        for (i = 0; i < n; i++) {
            VEC_Subtract((void *)(hits[i] + 0x190), state + 2, &push);
            push.y = 0;
            VEC_Normalize(&push, &push);
            ScaleVec3Fx12(0x800, &push, &push);
            if (Ov107_InvokeHitCallback(hits[i], state[1], state[1], 0, &push, 0) == 0) {
                continue;
            }
            Ov107_BuildAndSendUpdate(state[1], 0x148, 7, state + 2);
            *(u16 *)(state[1] + 0x57a) ^= 1 << *((signed char *)state + 0x24);
        }
    }
    owner = *(int *)(state[1] + 4);
    if (Collision_CastRay(*(int *)(owner + 0x7c), state + 2, state + 5) != 0) {
        *(u16 *)(state[1] + 0x57a) ^= 1 << *((signed char *)state + 0x24);
    }
    if (Collision_CastSphere(*(int *)(owner + 0x7c), state + 2, state + 5, 0xc00) != 0) {
        *(u16 *)(state[1] + 0x57a) ^= 1 << *((signed char *)state + 0x24);
    }
    Srt_SetTranslation(*state + 4, state + 2);
    if (state[8] < 0x1000 && (*(u16 *)(state[1] + 0x57a) & (1 << *((signed char *)state + 0x24))) != 0) {
        return;
    }
    *(u16 *)(state[1] + 0x57a) ^= 1 << *((signed char *)state + 0x24);
    func_ov107_020c0b90(state[1], 0, *(VecFx32 *)(state + 2), 0);
    *(int *)(state[1] + (*((signed char *)state + 0x24) + 0x13) * 8 + 0x640) = 0;
    Task_MarkFinished(node);
}
