/* Dock entry of an ov256 claw: +0x1c and the +0x20 / +0x21 flags clear, bits 1-3 of the owner's
 * +0x60 high byte drop and bit 0 is set, the +0x388 shape shows, the +0xc velocity rests and the node
 * moves on to 020d0934. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { unsigned f : 8; } B8;

extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov256_HomingOrbTick(void);
extern const VecFx32 data_02041dc8;

void Ov256_ClawDockEntry(int *node)
{
    int *state = (int *)node[1];

    state[7] = 0;
    *((u8 *)state + 0x20) = 0;
    *((u8 *)state + 0x21) = 0;
    {
        u16 hw = *(u16 *)(*state + 0x60);
        *(u16 *)(*state + 0x60) = (hw & ~0xff00) |
            (((unsigned int)(u16)((((unsigned int)hw << 0x10) >> 0x18) & ~0xe) << 0x18) >> 0x10);
    }
    {
        u16 hw = *(u16 *)(*state + 0x60);
        *(u16 *)(*state + 0x60) = (hw & ~0xff00) |
            ((((((unsigned int)hw << 0x10) >> 0x18) | 1) << 0x18) >> 0x10);
    }
    ((B8 *)(*(int *)(*state + 0x388) + 8))->f |= 1;
    *(VecFx32 *)(state + 3) = data_02041dc8;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov256_HomingOrbTick);
}
