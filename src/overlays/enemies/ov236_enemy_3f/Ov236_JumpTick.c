/* Jump tick: the +0x18 velocity follows the +0x3c one, whose rise decays by 0x80 per tick;
 * once falling, and only if the actor's +0x17a bit 0 (grounded) is set, the landing cue
 * (first halfword pair of data_ov236_020d63c0) is sent through the +0x24 hook, effect 0x127 of kind 9
 * fires at the +0x38 anchor, pose request 5 is queued and the node dispatches null. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

struct Bits17a { u8 b0 : 1; };

extern void Ov107_BuildAndSendUpdate(int actor, int id, int kind, void *anchor);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern unsigned short data_ov236_020d63c0[];

void Ov236_JumpTick(int *node) {
    int *state = (int *)node[1];
    unsigned short pair[2];
    unsigned short *pp;
    void (*cb)();

    *(VecFx32 *)(state + 6) = *(VecFx32 *)(state + 0xf);
    state[0x10] -= 0x80;
    if (state[0x10] >= 0) {
        return;
    }
    if (((struct Bits17a *)(*state + 0x17a))->b0 == 0) {
        return;
    }
    pp = pair;
    pp[1] = data_ov236_020d63c0[1];
    pp[0] = data_ov236_020d63c0[0];
    cb = *(void (**)())(*state + 0x24);
    if (cb != 0) cb(*state, pp, 4);
    Ov107_BuildAndSendUpdate(*state, 0x127, 9, (void *)state[0xe]);
    *(unsigned char *)(*state + 0x1c7) = 5;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}
