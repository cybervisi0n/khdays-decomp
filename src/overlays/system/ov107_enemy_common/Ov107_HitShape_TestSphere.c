/* Overlap test of a hit shape against a sphere; contact point optionally written out. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct Sphere {
    VecFx32 center;
    fx32 radius;
} Sphere;

typedef struct Segment {
    VecFx32 p0;
    VecFx32 dir;
    fx32 scale;
} Segment;

typedef struct Box {
    VecFx32 center;
    VecFx32 axis[3];
    fx32 halfExtent[3];
} Box;

typedef struct HitShape {
    u8 mode : 4;
    char pad001[0x68 - 1];
    VecFx32 sphereCenter;   /* +0x68, mode 0 */
    fx32 sphereRadius;      /* +0x74 */
    Segment capsuleAxis;    /* +0x78, mode 1 */
    union {
        fx32 capsuleRadius; /* +0x94, mode 1 */
        Box box;            /* +0x94, mode 2 */
    } u94;
} HitShape;

extern void VEC_Subtract(VecFx32 *a, VecFx32 *b, VecFx32 *out);
extern fx32 VEC_Normalize(const VecFx32 *source, VecFx32 *destination);
extern void ScaleVec3Fx12(fx32 factor, VecFx32 *src, VecFx32 *dst);
extern void VEC_Add(VecFx32 *a, VecFx32 *b, VecFx32 *out);
extern fx32 Segment_ClosestPoint(VecFx32 *point, Segment *seg, fx64 *outDist);
extern fx32 FX_Sqrt(fx32 x);
extern fx32 OBB_DistSqToPoint(VecFx32 *point, Box *box, fx32 *outX, fx32 *outY, fx32 *outZ);

int Ov107_HitShape_TestSphere(HitShape *self, Sphere *sphere, VecFx32 *out)
{
    switch (self->mode) {
    case 0: {
        VecFx32 delta;
        fx32 dist;

        VEC_Subtract(&sphere->center, &self->sphereCenter, &delta);
        dist = VEC_Normalize(&delta, &delta);
        if (dist <= self->sphereRadius + sphere->radius) {
            if (out != 0) {
                ScaleVec3Fx12(self->sphereRadius + dist / 2, &delta, &delta);
                VEC_Add(&self->sphereCenter, &delta, out);
            }
            return 1;
        }
        break;
    }
    case 1: {
        fx64 t;
        if (FX_Sqrt(Segment_ClosestPoint(&sphere->center, &self->capsuleAxis, &t)) <= self->u94.capsuleRadius + sphere->radius) {
            if (out != 0) {
                fx64 along = t;
                out->x = (fx32)((along * self->capsuleAxis.dir.x + 0x80000000LL) >> 32);
                out->y = (fx32)((along * self->capsuleAxis.dir.y + 0x80000000LL) >> 32);
                out->z = (fx32)((along * self->capsuleAxis.dir.z + 0x80000000LL) >> 32);
                VEC_Add(&self->capsuleAxis.p0, out, out);
            }
            return 1;
        }
        break;
    }
    case 2: {
        fx32 x, y, z;
        if (FX_Sqrt(OBB_DistSqToPoint(&sphere->center, &self->u94.box, &x, &y, &z)) <= sphere->radius) {
            if (out != 0) {
                out->x = x;
                out->y = y;
                out->z = z;
            }
            return 1;
        }
        break;
    }
    }
    return 0;
}
