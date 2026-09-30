
#include "nitro/fx_types.h"

typedef struct MtxFx33_0203cf0c {
    fx32 m[9];
} MtxFx33_0203cf0c;

static inline fx32 FX_Mul_0203cf0c(fx32 a, fx32 b)
{
    return (fx32)(((long long)a * b + 0x800) >> 12);
}

/* Scale a 3x3 matrix's rows by (scale->x, scale->y, scale->z), one factor
 * per column, and write the result to *dst. */
void Mtx33_ScaleColumns(MtxFx33_0203cf0c *dst, const MtxFx33_0203cf0c *mtx, const VecFx32 *scale)
{
    MtxFx33_0203cf0c result;

    result.m[0] = FX_Mul_0203cf0c(mtx->m[0], scale->x);
    result.m[1] = FX_Mul_0203cf0c(mtx->m[1], scale->y);
    result.m[2] = FX_Mul_0203cf0c(mtx->m[2], scale->z);
    result.m[3] = FX_Mul_0203cf0c(mtx->m[3], scale->x);
    result.m[4] = FX_Mul_0203cf0c(mtx->m[4], scale->y);
    result.m[5] = FX_Mul_0203cf0c(mtx->m[5], scale->z);
    result.m[6] = FX_Mul_0203cf0c(mtx->m[6], scale->x);
    result.m[7] = FX_Mul_0203cf0c(mtx->m[7], scale->y);
    result.m[8] = FX_Mul_0203cf0c(mtx->m[8], scale->z);

    *dst = result;
}
