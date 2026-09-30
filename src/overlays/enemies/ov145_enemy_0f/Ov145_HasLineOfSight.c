/* Line-of-sight test of the ov144 enemy (and its byte-identical twin): casts a 0x40 sphere from
 * the actor's +0x74 position towards the target point, stopping the margin short (at least one
 * unit), and reports 1 when nothing is in the way. */

#include "nitro/fx_types.h"

extern void VEC_Subtract(void *a, void *b, void *d);
extern int VEC_Normalize(void *a, void *d);
extern void ScaleVec3Fx12(int scale, void *v, void *d);
extern void *Collision_CastSphere(void *world, void *from, void *step, int radius);

int Ov145_HasLineOfSight(int *state, VecFx32 target, int margin)
{
    VecFx32 step;
    VecFx32 pos;
    int scene;
    int len;

    scene = *(int *)(*state + 4);
    pos = *(VecFx32 *)(*state + 0x74);
    VEC_Subtract(&target, &pos, &step);
    len = VEC_Normalize(&step, &step) - margin;
    if (len <= 0) {
        len = 1;
    }
    ScaleVec3Fx12(len, &step, &step);
    if (Collision_CastSphere(*(void **)(scene + 0x7c), &pos, &step, 0x40) != 0) {
        return 0;
    }
    return 1;
}
