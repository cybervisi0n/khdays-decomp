/* Ov253_AimFollow -- camera/aim follow: the +8 distance eases towards 10.0 by a thirtieth;
 * the +4 item's +0x74 position is kept at +0x20 and the +0xc rotation's forward (data_02042258)
 * scaled by the distance gives an aim point, kept at least 1.0 above the +0x24 height; the
 * rotation is then rebuilt to face that point (0202ed60). */

#include "nitro/fx_types.h"
#include "game/engine.h"

extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern void Quat_FromTwoVectors(void *rotation, const VecFx32 *from, const VecFx32 *to);
extern const VecFx32 data_02042258;

void Ov253_AimFollow(int *node) {
    int *state = (int *)node[1];
    VecFx32 fwd;
    VecFx32 aim;
    VecFx32 dir;

    state[2] += (0xa000 - state[2]) / 30;
    *(VecFx32 *)(state + 8) = *(VecFx32 *)(state[1] + 0x74);
    Vec3TransformViaTempMtx(&fwd, (void *)(state + 3), &data_02042258);
    ScaleVec3Fx12(state[2], &fwd, &aim);
    VEC_Add(&aim, (VecFx32 *)(state + 8), &aim);
    if (aim.y < state[9] + 0x1000) {
        aim.y = state[9] + 0x1000;
    }
    VEC_Subtract(&aim, (VecFx32 *)(state + 8), &dir);
    VEC_Normalize(&dir, &dir);
    Quat_FromTwoVectors((void *)(state + 3), &data_02042258, &dir);
}
