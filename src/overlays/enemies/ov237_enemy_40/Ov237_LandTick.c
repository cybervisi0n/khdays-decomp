/* Land tick of the ov237 actor: the +0x3c aim point resets to data_ov237_020d1be8; once the +4 rig
 * is idle (or the +0x17a bit 3 lands early) the actor is placed at its +0x4c4 point at the +0x38
 * point's height with effect 0x12, +0x4bc is set, pose 0x10 plays and the brain waits on 020d06e0. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

typedef struct { u8 b0 : 1; u8 b1 : 1; u8 b2 : 1; u8 b3 : 1; } Bits;

extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov237_TickRejoin(void);
extern const VecFx32 data_ov237_020d1be8;

void Ov237_LandTick(int *node)
{
    int *state = (int *)node[1];
    VecFx32 pos;

    *(VecFx32 *)(state + 0xf) = data_ov237_020d1be8;
    if (*(u8 *)(state[1] + 0xad) != 0 && !((Bits *)(*state + 0x17a))->b3) {
        return;
    }
    if (*(int *)(*state + 0x4ac) != 0) {
        pos = *(VecFx32 *)(*state + 0x4c4);
        pos.y = ((VecFx32 *)state[0xe])->y;
    } else if (*(int *)(*state + 0x4ac) == 0) {
        pos = *(VecFx32 *)(*state + 0x4c4);
        pos.y = ((VecFx32 *)state[0xe])->y;
    }
    func_ov107_020c0b90(*state, 0x12, *(VecFx32 *)state[0xe], 0);
    Ov107_MoveNodeAndRelayout((Actor *)(*state), &pos);
    *(int *)(*state + 0x4bc) = 1;
    Ov107_PostTagUpdate((Actor *)(*state), 0x10, 0);
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov237_TickRejoin);
}
