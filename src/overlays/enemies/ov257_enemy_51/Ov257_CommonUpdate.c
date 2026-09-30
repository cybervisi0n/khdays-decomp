/* Common update of the ov257 states (slot 0): the attack cues advance (Ov257_StepAttackCues), the
 * +0x1c orientation turns towards +0x2c by the +0x40 rate and is applied to the owner's +0xa0
 * pose, the +0x58 timer accumulates the frame rate and the +0x54 cooldown counts down while not
 * negative; the +0x10 step is handed to the owner's +0xf0 velocity and cleared. */

#include "nitro/fx_types.h"

typedef struct { int w[4]; } Quat;

extern void Ov257_StepAttackCues(int *state, int rate);
extern void Quat_Slerp(Quat *dst, int t, const Quat *from, const Quat *to);
extern void Srt_SetRotationQuat(void *srt, const Quat *q);
extern const VecFx32 data_02041dc8;

void Ov257_CommonUpdate(int *node)
{
    int *state = (int *)node[1];

    Ov257_StepAttackCues(state, *(int *)(node[0] + 0x2c));
    Quat_Slerp((Quat *)(state + 7), state[0x10], (Quat *)(state + 7), (Quat *)(state + 0xb));
    Srt_SetRotationQuat((void *)(*state + 0xa0), (Quat *)(state + 7));
    state[0x14] += *(int *)(node[0] + 0x2c);
    if (state[0x13] >= 0) {
        state[0x13] -= *(int *)(node[0] + 0x2c);
    }
    {
        VecFx32 *step = (VecFx32 *)(state + 4);

        *(VecFx32 *)(*state + 0xf0) = *step;
        *step = data_02041dc8;
    }
}
