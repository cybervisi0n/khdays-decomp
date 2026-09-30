/* Model pose init of the ov163 enemy (x3: ov163/164/165), variant of the matched ov202 sibling:
 * copies the +0x3c0 item's transform into +0x394, then moves its translation 0x800 towards the
 * camera's +0x88 focus (direction normalised from the pose position). */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

typedef struct {
    int data[11];
} Mat;

typedef struct {
    char pad0[0x394];
    Mat mat;
    char *src;
} Obj;

extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int VEC_Normalize(VecFx32 *v, VecFx32 *out);
extern void ScaleVec3Fx12(int scale, VecFx32 *v, VecFx32 *out);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern void Srt_SetTranslation(Mat *mat, VecFx32 *translation);

void Ov165_InitModelPose(Obj *obj, int flag) {
    VecFx32 at;
    VecFx32 dir;

    Ov107_AiState_DispatchModelCallbacks(obj, flag);

    obj->mat = *(Mat *)(obj->src + 4);

    VEC_Subtract((VecFx32 *)(*(char **)Ov107_GetActorManager() + 0x88), (VecFx32 *)((char *)&obj->mat + 16), &dir);
    VEC_Normalize(&dir, &dir);
    ScaleVec3Fx12(0x800, &dir, &at);
    VEC_Add((VecFx32 *)((char *)&obj->mat + 16), &at, &at);
    Srt_SetTranslation(&obj->mat, &at);
}
