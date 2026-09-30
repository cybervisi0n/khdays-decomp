/* Hover bob of the ov151 enemy (and its byte-identical twin): the +0x34 phase advances 30 per
 * frame-time unit (wrapping at 0xb4000) and its sine (phase x 0x6488 / 180 as an angle) over 32
 * lifts the +0x10 height; the +0xc offset is handed to the actor's +0xf0 vector and reset to
 * the zero vector. */

#include "nitro/fx_types.h"

extern const short data_0203d210[];
extern const VecFx32 data_02041dc8;

#define FX_MUL(a, b) ((int)(((long long)(a) * (b) + 0x800) >> 12))

void Ov152_HoverBob(int *node)
{
    int *state = (int *)node[1];
    int rad;
    int idx;

    state[0xd] += *(int *)(*node + 0x2c) * 30;
    if (state[0xd] >= 0xb4000) {
        state[0xd] = 0;
    }
    rad = FX_MUL(state[0xd], 0x6488) / 180;
    idx = (unsigned short)((0x28BE60DB9391LL * rad + 0x80000000000LL) >> 44);   /* FX_RAD_TO_IDX */
    state[4] += data_0203d210[(idx >> 4) << 1] >> 5;                              /* FX_SinIdx */
    {
        VecFx32 *off = (VecFx32 *)(state + 3);
        *(VecFx32 *)(*state + 0xf0) = *off;
        *off = data_02041dc8;
    }
}
