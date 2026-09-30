/* Turn tick of the ov144 enemy (and its byte-identical twin): the +0x30 yaw turns toward the
 * +0x34 target yaw by the frame-time (doubled in sub-states 5-7, frozen in sub-state 8), the
 * actor's +0xa0 transform takes the yaw as a quaternion about the up axis, the +0x24 velocity
 * moves to the actor's +0xf0 and is zeroed; in sub-states 2/3 with a non-negative +0x3c0 table
 * entry the +0x3c timer counts the frame-time down and, once spent, the sub-state is remembered
 * in +0x50 and sub-state 8 is requested. */

#include "nitro/fx_types.h"
#include "game/engine.h"

extern void Srt_SetRotationQuat(void *srt, int *quat);
extern const VecFx32 data_02041dc8;
extern const VecFx32 data_02042264;

#define FX_MUL(a, b) ((int)(((long long)(a) * (b) + 0x800) >> 12))

void Ov144_OrientationTick(int *node)
{
    int *state = (int *)node[1];
    int quat[4];
    VecFx32 *vel;

    switch (*(signed char *)(*state + 0x1c6)) {
    case 5:
    case 6:
    case 7:
        state[0xc] = Angle_TurnToward(state[0xc], state[0xd], FX_MUL(*(int *)(*node + 0x2c), 0x2000), 0);
        break;
    case 8:
        break;
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    default:
        state[0xc] = Angle_TurnToward(state[0xc], state[0xd], FX_MUL(*(int *)(*node + 0x2c), 0x1000), 0);
        break;
    }
    QuatFromAxisAngle(quat, &data_02042264, state[0xc]);
    Srt_SetRotationQuat((char *)*state + 0xa0, quat);
    vel = (VecFx32 *)(state + 9);
    *(VecFx32 *)(*state + 0xf0) = *vel;
    *vel = data_02041dc8;
    if (*(signed char *)(*state + 0x1c6) != 2 && *(signed char *)(*state + 0x1c6) != 3) {
        return;
    }
    if (*(int *)(*state + state[0x12] * 4 + 0x3c0) < 0) {
        return;
    }
    state[0xf] -= *(int *)(*node + 0x2c);
    if (state[0xf] > 0) {
        return;
    }
    *(unsigned char *)(state + 0x14) = *(signed char *)(*state + 0x1c6);
    *(unsigned char *)(*state + 0x1c7) = 8;
}
