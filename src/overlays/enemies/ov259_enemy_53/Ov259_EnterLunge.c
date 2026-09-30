/* Enter the lunge reaction: snapshot the current velocity (node[8..10] -> node[5..7]), scale the
 * live velocity by 0xb00 (Fx12), set the reaction state to 2, and re-register the think callback. */

#include "nitro/fx_types.h"

extern void ScaleVec3Fx12(int scale, void *dst, void *src);
extern void SetIndexedSlot(int self, int idx, int cb);

void Ov259_EnterLunge(int param_1) {
    int *node = *(int **)(param_1 + 4);
    *(VecFx32 *)(node + 5) = *(VecFx32 *)(node + 8);
    ScaleVec3Fx12(0xb00, node + 8, node + 8);
    *(char *)(*node + 0x1c7) = 2;
    SetIndexedSlot(param_1, *(signed char *)((char *)param_1 + 0x20), 0);
}
