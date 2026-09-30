/* Ov253_StopEnter -- stop entry: raises bits 1 and 7 and clears bit 0 of the actor's +0x60
 * high byte, zeroes the state's +8 vector and moves the node to 020d407c. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const VecFx32 data_02041dc8;
extern void Ov253_Item_AiWaitActive(void);

void Ov253_StopEnter(int *node) {
    int *state = (int *)node[1];

    {
        u16 hw = *(u16 *)(*state + 0x60);
        *(u16 *)(*state + 0x60) = (hw & ~0xff00) |
            ((((((unsigned int)hw << 0x10) >> 0x18) | 0x82) << 0x18) >> 0x10);
    }
    {
        u16 hw = *(u16 *)(*state + 0x60);
        *(u16 *)(*state + 0x60) = (hw & ~0xff00) |
            (((unsigned int)(unsigned short)((((unsigned int)hw << 0x10) >> 0x18) & ~1) << 0x18) >> 0x10);
    }
    *(VecFx32 *)(state + 2) = data_02041dc8;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov253_Item_AiWaitActive);
}
