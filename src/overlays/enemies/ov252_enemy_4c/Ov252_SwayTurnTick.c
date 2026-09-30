/* Sway turn tick of the ov252 actor: +0x64 accumulates the frame rate and the +0xc velocity follows the
 * +0x574 part's +0x2c vector turned by the +0x54 heading, scaled by +0x70 + 0.5. By the +0x579 phase:
 * in phase 0, once the partner holds no queued move it faces the target and the node goes back to
 * 020cf3b8 unless the actor picks a move (020cdef4); in phases 1 and 2 the +0x58 goal turns 0x430 to one
 * side and, once the partner is idle, the turn continues while the target (+0x18 relative to the +8
 * point) is still on the phase's side (020ce42c) with the +0x84 pose replayed; in phases 3 and 4 it
 * rises or sinks at 0.625. Advancing plays the next +0x84 pose, motion 5 (phase 1) or 8 (phase 2),
 * clears +0x64 and moves on to 020cfa28. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

extern VecFx32 Ov252_TurnVecY(int angle, VecFx32 *vec);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern u8 Ov252_TurnSide(int *state, VecFx32 v);
extern void Ov107_PostTagUpdate(int actor, int pose, int loop);
extern int Ov107_StartAnim(int part, int motion, int mode);
extern int Ov252_CheckTarget(int *node, VecFx32 *delta, int face);
extern int Ov252_PickMove(int *node);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov252_RetreatDecision(void);
extern void Ov252_SwaySettleTick(void);

void Ov252_SwayTurnTick(int *node)
{
    int *state = (int *)node[1];
    int idle = *(u8 *)(state[1] + 0xad) == 0;
    VecFx32 d;
    VecFx32 delta;
    VecFx32 v;
    int actor;

    state[0x19] += *(int *)(node[0] + 0x2c);
    v = Ov252_TurnVecY(state[0x15], (VecFx32 *)(*(int *)(*state + 0x574) + 0x2c));
    *(VecFx32 *)(state + 3) = v;
    ScaleVec3Fx12(state[0x1c] + 0x800, (VecFx32 *)(state + 3), (VecFx32 *)(state + 3));
    switch (*(u8 *)(*state + 0x579)) {
    case 1:
    case 2:
        state[0x16] = state[0x15] + (*(u8 *)(*state + 0x579) == 2 ? 0x430 : -0x430);
        d.x = state[6] - ((VecFx32 *)state[2])->x;
        d.y = 0;
        d.z = state[8] - ((VecFx32 *)state[2])->z;
        if (!idle) {
            return;
        }
        actor = *state;
        if (*(u8 *)(actor + 0x579) == Ov252_TurnSide(state, d)) {
            Ov107_PostTagUpdate(actor, *((u8 *)state + 0x84), 0);
            if (*(u8 *)(*state + 0x579) == 2) {
                Ov107_StartAnim(*(int *)(*state + 0x574), 7, 0);
            } else {
                Ov107_StartAnim(*(int *)(*state + 0x574), 4, 0);
            }
            return;
        }
        break;
    case 3:
    case 4:
        state[4] = *(u8 *)(*state + 0x579) == 4 ? 0xa00 : -0xa00;
        if (!idle) {
            return;
        }
        break;
    case 0:
        if (!idle) {
            return;
        }
        Ov252_CheckTarget(node, &delta, 0);
        if (Ov252_PickMove(node) == 0) {
            SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov252_RetreatDecision);
        } else {
            SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
        }
        return;
    default:
        return;
    }
    Ov107_PostTagUpdate(*state, ++*((u8 *)state + 0x84), 0);
    switch (*(u8 *)(*state + 0x579)) {
    case 1:
        Ov107_StartAnim(*(int *)(*state + 0x574), 5, 0);
        break;
    case 2:
        Ov107_StartAnim(*(int *)(*state + 0x574), 8, 0);
        break;
    }
    state[0x19] = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov252_SwaySettleTick);
}
