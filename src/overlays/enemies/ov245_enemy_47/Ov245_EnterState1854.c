/* Ov245_EnterState1854 -- entry of the sub-state: raises bit 7 and clears bit 0 of the actor's
 * +0x60 high byte, clears bit 0 of the +0x388 item's +8 low byte, zeroes the state's +0xc vector
 * and the actor's +0x390, and installs 020d1854 in the node's slot. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

struct hw60 { unsigned short lo : 8, hi : 8; };
struct w8 { unsigned int lo : 8, rest : 24; };

extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const VecFx32 data_02041dc8;
extern void Ov245_IdleStep(void);

void Ov245_EnterState1854(int *node) {
    int *state = (int *)node[1];
    VecFx32 zero = data_02041dc8;

    {
        u16 hw = *(u16 *)(*state + 0x60);
        *(u16 *)(*state + 0x60) = (hw & ~0xff00) |
            ((((((unsigned int)hw << 0x10) >> 0x18) | 0x80) << 0x18) >> 0x10);
    }
    ((struct hw60 *)(*state + 0x60))->hi &= ~1;
    ((struct w8 *)(*(int *)(*state + 0x388) + 8))->lo &= ~1;
    *(VecFx32 *)(state + 3) = zero;
    *(int *)(*state + 0x390) = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov245_IdleStep);
}
