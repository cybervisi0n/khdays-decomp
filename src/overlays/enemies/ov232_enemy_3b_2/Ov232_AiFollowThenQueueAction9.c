/* Compute the offset vector (020cca74) from the owner spline at *(*child+0x388)+0x2c into
 * (child)+0x30; unless the gate byte at *(child+0x10) is set, mark sub-state 9 and dispatch. */

#include "nitro/fx_types.h"

extern void Ov232_rotateVecByOwnerYaw(void *out, int a, int b);
extern int SetIndexedSlot(int a, int b, void *handler);
void Ov232_AiFollowThenQueueAction9(int param_1) {
    int child = *(int *)(param_1 + 4);
    VecFx32 out;
    Ov232_rotateVecByOwnerYaw(&out, param_1, *(int *)(*(int *)child + 0x388) + 0x2c);
    *(VecFx32 *)(child + 0x30) = out;
    if (*(unsigned char *)*(int *)(child + 0x10) != 0) return;
    *(signed char *)(*(int *)child + 0x1c7) = 9;
    SetIndexedSlot(param_1, *(signed char *)(param_1 + 0x20), (void *)0);
}
