/* Update handler of the ov255 enemy (+8): the +0x3a4 part and the common update advance by dt
 * (0 while bit 1 of +0x1ac is set); the +0x3bc pose then follows the +0x3b8 part's pose, turned by
 * the rotation from data_02042270 to data_0204227c. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

typedef struct { int w[4]; } Quat;
typedef struct { int w[11]; } Srt;

extern void Ov107_ProcessObjectTick(char *self, int dt);
extern void Quat_FromTwoVectors(Quat *out, const VecFx32 *a, const VecFx32 *b);
extern void Quat_Multiply(Quat *out, const void *a, const Quat *b);
extern void Srt_SetRotationQuat(void *srt, const Quat *q);
extern const VecFx32 data_02042270;
extern const VecFx32 data_0204227c;

void Ov255_Update(char *self, int dt)
{
    Quat turned;
    Quat turn;

    if (*(unsigned short *)(self + 0x1ac) & 2) {
        dt = 0;
    }
    Ov107_RefreshAndSelectChild(*(int *)(self + 0x3a4), dt);
    Ov107_ProcessObjectTick(self, dt);
    *(Srt *)(self + 0x3bc) = *(Srt *)(*(int *)(self + 0x3b8) + 4);
    Quat_FromTwoVectors(&turn, &data_02042270, &data_0204227c);
    Quat_Multiply(&turned, self + 0x3bc, &turn);
    Srt_SetRotationQuat(self + 0x3bc, &turned);
}
