/* Charge entry of the ov226 enemy: raises bit 7 of the owner's +0x60 high byte and bit 1 of
 * the +4 item's +0x5c, clears the owner's +0x38c, clears bit 0 of the +0x60 high byte and of
 * the +0x388 sub-item's +8 low byte, zeroes the +0x18 velocity and hands the tick over to
 * Ov226_ChargeIdleStep. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

struct hw60 { unsigned short lo : 8, hi : 8; };
struct w8 { unsigned int lo : 8, rest : 24; };

extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const VecFx32 data_02041dc8;
extern void Ov226_ChargeIdleStep(void);

void Ov226_EnterCharge(int *node) {
    int *state = (int *)node[1];
    VecFx32 zero = data_02041dc8;

    {
        u16 hw = *(u16 *)(*state + 0x60);
        *(u16 *)(*state + 0x60) = (hw & ~0xff00) |
            ((((((unsigned int)hw << 0x10) >> 0x18) | 0x80) << 0x18) >> 0x10);
    }
    *(int *)(state[1] + 0x5c) |= 2;
    *(int *)(*state + 0x38c) = 0;
    ((struct hw60 *)(*state + 0x60))->hi &= ~1;
    ((struct w8 *)(*(int *)(*state + 0x388) + 8))->lo &= ~1;
    *(VecFx32 *)(state + 6) = zero;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov226_ChargeIdleStep);
}
