/* Ov277_AdvanceMotionPublish -- x3 (ov114/...). Advance the tracked motion and publish it to the owner.
 * The stored value at +0xc is stepped by Angle_TurnToward (which also updates the state block at +0x40),
 * the step is turned into a delta against data_02042264, composed with the owner's own rig value
 * (+0x124), and the result written to the owner at +0xa0. A non-negative countdown at +0x44 is debited
 * by the caller's per-tick amount (+0x2c of self[0]). Finally the cached offset at +0x18 is published
 * to the owner at +0xf0 and reset to the neutral constant. (Twin of Ov158_AiApplyHeadingAndNormal; differs only
 * in the countdown test: >= 0 here vs > 0 there.) */

#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct {
    int v[4];
} Xform;

extern void Quat_FromTwoVectors(Xform *out, const Xform *src, int rig);
extern void Quat_Multiply(Xform *out, const Xform *a, const Xform *b);
extern void Srt_SetRotationQuat(int dst, const Xform *x);
extern Xform data_02042264;
extern VecFx32 data_02041dc8;

void Ov277_AdvanceMotionPublish(int self) {
    int *ctx;
    Xform b;
    Xform a;
    int owner;

    ctx = *(int **)(self + 4);
    ctx[5] = Angle_TurnToward(ctx[5], ctx[6], *(int *)(*(int *)self + 0x2c) * 0x1e >> 3, 0);
    QuatFromAxisAngle(&a, &data_02042264, ctx[5]);
    Quat_FromTwoVectors(&b, &data_02042264, ctx[0] + 0x124);
    Quat_Multiply(&b, &b, &a);
    Srt_SetRotationQuat(ctx[0] + 0xa0, &b);

    if (ctx[0x13] > 0) {
        ctx[0x13] = ctx[0x13] - *(int *)(*(int *)self + 0x2c);
    }

    /* ctx is walked forward here on purpose: reading the owner and stepping to +0x18 in one go is
     * what gives the ROM's post-indexed `ldr r0,[r4],#0x18`. It is ctx's last use. */
    owner = *ctx;
    ctx = (int *)((char *)ctx + 0x50);
    *(VecFx32 *)(owner + 0xf0) = *(VecFx32 *)ctx;
    *(VecFx32 *)ctx = data_02041dc8;
}
