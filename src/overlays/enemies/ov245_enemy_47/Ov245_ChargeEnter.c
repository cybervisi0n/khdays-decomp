/* Ov245_ChargeEnter -- charge entry: raises bit 7 and clears bit 0 of the actor's +0x60 high
 * byte, clears bit 0 of the +0x388 item's +8 low byte, zeroes the state's +0xc vector, keeps
 * the +8 origin at +0x34, clears the +0x30 timer and installs the charge delay (020ceef0). */

#include "nitro/types.h"
#include "nitro/fx_types.h"

struct hw60 { unsigned short lo : 8, hi : 8; };
struct w8 { unsigned int lo : 8, rest : 24; };

extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const VecFx32 data_02041dc8;
extern void Ov245_ChargeDelay(void);

void Ov245_ChargeEnter(int *node) {
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
    *(VecFx32 *)(state + 0xd) = *(VecFx32 *)state[2];
    state[0xc] = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov245_ChargeDelay);
}
