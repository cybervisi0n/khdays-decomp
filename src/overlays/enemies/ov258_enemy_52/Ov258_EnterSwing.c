/* Enter the ov258 actor's swing: the +0x30 / +0x34 / +0x3c timers clear, the +0x52 high nibble is 2,
 * +0x50 = 3, the +0x53 step countdown 6 with the +0x44 clock cleared and the +0x52 low nibble 0, the
 * rig switches (020cd028 mode 1), pose 2 plays with effect 1 at the origin, the +0x45c partner arms
 * its 2.16 to 2.32 window (mode 1, 020cfd3c) and the brain waits on 020ce0a0. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

typedef struct { u8 lo : 4; u8 hi : 4; } NibblePair;

extern void Ov258_AcquireTarget(int *node, int mode);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern void Ov258_ForwardEventIfStateOne(int partner, int from, int to, int d);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov258_StompTick(void);
extern const VecFx32 data_02041dc8;

void Ov258_EnterSwing(int *node)
{
    int *state = (int *)node[1];

    state[0xc] = 0;
    state[0xd] = 0;
    state[0xf] = 0;
    ((NibblePair *)((u8 *)state + 0x52))->hi = 2;
    *(short *)(state + 0x14) = 3;
    ((NibblePair *)((u8 *)state + 0x53))->lo = 6;
    state[0x11] = 0;
    ((NibblePair *)((u8 *)state + 0x52))->lo = 0;
    Ov258_AcquireTarget(node, 1);
    Ov107_PostTagUpdate((Actor *)(*state), 2, 0);
    func_ov107_020c0b90(*state, 1, data_02041dc8, 0);
    Ov258_ForwardEventIfStateOne(*(int *)(*state + 0x45c), 0x2288, 0x2530, 1);
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov258_StompTick);
}
