/* Dock entry of an ov259 helper: bits 2 and 0 of the owner's +0x60 high byte are set, the +0x38c
 * shape hides, the +0xc step resets and the node ends. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { unsigned f : 8; } B8;

extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const VecFx32 data_02041dc8;

void Ov259_HelperDockEntry(int *node)
{
    int *state = (int *)node[1];

    {
        u16 hw = *(u16 *)(*state + 0x60);
        *(u16 *)(*state + 0x60) = (hw & ~0xff00) |
            ((((((unsigned int)hw << 0x10) >> 0x18) | 4) << 0x18) >> 0x10);
    }
    {
        u16 hw = *(u16 *)(*state + 0x60);
        *(u16 *)(*state + 0x60) = (hw & ~0xff00) |
            ((((((unsigned int)hw << 0x10) >> 0x18) | 1) << 0x18) >> 0x10);
    }
    ((B8 *)(*(int *)(*state + 0x38c) + 8))->f &= ~1;
    *(VecFx32 *)(state + 3) = data_02041dc8;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}
