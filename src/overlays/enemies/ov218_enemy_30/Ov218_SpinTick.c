/* Spin tick of the ov218 actor: the +0xc heading turns toward the +0x10 goal at four times the frame
 * rate (0203d040); the actor's pose becomes the rotation from up to its +0x124 normal combined with the
 * heading about up, its +0xf0 velocity mirrors +0x28, and for the frame (in 0x88-sized slices) the
 * velocity damps by 0.12 and the +0x1c spin by 0.125 per slice. */

#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct { int x, y, z, w; } Quat;

extern void Quat_FromTwoVectors(Quat *out, const VecFx32 *from, const VecFx32 *to);
extern void Quat_Multiply(Quat *out, const Quat *a, const Quat *b);
extern void Srt_SetRotationQuat(void *srt, const Quat *rot);
extern int FX_Div(int num, int den);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern const VecFx32 data_02042264;

#define FX_MUL(a, b) ((int)(((long long)(a) * (b) + 0x800) >> 12))

void Ov218_SpinTick(int *node)
{
    int *state = (int *)node[1];
    Quat spin;
    Quat tilt;
    int remaining;

    state[3] = Angle_TurnToward(state[3], state[4], *(int *)(node[0] + 0x2c) * 4, 0);
    QuatFromAxisAngle(&spin, &data_02042264, state[3]);
    Quat_FromTwoVectors(&tilt, &data_02042264, (VecFx32 *)(*state + 0x124));
    Quat_Multiply(&tilt, &tilt, &spin);
    Srt_SetRotationQuat((void *)(*state + 0xa0), &tilt);
    *(VecFx32 *)(*state + 0xf0) = *(VecFx32 *)(state + 0xa);
    for (remaining = *(int *)(node[0] + 0x2c); remaining > 0; remaining -= 0x88) {
        ScaleVec3Fx12(0x1000 - FX_MUL(FX_Div(remaining <= 0x88 ? remaining : 0x88, 0x88), 0x1f0),
                      (VecFx32 *)(state + 0xa), (VecFx32 *)(state + 0xa));
        state[7] = FX_MUL(state[7], 0x1000 - FX_MUL(FX_Div(remaining <= 0x88 ? remaining : 0x88, 0x88), 0x200));
    }
}
