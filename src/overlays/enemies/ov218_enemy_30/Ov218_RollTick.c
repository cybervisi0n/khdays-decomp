/* Roll tick of the ov218 actor (move 1 only): it faces along its ground-plane +0x28 motion (the
 * rotation from data_02042258 to it, 0202ed60), its +0xf0 velocity mirrors +0x10, the horizontal
 * velocity damps to 0.906 on the ground (0.969 in the air) and gravity pulls 0.94 per frame. On the
 * ground a slow roll (under 0.0625) stops; otherwise the +0x38 bounce speed damps by 0.094 per 0x88
 * slice and becomes the vertical velocity. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { int x, y, z, w; } Quat;
typedef struct { u8 b0 : 1; } Bit0;

extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern void Quat_FromTwoVectors(Quat *out, const VecFx32 *from, const VecFx32 *to);
extern void Srt_SetRotationQuat(void *srt, const Quat *rot);
extern int FX_Div(int num, int den);
extern const VecFx32 data_02042258;
extern const VecFx32 data_02041dc8;

#define FX_MUL(a, b) ((int)(((long long)(a) * (b) + 0x800) >> 12))

void Ov218_RollTick(int *node)
{
    int *state = (int *)node[1];
    Quat q;
    VecFx32 dir;
    int grounded;
    int damp;
    int remaining;

    if (*(signed char *)(*state + 0x1c6) != 1) {
        return;
    }
    grounded = ((Bit0 *)(*state + 0x17a))->b0;
    damp = grounded ? 0xe80 : 0xf80;
    dir = *(VecFx32 *)(state + 0xa);
    dir.y = 0;
    if (VEC_Normalize(&dir, &dir) == 0) {
        dir = data_02042258;
    }
    Quat_FromTwoVectors(&q, &data_02042258, &dir);
    Srt_SetRotationQuat((void *)(*state + 0xa0), &q);
    *(VecFx32 *)(*state + 0xf0) = *(VecFx32 *)(state + 4);
    state[4] = FX_MUL(state[4], damp);
    state[6] = FX_MUL(state[6], damp);
    state[5] += -(*(int *)(node[0] + 0x2c) << 7) / 0x88;
    if (!grounded) {
        return;
    }
    if (VEC_Normalize((VecFx32 *)(state + 4), &dir) < 0x100) {
        *(VecFx32 *)(state + 4) = data_02041dc8;
        return;
    }
    for (remaining = *(int *)(node[0] + 0x2c); remaining > 0; remaining -= 0x88) {
        state[0xe] = FX_MUL(state[0xe], 0x1000 - FX_MUL(FX_Div(remaining <= 0x88 ? remaining : 0x88, 0x88), 0x180));
    }
    state[5] = state[0xe];
}
