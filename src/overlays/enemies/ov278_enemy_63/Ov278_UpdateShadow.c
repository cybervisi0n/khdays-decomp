/* Shadow update: refresh the +0x3c8 sub-object, then place the +0x390 item's transform (+4) on
 * the actor's +0xa0 pose at the +0x74 position lowered to y = 0x200, scaled in x/z by
 * 1.0 - height(+0x13c)/20 (at least 1/16), and run the base update. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern void Srt_SetRotationQuat(int srt, void *pose);
extern void Srt_SetTranslation(int srt, VecFx32 *pos);
extern void Srt_SetScaleXYZ(int srt, int sx, int sy, int sz);
extern void Ov107_ProcessObjectTick(char *obj, int arg1);

void Ov278_UpdateShadow(char *obj, int arg1) {
    VecFx32 pos;
    int scale;

    Ov107_RefreshAndSelectChild(*(int *)(obj + 0x3c8), arg1);
    scale = 0x1000 - *(int *)(obj + 0x13c) / 20;
    if (scale < 0x100) scale = 0x100;
    pos = *(VecFx32 *)(obj + 0x74);
    pos.y = 0x200;
    Srt_SetRotationQuat(*(int *)(obj + 0x390) + 4, obj + 0xa0);
    Srt_SetTranslation(*(int *)(obj + 0x390) + 4, &pos);
    Srt_SetScaleXYZ(*(int *)(obj + 0x390) + 4, scale, 1, scale);
    Ov107_ProcessObjectTick(obj, arg1);
}
