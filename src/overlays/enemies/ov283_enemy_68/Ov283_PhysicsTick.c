/* Physics tick of the ov283 actor: the +0x38 heading snaps to the +0x40 goal while +0x70 is set, else
 * turns toward it at 9x the frame rate, and orients the pose about up; the +0x4c and +0x50 timers run
 * down (to 0), the +0x3c tilt eases back toward 0 by 0.066, the +0xf0 velocity mirrors +0x10, which
 * is scaled by +0x5c; both +0x394/+0x39c limb pairs update (020ced80) and a pending +0x60 timer runs
 * down. */

#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct { int x, y, z, w; } Quat;
struct Ov283Limbs { char pad[0x394]; int bones[2]; int parts[2]; };

extern void Srt_SetRotationQuat(void *srt, const Quat *rot);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern int Ov283_ForwardToAiTaskWhenReady(int part, int bone);
extern const VecFx32 data_02042264;

void Ov283_PhysicsTick(int *node)
{
    int *state = (int *)node[1];
    Quat q;
    int rate = *(int *)(node[0] + 0x2c) * 0x5a / 10;
    int i;

    if (state[0x1c] != 0) {
        state[0xe] = state[0x10];
    } else {
        state[0xe] = Angle_TurnToward(state[0xe], state[0x10], rate, 0);
    }
    QuatFromAxisAngle(&q, &data_02042264, state[0xe]);
    Srt_SetRotationQuat((void *)(*state + 0xa0), &q);
    state[0x13] -= *(int *)(node[0] + 0x2c);
    if (state[0x13] <= 0) {
        state[0x13] = 0;
    }
    state[0x14] -= *(int *)(node[0] + 0x2c);
    if (state[0x14] <= 0) {
        state[0x14] = 0;
    }
    if (state[0xf] >= 0x110) {
        state[0xf] -= 0x110;
    } else if (state[0xf] <= -0x110) {
        state[0xf] += 0x110;
    }
    {
        VecFx32 *vel = (VecFx32 *)(state + 4);

        *(VecFx32 *)(*state + 0xf0) = *vel;
        ScaleVec3Fx12(state[0x17], vel, vel);
    }
    for (i = 0; i < 2; i++) {
        Ov283_ForwardToAiTaskWhenReady(((struct Ov283Limbs *)*state)->parts[i], ((struct Ov283Limbs *)*state)->bones[i]);
    }
    if (state[0x18] <= 0) {
        return;
    }
    state[0x18] -= *(int *)(node[0] + 0x2c);
}
