/* Facing step of an ov256 part: in move 1 the +0xa0 pose follows the +0x18 heading; +0xf0 keeps the
 * last +0xc velocity, which then clears. */

#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct { int x, y, z, w; } Quat;

extern void Srt_SetRotationQuat(char *srt, Quat *q);
extern const VecFx32 data_02042264;
extern const VecFx32 data_02041dc8;

void Ov256_FacingStep(int *node)
{
    int *state = (int *)node[1];
    Quat q;

    if (*(signed char *)(*state + 0x1c6) == 1) {
        QuatFromAxisAngle(&q, &data_02042264, state[6]);
        Srt_SetRotationQuat((char *)(*state + 0xa0), &q);
    }
    {
        VecFx32 *vel = (VecFx32 *)(state + 3);

        *(VecFx32 *)(*state + 0xf0) = *vel;
        *vel = data_02041dc8;
    }
}
