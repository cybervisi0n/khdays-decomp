/* Move entry: bit 0 of the actor's +0x60 high byte clears and bit 7 is set, the +0x388
 * shape is disarmed (bit 0 of its +8 byte), the +0x10 velocity resets to zero and the node moves
 * to 020d1228. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { unsigned f : 8; } B8;

extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const VecFx32 data_02041dc8;
extern void Ov218_AiStep_QueueAction1IfActive(void);

void Ov218_HaltEntry(int *node)
{
    int *state = (int *)node[1];

    {
        int actor = *state;
        u16 hw = *(u16 *)(actor + 0x60);
        *(u16 *)(actor + 0x60) = (hw & ~0xff00) |
            (((unsigned int)(unsigned short)((((unsigned int)hw << 0x10) >> 0x18) & ~1) << 0x18) >> 0x10);
    }
    {
        u16 hw = *(u16 *)(*state + 0x60);
        *(u16 *)(*state + 0x60) = (hw & ~0xff00) |
            ((((((unsigned int)hw << 0x10) >> 0x18) | 0x80) << 0x18) >> 0x10);
    }
    ((B8 *)(*(int *)(*state + 0x388) + 8))->f &= ~1;
    *(VecFx32 *)(state + 4) = data_02041dc8;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov218_AiStep_QueueAction1IfActive);
}
