/* Ov265_UpdateAimPoint -- per-tick update: refresh the cached aim point, then re-arm the driver.
 * The aim point is recomputed from the owner's rig (+0x388, offset 0x2c) and copied into +0x30.
 * Nothing else happens while the gate byte at *(+0x10) is set.
 * Otherwise the pending value (+0x28) and the retry flag (+0x48) are cleared, and when the owner
 * is in state 0xe the target is re-acquired and its bearing (+0x1c) promoted to the live one
 * (+0x18). Either way the driver is re-armed through Ov265_PlayPoseAnims with
 * Ov265_AiDiveTick as the continuation. */

#include "nitro/fx_types.h"

extern void Ov265_rotateVecByOwnerYaw(VecFx32 *out, int self, int rig);
extern int Ov265_AcquireTarget(int self);
extern void Ov265_PlayPoseAnims(int self, int a, int b, int c, void (*cb)(void));
extern void Ov265_AiDiveTick(void);

void Ov265_UpdateAimPoint(int self) {
    int *ctx;
    VecFx32 aim;

    ctx = *(int **)(self + 4);
    Ov265_rotateVecByOwnerYaw(&aim, self, *(int *)(ctx[0] + 0x388) + 0x2c);
    *(VecFx32 *)((char *)ctx + 0x30) = aim;

    if (**(unsigned char **)(ctx + 4) != 0) {
        return;
    }
    ctx[0xa] = 0;
    *(unsigned char *)((char *)ctx + 0x48) = 0;

    if (*(signed char *)(ctx[0] + 0x1c6) == 0xe && *(unsigned char *)((char *)ctx + 0x48) == 0) {
        Ov265_AcquireTarget(self);
        ctx[6] = ctx[7];
    }
    Ov265_PlayPoseAnims(self, 3, 3, 1, Ov265_AiDiveTick);
}
