/* Sway settle tick of the ov252 actor: the +0xc velocity follows the +0x574 part's +0x2c vector turned
 * by the +0x54 heading, scaled by +0x70 + 0.5, and +0x64 accumulates the frame rate; phase 4 rises at
 * 0.125 until 0.43 and phase 3 sinks until 0.93. Once the partner holds no queued move: a pending turn
 * gives next move 5, phase 0 gives 2; with a reward pending or in phase 5 it faces the target and, in
 * phase 5, a target within the turn cone (020cdb88 under 1.05) marks a pending turn while one outside
 * it (or a +0xb4 hit) makes 0xd current and moves on to 020d1abc. With a turn or reward pending pose 3
 * and motion 2 start and the phase becomes 0 (reward) or 5; otherwise the node goes back to 020cf3b8. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern VecFx32 Ov252_TurnVecY(int angle, VecFx32 *vec);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern int Ov252_CheckTarget(int *node, VecFx32 *delta, int face);
extern int Ov252_HeadingDelta(int *node, VecFx32 *v, int angle, int wantAbs);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov252_HoverTick_2(void);
extern void Ov252_RetreatDecision(void);

void Ov252_SwaySettleTick(int *node)
{
    int *state = (int *)node[1];
    VecFx32 delta;
    VecFx32 v;
    u8 phase;
    int gap;

    v = Ov252_TurnVecY(state[0x15], (VecFx32 *)(*(int *)(*state + 0x574) + 0x2c));
    *(VecFx32 *)(state + 3) = v;
    ScaleVec3Fx12(state[0x1c] + 0x800, (VecFx32 *)(state + 3), (VecFx32 *)(state + 3));
    state[0x19] += *(int *)(node[0] + 0x2c);
    switch (*(u8 *)(*state + 0x579)) {
    case 0:
        break;
    case 4:
        if (state[0x19] <= 0x6e8) {
            state[4] = 0x200;
        }
        break;
    case 3:
        if (state[0x19] <= 0xee0) {
            state[4] = -0x200;
        }
        break;
    }
    if (*(u8 *)(state[1] + 0xad) != 0) {
        return;
    }
    if (state[0x2a] != 0) {
        *(u8 *)(*state + 0x1c7) = 5;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
        return;
    }
    phase = *(u8 *)(*state + 0x579);
    if (phase == 0) {
        *(u8 *)(*state + 0x1c7) = 2;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
        return;
    }
    if (state[0x28] != 0 || phase == 5) {
        Ov252_CheckTarget(node, &delta, 0);
        if (*(u8 *)(*state + 0x579) == 5) {
            gap = Ov252_HeadingDelta(node, &delta, state[0x15], 1);
            if (state[0x2d] != 0) {
                state[0x2d] = 0;
                *(u8 *)(*state + 0x1c6) = 0xd;
                SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov252_HoverTick_2);
                return;
            }
            if (gap < 0x10c1) {
                state[0x2a] = 1;
            } else {
                *(u8 *)(*state + 0x1c6) = 0xd;
                SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov252_HoverTick_2);
                return;
            }
        }
        if (state[0x2a] != 0 || state[0x28] != 0) {
            Ov107_PostTagUpdate((Actor *)(*state), 3, 0);
            Ov107_StartAnim(*(int *)(*state + 0x574), 2, 0);
            if (state[0x28] != 0) {
                *(u8 *)(*state + 0x579) = 0;
            } else {
                *(u8 *)(*state + 0x579) = 5;
            }
        }
    } else {
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov252_RetreatDecision);
    }
}
