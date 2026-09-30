/* Start of the ov283 actor's brain: no move is current or next, the +0x388 shape hides, +0x50 clears,
 * the +8 point tracks the actor's +0xb0 position (copied to +0x28), bits 1-2 of the +0x60 high byte are
 * set and the three brain slots start (020cd108 in slot 1, 020ccdfc in slot 0, 020ccfb8 in slot 2). */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { unsigned f : 8; } B8;

extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov283_stateSetFlagsClearBit(void);
extern void Ov283_AiDispatchAction(void);
extern void Ov283_PhysicsTick(void);

void Ov283_BrainStart(int *node)
{
    int *state = (int *)node[1];

    *(unsigned char *)(*state + 0x1c6) = 0;
    *(signed char *)(*state + 0x1c7) = -1;
    ((B8 *)(*(int *)(*state + 0x388) + 8))->f &= ~1;
    state[0x14] = 0;
    state[2] = *state + 0xb0;
    *(VecFx32 *)(state + 0xa) = *(VecFx32 *)(*state + 0xb0);
    {
        u16 hw = *(u16 *)(*state + 0x60);
        *(u16 *)(*state + 0x60) = (hw & ~0xff00) |
            ((((((unsigned int)hw << 0x10) >> 0x18) | 6) << 0x18) >> 0x10);
    }
    SetIndexedSlot(node, 1, Ov283_stateSetFlagsClearBit);
    SetIndexedSlot(node, 0, Ov283_AiDispatchAction);
    SetIndexedSlot(node, 2, Ov283_PhysicsTick);
}
