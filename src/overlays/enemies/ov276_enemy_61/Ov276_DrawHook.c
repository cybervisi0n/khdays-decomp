/* Draw hook of the ov276 enemy: releases the +0x470 item, runs the base draw and then anchors
 * the +0x474 point either at the +0x3d0 spot or, while bit 7 of the +0x60 flag is set, at the
 * +0xb0 position raised by 0x10cc; the +0x480 word takes the overlay constant. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

struct hw60 { unsigned short lo : 8, hi : 8; };

extern void Ov107_ProcessObjectTick(int actor, int arg);

void Ov276_DrawHook(int actor, int arg)
{
    Ov107_RefreshAndSelectChild(*(int *)(actor + 0x470), arg);
    Ov107_ProcessObjectTick(actor, arg);
    if ((((struct hw60 *)(actor + 0x60))->lo & 0x80) != 0) {
        *(VecFx32 *)(actor + 0x474) = *(VecFx32 *)(actor + 0xb0);
        *(int *)(actor + 0x478) += 0x10cc;
    } else {
        *(VecFx32 *)(actor + 0x474) = *(VecFx32 *)(actor + 0x3d0);
    }
    *(int *)(actor + 0x480) = 0x10cc;
}
