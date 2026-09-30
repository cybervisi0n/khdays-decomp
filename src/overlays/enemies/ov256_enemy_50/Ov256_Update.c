/* Update of the ov256 actor (+0x30): the +0x40 heading turns towards +0x44 at three times the
 * frame rate, is mirrored to +0x458 and orients the +0xa0 pose; the +0x10 velocity goes to +0xf0
 * and then halves. Outside move 3 the leash is checked: beyond 19.0 (21.0 when +0x6b is 2) from
 * the arena anchor (-0.25, 3.25, -1.5) +0x6b becomes 4, below height 12.0 of the +0xc track 0,
 * above 19.0 it becomes 1, each setting the +0x74 request. Past x 12.0 +0x78 / +0x7c are set.
 * The +0x48 timer advances and, in moves 2-4, the +0x50 countdown runs down to 0. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct { int x, y, z, w; } Quat;

extern void Srt_SetRotationQuat(int a, void *b);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int VEC_Normalize(VecFx32 *v, VecFx32 *out);
extern void ScaleVec3Fx12(int scale, VecFx32 *v, VecFx32 *d);
extern const VecFx32 data_02042264;
extern const VecFx32 data_ov256_020d2594;

void Ov256_Update(int *node)
{
    int *state = (int *)node[1];
    Quat q;
    VecFx32 vel = *(VecFx32 *)(state + 4);
    VecFx32 anchor = data_ov256_020d2594;
    VecFx32 d;
    int dist;
    int limit = 0x13000;

    state[0x10] = Angle_TurnToward(state[0x10], state[0x11], *(int *)(node[0] + 0x2c) * 3, 0);
    *(int *)(*state + 0x458) = state[0x10];
    QuatFromAxisAngle(&q, &data_02042264, state[0x10]);
    Srt_SetRotationQuat(*state + 0xa0, &q);
    VEC_Subtract(&anchor, (VecFx32 *)(*state + 0xb0), &d);
    dist = VEC_Normalize(&d, &d);
    if (*((u8 *)state + 0x6b) == 2) {
        limit += 0x2000;
    }
    *(VecFx32 *)(*state + 0xf0) = vel;
    ScaleVec3Fx12(0x800, (VecFx32 *)(state + 4), (VecFx32 *)(state + 4));
    if (*(signed char *)(*state + 0x100 + 0xc6) != 3) {
        if (dist > limit) {
            *((u8 *)state + 0x6b) = 4;
            state[0x1d] = 1;
        }
        if (((VecFx32 *)state[3])->y < 0xc000) {
            *((u8 *)state + 0x6b) = 0;
            state[0x1d] = 1;
        }
        if (((VecFx32 *)state[3])->y > 0x13000) {
            *((u8 *)state + 0x6b) = 1;
            state[0x1d] = 1;
        }
    }
    if (((VecFx32 *)state[3])->x > 0xc000) {
        state[0x1e] = 1;
        state[0x1f] = 1;
    }
    state[0x12] += *(int *)(node[0] + 0x2c);
    if (!(*(signed char *)(*state + 0x100 + 0xc6) != 2 && *(signed char *)(*state + 0x100 + 0xc6) != 3 &&
          *(signed char *)(*state + 0x100 + 0xc6) != 4)) {
        state[0x14] -= *(int *)(node[0] + 0x2c);
        if (state[0x14] < 0) {
            state[0x14] = 0;
        }
    }
}
