/* Bounce entry of the ov283 actor: the +0x10 velocity rests, +0x54 clears, sound 0x173/0xc plays at
 * the +8 point (020cc92c), pose 0xd plays, effects 2 and 3 fire there and the node moves on to
 * 020cdeac. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern void Ov283_PostItemUpdate(int actor, int bank, int variant, int at);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov283_AiBarrageStart(void);
extern const VecFx32 data_02041dc8;

void Ov283_BounceEntry(int *node)
{
    int *state = (int *)node[1];

    *(VecFx32 *)(state + 4) = data_02041dc8;
    state[0x15] = 0;
    Ov283_PostItemUpdate(*state, 0x173, 0xc, state[2]);
    Ov107_PostTagUpdate((Actor *)(*state), 0xd, 0);
    func_ov107_020c0b90(*state, 2, *(VecFx32 *)state[2], 0);
    func_ov107_020c0b90(*state, 3, *(VecFx32 *)state[2], 0);
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov283_AiBarrageStart);
}
