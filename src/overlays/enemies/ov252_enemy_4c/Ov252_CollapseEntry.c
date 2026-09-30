/* Collapse entry of the ov252 actor: poses 0x2f, 0x31 and 0x35 are stacked, bits 1 and 7 of the +0x60
 * high byte are set, the +0xc velocity rests and the node moves on to 020cf150. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov252_RecoverTick(void);
extern const VecFx32 data_02041dc8;

void Ov252_CollapseEntry(int *node)
{
    int *state = (int *)node[1];

    Ov107_PostTagUpdate((Actor *)(*state), 0x2f, 0);
    Ov107_PostTagUpdate((Actor *)(*state), 0x31, 0);
    Ov107_PostTagUpdate((Actor *)(*state), 0x35, 0);
    {
        u16 hw = *(u16 *)(*state + 0x60);
        *(u16 *)(*state + 0x60) = (hw & ~0xff00) |
            ((((((unsigned int)hw << 0x10) >> 0x18) | 0x82) << 0x18) >> 0x10);
    }
    *(VecFx32 *)(state + 3) = data_02041dc8;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov252_RecoverTick);
}
