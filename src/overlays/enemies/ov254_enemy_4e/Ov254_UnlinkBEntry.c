/* Move entry: the actor's +0x390 link clears, bit 7 of its +0x60 high byte is set and bit 0
 * cleared, the +0x388 shape is disarmed, the +0x1c velocity resets to zero and the node moves to
 * 020d3408. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { unsigned f : 8; } B8;

extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const VecFx32 data_02041dc8;
extern void Ov254_UnlinkBEntryIdleStep(void);

void Ov254_UnlinkBEntry(int *node)
{
    int *state = (int *)node[1];

    *(int *)(*state + 0x390) = 0;
    {
        u16 hw = *(u16 *)(*state + 0x60);
        *(u16 *)(*state + 0x60) = (hw & ~0xff00) |
            ((((((unsigned int)hw << 0x10) >> 0x18) | 0x80) << 0x18) >> 0x10);
    }
    {
        int actor = *state;
        u16 hw = *(u16 *)(actor + 0x60);
        *(u16 *)(actor + 0x60) = (hw & ~0xff00) |
            (((unsigned int)(unsigned short)((((unsigned int)hw << 0x10) >> 0x18) & ~1) << 0x18) >> 0x10);
    }
    ((B8 *)(*(int *)(*state + 0x388) + 8))->f &= ~1;
    *(VecFx32 *)(state + 7) = data_02041dc8;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov254_UnlinkBEntryIdleStep);
}
