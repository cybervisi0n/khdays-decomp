/* Swing tick of the ov258 actor: the +0x30 clock runs up at the frame rate and the swing hit test
 * (020cf6dc) runs until 0x330; a pending +0x50 flare (1) fires effect 0x25 at the +0x1c point. Once
 * the +4 rig is idle, without a +0x38 delay a follow-up (020cd2cc) may be picked, otherwise the next
 * move is 2. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

extern void Ov258_SwingHitTest(int *node);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern int Ov258_PickMove(int *node);
extern void SetIndexedSlot(int *node, int slot, void *cb);

void Ov258_SwingTick(int *node)
{
    int *state = (int *)node[1];

    state[0xc] += *(int *)(node[0] + 0x2c);
    if (state[0xc] < 0x330) {
        Ov258_SwingHitTest(node);
    }
    if (*(u16 *)(state + 0x14) == 1) {
        (*(u16 *)(state + 0x14))--;
        func_ov107_020c0b90(*state, 0x25, *(VecFx32 *)(state + 7), 0);
    }
    if (*(u8 *)(state[1] + 0xad) != 0) {
        return;
    }
    if (state[0xe] == 0 && Ov258_PickMove(node) != 0) {
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
        return;
    }
    *(signed char *)(*state + 0x1c7) = 2;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}
