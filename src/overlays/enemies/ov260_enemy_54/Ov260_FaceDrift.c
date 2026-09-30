/* Facing step of an ov260 part: when its +0x28 drift has a length the +0xa0 pose turns from the
 * rest axis to the drift direction; +0xf0 keeps the drift. */

#include "nitro/fx_types.h"

typedef struct { int x, y, z, w; } Quat;

extern int VEC_Normalize(VecFx32 *v, VecFx32 *out);
extern void Quat_FromTwoVectors(Quat *out, const VecFx32 *from, const VecFx32 *to);
extern void Srt_SetRotationQuat(char *srt, Quat *q);
extern const VecFx32 data_02042258;

void Ov260_FaceDrift(int *node)
{
    int *state = (int *)node[1];
    Quat q;
    VecFx32 dir;

    if (VEC_Normalize((VecFx32 *)(state + 10), &dir) != 0) {
        Quat_FromTwoVectors(&q, &data_02042258, &dir);
        Srt_SetRotationQuat((char *)(*state + 0xa0), &q);
    }
    *(VecFx32 *)(*state + 0xf0) = *(VecFx32 *)(state + 10);
}
