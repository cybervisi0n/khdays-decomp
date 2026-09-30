/* Ride step: while bit 0 of the actor's +0x3c0 is set its +0xa0 pose is copied from the +0x394
 * mount; otherwise the +0x2c yaw steps towards the +0x30 target by the +0x28 rate and the pose
 * is rebuilt about world Y. Outside kind 0xc an emptied +0x21a health requests pose 0xc. The +8
 * velocity is handed to the actor's +0xf0 motion slot and cleared. */

#include "nitro/fx_types.h"

struct blk11 { int w[11]; };
struct Bits3c0 { int b0 : 1; };
struct Ov236ActorPose { char pad[0xa0]; struct blk11 pose; };
extern int Angle_TurnToward(int cur, int want, int step, int *state);
extern void Srt_SetRotationAxisAngle(void *quat, const VecFx32 *axis, int angle);
extern const VecFx32 data_02042264;
extern const VecFx32 data_02041dc8;

void Ov278_RideStepA(int *self) {
    int *ctx = (int *)self[1];
    int actor = ctx[0];
    if (((struct Bits3c0 *)(actor + 0x3c0))->b0 != 0) {
        ((struct Ov236ActorPose *)actor)->pose = ((struct Ov236ActorPose *)*(int *)(actor + 0x394))->pose;
    } else {
        ctx[0xb] = Angle_TurnToward(ctx[0xb], ctx[0xc], ctx[0xa], 0);
        Srt_SetRotationAxisAngle((void *)(ctx[0] + 0xa0), &data_02042264, ctx[0xb]);
    }
    actor = ctx[0];
    if (*(signed char *)(actor + 0x100 + 0xc6) != 0xc) {
        if (*(short *)(actor + 0x200 + 0x1a) == 0) {
            *(unsigned char *)(actor + 0x1c7) = 0xc;
        }
    }
    {
        VecFx32 *p8 = (VecFx32 *)(ctx + 2);
        *(VecFx32 *)(ctx[0] + 0xf0) = *p8;
        *p8 = data_02041dc8;
    }
}
