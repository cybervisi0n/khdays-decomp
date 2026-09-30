/* Shadow update (second form): mirror bit 1 of the +0x9c list's +0x5c into the +0x38c item's
 * +0x5c bit 1, place that item's transform (+4) on the actor's +0xa0 pose at the +0x74 position
 * lowered to y = 0x200, scaled in x/z by 1.0 - height(+0x13c)/20 (at least 1/16), push the
 * +0xa0 pose into the +0x3b8 clip's +4 slot and run the base update. */

#include "nitro/fx_types.h"

struct blk11 { int w[11]; };
struct Flags5c { int b0 : 1; int b1 : 1; };
extern void Ov107_ProcessObjectTick(char *obj, int arg1);
extern void Srt_SetRotationQuat(int srt, void *pose);
extern void Srt_SetTranslation(int srt, VecFx32 *pos);
extern void Srt_SetScaleXYZ(int srt, int sx, int sy, int sz);

void Ov236_UpdateShadowB(char *obj, int arg1) {
    VecFx32 pos;
    int scale;

    scale = 0x1000 - *(int *)(obj + 0x13c) / 20;
    if (scale < 0x100) scale = 0x100;
    pos = *(VecFx32 *)(obj + 0x74);
    pos.y = 0x200;
    ((struct Flags5c *)(*(int *)(obj + 0x38c) + 0x5c))->b1 = ((struct Flags5c *)(*(int *)(obj + 0x9c) + 0x5c))->b1;
    Srt_SetRotationQuat(*(int *)(obj + 0x38c) + 4, obj + 0xa0);
    Srt_SetTranslation(*(int *)(obj + 0x38c) + 4, &pos);
    Srt_SetScaleXYZ(*(int *)(obj + 0x38c) + 4, scale, 1, scale);
    *(struct blk11 *)(*(char **)(*(char **)(obj + 0x3b8)) + 4) = *(struct blk11 *)(obj + 0xa0);
    Ov107_ProcessObjectTick(obj, arg1);
}
