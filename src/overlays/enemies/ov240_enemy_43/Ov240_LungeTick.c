/* Lunge tick of the ov240 enemy: the +0x38 timer accumulates the frame-time and, past 0x880,
 * reaction 0x139 mode 0xa fires once (+0x3e) at the +8 point. Once the +4 item's +0xad byte
 * clears, the +0x20 lunge is cut short by a sphere cast and a ray cast of the scene's +0x7c
 * world (scaled to the first hit's +0xc fraction), the actor is placed at the +8 point plus the
 * lunge, animation 5 plays, effect 4 spawns there and the tick hands off to cf208. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

struct CastHit { char pad000[0xc]; int nFraction; };

extern void Ov107_BuildAndSendUpdate(int actor, int a, int b, void *at);
extern struct CastHit *Collision_CastSphere(void *world, void *from, VecFx32 *step, int radius);
extern struct CastHit *Collision_CastRayEx(void *world, void *from, VecFx32 *ray, void *arg);
extern void ScaleVec3Fixed27(int scale, VecFx32 *v, VecFx32 *d);
extern void VEC_Add(void *a, void *b, VecFx32 *d);
extern void func_ov107_020c0b90(int actor, int a, VecFx32 v, int d);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov240_stClearReadyFlag(int *node);

void Ov240_LungeTick(int *node)
{
    int *state = (int *)node[1];
    VecFx32 at;
    int scene;
    struct CastHit *cast;

    state[0xe] += *(int *)(*node + 0x2c);
    if (*(unsigned char *)((char *)state + 0x3e) == 0 && state[0xe] >= 0x880) {
        Ov107_BuildAndSendUpdate(*state, 0x139, 0xa, (void *)state[2]);
        *(unsigned char *)((char *)state + 0x3e) = 1;
    }
    if (*(unsigned char *)(state[1] + 0xad) != 0) {
        return;
    }
    scene = *(int *)(*state + 4);
    cast = Collision_CastSphere(*(void **)(scene + 0x7c), (void *)(*state + 0x74), (VecFx32 *)(state + 8), *(int *)(*state + 0x80));
    if (cast != 0) {
        ScaleVec3Fixed27(cast->nFraction, (VecFx32 *)(state + 8), (VecFx32 *)(state + 8));
    }
    cast = Collision_CastRayEx(*(void **)(scene + 0x7c), (void *)(*state + 0x74), (VecFx32 *)(state + 8), 0);
    if (cast != 0) {
        ScaleVec3Fixed27(cast->nFraction, (VecFx32 *)(state + 8), (VecFx32 *)(state + 8));
    }
    VEC_Add((void *)state[2], state + 8, &at);
    Ov107_MoveNodeAndRelayout((Actor *)(*state), &at);
    Ov107_PostTagUpdate((Actor *)(*state), 5, 0);
    func_ov107_020c0b90(*state, 4, at, 0);
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov240_stClearReadyFlag);
}
