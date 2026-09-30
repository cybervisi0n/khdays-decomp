/* Slam aim tick of the ov225 enemy. The +0x5c timer accumulates the owner's rate and, once it
 * reaches 0x600 with a +0x10 target, the +0x14 leap is the flattened unit direction from the
 * owner's +0x74 to the target's +0x74 scaled 7.5 and turned by a random angle within +-0xc91;
 * a swept cast of the owner's +0x80 radius and a plain ray from the +0xc point both clip it to
 * their first hit, and the target is released. The tick then hands over to
 * Ov225_SlamLanding. */

#include "nitro/fx_types.h"

typedef struct { int m[9]; } Mtx33;

struct CollisionHit { int pad00; int pad04; int pad08; int nAlong; };

extern int RandNextScaled(int bound);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void MTX_RotY33_(Mtx33 *m, int sin, int cos);
extern void MTX_MultVec33(const VecFx32 *v, Mtx33 *m, VecFx32 *d);
extern struct CollisionHit *Collision_CastSphere(void *collision, void *origin, VecFx32 *dir, int radius);
extern void ScaleVec3Fixed27(int scale, VecFx32 *in, VecFx32 *out);
extern struct CollisionHit *Collision_CastRayEx(void *collision, void *origin, VecFx32 *dir, void *ignore);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern short data_0203d210[];
extern void Ov225_SlamLanding(int *node);

#define ANG2IDX(a) ((unsigned short)(((long long)(a) * 0x28be60db9391LL + 0x80000000000LL) >> 44) >> 4)

void Ov225_SlamAimTick(int *node)
{
    int *state = (int *)node[1];
    Mtx33 m;
    VecFx32 d;
    int ang;
    unsigned int idx;
    char *coll;
    struct CollisionHit *hit;

    state[0x17] += *(int *)(*node + 0x2c);
    if (state[0x17] < 0x600) {
        return;
    }
    if (state[4] != 0) {
        ang = RandNextScaled(0x1923) - 0xc91;
        coll = *(char **)(*state + 4);
        VEC_Subtract((VecFx32 *)(state[4] + 0x74), (VecFx32 *)(*state + 0x74), &d);
        d.y = 0;
        VEC_Normalize(&d, &d);
        ScaleVec3Fx12(0x7800, &d, &d);
        idx = ANG2IDX(ang);
        MTX_RotY33_(&m, data_0203d210[idx * 2], data_0203d210[idx * 2 + 1]);
        MTX_MultVec33(&d, &m, (VecFx32 *)(state + 5));
        hit = Collision_CastSphere(*(void **)(coll + 0x7c), (void *)state[3], (VecFx32 *)(state + 5), *(int *)(*state + 0x80));
        if (hit != 0) {
            ScaleVec3Fixed27(hit->nAlong, (VecFx32 *)(state + 5), (VecFx32 *)(state + 5));
        }
        hit = Collision_CastRayEx(*(void **)(coll + 0x7c), (void *)state[3], (VecFx32 *)(state + 5), 0);
        if (hit != 0) {
            ScaleVec3Fixed27(hit->nAlong, (VecFx32 *)(state + 5), (VecFx32 *)(state + 5));
        }
        state[4] = 0;
    }
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov225_SlamLanding);
}
