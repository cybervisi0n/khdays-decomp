/* Projectile tick of the ov221 enemy's item. With a strike enabled (bStrike) the item's +0x74
 * sphere is swept first (ov221 44e0): a hit requests sub-state 0 and ends the action. Otherwise
 * the +0xc velocity is subtracted from the +8 point and a ray along it is cast (01fff920): a
 * blocking hit bursts (ov221 4498 with the sphere's point, flag 1), fires reaction 0x14b mode 9
 * at the +8 point and, with the strike enabled, sweeps once more, then ends the action with
 * sub-state 0. Failing that a swept cast of the +0x80 radius (01fff8e8) that lands on a solid
 * hit bursts the same way with flag 0. Without any hit the +0x44 travelled distance grows by
 * the velocity's length; past the +0x4c range plus the +0x390 pool's +0x80 it bursts (flag 0,
 * no reaction), else the velocity moves the point. */

#include "nitro/fx_types.h"

typedef struct { VecFx32 pos; int nRadius; } Sphere;

struct CollisionHit {
    int pad00;
    int pad04;
    int nBlocked;
    int nAlong;
};

extern int Ov225_SweepStrike(int *node, Sphere *sphere);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern struct CollisionHit *Collision_CastRay(void *collision, VecFx32 *origin, VecFx32 *dir);
extern struct CollisionHit *Collision_CastSphereEx(void *collision, VecFx32 *origin, VecFx32 *dir, int radius, void *ignore);
extern void Ov225_ForwardVecToOwner(int *state, VecFx32 v, int flag);
extern void Ov107_BuildAndSendUpdate(int owner, int id, int mode, void *at);
extern int VEC_Mag(const VecFx32 *v);

void Ov225_ProjectileTick(int *node, int bStrike, int bBurst)
{
    int *state = (int *)node[1];
    Sphere sphere;
    VecFx32 next;
    char *coll = *(char **)(*state + 4);
    struct CollisionHit *hit;
    int nTravel;

    sphere = *(Sphere *)(*state + 0x74);
    if (bStrike != 0 && Ov225_SweepStrike(node, &sphere) != 0) {
        *(unsigned char *)(*state + 0x1c7) = 0;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
        return;
    }
    VEC_Subtract((VecFx32 *)state[2], (VecFx32 *)(state + 3), &next);
    sphere = *(Sphere *)(*state + 0x74);
    sphere.nRadius = 0x1500;
    hit = Collision_CastRay(*(void **)(coll + 0x7c), (VecFx32 *)state[2], &next);
    if (hit != 0) {
        if (bBurst != 0) {
            Ov225_ForwardVecToOwner(state, sphere.pos, 1);
            Ov107_BuildAndSendUpdate(*state, 0x14b, 9, (void *)state[2]);
            if (bStrike != 0) {
                Ov225_SweepStrike(node, &sphere);
            }
        }
        *(unsigned char *)(*state + 0x1c7) = 0;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
        return;
    }
    hit = Collision_CastSphereEx(*(void **)(coll + 0x7c), (VecFx32 *)(state + 3), &next, *(int *)(*state + 0x80), 0);
    if (hit != 0 && hit->nBlocked == 0) {
        if (bBurst != 0) {
            Ov225_ForwardVecToOwner(state, sphere.pos, 0);
            Ov107_BuildAndSendUpdate(*state, 0x14b, 9, (void *)state[2]);
            if (bStrike != 0) {
                Ov225_SweepStrike(node, &sphere);
            }
        }
        *(unsigned char *)(*state + 0x1c7) = 0;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
        return;
    }
    nTravel = state[0x11] + VEC_Mag(&next);
    state[0x11] = nTravel;
    if (nTravel > state[0x13] + *(int *)(*(int *)(*state + 0x390) + 0x80)) {
        if (bBurst != 0) {
            Ov225_ForwardVecToOwner(state, sphere.pos, 0);
            if (bStrike != 0) {
                Ov225_SweepStrike(node, &sphere);
            }
        }
        *(unsigned char *)(*state + 0x1c7) = 0;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
        return;
    }
    *(VecFx32 *)(state + 3) = *(VecFx32 *)state[2];
}
