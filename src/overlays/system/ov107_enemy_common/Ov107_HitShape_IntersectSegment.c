/* Tests a segment against the hit shape (sphere, capsule or box); stores the contact point. Returns
 * 1 on a hit. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct Segment {
    VecFx32 p0;
    VecFx32 dir;
    fx32 scale;
} Segment;

typedef struct HitShape {
    u8 mode : 4;
    char pad001[0x68 - 1];
    VecFx32 sphereCenter;   /* +0x68, mode 0 */
    fx32 sphereRadius;      /* +0x74 */
    Segment capsuleAxis;    /* +0x78, mode 1 */
    fx32 capsuleRadius;     /* +0x94 */
} HitShape;

extern fx32 Segment_ClosestPoint(VecFx32 *point, Segment *seg, fx64 *outDist);
extern fx32 FX_Sqrt(fx32 x);
extern void VEC_Add(VecFx32 *a, VecFx32 *b, VecFx32 *out);
extern fx32 Seg_SqrDistToSegment(Segment *a, Segment *b, fx64 *outDistA, fx64 *outDistB);
extern int Capsule_ClosestToBox(Segment *seg, void *b, fx32 *outDist, int c, int d, int e);
extern void ScaleVec3Fx12(fx32 factor, VecFx32 *src, VecFx32 *dst);

int Ov107_HitShape_IntersectSegment(HitShape *self, Segment *seg, VecFx32 *out)
{
    switch (self->mode) {
    case 0: {
        fx64 t;
        if (FX_Sqrt(Segment_ClosestPoint(&self->sphereCenter, seg, &t)) <= self->sphereRadius) {
            if (out != 0) {
                fx64 along = t;
                out->x = (fx32)((along * seg->dir.x + 0x80000000LL) >> 32);
                out->y = (fx32)((along * seg->dir.y + 0x80000000LL) >> 32);
                out->z = (fx32)((along * seg->dir.z + 0x80000000LL) >> 32);
                VEC_Add(&seg->p0, out, out);
            }
            return 1;
        }
        break;
    }
    case 1: {
        fx64 tSelf;
        fx64 t;
        if (FX_Sqrt(Seg_SqrDistToSegment(&self->capsuleAxis, seg, &tSelf, &t)) <= self->capsuleRadius) {
            if (out != 0) {
                fx64 along = t;
                out->x = (fx32)((along * seg->dir.x + 0x80000000LL) >> 32);
                out->y = (fx32)((along * seg->dir.y + 0x80000000LL) >> 32);
                out->z = (fx32)((along * seg->dir.z + 0x80000000LL) >> 32);
                VEC_Add(&seg->p0, out, out);
            }
            return 1;
        }
        break;
    }
    case 2: {
        fx32 t;
        if (Capsule_ClosestToBox(seg, &self->capsuleRadius, &t, 0, 0, 0) <= 8) {
            if (out != 0) {
                ScaleVec3Fx12(t, &seg->dir, out);
                VEC_Add(&seg->p0, out, out);
            }
            return 1;
        }
        break;
    }
    }
    return 0;
}
