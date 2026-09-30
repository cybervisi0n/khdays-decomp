/* Resolve the ground an actor is standing on, and settle its vertical position.
 *
 * `self` is the sub-object constructed by `Actor_InitEntityLink` (embedded at +0x20 of
 * an actor object): a record pointer at +0x00, a flag word at +0x04, a sentinel
 * halfword at +0x46, a max-tracker int at +0x38, a 0x98-byte copy of the
 * collision system's cast state at +0x2e8, a derived hit point at +0x380 and a
 * ground-kind code at +0x38c. Its layout is only partly known -- the fields are
 * scattered over a 0x440-byte block -- so, like `Actor_InitEntityLink.c`, it stays word
 * indexing with byte offsets in comments rather than a struct that would be
 * mostly padding.
 *
 * A negative sentinel means the object is not bound to an actor yet, and the
 * function no-ops. Otherwise it asks the record's +0x110 sub-object which of two
 * known kinds it is to pick the base height from the record's own fields, then
 * finds the ground:
 *
 *   - With flag 0x1000 set, the hit point cached at +0x384 from a previous frame
 *     is reused directly and the actor counts as grounded.
 *   - Otherwise it casts straight up from the actor position, copies the whole
 *     cast-state record into +0x2e8, derives the hit point into +0x380, and -- if
 *     the surface normal's Y is a shallow negative slope -- adds a slope-height
 *     correction. Everything downstream of the cast, the ceiling test included,
 *     is skipped when the cast misses.
 *
 * When the result counts as grounded, two tracked maxima are clamped non-positive,
 * the grounded bit is set, and two downward casts settle the final height, taking
 * the larger of the two candidate depths. The second cast only runs if the first
 * one hit.
 *
 * Two notes on spelling, both load-bearing:
 *   - `FX_Inv` at 0x01ff8a04 forwards r0 AND r1 to FX_DivAsync, so it is the SDK's
 *     two-argument `FX_Div(numerator, denominator)`, not the single-argument
 *     forwarder in `libs/nitro/fx/calls/FX_Inv.c`.
 *   - The downward cast's vectors are built with the SDK's `VEC_Set` inline, which
 *     groups the three loads and leaves the three stores separate -- the shape the
 *     ROM has and that field-by-field assignment cannot reach here. The upward
 *     cast's vectors are the opposite: written field by field, with the two zeroed
 *     components chained. Using VEC_Set there instead costs 8 bytes.
 */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct CollCastParams {
    VecFx32 *origin;
    VecFx32 *direction;
    fx32 radius;
    u16 directionIsUnit;
    u16 flags;
    void *exclude;
} CollCastParams;

typedef struct CollisionHit {
    void *model;
    void *face;
    int unknown08;
    fx32 distance;
} CollisionHit;

/* Wholesale copy of the collision system's cast-state record. Only its size is
 * known: 0x98 bytes, the gap between self+0x2e8 and self+0x380. */
typedef struct HitSnapshot {
    int w[38];
} HitSnapshot;

extern void *GetTrackEntryBase(int idx);
extern CollisionHit *Collision_CastNearest(void *world, CollCastParams *params);
extern CollisionHit *Collision_RunRayCast(void *world, CollCastParams *params);
extern int FX_Div(int numerator, int denominator);
extern int FX_Sqrt(int x);

static inline fx32 MulRound(fx32 a, fx32 b) { return (fx32)(((fx64)a * b + 0x800) >> 12); }

static inline void VEC_Set(VecFx32 *vec, fx32 x, fx32 y, fx32 z) {
    vec->x = x;
    vec->y = y;
    vec->z = z;
}

