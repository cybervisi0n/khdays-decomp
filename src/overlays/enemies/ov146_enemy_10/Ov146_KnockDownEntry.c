/* Knock-down entry of the ov146 actor: its partner (+8) is released from mode 0 and, without a partner
 * guard (+0x58), the actor plays effect 1 at the partner's +0x74 point; bit 0 of the +0x60 high byte
 * clears, bits 0-1 of +0x1ae and bits 1, 2 and 7 of the high byte are set, the +0x3ac shape hides,
 * sound 0/0x4a plays at the actor, the +0x3bc effect stops, the next move is 0 and the node ends. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { unsigned f : 8; } B8;

extern int Ov146_Rider_SetFlagIfReady(int a, int b);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern void Ov107_BuildAndSendUpdate(int actor, int bank, int variant, void *at);
extern int Ov146_Mount_SetStateIfReady(int param_1, int param_2);
extern void SetIndexedSlot(int *node, int slot, void *cb);

void Ov146_KnockDownEntry(int *node)
{
    int *state = (int *)node[1];

    Ov146_Rider_SetFlagIfReady(state[2], 0);
    if (state[0x16] == 0) {
        func_ov107_020c0b90(*state, 1, *(VecFx32 *)(state[2] + 0x74), 0);
    }
    {
        u16 hw = *(u16 *)(*state + 0x60);
        *(u16 *)(*state + 0x60) = (hw & ~0xff00) |
            (((unsigned int)(u16)((((unsigned int)hw << 0x10) >> 0x18) & ~1) << 0x18) >> 0x10);
    }
    *(u16 *)(*state + 0x1ae) |= 3;
    {
        u16 hw = *(u16 *)(*state + 0x60);
        *(u16 *)(*state + 0x60) = (hw & ~0xff00) |
            ((((((unsigned int)hw << 0x10) >> 0x18) | 0x86) << 0x18) >> 0x10);
    }
    ((B8 *)(*(int *)(*state + 0x3ac) + 8))->f &= ~1;
    Ov107_BuildAndSendUpdate(*state, 0, 0x4a, (void *)(*state + 0x74));
    Ov146_Mount_SetStateIfReady(*(int *)(*state + 0x3bc), 0);
    *(unsigned char *)(*state + 0x1c7) = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}
