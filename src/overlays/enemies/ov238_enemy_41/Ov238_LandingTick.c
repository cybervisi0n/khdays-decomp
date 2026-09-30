/* Landing tick of the ov238 actor: +0x20 accumulates the frame rate, the +0xc velocity follows the
 * +0x3e0 part's +0x2c vector turned by the heading and sound 0x12e/0xa cues after 15 frames; once the
 * partner holds no queued move pose 0 loops, bits 0-1 of +0x1ae are set and the +0x38c shape hides. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

typedef struct { unsigned f : 8; } B8;

extern void Ov238_TurnVelocity(int *node, VecFx32 *vec);
extern void Ov238_TimedCue(int *node, int ticks, int cue, int variant);

void Ov238_LandingTick(int *node)
{
    int *state = (int *)node[1];

    state[8] += *(int *)(node[0] + 0x2c);
    Ov238_TurnVelocity(node, (VecFx32 *)(*(int *)(*state + 0x3e0) + 0x2c));
    Ov238_TimedCue(node, 0xf, 2, 0xa);
    if (*(unsigned char *)(state[1] + 0xad) != 0) {
        return;
    }
    Ov107_PostTagUpdate((Actor *)(*state), 0, 1);
    *(u16 *)(*state + 0x1ae) |= 3;
    ((B8 *)(*(int *)(*state + 0x38c) + 8))->f &= ~1;
}
