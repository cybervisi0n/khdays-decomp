/* Lock on and start a lunge (and its byte-identical twins). Pick the current target; with none,
 * do nothing. Otherwise take the direction from the owner (owner+0xb0) to the target
 * (target+0x190), normalise it -- VEC_Normalize writes the unit vector back and
 * returns the distance -- and cache that distance at +0x28, capped at 0x9000. Mark the
 * target slot live (owner+0x1c7 = 1), store the caller's aim vector at +0x14 and its
 * tag at +0x30, and clear the accumulated offset at +8. */

#include "nitro/fx_types.h"

extern int *Ov107_FindNearestObject(int a, int b);
extern int VEC_Subtract(void *a, void *b, void *out);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern VecFx32 data_02041dc8;

void Ov266_LockOnTarget(int *self, VecFx32 *param_2, int param_3) {
    VecFx32 d;
    int *tgt = Ov107_FindNearestObject(self[0], 0);

    if (tgt == 0) {
        return;
    }
    VEC_Subtract((char *)tgt + 0x190, (char *)self[0] + 0xb0, &d);
    self[0xa] = VEC_Normalize(&d, &d);
    self[9] = 0;
    if (self[0xa] > 0x9000) {
        self[0xa] = 0x9000;
    }
    *(char *)(self[0] + 0x1c7) = 1;
    *(VecFx32 *)((char *)self + 0x14) = *param_2;
    self[0xc] = param_3;
    *(VecFx32 *)((char *)self + 8) = data_02041dc8;
}
