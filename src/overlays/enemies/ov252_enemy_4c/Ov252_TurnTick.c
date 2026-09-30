/* Turn tick of the ov252 actor: it faces the target (020cdfe8 0, 1); once the partner holds no queued
 * move bit 3 of the +0x60 high byte clears and bit 2 is set, and with a +0xa4 retreat pending pose 3
 * and part motion 2 start and the node moves on to 020d1428, else the next move is 4. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern int Ov252_CheckTarget(int *node, VecFx32 *delta, int face);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov252_ShedTick(void);

void Ov252_TurnTick(int *node)
{
    int *state = (int *)node[1];

    Ov252_CheckTarget(node, 0, 1);
    if (*(unsigned char *)(state[1] + 0xad) != 0) {
        return;
    }
    {
        u16 hw = *(u16 *)(*state + 0x60);
        *(u16 *)(*state + 0x60) = (hw & ~0xff00) |
            (((unsigned int)(u16)((((unsigned int)hw << 0x10) >> 0x18) & ~8) << 0x18) >> 0x10);
    }
    {
        u16 hw = *(u16 *)(*state + 0x60);
        *(u16 *)(*state + 0x60) = (hw & ~0xff00) |
            ((((((unsigned int)hw << 0x10) >> 0x18) | 4) << 0x18) >> 0x10);
    }
    if (state[0x29] != 0) {
        Ov107_PostTagUpdate((Actor *)(*state), 3, 0);
        Ov107_StartAnim(*(int *)(*state + 0x574), 2, 0);
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov252_ShedTick);
    } else {
        *(unsigned char *)(*state + 0x1c7) = 4;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
    }
}
