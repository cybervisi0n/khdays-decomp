/* Fetch the target vector via 020cdb50 into +0x3c; unless busy mark state 2 and dispatch. */

#include "nitro/fx_types.h"

extern VecFx32 Ov237_RotateByActorHeading(int *node, VecFx32 *target);
extern int SetIndexedSlot(int, int, int);
void Ov237_AiPoseChainEnd(int param_1) {
    int owner = *(int *)(param_1 + 4);
    VecFx32 buf;
    buf = Ov237_RotateByActorHeading((int *)param_1,
                                     (VecFx32 *)(*(int *)(*(int *)owner + 0x3d8) + 0x2c));
    *(VecFx32 *)(owner + 0x3c) = buf;
    if (*(unsigned char *)(*(int *)(owner + 4) + 0xad) != 0) return;
    *(signed char *)(*(int *)owner + 0x1c7) = 2;
    SetIndexedSlot(param_1, *(signed char *)(param_1 + 0x20), 0);
}
