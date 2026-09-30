/* Ov245_AimTick -- in sub-state 1: rescales the state's +0x18 direction by 0.875 into the
 * +0xc velocity, builds the rotation that turns the +0x2c2258 reference onto that direction
 * (0202ed60) into the actor's +0xa0 placement (0203c9d0) and copies the velocity to +0xf0. */

#include "nitro/fx_types.h"

extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void Quat_FromTwoVectors(void *out, const VecFx32 *a, const VecFx32 *b);
extern void Srt_SetRotationQuat(int placement, void *rotation);
extern const VecFx32 data_02042258;

void Ov245_AimTick(int *node) {
    int *state = (int *)node[1];
    int rot[4];

    if (*(signed char *)(*state + 0x1c6) != 1) {
        return;
    }
    ScaleVec3Fx12(0xe00, (VecFx32 *)(state + 6), (VecFx32 *)(state + 3));
    Quat_FromTwoVectors(rot, &data_02042258, (VecFx32 *)(state + 6));
    Srt_SetRotationQuat(*state + 0xa0, rot);
    *(VecFx32 *)(*state + 0xf0) = *(VecFx32 *)(state + 3);
}
