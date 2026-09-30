/* Ov253_HitFilterFlip -- hit filter of the +0x214 sub-state: a hit with low bit 4, while the
 * +0x44 latch is clear, flips the +0x14 direction, latches +0x44 and clears +0x24. Returns 1
 * when handled. */

#include "nitro/fx_types.h"

extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);

int Ov253_HitFilterFlip(int self, int a, unsigned int *hit) {
    int *state = *(int **)(self + 0x214);

    if (((unsigned short)*hit & 0x10) != 0) {
        if (state[0x11] != 0) {
            return 0;
        }
        ScaleVec3Fx12(-0x1000, (VecFx32 *)(state + 5), (VecFx32 *)(state + 5));
        state[0x11] = 1;
        state[9] = 0;
        return 1;
    }
    return 0;
}
