/* Watch slot of an ov256 claw (every frame). While docked (+0x3a0 and +0x39c clear) it follows the
 * owner's hand bone (+0x418 left / +0x424 right, +0x394) and +0x80 rests on the vertical axis; when
 * fully idle (+0x398 too) the +0x50 heading follows the owner's +0x3ac part (+0x458). The heading
 * quaternion is tilted onto the ground normal (+0x124) while landed and idle. The +0xa0 pose takes the
 * hand bone's rotation while docked, at the end of a 9-orbit launch (0x550 into it) or early in the
 * first orbit (before 0x908); otherwise the heading. +0xf0 keeps the last +0x10 velocity, which then
 * clears. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"
#include "game/engine.h"

typedef struct { int x, y, z, w; } Quat;
struct Flag17a { u8 b0 : 1; };

extern void Quat_FromTwoVectors(Quat *out, const VecFx32 *from, const VecFx32 *to);
extern void Quat_Multiply(Quat *out, const Quat *a, const Quat *b);
extern void Srt_SetRotationQuat(char *srt, void *q);
extern const VecFx32 data_02042264;
extern const VecFx32 data_02041dc8;

#define HAND_BONE(s) (*(u8 *)((s) + 0x394) == 0 ? *(int *)(*(int *)((s) + 0x3ac) + 0x418) \
                                                 : *(int *)(*(int *)((s) + 0x3ac) + 0x424))

void Ov256_ClawWatchSlot(int *node)
{
    int *state = (int *)node[1];
    VecFx32 hand;
    Quat q;
    Quat tilt;

    if (*(int *)(*state + 0x3a0) == 0 && *(int *)(*state + 0x39c) == 0) {
        hand = *(VecFx32 *)(HAND_BONE(*state) + 0x14);
        Ov107_MoveNodeAndRelayout((Actor *)(*state), &hand);
        *(VecFx32 *)(state + 0x20) = data_02042264;
    }
    if (*(int *)(*state + 0x39c) == 0 && *(int *)(*state + 0x3a0) == 0 && *(int *)(*state + 0x398) == 0) {
        state[0x14] = *(int *)(*(int *)(*state + 0x3ac) + 0x458);
    }
    QuatFromAxisAngle(&q, &data_02042264, state[0x14]);
    if (((struct Flag17a *)(*state + 0x17a))->b0 && *(int *)(*state + 0x3a0) == 0 &&
        *(int *)(*state + 0x39c) == 0 && *(int *)(*state + 0x398) == 0) {
        Quat_FromTwoVectors(&tilt, &data_02042264, (VecFx32 *)(*state + 0x124));
        Quat_Multiply(&q, &tilt, &q);
    }
    if (*(int *)(*state + 0x3a0) == 0 && *(int *)(*state + 0x39c) == 0) {
        if (*(u8 *)(*state + 0x394) == 0) {
            Srt_SetRotationQuat((char *)(*state + 0xa0), (void *)(*(int *)(*(int *)(*state + 0x3ac) + 0x418) + 4));
        } else {
            Srt_SetRotationQuat((char *)(*state + 0xa0), (void *)(*(int *)(*(int *)(*state + 0x3ac) + 0x424) + 4));
        }
    } else if ((*(int *)(*state + 0x3a0) != 0 && state[0x18] >= 0x550 && state[0x19] == 9) ||
               (state[0x18] < 0x908 && state[0x19] == 0 && *(int *)(*state + 0x3a0) != 0)) {
        if (*(u8 *)(*state + 0x394) == 0) {
            Srt_SetRotationQuat((char *)(*state + 0xa0), (void *)(*(int *)(*(int *)(*state + 0x3ac) + 0x418) + 4));
        } else {
            Srt_SetRotationQuat((char *)(*state + 0xa0), (void *)(*(int *)(*(int *)(*state + 0x3ac) + 0x424) + 4));
        }
    } else {
        Srt_SetRotationQuat((char *)(*state + 0xa0), &q);
    }
    {
        VecFx32 *vel = (VecFx32 *)(state + 4);

        *(VecFx32 *)(*state + 0xf0) = *vel;
        *vel = data_02041dc8;
    }
}
