/* Move entry: the actor plays pose 0x13, is knocked back with mode 3 in place, the +0x44 timer and
 * the +0x70 / +0x74 flags clear and the node moves to 020d0ad4. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const VecFx32 data_02041dc8;
extern void Ov254_AiWindupTrackTick(void);

void Ov254_KnockbackEntry(int *node)
{
    int *state = (int *)node[1];

    Ov107_PostTagUpdate((Actor *)(*state), 0x13, 0);
    func_ov107_020c0b90(*state, 3, data_02041dc8, 0);
    state[0x11] = 0;
    *((u8 *)state + 0x70) = 0;
    *((u8 *)state + 0x74) = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov254_AiWindupTrackTick);
}
