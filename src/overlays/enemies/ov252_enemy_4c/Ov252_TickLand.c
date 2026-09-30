/* Land tick of the ov252 actor: the +0xc velocity follows the +0x574 part's +0x2c vector turned by the
 * +0x54 heading; once the partner holds no queued move pose 0x16 plays, the part takes motion 0x15,
 * bit 3 of the +0x60 high byte is set and bit 2 clears, +0x64 clears and the node moves on to
 * 020d0c28. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern VecFx32 Ov252_TurnVecY(int angle, VecFx32 *vec);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov252_DriftOutEntryTick(void);

void Ov252_TickLand(int *node)
{
    int *state = (int *)node[1];
    VecFx32 v;

    v = Ov252_TurnVecY(state[0x15], (VecFx32 *)(*(int *)(*state + 0x574) + 0x2c));
    *(VecFx32 *)(state + 3) = v;
    if (*(unsigned char *)(state[1] + 0xad) == 0) {
        Ov107_PostTagUpdate((Actor *)(*state), 0x16, 0);
        Ov107_StartAnim(*(int *)(*state + 0x574), 0x15, 0);
        {
            u16 hw = *(u16 *)(*state + 0x60);
            *(u16 *)(*state + 0x60) = (hw & ~0xff00) |
                ((((((unsigned int)hw << 0x10) >> 0x18) | 8) << 0x18) >> 0x10);
        }
        {
            u16 hw = *(u16 *)(*state + 0x60);
            *(u16 *)(*state + 0x60) = (hw & ~0xff00) |
                (((unsigned int)(u16)((((unsigned int)hw << 0x10) >> 0x18) & ~4) << 0x18) >> 0x10);
        }
        state[0x19] = 0;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov252_DriftOutEntryTick);
        return;
    }
}
