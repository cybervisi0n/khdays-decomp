/* Watch slot of the ov259 actor (every frame): the +0x78 heading turns toward +0x7c by 90 x the
 * frame rate over the +0x94 turn time, the +0xa0 pose becomes that heading composed with the ground
 * normal (+0x124), the +0x8c clock runs while +0x4c is clear, +0xf0 keeps the last +0x14 drift and
 * the drift decays by +0x88, the physics step runs (020ce63c) and a pending +0x90 delay fires sound
 * 0x172 with the +0xa4 variant at the +0x10 point when it runs out; then the base tick (020cd648). */

#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct { int x, y, z, w; } Quat;

extern int func_02020400(int num, int den);
extern void Quat_FromTwoVectors(Quat *out, const VecFx32 *from, const VecFx32 *to);
extern void Quat_Multiply(Quat *out, const Quat *a, const Quat *b);
extern void Srt_SetRotationQuat(char *srt, Quat *q);
extern void ScaleVec3Fx12(int scale, VecFx32 *v, VecFx32 *d);
extern void Ov259_HitWindowStep(int *node);
extern void Ov259_PlaySound(int actor, int id, int variant, void *at);
extern void Ov259_SoundCueTick(int *node);
extern const VecFx32 data_02042264;

void Ov259_WatchSlot(int *node)
{
    int *state = (int *)node[1];
    Quat turn;
    Quat pose;

    state[0x1e] = Angle_TurnToward(state[0x1e], state[0x1f],
                                func_02020400(*(int *)(node[0] + 0x2c) * 0x5a, state[0x25]), 0);
    QuatFromAxisAngle(&turn, &data_02042264, state[0x1e]);
    Quat_FromTwoVectors(&pose, &data_02042264, (VecFx32 *)(*state + 0x124));
    Quat_Multiply(&pose, &pose, &turn);
    Srt_SetRotationQuat((char *)(*state + 0xa0), &pose);
    if (state[0x13] == 0) {
        state[0x23] += *(int *)(node[0] + 0x2c);
    }
    {
        VecFx32 *drift = (VecFx32 *)(state + 5);
        *(VecFx32 *)(*state + 0xf0) = *drift;
        ScaleVec3Fx12(state[0x22], drift, drift);
    }
    Ov259_HitWindowStep(node);
    if (state[0x24] > 0) {
        state[0x24] -= *(int *)(node[0] + 0x2c);
        if (state[0x24] <= 0) {
            Ov259_PlaySound(*state, 0x172, (unsigned short)state[0x29], (void *)state[4]);
        }
    }
    Ov259_SoundCueTick(node);
}
