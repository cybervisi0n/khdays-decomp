/* Charge entry of the ov223 enemy: raises bit 0 of the owner's +0x60 high byte, copies the +4
 * point to +8, clears +0x3c and sets +0x40 to 1.0, then hands the tick over by the +0x48
 * variant: 0 to Ov223_RingSweepChargeTick, 1 to Ov223_RingChargeTick. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov223_RingSweepChargeTick(int *node);
extern void Ov223_RingChargeTick(int *node);

void Ov223_EnterCharge(int *node)
{
    int *state = (int *)node[1];
    u16 flags;

    flags = *(u16 *)(*state + 0x60);
    *(u16 *)(*state + 0x60) = (u16)((flags & ~0xff00) | (((((unsigned int)flags << 0x10) >> 0x18 | 1) << 0x18) >> 0x10));
    *(VecFx32 *)(state + 2) = *(VecFx32 *)state[1];
    state[0xf] = 0;
    state[0x10] = 0x1000;
    switch (state[0x12]) {
    case 0:
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov223_RingSweepChargeTick);
        break;
    case 1:
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov223_RingChargeTick);
        break;
    }
}
