/* Per-frame update of an ov259 helper: move 3 (latch) holds it free (+0x39c = 1) and move 4
 * releases it. While held to its owner the helper follows the owner's +0x40c bone (position at +0x14,
 * rotation at +4; the turn from the rest axis is composed but unused), then the base update runs
 * (020c6980) and the +0xa0 pose is copied into the +0x390 model and on to the +0x38c shape. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

typedef struct { int x, y, z, w; } Quat;
typedef struct { int w[11]; } Pose;

extern void Quat_FromTwoVectors(Quat *out, const VecFx32 *from, const VecFx32 *to);
extern void Quat_Multiply(Quat *out, const Quat *a, const Quat *b);
extern void Srt_SetRotationQuat(char *srt, Quat *q);
extern void Ov107_ProcessObjectTick(char *self, int arg);
extern const VecFx32 data_02042270;
extern const VecFx32 data_02042264;

void Ov259_HelperUpdate(char *self, int arg)
{
    char *bone = 0;
    VecFx32 axis;
    Quat q;

    if (*(signed char *)(self + 0x1c6) == 3) {
        *(int *)(self + 0x39c) = 1;
    }
    if (*(signed char *)(self + 0x1c6) == 4) {
        *(int *)(self + 0x39c) = 0;
    }
    if (*(int *)(self + 0x39c) == 0) {
        bone = *(char **)(*(char **)(self + 0x394) + 0x40c);
        axis = data_02042270;
    }
    if (bone != 0) {
        Ov107_MoveNodeAndRelayout((Actor *)self, (VecFx32 *)(bone + 0x14));
        Quat_FromTwoVectors(&q, &data_02042264, &axis);
        Quat_Multiply(&q, (Quat *)(bone + 4), &q);
        Srt_SetRotationQuat(self + 0xa0, (Quat *)(bone + 4));
    }
    Ov107_ProcessObjectTick(self, arg);
    *(Pose *)(*(char **)(self + 0x390) + 0x10) = *(Pose *)(self + 0xa0);
    *(Pose *)(**(char ***)(self + 0x38c) + 0x10) = *(Pose *)(*(char **)(self + 0x390) + 0x10);
}