int Actor_ResolveGround(VecFx32 *pos, int *self, int *pOut)
{
    s16 sentinel;
    int flags;
    int record;
    void *world;
    CollisionHit *hit;
    int isSolid;
    fx32 depth;
    fx32 baseHeight;
    /* The block below is declared in reverse of its stack-offset order: mwcc
     * lays locals out ascending in reverse declaration order, so `origin` gets
     * the highest offset and `ret` the lowest. */
    VecFx32 origin, direction;
    CollCastParams params;
    int normal[3];
    VecFx32 direction2, origin2;
    VecFx32 outVec;
    int ret;

    sentinel = *(s16 *)((char *)self + 0x46);
    ret = 0;
    record = self[0];                                   /* +0x000 */
    isSolid = 0;
    depth = 0;
    baseHeight = 0;

    if (sentinel < 0) {
        return 0;
    }

    flags = self[1];                                    /* +0x004 */
    if (!(flags & 0x800)) {
        self[1] = flags & ~8;
    }

    if (IsField1cEqualData42910(record + 0x110)) {
        baseHeight = *(int *)(record + 0x14c) + *(int *)(record + 0x148);
    } else if (IsField1cEqualData42940(record + 0x110)) {
        baseHeight = *(int *)(record + 0x14c);
    }

    if (self[1] & 0x1000) {
        isSolid = 1;
        self[0xe3] = 1;                                 /* +0x38c ground kind */
        depth = self[0xe1] - baseHeight;                /* +0x384 cached hit Y */
    } else {
        world = GetTrackEntryBase(*(u16 *)((char *)self + 0x46));

        origin.x = pos->x;
        origin.y = pos->y + 0x800;
        origin.z = pos->z;
        direction.x = direction.z = 0;
        direction.y = 0xa000;
        params.origin = &origin;
        params.directionIsUnit = 0;
        params.flags = 0;
        params.direction = &direction;
        params.exclude = (void *)self[0];
        hit = Collision_CastNearest(world, &params);

        if (hit != 0) {
            *(HitSnapshot *)((char *)self + 0x2e8) = *(HitSnapshot *)hit;
            Vec3ScaleAddQ27(hit->distance, &direction, &origin,
                          (VecFx32 *)((char *)self + 0x380));
            if (hit->unknown08 != 0) {
                self[0xe3] = 1;                         /* +0x38c */
            } else {
                self[0xe3] = 2;
            }
        }
        if (hit != 0) {
            VecFx32FromVecS16((int)hit->model, (short *)((char *)hit->face + 0x14), normal);
            if (normal[1] < 0 && normal[1] > -0x1000) {
                fx32 sq = MulRound(normal[1], normal[1]);
                fx32 t = FX_Sqrt(FX_Div(0x1000, sq) - 0x1000);
                baseHeight += MulRound(t, 0xd00);
            }

            if (*pOut + (pos->y + baseHeight) >= self[0xe1]) {
                depth = self[0xe1] - baseHeight;
                isSolid = 1;
            }
        }
    }

    if (isSolid != 0) {
        if (self[0xe] > 0) self[0xe] = 0;               /* +0x038 max tracker */
        if (*pOut > 0) *pOut = 0;
        ret = 1;
        self[1] |= 8;                                   /* +0x004 grounded */

        world = GetTrackEntryBase(*(u16 *)((char *)self + 0x46));

        VEC_Set(&direction2, 0, 0 - 0x50000, 0);
        VEC_Set(&origin2, pos->x, pos->y + baseHeight, pos->z);
        params.origin = &origin2;
        params.flags = 0;
        params.direction = &direction2;
        params.directionIsUnit = 1;
        params.exclude = (void *)self[0];

        hit = Collision_RunRayCast(world, &params);
        if (hit != 0) {
            Vec3ScaleAddQ27(hit->distance, &direction2, &origin2, &outVec);
            if (outVec.y > depth) depth = outVec.y;

            params.directionIsUnit = 0;
            hit = Collision_RunRayCast(world, &params);
            if (hit != 0) {
                Vec3ScaleAddQ27(hit->distance, &direction2, &origin2, &outVec);
                if (outVec.y > depth) depth = outVec.y;
            }
        }

        pos->y = depth;
    }

    return ret;
}
