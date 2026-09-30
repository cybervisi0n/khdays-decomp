/* Per-frame model update: run the animator; while the current kind is 8 and bit 1 of the
 * +0x384 list's +0x5c word is set, finalize that list (0203c86c mode 1). Then place the +0x3f8
 * transform at the actor's +0xb0 position lowered by 0x2000 and pushed 0x2000 towards the
 * camera's +0x88 focus. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

struct ListFlags5c { int b0 : 1; int b1 : 1; };

extern void Ov107_ProcessObjectTick(int self, int);
extern void DispatchObjectCallbacks(int list, int a);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int VEC_Normalize(VecFx32 *v, VecFx32 *out);
extern void ScaleVec3Fx12(int scale, VecFx32 *v, VecFx32 *out);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern void SrtTransform_SetIdentity(void *srt);
extern void Srt_SetTranslation(void *srt, VecFx32 *translation);

void Ov273_UpdateModelTransform(int self, int delta) {
    VecFx32 dir;
    VecFx32 at;

    Ov107_ProcessObjectTick(self, delta);
    if (*(signed char *)(self + 0x100 + 0xc6) == 8) {
        if (((struct ListFlags5c *)(*(int *)(self + 0x384) + 0x5c))->b1 != 0) {
            DispatchObjectCallbacks(*(int *)(self + 0x384), 1);
        }
    }
    at = *(VecFx32 *)(self + 0xb0);
    at.y -= 0x2000;
    VEC_Subtract((VecFx32 *)(*(char **)Ov107_GetActorManager() + 0x88), &at, &dir);
    VEC_Normalize(&dir, &dir);
    ScaleVec3Fx12(0x2000, &dir, &dir);
    VEC_Add(&at, &dir, &at);
    SrtTransform_SetIdentity((void *)(self + 0x3f8));
    Srt_SetTranslation((void *)(self + 0x3f8), &at);
}
