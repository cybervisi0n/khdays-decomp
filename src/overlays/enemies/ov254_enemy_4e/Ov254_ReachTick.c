/* Reach tick of the ov254 arm helper: it stays on the +0x38c owner's +0x414 part (released, +0x60
 * high byte bit 7 cleared) and loops pose 1 once the +4 item's +0xad byte clears. The +0x24 reach
 * timer (0..0x7f8) grows a capsule from that point along the direction to the owner's +0x3dc
 * target (pitch clamped to about +-35 degrees), turned by the owner's +0xa0 pose into the +0xc
 * direction: 25.0 long at full reach, radius 1.46, tested by 020d4458. Until it latched (+0x2d)
 * the tip is traced through the item's world (ground ray, then a swept sphere); on contact or at
 * full reach +0x18 takes the tip, the owner is knocked back there (mode 0) and it latches.
 * Latched, for 0xdd0 more (+0x28) a sphere at +0x18 grows to 10.0 and is tested instead. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

typedef struct { int m[4]; VecFx32 trans; int pad[4]; } Srt;
typedef struct { VecFx32 a; VecFx32 d; int len; int r; } Capsule;
typedef struct { VecFx32 p; int r; } Sphere;
typedef struct { void *a; void *b; void *c; int d; } CollisionHit;
struct Hw60 { u16 lo : 8; u16 hi : 8; };

extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int VEC_Normalize(VecFx32 *v, VecFx32 *out);
extern void Vec3TransformViaTempMtx(VecFx32 *out, const Srt *m, const VecFx32 *in);
extern void Ov254_SubActorHitTest(int *node, Sphere *s, Capsule *c);
extern void ScaleVec3Fx12(int scale, VecFx32 *v, VecFx32 *d);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern CollisionHit *Collision_CastRay(void *collision, VecFx32 *origin, VecFx32 *direction);
extern void ScaleVec3Fixed27(int factor, VecFx32 *in, VecFx32 *out);
extern CollisionHit *Collision_CastSphereEx(void *collision, VecFx32 *origin, VecFx32 *dir, int radius, void *ignore);

static inline void VecSet(VecFx32 *v, int x, int y, int z)
{
    v->x = x;
    v->y = y;
    v->z = z;
}

void Ov254_ReachTick(int *node)
{
    int *state = (int *)node[1];
    Capsule cap;
    VecFx32 anchor;
    VecFx32 dir;
    VecFx32 flat;
    VecFx32 tip;
    Sphere sphere;
    int item;
    CollisionHit *hit;

    anchor = *(VecFx32 *)(*(int *)(*(int *)(*state + 0x38c) + 0x414) + 0x14);
    if (*(unsigned char *)(state[1] + 0xad) == 0) {
        Ov107_PostTagUpdate((Actor *)(*state), 1, 1);
    }
    state[9] += *(int *)(node[0] + 0x2c);
    state[9] = state[9] > 0x7f8 ? 0x7f8 : (state[9] < 0 ? 0 : state[9]);
    ((struct Hw60 *)(*state + 0x60))->hi &= ~0x80;
    Ov107_MoveNodeAndRelayout((Actor *)(*state), &anchor);
    VEC_Subtract((VecFx32 *)(*(int *)(*(int *)(*state + 0x38c) + 0x3dc) + 0x190), &anchor, &dir);
    VEC_Normalize(&dir, &dir);
    if (dir.y < -0x92d) {
        VecSet(&dir, 0, -0x92d, 0xd1b);
    } else if (dir.y > 0x92d) {
        VecSet(&dir, 0, 0x92d, 0xd1b);
    } else {
        VecSet(&flat, dir.x, 0, dir.z);
        VecSet(&dir, 0, dir.y, VEC_Normalize(&flat, &flat));
    }
    Vec3TransformViaTempMtx((VecFx32 *)(state + 3), (Srt *)(*(int *)(*state + 0x38c) + 0xa0), &dir);
    cap.a = anchor;
    cap.d = *(VecFx32 *)(state + 3);
    cap.len = state[9] * 0x64000 / 0x7f8;
    cap.r = 0x174c;
    Ov254_SubActorHitTest(node, 0, &cap);
    if (*((unsigned char *)state + 0x2d) == 0) {
        ScaleVec3Fx12(cap.len, &cap.d, &tip);
        if (state[9] >= 0x7f8) {
            VEC_Add(&cap.a, &tip, (VecFx32 *)(state + 6));
            func_ov107_020c0b90(*state, 0, *(VecFx32 *)(state + 6), 0);
            *((unsigned char *)state + 0x2d) = 1;
            return;
        }
        item = *(int *)(*state + 4);
        hit = Collision_CastRay(*(void **)(item + 0x7c), &cap.a, &tip);
        if (hit != 0 && hit->c == 0) {
            ScaleVec3Fixed27(hit->d, &tip, &tip);
            VEC_Add(&cap.a, &tip, (VecFx32 *)(state + 6));
            func_ov107_020c0b90(*state, 0, *(VecFx32 *)(state + 6), 0);
            *((unsigned char *)state + 0x2d) = 1;
            return;
        }
        hit = Collision_CastSphereEx(*(void **)(item + 0x7c), &cap.a, &tip, cap.r, 0);
        if (hit == 0) {
            return;
        }
        if (hit->c != 0) {
            return;
        }
        ScaleVec3Fixed27(hit->d, &tip, &tip);
        VEC_Add(&cap.a, &tip, (VecFx32 *)(state + 6));
        func_ov107_020c0b90(*state, 0, *(VecFx32 *)(state + 6), 0);
        *((unsigned char *)state + 0x2d) = 1;
        return;
    }
    if (state[0xa] >= 0xdd0) {
        return;
    }
    state[0xa] += *(int *)(node[0] + 0x2c);
    sphere.p = *(VecFx32 *)(state + 6);
    sphere.r = (state[0xa] > 0x7f8 ? 0x7f8 : (state[0xa] < 0 ? 0 : state[0xa])) * 0xa000 / 0x7f8;
    Ov254_SubActorHitTest(node, &sphere, 0);
}
