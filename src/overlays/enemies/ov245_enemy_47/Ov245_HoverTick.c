/* Ov245_HoverTick -- hover tick: pushes the state's +0xc velocity along the world's +0x4c8
 * anchor's +0x2c direction, by -1.0 when the actor's +0x434 owner holds a +0x3a0 target and by the
 * negated +0x30 speed otherwise; then, when the animation gate (020cce48) reports idle, requests
 * sub-state 2 and releases the node's slot. */

#include "nitro/fx_types.h"

extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern int Ov245_AnimGate(int self);
extern void SetIndexedSlot(int *node, int slot, void *cb);

void Ov245_HoverTick(int *node) {
    int *state = (int *)node[1];
    int actor = *state;
    int scale;

    if (*(int *)(*(int *)(actor + 0x434) + 0x3a0) != 0) {
        scale = 0x400;
    } else {
        scale = state[0xc];
    }
    ScaleVec3Fx12(-scale, (VecFx32 *)(*(int *)(actor + 0x4c8) + 0x2c), (VecFx32 *)(state + 3));
    if (Ov245_AnimGate(*state) == 0) {
        *(unsigned char *)(*state + 0x1c7) = 2;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
    }
}
