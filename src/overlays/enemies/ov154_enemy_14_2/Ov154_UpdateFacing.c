/* Facing update of the ov153 enemy (x3: ov153/154/155): unless the actor carries bit 1 of its
 * +0x1c4 flags, the +0x10 heading eases towards the +0x14 target at the +0x20 rate; the heading
 * becomes a quaternion about the world Y axis, is multiplied by the quaternion that turns Y onto
 * the actor's +0x124 up vector, and the product goes to the actor's +0xa0 orientation. */

#include "nitro/fx_types.h"
#include "game/engine.h"

struct Quat { int a, b, c, d; };

extern void Quat_FromTwoVectors(struct Quat *out, const VecFx32 *fwd, VecFx32 *dir);
extern void Quat_Multiply(struct Quat *out, const struct Quat *a, const struct Quat *b);
extern void Srt_SetRotationQuat(int dst, struct Quat *src);
extern const VecFx32 data_02042264;

void Ov154_UpdateFacing(int node)
{
    int *state = *(int **)(node + 4);
    struct Quat qUp;
    struct Quat qHeading;

    if ((*(unsigned char *)(*state + 0x1c4) & 2) == 0) {
        state[4] = Angle_TurnToward(state[4], state[5], state[8], 0);
    }
    QuatFromAxisAngle(&qHeading, &data_02042264, state[4]);
    Quat_FromTwoVectors(&qUp, &data_02042264, (VecFx32 *)(*state + 0x124));
    Quat_Multiply(&qUp, &qUp, &qHeading);
    Srt_SetRotationQuat(*state + 0xa0, &qUp);
}
