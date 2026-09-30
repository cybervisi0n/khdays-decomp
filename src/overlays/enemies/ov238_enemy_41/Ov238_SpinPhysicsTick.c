/* Spin physics tick of the ov238 actor: its +0x18 heading turns toward +0x1c at 0.033 per frame and
 * orients the pose about up; the +0xf0 velocity mirrors +0xc, which damps to 0.75. In moves 2-4 the
 * +0x28 timer runs down (to 0). Once the health (+0x21a) is out it is clamped to 0 and, outside move 9,
 * the +0x384 rider is flagged (+0x390) and the next move is 9. */

#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct { int x, y, z, w; } Quat;
struct Mover { char pad[0xf0]; VecFx32 vel; };
struct Health { char pad[0x21a]; short hp; };
struct Ov238Node { int actor; char pad[8]; VecFx32 vel; };

extern void Srt_SetRotationQuat(void *srt, const Quat *rot);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern const VecFx32 data_02042264;

void Ov238_SpinPhysicsTick(int *node)
{
    int *state = (int *)node[1];
    Quat q;

    state[6] = Angle_TurnToward(state[6], state[7], 0x88, 0);
    QuatFromAxisAngle(&q, &data_02042264, state[6]);
    Srt_SetRotationQuat((void *)(*state + 0xa0), &q);
    ((struct Mover *)*state)->vel = ((struct Ov238Node *)state)->vel;
    ScaleVec3Fx12(0xc00, (VecFx32 *)(state + 3), (VecFx32 *)(state + 3));
    if (!(*(signed char *)(*state + 0x1c6) != 2 && *(signed char *)(*state + 0x1c6) != 3 &&
          *(signed char *)(*state + 0x1c6) != 4)) {
        state[0xa] -= *(int *)(node[0] + 0x2c);
        if (state[0xa] <= 0) {
            state[0xa] = 0;
        }
    }
    if (((struct Health *)*state)->hp > 0) {
        return;
    }
    ((struct Health *)*state)->hp = 0;
    if (*(signed char *)(*state + 0x1c6) == 9) {
        return;
    }
    *(int *)(*(int *)(*state + 0x384) + 0x390) = 1;
    *(unsigned char *)(*state + 0x1c7) = 9;
}
