/* Hit filter of an ov254 helper (+0x1d0): ignored while guarding (+0x1ac bit 0) or for flag-8
 * hits. The source is kept in +4; the +0x1c push is the flattened direction from the actor to its
 * +0x394 owner when it points within ~50 degrees of the hit's own direction, else the hit
 * direction. A sourced hit resets the +0x40 / +0x44 timers, sets +0x4c and clears the +0x34 hit
 * mask; an unsourced one adds bit (short)hit[4] to that 64-bit mask. Returns 1. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

struct HitWord { unsigned int lo : 16, hi : 16; };

extern int VEC_Normalize(VecFx32 *v, VecFx32 *d);
extern void VEC_Subtract(const void *a, const void *b, VecFx32 *out);
extern int VEC_DotProduct(const VecFx32 *a, const VecFx32 *b);
extern void ScaleVec3Fx12(int scale, VecFx32 *v, VecFx32 *d);

int Ov254_HelperEHitFilter(char *self, int src, unsigned int *hit)
{
    int *state = *(int **)(self + 0x214);
    VecFx32 dir;
    VecFx32 toOwner;

    if ((*(u16 *)(self + 0x100 + 0xac) & 1) != 0) {
        return 0;
    }
    if ((((struct HitWord *)hit)->lo & 8) != 0) {
        return 0;
    }
    state[1] = src;
    dir = *(VecFx32 *)(hit + 1);
    dir.y = 0;
    VEC_Normalize(&dir, &dir);
    VEC_Subtract((void *)(*(int *)(*state + 0x394) + 0x74), (void *)(*state + 0x74), &toOwner);
    toOwner.y = 0;
    VEC_Normalize(&toOwner, &toOwner);
    if (VEC_DotProduct(&toOwner, &dir) >= 0xa49) {
        ScaleVec3Fx12(0x1000, &toOwner, (VecFx32 *)(state + 7));
    } else {
        ScaleVec3Fx12(0x1000, &dir, (VecFx32 *)(state + 7));
    }
    if (state[1] != 0) {
        state[0x11] = 0;
        state[0x10] = 0;
        state[0x13] = 1;
        state[0xd] = 0;
        state[0xe] = 0;
    } else {
        *(unsigned long long *)(state + 0xd) |= 1ULL << (short)hit[4];
    }
    return 1;
}
