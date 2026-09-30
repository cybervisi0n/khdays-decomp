/* Place an ov260 part at `at` (020c5c54), tell its +0xc handler when +0x40 bit 1 allows it, restart
 * the +0x384 model's frame, store the +0x3b0 and +0x3a4 anchors and set bit 0 of the +0x60 high
 * byte. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

struct Flags40 { int b0 : 1; int b1 : 1; };

extern void RefreshObjectCallbacks(int item, int a);

void Ov260_PlacePartSpan(char *self, VecFx32 *at, VecFx32 *from, VecFx32 *to)
{
    Ov107_MoveNodeAndRelayout((Actor *)self, at);
    if (((struct Flags40 *)(self + 0x40))->b1 && *(void (**)(char *, int))(self + 0xc) != 0) {
        (*(void (**)(char *, int))(self + 0xc))(self, 0);
    }
    RefreshObjectCallbacks(*(int *)(self + 0x384), 0);
    *(VecFx32 *)(self + 0x3b0) = *from;
    *(VecFx32 *)(self + 0x3a4) = *to;
    {
        u16 hw = *(u16 *)(self + 0x60);
        *(u16 *)(self + 0x60) = (hw & ~0xff00) |
            ((((((unsigned int)hw << 0x10) >> 0x18) | 1) << 0x18) >> 0x10);
    }
}
