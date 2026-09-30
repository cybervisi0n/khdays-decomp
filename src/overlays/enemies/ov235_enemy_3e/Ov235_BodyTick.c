/* Body tick of the ov235 enemy (slot 2): the steering step runs (Ov235_StepAttackCues with the
 * rate), the +0x1c orientation eases towards +0x2c by the +0x40 rate and orients the owner's +0xa0
 * pose, the +0x50 clock runs and a non-negative +0x4c cooldown counts down. Within 5.0 of the
 * origin the +0x10 step is pushed outwards by the missing distance; the step then becomes the
 * owner's +0xf0 velocity and resets. */

#include "nitro/fx_types.h"

typedef struct { int w[4]; } Quat;

extern void Ov235_StepAttackCues(int *state, int rate);
extern void Quat_Slerp(Quat *dst, int t, const Quat *from, const Quat *to);
extern void Srt_SetRotationQuat(void *srt, const Quat *q);
extern void VEC_Subtract(const void *a, const void *b, VecFx32 *out);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern const VecFx32 data_02041dc8;

void Ov235_BodyTick(int *node)
{
    int *state = (int *)node[1];
    VecFx32 d;
    int len;

    Ov235_StepAttackCues(state, *(int *)(node[0] + 0x2c));
    Quat_Slerp((Quat *)(state + 7), state[0x10], (Quat *)(state + 7), (Quat *)(state + 0xb));
    Srt_SetRotationQuat((void *)(*state + 0xa0), (Quat *)(state + 7));
    state[0x14] += *(int *)(node[0] + 0x2c);
    if (state[0x13] >= 0) {
        state[0x13] -= *(int *)(node[0] + 0x2c);
    }
    VEC_Subtract((void *)state[1], &data_02041dc8, &d);
    len = VEC_Normalize(&d, &d);
    if (len < 0x5000) {
        ScaleVec3Fx12(0x5000 - len, &d, &d);
        VEC_Add((VecFx32 *)(state + 4), &d, (VecFx32 *)(state + 4));
    }
    {
        VecFx32 *step = (VecFx32 *)(state + 4);

        *(VecFx32 *)(*state + 0xf0) = *step;
        *step = data_02041dc8;
    }
}
