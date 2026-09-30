/* Lunge entry of the ov240 enemy: the +0x20 velocity is the direction from the +8 position to
 * the actor's +0x394 target's +0x190 point, scaled by the gap beyond both +0x80 radii; no gap
 * requests sub-state 2 and ends the state, else bit 0 of the actor's +0x1ae is raised,
 * animation 4 plays, the +0x38 timer and +0x3e flag reset and the tick hands off to cf0c8. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern void VEC_Subtract(void *a, void *b, VecFx32 *d);
extern int VEC_Normalize(VecFx32 *v, VecFx32 *d);
extern void ScaleVec3Fx12(int scale, VecFx32 *v, VecFx32 *d);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov240_LungeTick(int *node);

void Ov240_LungeEntry(int *node)
{
    int *state = (int *)node[1];
    VecFx32 d;
    int gap;
    int radii;

    radii = *(int *)(*(int *)(*state + 0x394) + 0x80) + *(int *)(*state + 0x80);
    VEC_Subtract((void *)(*(int *)(*state + 0x394) + 0x190), (void *)state[2], &d);
    gap = VEC_Normalize(&d, (VecFx32 *)(state + 8)) - radii;
    ScaleVec3Fx12(gap, (VecFx32 *)(state + 8), (VecFx32 *)(state + 8));
    if (gap <= 0) {
        *(unsigned char *)(*state + 0x1c7) = 2;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
        return;
    }
    *(unsigned short *)(*state + 0x1ae) |= 1;
    Ov107_PostTagUpdate((Actor *)(*state), 4, 0);
    state[0xe] = 0;
    *(unsigned char *)((char *)state + 0x3e) = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov240_LungeTick);
}
