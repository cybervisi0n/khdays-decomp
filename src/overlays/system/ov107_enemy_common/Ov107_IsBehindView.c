/* Whether the point lies outside the node's 60-degree view cone. */

#include "nitro/fx_types.h"

extern void VEC_Subtract(int *a, int *b, int *out);
extern fx32 VEC_Mag(const VecFx32 *v);
extern fx32 VEC_DotProduct(const VecFx32 *a, const VecFx32 *b);

typedef struct {
    char pad[0x7c];
    VecFx32 f7c;
    VecFx32 f88;
} Self;

static inline int FX_Mul(int a, int b) {
    return (int)(((long long)a * b + 0x800) >> 12);
}

int Ov107_IsBehindView(Self *self, VecFx32 *other)
{
    VecFx32 diff;
    fx32 mag;
    fx32 dot;

    VEC_Subtract((int *)other, (int *)&self->f88, (int *)&diff);
    mag = VEC_Mag(&diff);
    dot = VEC_DotProduct(&diff, &self->f7c);
    return dot < FX_Mul(mag, 0x800);
}
