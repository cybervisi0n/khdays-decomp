/* Hover entry of the ov260 actor: pose 0xf plays, its +0x428 part takes motion 7, the +0x2c push
 * becomes (0, 0.25, 0) and is copied to +0x20, effect 0x1f starts at the +0x10 point (020cd148) and
 * the node moves on to 020cec98. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern void Ov260_PlaySound(int owner, int mode, int arg);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov260_FallTick(void);

void Ov260_HoverEntry(int *node)
{
    int *state = (int *)node[1];

    Ov107_PostTagUpdate((Actor *)(*state), 0xf, 0);
    Ov107_StartAnim(*(int *)(*state + 0x428), 7, 0);
    state[0xb] = 0;
    state[0xc] = 0x400;
    state[0xd] = 0;
    *(VecFx32 *)(state + 8) = *(VecFx32 *)(state + 0xb);
    Ov260_PlaySound(*state, 0x1f, state[4]);
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov260_FallTick);
}
