/* Advance tick of the ov238 actor: the +0x3e0 part's +0x2c vector scaled to 0.70 and turned by the
 * heading drives the +0xc velocity, +0x20 accumulates the frame rate and sounds 0x12e/5 and 0x12e/4 cue
 * after 7 and 19 frames. Once the partner holds no queued move: within 3.0 of the target a lunge starts
 * (pose 0x15, part motion 0xb, cues re-armed, node 020d16e8); out of charges (+0x2d) the walk resets
 * (pose 0xf, motion 5) and also goes to 020d16e8; otherwise the node goes back to 020d1510. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern int Ov238_TargetGap(int *node);
extern void Ov238_TimedCue(int *node, int ticks, int cue, int variant);
extern void Ov238_TurnVelocity(int *node, VecFx32 *vec);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov238_ClawTick(void);
extern void Ov238_AiWalkStep(void);

void Ov238_AdvanceTick(int *node)
{
    int *state = (int *)node[1];
    VecFx32 v;
    int dist;

    ScaleVec3Fx12(0xb40, (VecFx32 *)(*(int *)(*state + 0x3e0) + 0x2c), &v);
    dist = Ov238_TargetGap(node);
    state[8] += *(int *)(node[0] + 0x2c);
    Ov238_TimedCue(node, 7, 2, 5);
    Ov238_TimedCue(node, 0x13, 1, 4);
    Ov238_TurnVelocity(node, &v);
    if (*(unsigned char *)(state[1] + 0xad) != 0) {
        return;
    }
    if (dist < 0x3000) {
        *((unsigned char *)state + 0x2e) = 1;
        *((unsigned char *)state + 0x31) = 2;
        state[8] = 0;
        Ov107_PostTagUpdate((Actor *)(*state), 0x15, 0);
        Ov107_StartAnim(*(int *)(*state + 0x3e0), 0xb, 0);
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov238_ClawTick);
        return;
    }
    if (*((unsigned char *)state + 0x2d) == 0) {
        *((unsigned char *)state + 0x2e) = 0;
        Ov107_PostTagUpdate((Actor *)(*state), 0xf, 0);
        Ov107_StartAnim(*(int *)(*state + 0x3e0), 5, 0);
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov238_ClawTick);
        return;
    }
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov238_AiWalkStep);
}
