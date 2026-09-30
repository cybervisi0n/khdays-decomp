/* Enter the ov258 actor's leap: the actor's +0x424 clears, the +0x53 step countdown is 5 and the
 * +0x44 clock clears, the rig switches (020cd028 mode 1), the +0x1c point is (0, 15.6, 11.0), pose 5
 * plays with effect 0x21 there and the brain waits on 020cf7c8. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

typedef struct { u8 lo : 4; u8 hi : 4; } NibblePair;

extern void Ov258_AcquireTarget(int *node, int mode);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov258_LeapTick(void);

void Ov258_EnterLeap(int *node)
{
    int *state = (int *)node[1];

    *(int *)(*state + 0x424) = 0;
    ((NibblePair *)((u8 *)state + 0x53))->lo = 5;
    state[0x11] = 0;
    Ov258_AcquireTarget(node, 1);
    state[7] = 0;
    state[8] = 0xfa00;
    state[9] = 0xb000;
    Ov107_PostTagUpdate((Actor *)(*state), 5, 0);
    func_ov107_020c0b90(*state, 0x21, *(VecFx32 *)(state + 7), 0);
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov258_LeapTick);
}
