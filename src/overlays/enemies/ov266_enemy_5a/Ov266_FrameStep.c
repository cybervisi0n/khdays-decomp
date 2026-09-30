/* Shared frame step of the ov266 enemy: with the +0x60 low bit set the grab phase machine
 * runs; the +0x34 heading turns towards +0x3c by twice the owner's rate and is written into the
 * +0xa0 pose (quaternion about data_02042264). Outside mode 8 the zero vector (data_02041dc8) turned by the pose is added to the +0x74 position and the
 * +0x10 velocity to give the next point; if a thin ray from it straight down by 2.0 plus the
 * actor radius finds no floor, the velocity is zeroed. The velocity is then published to +0xf0 and reset to zero
 * (data_02041dc8); if the 020cf3fc check passes the +0x44 timer resets, otherwise it counts
 * the rate up to 0xf000, and the rate is recorded at +0x580. */

#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct { int q[4]; } Quat;
typedef struct { unsigned short lo : 8, hi : 8; } Hw60;

extern void Ov266_GrabPhaseMachine(int *state);
extern void Srt_SetRotationQuat(void *srt, Quat *q);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int *Collision_CastRay(void *world, VecFx32 *origin, VecFx32 *dir);
extern int Ov266_CheckState6c(int *state, int a);
extern const VecFx32 data_02042264;
extern const VecFx32 data_02041dc8;

void Ov266_FrameStep(int *node)
{
    int *state = (int *)node[1];
    VecFx32 down;
    Quat q;
    VecFx32 next;
    VecFx32 up;
    char *owner = *(char **)(*state + 4);

    if ((((Hw60 *)(*state + 0x60))->lo & 1) != 0) {
        Ov266_GrabPhaseMachine(state);
    }
    state[0xd] = Angle_TurnToward(state[0xd], state[0xf], *(int *)(node[0] + 0x2c) * 2, 0);
    QuatFromAxisAngle(&q, &data_02042264, state[0xd]);
    Srt_SetRotationQuat((void *)(*state + 0xa0), &q);
    if (*(signed char *)(*state + 0x1c6) != 8) {
        up = data_02041dc8;
        next = data_02041dc8;
        Vec3TransformViaTempMtx(&next, (void *)(state + 0x28), &next);
        VEC_Add(&next, (VecFx32 *)(*state + 0x74), &next);
        VEC_Add(&next, (VecFx32 *)(state + 4), &next);
        down.y = -(*(int *)(*state + 0x80) + 0x2000);
        down.x = 0;
        down.z = 0;
        if (Collision_CastRay(*(void **)(owner + 0x7c), &next, &down) == 0) {
            *(VecFx32 *)(state + 4) = up;
        }
    }
    {
        VecFx32 *pVel = (VecFx32 *)(state + 4);
        *(VecFx32 *)(*state + 0xf0) = *pVel;
        *pVel = data_02041dc8;
    }
    if (Ov266_CheckState6c(state, 1) != 0) {
        state[0x11] = 0;
    } else if (state[0x11] < 0xf000) {
        state[0x11] += *(int *)(node[0] + 0x2c);
    }
    *(int *)(*state + 0x580) = *(int *)(node[0] + 0x2c);
}
