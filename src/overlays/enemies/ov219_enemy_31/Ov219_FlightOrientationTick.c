/* Flight orientation tick of the ov219 enemy: the +0xc yaw eases towards the +0x10 target yaw
 * (four frame-times per step), the actor's +0xa0 rotation becomes the up-vector-to-+0x124 tilt
 * combined with that yaw spin, and the +0x24 velocity is written to the actor's +0xf0. Then, in
 * 0x88-frame-time slices, the velocity decays by 0x180/0x88 of the slice and the +0x20 speed by
 * 0x200/0x88 of it. */

#include "nitro/fx_types.h"
#include "game/engine.h"

struct Quat { int a, b, c, d; };

extern void Quat_FromTwoVectors(struct Quat *q, const VecFx32 *from, const VecFx32 *to);
extern void Quat_Multiply(struct Quat *dst, const struct Quat *a, const struct Quat *b);
extern void Srt_SetRotationQuat(int transform, const struct Quat *q);
extern int FX_Div(int a, int b);
extern void ScaleVec3Fx12(int scale, void *v, void *d);
extern const VecFx32 data_02042264;

static inline int FX_Mul(int a, int b)
{
    return (int)(((long long)a * b + 0x800) >> 12);
}

void Ov219_FlightOrientationTick(int *node)
{
    int *state = (int *)node[1];
    struct Quat spin;
    struct Quat q;
    int dt;
    int t;

    state[3] = Angle_TurnToward(state[3], state[4], *(int *)(*node + 0x2c) << 2, 0);
    QuatFromAxisAngle(&spin, &data_02042264, state[3]);
    Quat_FromTwoVectors(&q, &data_02042264, (const VecFx32 *)(*state + 0x124));
    Quat_Multiply(&q, &q, &spin);
    Srt_SetRotationQuat(*state + 0xa0, &q);
    *(VecFx32 *)(*state + 0xf0) = *(VecFx32 *)(state + 9);
    dt = *(int *)(*node + 0x2c);
    if (dt <= 0) {
        return;
    }
    do {
        t = FX_Div(dt <= 0x88 ? dt : 0x88, 0x88);
        ScaleVec3Fx12(0x1000 - FX_Mul(t, 0x180), state + 9, state + 9);
        t = FX_Div(dt <= 0x88 ? dt : 0x88, 0x88);
        state[8] = FX_Mul(state[8], 0x1000 - FX_Mul(t, 0x200));
        dt -= 0x88;
    } while (dt > 0);
}
