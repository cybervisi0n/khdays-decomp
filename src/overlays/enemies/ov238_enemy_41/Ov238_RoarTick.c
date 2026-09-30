/* Roar tick of the ov238 actor: +0x20 accumulates the frame rate, the cues play sound 0x12e/5 after 5
 * frames and 0x12e/4 after 15, the +0xc velocity follows the +0x3e0 part's +0x2c vector turned by the
 * heading (020d07f0); once the partner holds no queued move the next move is 2 and the node ends. */

#include "nitro/fx_types.h"

extern void Ov238_TimedCue(int *node, int ticks, int cue, int variant);
extern void Ov238_TurnVelocity(int *node, VecFx32 *vec);
extern void SetIndexedSlot(int *node, int slot, void *cb);

void Ov238_RoarTick(int *node)
{
    int *state = (int *)node[1];

    state[8] += *(int *)(node[0] + 0x2c);
    Ov238_TimedCue(node, 5, 2, 5);
    Ov238_TimedCue(node, 0xf, 1, 4);
    Ov238_TurnVelocity(node, (VecFx32 *)(*(int *)(*state + 0x3e0) + 0x2c));
    if (*(unsigned char *)(state[1] + 0xad) != 0) {
        return;
    }
    *(unsigned char *)(*state + 0x1c7) = 2;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}
