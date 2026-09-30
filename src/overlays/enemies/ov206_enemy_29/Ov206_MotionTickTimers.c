/* Ov206_MotionTickTimers -- x4 (ov206/...). Advance the tracked motion, tick down four timers, nudge on
 * the active side, and publish to the owner. flag = (current scene 0206b84c() == 0x12e). Step the
 * value at +0x40 via 0203d040, turn it into a delta against data_02042264 (0202f188) and write to the
 * owner at +0xa0 (0203c9d0). Decrement each of the four bytes at +0x53..+0x56 that are still non-zero.
 * If flag: when *(ctx[1]+8) > 0x1a000 subtract 0x200 from ctx[7]; when < -0x1a000 add 0x200. Publish
 * the cached offset at +0x14 to the owner at +0xf0 and reset it to the neutral constant.
 * (Same shape as Ov208_MotionTickTimers; different field offsets.)
 */

#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct { int v[4]; } Xform;

extern int Ov002_GetStateWord(void);
extern void Srt_SetRotationQuat(int dst, const Xform *x);
extern Xform data_02042264;
extern VecFx32 data_02041dc8;

void Ov206_MotionTickTimers(int self, int p2, int p3, int p4) {
    int *ctx = *(int **)(self + 4);
    unsigned char *cb = (unsigned char *)ctx;
    int flag = (Ov002_GetStateWord() == 0x12e);
    Xform a;
    int i;
    int owner;

    ctx[0x10] = Angle_TurnToward(ctx[0x10], ctx[0x11], ctx[0xf], 0);
    QuatFromAxisAngle(&a, &data_02042264, ctx[0x10]);
    Srt_SetRotationQuat(ctx[0] + 0xa0, &a);

    for (i = 0; i < 4; i++) {
        unsigned char c = cb[i + 0x53];
        if (c != 0) {
            cb[i + 0x53] = c - 1;
        }
    }

    if (flag != 0) {
        int t = *(int *)(ctx[1] + 8);
        if (t > 0x1a000) {
            ctx[7] = ctx[7] - 0x200;
        } else if (t < -0x1a000) {
            ctx[7] = ctx[7] + 0x200;
        }
    }

    owner = *ctx;
    ctx = (int *)((char *)ctx + 0x14);
    *(VecFx32 *)(owner + 0xf0) = *(VecFx32 *)ctx;
    *(VecFx32 *)ctx = data_02041dc8;
}
