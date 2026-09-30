/* Ground-probe helper of the ov191 enemy (x3: ov191/192/193): takes the pool's node position
 * (+0x14 of +0x398) raised by 0x400, hands it back through `out`, casts a 0x3000 ray straight
 * down from it through the actor's +0x7c collision world and, on a hit, moves the point onto
 * the surface (the hit's +0xc fraction of the ray, plus 0x200); the result is pushed to the
 * render hook (cmd 2) and effect 0x133 (7) is spawned at the node position. */

#include "nitro/fx_types.h"

extern void *Collision_CastRayEx(void *world, VecFx32 *from, VecFx32 *ray, void *arg);   /* Collision_CastRayEx */
extern void ScaleVec3Fixed27(int scale, VecFx32 *in, VecFx32 *out);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern void func_ov107_020c0b90(int obj, int cmd, VecFx32 v, int flag);
extern void Ov107_BuildAndSendUpdate(int obj, int effect, int kind, void *pos);

void Ov192_ProbeGroundBelowNode(int *node, VecFx32 *out)
{
    VecFx32 at;
    VecFx32 ray;
    int actor = *(int *)(*node + 4);
    void *hit;

    at = *(VecFx32 *)(*(int *)(*node + 0x398) + 0x14);
    at.y += 0x400;
    *out = at;
    ray.x = 0;
    ray.y = -0x3000;
    ray.z = 0;
    hit = Collision_CastRayEx(*(void **)(actor + 0x7c), &at, &ray, 0);
    if (hit != 0) {
        ScaleVec3Fixed27(*(int *)((char *)hit + 0xc), &ray, &ray);
        ray.y += 0x200;
        VEC_Add(&at, &ray, &at);
    }
    func_ov107_020c0b90(*node, 2, at, 0);
    Ov107_BuildAndSendUpdate(*node, 0x133, 7, (void *)(*(int *)(*node + 0x398) + 0x14));
}
