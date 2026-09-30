/* Fall tick of the ov260 actor: the +0x20 velocity takes the +0x2c one, whose y then loses 1/32 per
 * frame. On landing (+0x17a bit 0) the owner's +0x180 point is kept in +0x54, lowered by the +0x80
 * floor, bit 6 of the +0x60 high byte clears, effect 6 spawns there, move 0xb starts at it (020cd148),
 * pose 0x11 plays, the +0x70/+0x74 clocks and the +0x79/+0x7b flags reset and 020cee94 runs next. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

struct Flag17a { u8 b0 : 1; u8 b1 : 1; };

extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern void Ov260_PlaySound(int owner, int mode, int arg);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov260_StompTick(void);

void Ov260_TickFall(int *node)
{
    int *state = (int *)node[1];

    *(VecFx32 *)(state + 8) = *(VecFx32 *)(state + 0xb);
    state[0xc] -= 0x80;
    if (((struct Flag17a *)(*state + 0x17a))->b0 != 0) {
        *(VecFx32 *)(state + 0x15) = *(VecFx32 *)(*state + 0x180);
        state[0x16] -= *(int *)(*state + 0x80);
        {
            u16 hw = *(u16 *)(*state + 0x60);

            *(u16 *)(*state + 0x60) = (hw & ~0xff00) |
                (((unsigned int)(unsigned short)((((unsigned int)hw << 0x10) >> 0x18) & ~0x40) << 0x18) >> 0x10);
        }
        func_ov107_020c0b90(*state, 6, *(VecFx32 *)(state + 0x15), 0);
        Ov260_PlaySound(*state, 0xb, (int)(state + 0x15));
        Ov107_PostTagUpdate((Actor *)(*state), 0x11, 0);
        state[0x1c] = 0;
        state[0x1d] = 0;
        *((u8 *)state + 0x79) = 0;
        *((u8 *)state + 0x7b) = 0;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov260_StompTick);
        return;
    }
}
