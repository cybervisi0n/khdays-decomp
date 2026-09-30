/* Hover motion tick of the ov261 enemy (and its byte-identical twin): slerps the +0xc
 * orientation towards the +0x1c anchor by the +0x3c rate and applies it to the actor's +0xa0
 * rotation; while bit 0 of the +0x60 flags is set the +0x44 phase accumulates the frame-time and
 * the +0x34 lift bobs by sin(phase * 8) / 20. The +0x30 velocity is written to the actor's +0xf0
 * and then cleared. */

#include "nitro/fx_types.h"

struct hw60 { unsigned short lo : 8, hi : 8; };

extern void Quat_Slerp(void *dst, int t, void *a, void *b);
extern void Srt_SetRotationQuat(void *transform, void *quat);
extern const short data_0203d210[];
extern const VecFx32 data_02041dc8;

void Ov262_HoverMotionTick(int *node)
{
    int *state = (int *)node[1];
    int idx;

    Quat_Slerp(state + 3, state[0xf], state + 3, state + 7);
    Srt_SetRotationQuat((void *)(*state + 0xa0), state + 3);
    if (((struct hw60 *)(*state + 0x60))->lo & 1) {
        state[0x11] += *(int *)(*node + 0x2c);
        idx = (unsigned short)((0x28BE60DB9391LL * (state[0x11] * 8) + 0x80000000000LL) >> 44);   /* FX_RAD_TO_IDX */
        state[0xd] += data_0203d210[(idx >> 4) << 1] / 20;                                        /* FX_SinIdx */
    }
    {
        int actor = *state;
        state = (int *)((char *)state + 0x30);
        *(VecFx32 *)(actor + 0xf0) = *(VecFx32 *)state;
        *(VecFx32 *)state = data_02041dc8;
    }
}
