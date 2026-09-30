/* Per-frame update of the ov245 enemy's claws: outside move 1 the +0x3b8 scene link is released
 * (0203c650 on the +0x3c scene) and the +0x3ac effect freed (020cb100). Each claw (the +0x38c part,
 * then the one *+0x388 points to) copies the +0x394 body transform into its +0x10 transform and is
 * pulled back along its +0x64 direction by its +0x70 reach, turned by that transform. The base
 * update (020c7ca4) follows. */

#include "nitro/fx_types.h"

typedef struct { int w[11]; } SrtTransform;

extern void TaskList_FinishByTag(int scene, int link);
extern void Ov107_UnlinkNodeFromOwner(int effect);
extern void Vec3TransformViaTempMtx(VecFx32 *out, void *rotation, const VecFx32 *in);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void VEC_Subtract(const void *a, const void *b, VecFx32 *out);
extern void Srt_SetTranslation(void *srt, const VecFx32 *t);
extern void Ov107_AiState_PostTickBase(char *self);

static inline int ClawPart(char *self, int first)
{
    return first ? *(int *)(self + 0x38c) : **(int **)(self + 0x388);
}

static inline SrtTransform *ClawXf(char *self, int first)
{
    return first ? (SrtTransform *)(*(int *)(self + 0x38c) + 0x10) : (SrtTransform *)(**(int **)(self + 0x388) + 0x10);
}

void Ov245_UpdateClaws(char *self)
{
    VecFx32 v;
    int k;
    int part;
    char *xf;

    if (*(signed char *)(self + 0x100 + 0xc6) != 1) {
        if (*(int *)(self + 0x3b8) != 0) {
            TaskList_FinishByTag(*(int *)(self + 0x3c), *(int *)(self + 0x3b8));
            *(int *)(self + 0x3b8) = 0;
        }
        if (*(int *)(self + 0x3ac) != 0) {
            Ov107_UnlinkNodeFromOwner(*(int *)(self + 0x3ac));
            *(int *)(self + 0x3ac) = 0;
        }
    }
    for (k = 0; k < 2; k++) {
        int first = k == 0;

        part = ClawPart(self, first);
        xf = (char *)ClawXf(self, first);
        *(SrtTransform *)xf = *(SrtTransform *)(*(int *)(self + 0x394) + 4);
        Vec3TransformViaTempMtx(&v, xf, (VecFx32 *)(part + 0x64));
        ScaleVec3Fx12(*(int *)(part + 0x70), &v, &v);
        VEC_Subtract(xf + 0x10, &v, &v);
        Srt_SetTranslation(xf, &v);
    }
    Ov107_AiState_PostTickBase(self);
}
