/* Ov240_OrientFromYaw: ported from a matched sibling family (same shape, constants and offsets adjusted). */

#include "nitro/fx_types.h"
#include "game/engine.h"

extern void Quat_FromTwoVectors(void *dst, void *src, int m);
extern void Quat_Multiply(void *dst, void *a, void *b);
extern void Srt_SetRotationQuat(int a, void *b);

extern VecFx32 data_02042264;
extern VecFx32 data_02041dc8;

void Ov240_OrientFromYaw(int *ctx) {
    int b[4];
    int a[4];
    int *p = (int *)ctx[0];
    int *s = (int *)ctx[1];
    s[3] = Angle_TurnToward(s[3], s[4], p[0xb] * 3, 0);
    QuatFromAxisAngle(b, &data_02042264, s[3]);
    Quat_FromTwoVectors(a, &data_02042264, s[0] + 0x124);
    Quat_Multiply(a, a, b);
    Srt_SetRotationQuat(s[0] + 0xa0, a);
    {
        VecFx32 *q = (VecFx32 *)((char *)s + 0x14);
        *(VecFx32 *)(s[0] + 0xf0) = *q;
        *q = data_02041dc8;
    }
}
