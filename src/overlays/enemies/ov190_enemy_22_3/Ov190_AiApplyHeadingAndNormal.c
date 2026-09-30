/* Turns toward the heading (3x rate), composes it with the surface-normal tilt and applies it. */

#include "nitro/fx_types.h"
#include "game/engine.h"

extern void Quat_FromTwoVectors(void *dst, void *src, int m);
extern void Quat_Multiply(void *dst, void *a, void *b);
extern void Srt_SetRotationQuat(int a, void *b);

extern VecFx32 data_02042264;
extern VecFx32 data_02041dc8;

void Ov190_AiApplyHeadingAndNormal(int *ctx) {
    int b[4];
    int a[4];
    int *p = (int *)ctx[0];
    int *s = (int *)ctx[1];
    s[4] = Angle_TurnToward(s[4], s[5], p[0xb] * 3, 0);
    QuatFromAxisAngle(b, &data_02042264, s[4]);
    Quat_FromTwoVectors(a, &data_02042264, s[0] + 0x124);
    Quat_Multiply(a, a, b);
    Srt_SetRotationQuat(s[0] + 0xa0, a);
    {
        VecFx32 *q = (VecFx32 *)((char *)s + 0x20);
        *(VecFx32 *)(s[0] + 0xf0) = *q;
        *q = data_02041dc8;
    }
}
