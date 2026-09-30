/* Claw tick of the ov238 actor: +0x20 accumulates the frame rate, sounds 0x12e/5 and 0x12e/4 cue after
 * 8 and 20 frames, the +0xc velocity follows the +0x3e0 part's +0x2c vector turned by the heading;
 * once the partner holds no queued move the next move is 3 (with +0x28 and +0x2e set), else 2. */

#include "nitro/fx_types.h"

extern void Ov238_TimedCue(int *node, int ticks, int cue, int variant);
extern void Ov238_TurnVelocity(int *node, VecFx32 *vec);
extern void SetIndexedSlot(int *node, int slot, void *cb);

void Ov238_ClawTick(int *node)
{
    int *state = (int *)node[1];

    state[8] += *(int *)(node[0] + 0x2c);
    Ov238_TimedCue(node, 8, 2, 5);
    Ov238_TimedCue(node, 0x14, 1, 4);
    Ov238_TurnVelocity(node, (VecFx32 *)(*(int *)(*state + 0x3e0) + 0x2c));
    if (*(unsigned char *)(state[1] + 0xad) != 0) {
        return;
    }
    if (*((unsigned char *)state + 0x2e) == 0 || state[0xa] == 0) {
        *(unsigned char *)(*state + 0x1c7) = 2;
    } else {
        *(unsigned char *)(*state + 0x1c7) = 3;
    }
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}
