/* Watch slot of the ov260 actor (every frame): the +0x64 heading turns toward +0x68 by 2.5 x the
 * frame rate, the +0xa0 pose follows it, +0xf0 keeps the last +0x20 velocity which then clears, and
 * in move 4 the +0x60 idle time runs down to 0. */

#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct { int x, y, z, w; } Quat;

extern void Srt_SetRotationQuat(char *srt, Quat *q);
extern const VecFx32 data_02042264;
extern const VecFx32 data_02041dc8;

#define FX_MUL(a, b) ((int)(((long long)(a) * (b) + 0x800) >> 12))

void Ov260_WatchSlot(int *node)
{
    int *state = (int *)node[1];
    Quat q;

    state[0x19] = Angle_TurnToward(state[0x19], state[0x1a], FX_MUL(*(int *)(node[0] + 0x2c), 0x2800), 0);
    QuatFromAxisAngle(&q, &data_02042264, state[0x19]);
    Srt_SetRotationQuat((char *)(*state + 0xa0), &q);
    {
        VecFx32 *vel = (VecFx32 *)(state + 8);

        *(VecFx32 *)(*state + 0xf0) = *vel;
        *vel = data_02041dc8;
    }
    if (*(signed char *)(*state + 0x1c6) != 4) {
        return;
    }
    if ((state[0x18] -= *(int *)(node[0] + 0x2c)) < 0) {
        state[0x18] = 0;
    }
}
