/* Physics tick of the ov238 actor: with a target (+0x390) its pose is the rotation from up to its
 * +0x124 normal combined with the +0x24 spin about the +0x40 axis; landed in move 2 the ground normal is
 * copied to +0x30. Its +0xf0 velocity takes the +0xc velocity, which then rests. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct { int x, y, z, w; } Quat;
typedef struct { u8 b0 : 1; } Bit0;

extern void Quat_FromTwoVectors(Quat *out, const VecFx32 *from, const VecFx32 *to);
extern void Quat_Multiply(Quat *out, const Quat *a, const Quat *b);
extern void Srt_SetRotationQuat(void *srt, const Quat *rot);
extern const VecFx32 data_02042264;
extern const VecFx32 data_02041dc8;

void Ov238_PhysicsTick(int *node)
{
    int *state = (int *)node[1];
    Quat tilt;
    Quat spin;

    if (*(int *)(*state + 0x390) != 0) {
        QuatFromAxisAngle(&spin, (VecFx32 *)(state + 0x10), state[9]);
        Quat_FromTwoVectors(&tilt, &data_02042264, (VecFx32 *)(*state + 0x124));
        Quat_Multiply(&tilt, &tilt, &spin);
        Srt_SetRotationQuat((void *)(*state + 0xa0), &tilt);
    }
    if (*(signed char *)(*state + 0x1c6) == 2 && ((Bit0 *)(*state + 0x17a))->b0) {
        *(VecFx32 *)(state + 0xc) = *(VecFx32 *)(*state + 0x124);
    }
    {
        VecFx32 *vel = (VecFx32 *)(state + 3);

        *(VecFx32 *)(*state + 0xf0) = *vel;
        *vel = data_02041dc8;
    }
}
