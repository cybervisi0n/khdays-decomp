/* Charge pose tick of the ov223 enemy: the +0x14 velocity is copied to the owner's +0xf0 and,
 * in variants 0 and 1, the owner's +0xa0 pose is scaled by 2.0 x +0x40 (y by the inverse of
 * 0x733 over 1.0) and set to face the +0x20 target from data_02042240 (ed60). */

#include "nitro/fx_types.h"

typedef struct { int q[4]; } Quat;

#define FX_MUL(a, b) ((int)(((long long)(a) * (b) + 0x800) >> 12))

extern int FX_Div(int num, int den);
extern void Srt_SetScaleXYZ(void *pose, int x, int y, int z);
extern void Quat_FromTwoVectors(Quat *out, const VecFx32 *from, const VecFx32 *to);
extern void Srt_SetRotationQuat(void *pose, Quat *q);
extern const VecFx32 data_02042240;

void Ov223_ChargePoseTick(int *node)
{
    int *state = (int *)node[1];
    Quat q;
    int nScale;
    char *owner;

    *(VecFx32 *)(*state + 0xf0) = *(VecFx32 *)(state + 5);
    if (state[0x12] != 0 && state[0x12] != 1) {
        return;
    }
    nScale = FX_MUL(state[0x10], 0x2000);
    owner = (char *)*state;
    Srt_SetScaleXYZ(owner + 0xa0, nScale, FX_Div(0x10000, 0x733), nScale);
    Quat_FromTwoVectors(&q, &data_02042240, (VecFx32 *)(state + 8));
    Srt_SetRotationQuat((void *)(*state + 0xa0), &q);
}
