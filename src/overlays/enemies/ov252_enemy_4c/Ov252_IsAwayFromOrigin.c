/* Whether the ov252 actor's +8 point is at least 4.0 from the origin on the ground plane. */

#include "nitro/fx_types.h"

extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int VEC_Normalize(VecFx32 *v, VecFx32 *out);
extern const VecFx32 data_02041dc8;

int Ov252_IsAwayFromOrigin(int *node)
{
    int *state = (int *)node[1];
    VecFx32 d;
    VecFx32 p;

    p = *(VecFx32 *)state[2];
    p.y = 0;
    VEC_Subtract(&data_02041dc8, &p, &d);
    return VEC_Normalize(&d, &d) >= 0x40000;
}
