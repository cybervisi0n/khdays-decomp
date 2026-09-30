/* Dash entry of the ov223 enemy: raises bit 7 of the owner's +0x60 high byte, clears the
 * owner's +0x388, clears bit 0 of the +0x60 high byte, zeroes the +0x14 velocity and +0x40,
 * and hands the tick over to Ov223_DashIdleStep. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

struct hw60 { unsigned short lo : 8, hi : 8; };

extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const VecFx32 data_02041dc8;
extern void Ov223_DashIdleStep(int *node);

void Ov223_EnterDash(int *node)
{
    int *state = (int *)node[1];
    VecFx32 zero = data_02041dc8;
    u16 flags;

    flags = *(u16 *)(*state + 0x60);
    *(u16 *)(*state + 0x60) = (u16)((flags & ~0xff00) | (((((unsigned int)flags << 0x10) >> 0x18 | 0x80) << 0x18) >> 0x10));
    *(int *)(*state + 0x388) = 0;
    ((struct hw60 *)(*state + 0x60))->hi &= ~1;
    *(VecFx32 *)(state + 5) = zero;
    state[0x10] = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov223_DashIdleStep);
}
