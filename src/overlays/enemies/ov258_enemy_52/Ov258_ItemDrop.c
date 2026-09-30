/* Drop of the ov258 held item: the item is placed at the owner's (+0x390) left (+0x43c) or right
 * (+0x448) hand point by its +0x38c side flag; an item flagged in +0x40 bit 1 runs its +0xc release
 * callback, and the next move is 1. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { int b0 : 1; int b1 : 1; } Bits;
typedef void (*ReleaseCb)(char *item, int arg);

extern void Ov107_MoveNodeAndRelayout(char *owner, const VecFx32 *pos);

void Ov258_ItemDrop(int *state)
{
    char *item = (char *)*state;

    if (*(u8 *)(item + 0x38c) == 0) {
        Ov107_MoveNodeAndRelayout(item, (VecFx32 *)(*(int *)(*(int *)(item + 0x390) + 0x43c) + 0x14));
    } else {
        Ov107_MoveNodeAndRelayout(item, (VecFx32 *)(*(int *)(*(int *)(item + 0x390) + 0x448) + 0x14));
    }
    item = (char *)*state;
    if (((Bits *)(item + 0x40))->b1 && *(ReleaseCb *)(item + 0xc) != 0) {
        (*(ReleaseCb *)(item + 0xc))(item, 0);
    }
    *(signed char *)(*state + 0x1c7) = 1;
}
