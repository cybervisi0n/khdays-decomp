/* Recovery entry: once the +4 item's +0xad byte clears the actor plays pose 0x17 and is knocked
 * back with mode 7 in place, the +0x3e4 shape loses bit 1, the +0x44 / +0x40 timers and the +0x70
 * flag clear, the +0xc velocity resets to zero and the node moves to 020d06d0. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

typedef struct { unsigned f : 8; } B8;

extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const VecFx32 data_02041dc8;
extern void Ov254_SummonTick(void);

void Ov254_RecoverEntry(int *node)
{
    int *state = (int *)node[1];
    VecFx32 zero;

    if (*(u8 *)(state[1] + 0xad) != 0) {
        return;
    }
    Ov107_PostTagUpdate((Actor *)(*state), 0x17, 0);
    zero = data_02041dc8;
    func_ov107_020c0b90(*state, 7, zero, 0);
    ((B8 *)(*(int *)(*state + 0x3e4) + 8))->f &= ~2;
    state[0x11] = 0;
    *((u8 *)state + 0x70) = 0;
    state[0x10] = 0;
    *(VecFx32 *)(state + 3) = zero;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov254_SummonTick);
}
