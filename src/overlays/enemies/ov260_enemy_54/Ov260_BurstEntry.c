/* Burst entry of the ov260 actor: it is knocked back at its own +0x74 position (mode 3), effect 0xc
 * starts there, bits 1 and 7 of the +0x60 high byte are set, +0x70 clears and the node moves on to
 * 020cf518. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern void Ov260_PlaySound(int owner, int mode, int arg);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov260_SettleTick(void);

void Ov260_BurstEntry(int *node)
{
    int *state = (int *)node[1];

    func_ov107_020c0b90(*state, 3, *(VecFx32 *)(*state + 0x74), 0);
    Ov260_PlaySound(*state, 0xc, *state + 0x74);
    {
        u16 hw = *(u16 *)(*state + 0x60);
        *(u16 *)(*state + 0x60) = (hw & ~0xff00) |
            ((((((unsigned int)hw << 0x10) >> 0x18) | 0x82) << 0x18) >> 0x10);
    }
    state[0x1c] = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov260_SettleTick);
}
