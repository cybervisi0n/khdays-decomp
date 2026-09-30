/* Walk tick of the ov238 actor: the +0xc velocity follows the +0x3e0 part's +0x2c vector turned by the
 * heading, +0x20 accumulates the frame rate and sounds 0x12e/5 and 0x12e/4 cue after 10 and 35 frames.
 * Once the partner holds no queued move: a target beyond 20.0 makes it turn back (+0x34 set, pose 0x14,
 * motion 0xa); with charges left (+0x2d) and the target at least 6.0 away a charge restarts (pose 0x12,
 * motion 8) and the walk goes on; otherwise it lunges (pose 0x13, motion 9); both lead to 020d1400. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern int Ov238_TargetGap(int *node);
extern void Ov238_TurnVelocity(int *node, VecFx32 *vec);
extern void Ov238_TimedCue(int *node, int ticks, int cue, int variant);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov238_SwipeTick(void);

void Ov238_WalkTick(int *node)
{
    int *state = (int *)node[1];
    int dist = Ov238_TargetGap(node);

    Ov238_TurnVelocity(node, (VecFx32 *)(*(int *)(*state + 0x3e0) + 0x2c));
    state[8] += *(int *)(node[0] + 0x2c);
    Ov238_TimedCue(node, 0xa, 2, 5);
    Ov238_TimedCue(node, 0x23, 1, 4);
    if (*(unsigned char *)(state[1] + 0xad) != 0) {
        return;
    }
    if (dist > 0x5000) {
        state[0xd] = 1;
        *((unsigned char *)state + 0x2e) = 0;
        Ov107_PostTagUpdate((Actor *)(*state), 0x14, 0);
        Ov107_StartAnim(*(int *)(*state + 0x3e0), 0xa, 0);
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov238_SwipeTick);
        return;
    }
    if (*((unsigned char *)state + 0x2d) != 0 && dist >= 0x1800) {
        *((unsigned char *)state + 0x2d) -= 1;
        state[8] = 0;
        *((unsigned char *)state + 0x31) = 2;
        Ov107_PostTagUpdate((Actor *)(*state), 0x12, 0);
        Ov107_StartAnim(*(int *)(*state + 0x3e0), 8, 0);
    }
    if (*((unsigned char *)state + 0x2d) != 0 && dist >= 0x1800) {
        return;
    }
    *((unsigned char *)state + 0x2e) = 1;
    *((unsigned char *)state + 0x31) = 2;
    state[8] = 0;
    Ov107_PostTagUpdate((Actor *)(*state), 0x13, 0);
    Ov107_StartAnim(*(int *)(*state + 0x3e0), 9, 0);
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov238_SwipeTick);
}
