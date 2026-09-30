/* Ov246_AiApplyHeadingAndNormal -- advance the tracked motion and publish it to the owner.
 * The stored value at +0xc is stepped by Angle_TurnToward (which also updates the state block at
 * +0x40), the step is turned into a delta against data_02042264, composed with the owner's own
 * rig value (+0x124), and the result written to the owner at +0xa0.
 * A positive countdown at +0x44 is debited by the caller's per-tick amount (+0x2c of self[0]).
 * Finally the cached offset at +0x18 is published to the owner at +0xf0 and reset to the
 * neutral constant. */

#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct {
    int v[4];
} Ov158_Xform;

extern void Quat_FromTwoVectors(Ov158_Xform *out, const Ov158_Xform *src, int rig);
extern void Quat_Multiply(Ov158_Xform *out, const Ov158_Xform *a, const Ov158_Xform *b);
extern void Srt_SetRotationQuat(int dst, const Ov158_Xform *x);
extern Ov158_Xform data_02042264;
extern VecFx32 data_02041dc8;

void Ov246_AiApplyHeadingAndNormal(int self) {
    int *ctx;
    Ov158_Xform b;
    Ov158_Xform a;
    int owner;

    ctx = *(int **)(self + 4);
    ctx[3] = Angle_TurnToward(ctx[3], ctx[4], ctx[5], &ctx[0x10]);
    QuatFromAxisAngle(&a, &data_02042264, ctx[3]);
    Quat_FromTwoVectors(&b, &data_02042264, ctx[0] + 0x124);
    Quat_Multiply(&b, &b, &a);
    Srt_SetRotationQuat(ctx[0] + 0xa0, &b);

    if (ctx[0x11] > 0) {
        ctx[0x11] = ctx[0x11] - *(int *)(*(int *)self + 0x2c);
    }

    /* ctx is walked forward here on purpose: reading the owner and stepping to +0x18 in one go
     * is what gives the ROM's post-indexed `ldr r0,[r4],#0x18`. It is ctx's last use. */
    owner = *ctx;
    ctx = (int *)((char *)ctx + 0x18);
    *(VecFx32 *)(owner + 0xf0) = *(VecFx32 *)ctx;
    *(VecFx32 *)ctx = data_02041dc8;
}
