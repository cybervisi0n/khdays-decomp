/* Commits the trail node's orientation: turns the heading toward its target unless the actor is
 * locked, combines it with the ground-normal rotation into the model's SRT, speeds up while
 * flagged, and hands the step velocity to the actor (+0xf0), clearing it. */

#include "nitro/fx_types.h"
#include "game/engine.h"

extern void Quat_FromTwoVectors(void *dst, void *src, int m);
extern void Quat_Multiply(void *dst, void *a, void *b);
extern void Srt_SetRotationQuat(int a, void *b);

extern VecFx32 data_02042264;
extern VecFx32 data_02041dc8;

void Ov197_CommitTrailTransform(int *self) {
    int b[4];
    int a[4];
    int *s = (int *)self[1];
    if ((*(unsigned char *)(*s + 0x1c4) & 2) == 0) {
        s[13] = Angle_TurnToward(s[13], s[14], s[15], 0);
    }
    QuatFromAxisAngle(b, &data_02042264, s[13]);
    Quat_FromTwoVectors(a, &data_02042264, *s + 0x124);
    Quat_Multiply(a, a, b);
    Srt_SetRotationQuat(*s + 0xa0, a);
    if (*(signed char *)(*s + 0x100 + 0xc6) != 0) {
        s[5] += (0x1800 - *(int *)(*s + 0x13c)) / 30;
    }
    {
        int owner = *s;
        s = (int *)((char *)s + 0x10);
        *(VecFx32 *)(owner + 0xf0) = *(VecFx32 *)s;
        *(VecFx32 *)s = data_02041dc8;
    }
}
