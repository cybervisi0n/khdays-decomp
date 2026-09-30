/* Hover tick: the +0xc velocity is taken from the +0x54 angle around the +0x574 anchor's +0x2c
 * point (020cdafc), its height is set to the anchor's +0x30 and the vector is scaled by the +0x70
 * rate plus 0.5. Once the +4 item's +0xad byte clears the next move is 4. */

#include "nitro/fx_types.h"

extern VecFx32 Ov252_TurnVecY(int angle, VecFx32 *vec);
extern void ScaleVec3Fx12(int scale, VecFx32 *v, VecFx32 *d);
extern void SetIndexedSlot(int *node, int slot, void *cb);

void Ov252_HoverTick(int *node)
{
    int *state = (int *)node[1];
    VecFx32 v;

    v = Ov252_TurnVecY(state[0x15], (VecFx32 *)(*(int *)(*state + 0x574) + 0x2c));
    *(VecFx32 *)(state + 3) = v;
    state[4] = *(int *)(*(int *)(*state + 0x574) + 0x30);
    ScaleVec3Fx12(state[0x1c] + 0x800, (VecFx32 *)(state + 3), (VecFx32 *)(state + 3));
    if (*(unsigned char *)(state[1] + 0xad) != 0) {
        return;
    }
    *(signed char *)(*state + 0x1c7) = 4;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}
