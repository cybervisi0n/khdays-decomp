/* Draw pre-pass of the ov257 enemy (+0xc): the first +0x3b4 shape and the +0x3b8 shape take the
 * +0x3d4 part's pose and the +0x3bc shape the +0x3d8 part's; each of the four +0x3c0 segment
 * items gets a pose placed at its +0x3dc bone and turned to face its +0x3ec bone. While the kind
 * (+0x1c6) is not 0xc a running pair-6 effect is stopped; then the common draw handler runs. The
 * bone arrays are indexed as ((int *)self)[base + i] for the ROM's addressing. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

typedef struct { int w[4]; } Quat;
typedef struct { int w[11]; } Srt;
struct Part { char pad[0x10]; Srt pose; };
struct Rig { int pad; Srt srt; };

extern void SrtTransform_SetIdentity(Srt *srt);
extern void VEC_Subtract(const void *a, const void *b, VecFx32 *out);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern void Quat_FromTwoVectors(Quat *out, const VecFx32 *a, const VecFx32 *b);
extern void Srt_SetTranslation(Srt *srt, const void *pos);
extern void Srt_SetRotationQuat(Srt *srt, const Quat *q);
extern void TaskList_FinishByTag(int model, int handle);
extern const VecFx32 data_02042258;

void Ov257_DrawPrePass2(char *self)
{
    Srt srt;
    VecFx32 d;
    Quat q;
    int i;

    (**(struct Part ***)(self + 0x3b4))->pose = (*(struct Rig **)(self + 0x3d4))->srt;
    (*(struct Part **)(self + 0x3b8))->pose = (*(struct Rig **)(self + 0x3d4))->srt;
    (*(struct Part **)(self + 0x3bc))->pose = (*(struct Rig **)(self + 0x3d8))->srt;
    for (i = 0; i < 4; i++) {
        SrtTransform_SetIdentity(&srt);
        VEC_Subtract((void *)(((int *)self)[0xfb + i] + 0x14), (void *)(((int *)self)[0xf7 + i] + 0x14), &d);
        VEC_Normalize(&d, &d);
        Quat_FromTwoVectors(&q, &data_02042258, &d);
        Srt_SetTranslation(&srt, (void *)(((int *)self)[0xf7 + i] + 0x14));
        Srt_SetRotationQuat(&srt, &q);
        ((struct Part **)self)[0xf0 + i]->pose = srt;
    }
    if (*(signed char *)(self + 0x100 + 0xc6) != 0xc && *(int *)(*(int *)(self + 0x400) + 0x34) != 0) {
        TaskList_FinishByTag(*(int *)(self + 0x3c), *(int *)(*(int *)(self + 0x400) + 0x34));
        *(int *)(*(int *)(self + 0x400) + 0x34) = 0;
    }
    Ov107_AiState_PostTickBase(self);
}
