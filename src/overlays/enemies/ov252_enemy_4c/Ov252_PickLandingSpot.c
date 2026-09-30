/* Pick the landing spot of the ov252 actor: of the five arena points (data_ov252_020d43ec) the one
 * nearest to the +0x4e4 target's +0x190 position. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { VecFx32 v[5]; } Vec3x5;

extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int VEC_Normalize(VecFx32 *v, VecFx32 *out);
extern const Vec3x5 data_ov252_020d43ec;

VecFx32 Ov252_PickLandingSpot(int *node)
{
    int *state = (int *)node[1];
    VecFx32 d;
    Vec3x5 spots = data_ov252_020d43ec;
    int bestDist;
    u8 i;
    int best;
    int dist;

    for (i = 0; i < 5; i++) {
        VEC_Subtract((VecFx32 *)(*(int *)(*state + 0x4e4) + 0x190), &spots.v[i], &d);
        dist = VEC_Normalize(&d, &d);
        if (i == 0) {
            best = i;
            bestDist = dist;
        } else if (dist < bestDist) {
            bestDist = dist;
            best = i;
        }
    }
    return spots.v[best];
}
