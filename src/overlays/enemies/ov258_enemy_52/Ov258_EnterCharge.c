/* Enter the ov258 actor's charge: pose 1 plays, the +0x30 timer clears, +0x50 = 2, the +0x53 step
 * countdown is 6 with the +0x44 clock cleared and the +0x52 high nibble 2, the rig switches (020cd028
 * mode 1), effect 0 plays at the origin, the +0x45c partner arms its 2.82 to 3.15 window (020cfd3c) and
 * the brain waits on 020cdd6c. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

typedef struct { u8 lo : 4; u8 hi : 4; } NibblePair;

extern void Ov258_AcquireTarget(int *node, int mode);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern void Ov258_ForwardEventIfStateOne(int partner, int from, int to, int d);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov258_ComboTick(void);
extern const VecFx32 data_02041dc8;

void Ov258_EnterCharge(int *node)
{
    int *state = (int *)node[1];

    Ov107_PostTagUpdate((Actor *)(*state), 1, 0);
    state[0xc] = 0;
    *(short *)(state + 0x14) = 2;
    ((NibblePair *)((u8 *)state + 0x53))->lo = 6;
    state[0x11] = 0;
    ((NibblePair *)((u8 *)state + 0x52))->hi = 2;
    Ov258_AcquireTarget(node, 1);
    func_ov107_020c0b90(*state, 0, data_02041dc8, 0);
    Ov258_ForwardEventIfStateOne(*(int *)(*state + 0x45c), 0x2d28, 0x3278, 0);
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov258_ComboTick);
}
