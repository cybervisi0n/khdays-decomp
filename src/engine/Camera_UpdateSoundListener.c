/* Feeds the active camera part's position and view direction to the sound listener. */

#include "nitro/fx_types.h"
#include "game/engine.h"

extern void VEC_Subtract(VecFx32 *a, VecFx32 *b, VecFx32 *out);

typedef struct {
    char _00[4];
    char part0[0x48];
    char part1[0xac];
    int selector;
} Obj02020e20;

void Camera_UpdateSoundListener(Obj02020e20 *obj)
{
    char *base;
    VecFx32 tmp;

    if (obj->selector == 1) {
        base = obj->part1;
    } else {
        base = obj->part0;
    }

    VEC_Subtract((VecFx32 *)(base + 0x14), (VecFx32 *)(base + 0x20), &tmp);
    SoundMgr_SetListener((VecFx32 *)(base + 0x20), &tmp, (VecFx32 *)(base + 0x2c));
}
