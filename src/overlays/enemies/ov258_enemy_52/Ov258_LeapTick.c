/* Leap tick of the ov258 actor: the +0x44 clock runs up at the frame rate with step cues (020cd6c8)
 * at 1 x 0x88 (variant 0xb) and 0x1c x 0x88 (variant 0x1b with a +0x460 partner, else 0x16); once the
 * +4 rig is idle pose 8 plays with effect 0x22 at the +0x1c point, the +0x30 timer clears and the
 * brain waits on 020cf894. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern void Ov258_StepCue(int *node, int step, int phase, unsigned int variant);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov258_HoverTick(void);

void Ov258_LeapTick(int *node)
{
    int *state = (int *)node[1];

    state[0x11] += *(int *)(node[0] + 0x2c);
    Ov258_StepCue(node, 1, 5, (u16)0xb);
    Ov258_StepCue(node, 0x1c, 4, (u16)(*(int *)(*state + 0x460) != 0 ? 0x1b : 0x16));
    if (*(u8 *)(state[1] + 0xad) != 0) {
        return;
    }
    Ov107_PostTagUpdate((Actor *)(*state), 8, 0);
    func_ov107_020c0b90(*state, 0x22, *(VecFx32 *)(state + 7), 0);
    state[0xc] = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov258_HoverTick);
}
