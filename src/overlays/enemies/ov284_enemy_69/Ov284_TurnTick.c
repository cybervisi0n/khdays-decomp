/* Turn tick of the ov284 enemy: the +0x18 rate is 30 x dt / 15, the +0x20 timer counts the
 * frame-time down while positive, the +0x10 yaw steps towards the +0x14 target yaw by the rate
 * and the actor's +0xa0 orientation is rebuilt from the yaw about world Y. */

#include "nitro/fx_types.h"

extern int Angle_TurnToward(int cur, int want, int step, int *state);
extern void Srt_SetRotationAxisAngle(void *quat, const VecFx32 *axis, int angle);
extern const VecFx32 data_02042264;

void Ov284_TurnTick(int *self) {
    int *ctx = (int *)self[1];
    ctx[6] = *(int *)(self[0] + 0x2c) * 30 / 15;
    if (ctx[8] > 0) {
        ctx[8] = ctx[8] - *(int *)(self[0] + 0x2c);
    }
    ctx[4] = Angle_TurnToward(ctx[4], ctx[5], ctx[6], 0);
    Srt_SetRotationAxisAngle((void *)(ctx[0] + 0xa0), &data_02042264, ctx[4]);
}
