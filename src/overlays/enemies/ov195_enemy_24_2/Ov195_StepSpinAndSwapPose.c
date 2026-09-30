/* obj is walked forward with a BYTE cast (`(int *)((char *)obj + 0x18)`), not int
 * arithmetic: reading the owner and stepping in one go is what gives the ROM's
 * post-indexed `ldr r0,[r4],#0x18`. Same form as ov247_020d281c.
 * `v = obj[3] = f(...)` in that order stores the raw r0; declaration order `r` then `q`
 * gives the ROM's stack layout. */

#include "nitro/fx_types.h"
#include "game/engine.h"

struct quat { int x, y, z, w; };
extern void Quat_FromTwoVectors();
extern void Quat_Multiply();
extern void Srt_SetRotationQuat();
extern struct quat data_02042264;
extern VecFx32 data_02041dc8;

void Ov195_StepSpinAndSwapPose(int *self) {
    int *obj = (int *)self[1];
    struct quat r;
    struct quat q;
    int v;
    int owner;

    v = obj[3] = Angle_TurnToward(obj[3], obj[4], obj[5], 0);
    QuatFromAxisAngle(&r, &data_02042264, v);
    Quat_FromTwoVectors(&q, &data_02042264, *obj + 0x124);
    Quat_Multiply(&q, &q, &r);
    Srt_SetRotationQuat(*obj + 0xa0, &q);
    if (obj[0xd] > 0) {
        obj[0xd] = obj[0xd] - *(int *)(self[0] + 0x2c);
    }
    owner = *obj;
    obj = (int *)((char *)obj + 0x18);
    *(VecFx32 *)(owner + 0xf0) = *(VecFx32 *)obj;
    *(VecFx32 *)obj = data_02041dc8;
}
