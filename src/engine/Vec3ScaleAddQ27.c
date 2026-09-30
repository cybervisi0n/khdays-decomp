/* c = b + s * a, with the products shifted by 27 bits. */

#include "nitro/fx_types.h"

void Vec3ScaleAddQ27(fx32 s, const VecFx32 *a, const VecFx32 *b, VecFx32 *c)
{
    c->x = b->x + (fx32)(((fx64)s * a->x) >> 27);
    c->y = b->y + (fx32)(((fx64)s * a->y) >> 27);
    c->z = b->z + (fx32)(((fx64)s * a->z) >> 27);
}
