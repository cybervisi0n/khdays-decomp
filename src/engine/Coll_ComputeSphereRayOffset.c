/* Ray-sphere intersection: from the ray origin and direction and the sphere centre and radius,
 * computes the offset along the direction to the far intersection; returns 0 when the ray misses.
 */

#include "nitro/fx_types.h"

extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *dst);
extern fx32 VEC_DotProduct(const VecFx32 *a, const VecFx32 *b);
extern fx32 FX_Sqrt(fx32 x);
extern void ScaleVec3Fx12(fx32 a, const VecFx32 *b, void *c);

#define FX_MUL(a, b) ((fx32)(((long long)(a) * (long long)(b) + 0x800) >> 12))

int Coll_ComputeSphereRayOffset(const VecFx32 *a, const VecFx32 *b, const VecFx32 *c, fx32 d, void *e)
{
    VecFx32 diff;
    fx32 dot;
    fx32 disc;

    VEC_Subtract(a, c, &diff);
    dot = VEC_DotProduct(&diff, b);
    disc = FX_MUL(d, d) + (FX_MUL(dot, dot) - VEC_DotProduct(&diff, &diff));
    if (disc < 0) return 0;
    ScaleVec3Fx12(FX_Sqrt(disc) - dot, b, e);
    return 1;
}
