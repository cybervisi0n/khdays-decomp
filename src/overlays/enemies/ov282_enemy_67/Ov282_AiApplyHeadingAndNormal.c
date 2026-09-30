/* Turns toward the heading at the turn rate, composes it with the surface-normal tilt and applies
 * it. */

#include "nitro/fx_types.h"
#include "game/engine.h"

struct T { int a, b, c, d; };
struct S { struct T t; char pad[0x28 - 16]; unsigned char flag; };

extern void Quat_FromTwoVectors(void *dst, void *axis, int src);
extern void Quat_Multiply(void *a, void *b, void *c);
extern int Srt_SetRotationQuat(struct S *dst, struct S *src);
extern int VEC_Mag(const VecFx32 *v);
extern int VEC_Normalize(const VecFx32 *source, VecFx32 *destination);
extern void ScaleVec3Fx12(int factor, int *src, int *dst);
extern void INITi_CpuClear32_0x01ff86fc(int value, void *dst, int size);
extern int data_02042264[3];

void Ov282_AiApplyHeadingAndNormal(int *self) {
    int *state = (int *)self[1];
    int quat[4];
    struct T tmp;
    int newAngle;
    VecFx32 *v;

    newAngle = Angle_TurnToward(state[9], state[0xa],
        (int)((((long long)*(int *)(self[0] + 0x2c) * state[0x14]) + 0x800) >> 12), 0);
    QuatFromAxisAngle(quat, data_02042264, state[9] = newAngle);
    Quat_FromTwoVectors(&tmp, data_02042264, *state + 0x124);
    Quat_Multiply(&tmp, &tmp, quat);
    Srt_SetRotationQuat((struct S *)(*state + 0xa0), (struct S *)&tmp);
    if (VEC_Mag((VecFx32 *)(state + 5)) > 0x2000) {
        VEC_Normalize((VecFx32 *)(state + 5), (VecFx32 *)(state + 5));
        ScaleVec3Fx12(0x2000, (int *)(state + 5), (int *)(state + 5));
    }
    v = (VecFx32 *)(state + 5);
    *(VecFx32 *)(*state + 0xf0) = *v;
    INITi_CpuClear32_0x01ff86fc(0, v, 0xc);
    if (state[0x1b] > 0) {
        state[0x1b] -= *(int *)(self[0] + 0x2c);
    }
}
