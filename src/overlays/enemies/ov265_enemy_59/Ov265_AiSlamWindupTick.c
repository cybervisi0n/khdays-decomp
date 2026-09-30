/* Ov265_AiSlamWindupTick -- the per-tick update of the state entered by Ov231_AiStateInit: refresh
 * the tracked point, fire once the timer passes 0x1078, and hand off when the gate byte clears.
 *
 * Ov265_rotateVecByOwnerYaw recomputes the point relative to *(ctx[0]+0x388)+0x2c and it is cached at
 * ctx+0x30. The timer at +0x28 accumulates (*self)[0x2c].
 *
 * The one-shot fires only while the flag at +0x54 is set, and clears it afterwards: emit via
 * Ov107_BuildAndSendUpdate (with the SIGNED halfword at +0x50) and effect 5.
 *
 * The gate at *ctx[4] holds this state until it reads 0; then the timer resets, effect 6 fires
 * and Ov265_PlayPoseAnims hands off to Ov265_AiSlamDashTick.
 *
 * data_02041dc8 is a 12-byte vector passed BY VALUE -- that is what the `sub r3,sp,#8` is: mwcc
 * lays the struct out straddling sp so its first two words load into r2/r3 and the third already
 * sits in the outgoing stack slot at sp+0. Don't read the negative offset as a bug. */

#include "nitro/fx_types.h"

extern void Ov265_AcquireTarget(int self);
extern void Ov265_rotateVecByOwnerYaw(VecFx32 *out, int self, int ref);
extern void Ov107_BuildAndSendUpdate(int owner, int a, int b, int c);
extern void func_ov107_020c0b90(int owner, int effect, VecFx32 v, int a);
extern void Ov265_PlayPoseAnims(int self, int a, int b, int c, void (*cb)(void));
extern void Ov265_AiSlamDashTick(void);
extern VecFx32 data_02041dc8;

void Ov265_AiSlamWindupTick(int self) {
    int *ctx;
    VecFx32 pt;

    ctx = *(int **)(self + 4);
    Ov265_AcquireTarget(self);
    Ov265_rotateVecByOwnerYaw(&pt, self, *(int *)(ctx[0] + 0x388) + 0x2c);
    *(VecFx32 *)((char *)ctx + 0x30) = pt;

    ctx[0xa] += *(int *)(*(int *)self + 0x2c);
    if (ctx[0xa] >= 0x1078 && ctx[0x15] != 0) {
        Ov107_BuildAndSendUpdate(ctx[0], *(short *)((char *)ctx + 0x50), 0xa, ctx[2]);
        func_ov107_020c0b90(ctx[0], 5, data_02041dc8, 0);
        ctx[0x15] = 0;
    }

    if (*(unsigned char *)ctx[4] != 0) {
        return;
    }

    ctx[0xa] = 0;
    func_ov107_020c0b90(ctx[0], 6, data_02041dc8, 0);
    Ov265_PlayPoseAnims(self, 0xa, 0, 0, Ov265_AiSlamDashTick);
}
