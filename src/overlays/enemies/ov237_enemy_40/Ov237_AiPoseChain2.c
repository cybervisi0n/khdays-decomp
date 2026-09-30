/* Fetch the target vector via 020cdb50 into +0x3c; unless busy kick anims and dispatch. */

#include "nitro/fx_types.h"

extern VecFx32 Ov237_RotateByActorHeading(int *node, VecFx32 *target);
extern int Ov107_PostTagUpdate(int, int, int);
extern int Ov107_StartAnim(int, int, int);
extern int SetIndexedSlot(int, int, void *);
extern int Ov237_AiPoseChain3(int);
void Ov237_AiPoseChain2(int param_1) {
    int owner = *(int *)(param_1 + 4);
    VecFx32 buf;
    buf = Ov237_RotateByActorHeading((int *)param_1,
                                     (VecFx32 *)(*(int *)(*(int *)owner + 0x3d8) + 0x2c));
    *(VecFx32 *)(owner + 0x3c) = buf;
    if (*(unsigned char *)(*(int *)(owner + 4) + 0xad) != 0) return;
    Ov107_PostTagUpdate(*(int *)owner, 2, 0);
    Ov107_StartAnim(*(int *)(*(int *)owner + 0x3d8), 1, 0);
    SetIndexedSlot(param_1, *(signed char *)(param_1 + 0x20), (void *)&Ov237_AiPoseChain3);
}
