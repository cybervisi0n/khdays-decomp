/* Launch the ov146 actor along `dir`: the +0xc velocity is its unit vector at 0.5 with a 0.625 lift,
 * +0x1c clears and the next move is 1. */

#include "nitro/fx_types.h"

extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);

void Ov146_Launch(int *state, VecFx32 dir)
{
    VEC_Normalize(&dir, (VecFx32 *)(state + 3));
    ScaleVec3Fx12(0x800, (VecFx32 *)(state + 3), (VecFx32 *)(state + 3));
    state[4] = 0xa00;
    state[7] = 0;
    *(unsigned char *)(*state + 0x1c7) = 1;
}
