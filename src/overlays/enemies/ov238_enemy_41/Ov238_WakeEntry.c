/* Wake entry of the ov238 actor: its guard flag (+0x60 bit 0) is set, the +0x388 shape shows, effect 0
 * fires at the origin, the +0xc velocity rests, +0x2c clears and the node moves on to 020d2dd0. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { unsigned f : 8; } B8;

extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov238_Partner_AiWaitRelease(void);
extern const VecFx32 data_02041dc8;

void Ov238_WakeEntry(int *node)
{
    int *state = (int *)node[1];
    VecFx32 zero = data_02041dc8;

    {
        u16 hw = *(u16 *)(*state + 0x60);
        *(u16 *)(*state + 0x60) = (hw & ~0xff00) |
            ((((((unsigned int)hw << 0x10) >> 0x18) | 1) << 0x18) >> 0x10);
    }
    ((B8 *)(*(int *)(*state + 0x388) + 8))->f |= 1;
    func_ov107_020c0b90(*state, 0, zero, 0);
    *(VecFx32 *)(state + 3) = zero;
    state[0xb] = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov238_Partner_AiWaitRelease);
}
