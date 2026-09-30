/* Ov245_DescendTick -- descend tick: the +0xc velocity is the negated +0x4c8 anchor direction
 * (+0x2c); the +0x14 height grows by itself times the +0x38 factor clamped to 0..1; then the
 * height-gap check (020ccda4 with mode 0) feeds the arrival test (020ccb30) and on success the
 * node's slot is released. */

#include "nitro/fx_types.h"

extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern int Ov245_TargetHeightGap(int *node, int flat);
extern int Ov245_ArrivalDecision(int *node, int gap);
extern void SetIndexedSlot(int *node, int slot, void *cb);

void Ov245_DescendTick(int *node) {
    int *state = (int *)node[1];
    int factor;

    ScaleVec3Fx12(-0x1000, (VecFx32 *)(*(int *)(*state + 0x4c8) + 0x2c), (VecFx32 *)(state + 3));
    factor = state[0xe];
    if (factor > 0x1000) {
        factor = 0x1000;
    } else if (factor < 0) {
        factor = 0;
    }
    state[5] = state[5] + (int)(((long long)state[5] * factor + 0x800) >> 12);
    if (Ov245_ArrivalDecision(node, Ov245_TargetHeightGap(node, 0)) != 0) {
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
    }
}
