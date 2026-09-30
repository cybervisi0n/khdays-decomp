/* Orientation tick of the ov123 enemy (and its byte-identical twin): unless bit 1 of the actor's
 * +0x1c4 flags is set, the +0x18 yaw is stepped towards the +0x1c target yaw by the +0x20 rate;
 * the yaw's rotation about world Y is composed with the orientation that tilts world Y onto the
 * actor's +0x124 normal and written to the +0xa0 quaternion; the +4 offset is handed to the
 * actor's +0xf0 vector and reset to the zero vector. */

#include "nitro/fx_types.h"
#include "game/engine.h"

extern void Quat_FromTwoVectors(void *dst, void *src, int m);
extern void Quat_Multiply(void *dst, void *a, void *b);
extern void Srt_SetRotationQuat(int a, void *b);

extern VecFx32 data_02042264;
extern VecFx32 data_02041dc8;

void Ov124_OrientationTick(int *ctx) {
    int a[4];
    int b[4];
    int *s = (int *)ctx[1];
    if ((*(unsigned char *)(s[0] + 0x1c4) & 2) == 0) {
        s[6] = Angle_TurnToward(s[6], s[7], s[8], 0);
    }
    QuatFromAxisAngle(b, &data_02042264, s[6]);
    Quat_FromTwoVectors(a, &data_02042264, s[0] + 0x124);
    Quat_Multiply(a, a, b);
    Srt_SetRotationQuat(s[0] + 0xa0, a);
    {
        VecFx32 *q = (VecFx32 *)((char *)s + 0x4);
        *(VecFx32 *)(s[0] + 0xf0) = *q;
        *q = data_02041dc8;
    }
}
