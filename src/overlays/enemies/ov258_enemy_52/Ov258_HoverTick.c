/* Hover tick of the ov258 actor: the +0x30 and +0x44 clocks run up at the frame rate; at 3.85 the
 * third step (+0x53 countdown 3) plays sound variant 0xc of the +0x58 bank at the +0x1c point. Each
 * time the +4 rig finishes: after 1.0 pose 9 plays with the wind-up sound (variant 0x1b with a +0x460
 * partner, else 0x17) at the +0x430 rig, the +0x30 clock clears, effect 0x23 plays and the brain
 * waits on 020cf9f8; before that pose 8 replays with effect 0x22. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

typedef struct { u8 lo : 4; u8 hi : 4; } NibblePair;

extern void Ov107_BuildAndSendUpdate(int actor, int bank, u16 variant, int at);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov258_WindUpTick(void);

void Ov258_HoverTick(int *node)
{
    int *state = (int *)node[1];

    state[0xc] += *(int *)(node[0] + 0x2c);
    state[0x11] += *(int *)(node[0] + 0x2c);
    if (((NibblePair *)((u8 *)state + 0x53))->lo == 3 && state[0x11] >= 0x3da0) {
        ((NibblePair *)((u8 *)state + 0x53))->lo--;
        Ov107_BuildAndSendUpdate(*state, *(short *)(state + 0x16), 0xc, (int)(state + 7));
    }
    if (*(u8 *)(state[1] + 0xad) != 0) {
        return;
    }
    if (state[0xc] >= 0x1000) {
        Ov107_PostTagUpdate((Actor *)(*state), 9, 0);
        Ov107_BuildAndSendUpdate(*state, *(short *)(state + 0x16), *(int *)(*state + 0x460) != 0 ? 0x1b : 0x17,
                            *(int *)(*state + 0x430) + 0x14);
        state[0xc] = 0;
        func_ov107_020c0b90(*state, 0x23, *(VecFx32 *)(state + 7), 0);
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov258_WindUpTick);
        return;
    }
    Ov107_PostTagUpdate((Actor *)(*state), 8, 0);
    func_ov107_020c0b90(*state, 0x22, *(VecFx32 *)(state + 7), 0);
}
