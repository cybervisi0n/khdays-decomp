/* Wind-up tick of the ov258 actor: the +0x30 and +0x44 clocks run up at the frame rate; at 3.85 the
 * third step (+0x53 countdown 3) plays sound variant 0xc of the +0x58 bank at the +0x1c point, and
 * from 0x1298 the swing hit test runs (020cf6dc). Once the +4 rig is idle the +0x34 timer clears,
 * +0x50 = 1, pose 6 plays with effect 0x24 at the +0x1c point and the brain waits on 020cfb10. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

typedef struct { u8 lo : 4; u8 hi : 4; } NibblePair;

extern void Ov107_BuildAndSendUpdate(int actor, int bank, int variant, void *at);
extern void Ov258_SwingHitTest(int *node);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov258_ThrowTick(void);

void Ov258_WindUpTick(int *node)
{
    int *state = (int *)node[1];

    state[0xc] += *(int *)(node[0] + 0x2c);
    state[0x11] += *(int *)(node[0] + 0x2c);
    if (((NibblePair *)((u8 *)state + 0x53))->lo == 3 && state[0x11] >= 0x3da0) {
        ((NibblePair *)((u8 *)state + 0x53))->lo--;
        Ov107_BuildAndSendUpdate(*state, *(short *)(state + 0x16), 0xc, state + 7);
    }
    if (state[0xc] >= 0x1298) {
        Ov258_SwingHitTest(node);
    }
    if (*(u8 *)(state[1] + 0xad) != 0) {
        return;
    }
    state[0xd] = 0;
    *(u16 *)(state + 0x14) = 1;
    Ov107_PostTagUpdate((Actor *)(*state), 6, 0);
    func_ov107_020c0b90(*state, 0x24, *(VecFx32 *)(state + 7), 0);
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov258_ThrowTick);
}
