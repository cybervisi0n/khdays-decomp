/* Ov245_DiveTick -- dive tick: while the actor is still falling (020cce48) the +0x14 height
 * follows the +0x20 speed, and the speed decays once per 0x88 of the scene's +0x2c frame step
 * (each step: speed *= 1.0 - 0.03125 * min(rest, 0x88) / 0x88); on landing the +0xc velocity is
 * the negated +0x4c8 anchor direction plus -1.5 times the +0x430 item's +0x3bc direction, and if
 * the item has no +0x38c target the actor is reset (020cce28) and the node moves to 020cde98. */

#include "nitro/fx_types.h"

extern int Ov245_AnimGate(int actor);
extern int FX_Div(int num, int den);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern void Ov245_ResetMode(int actor);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov245_AiStep_FlagAndQueueAction2AfterGate(void);

void Ov245_DiveTick(int *node) {
    int *state = (int *)node[1];
    VecFx32 push;
    int rest;

    if (Ov245_AnimGate(*state) != 0) {
        state[5] = state[8];
        rest = *(int *)(node[0] + 0x2c);
        while (rest > 0) {
            int ratio = FX_Div(rest <= 0x88 ? rest : 0x88, 0x88);
            int t = (int)(((long long)ratio * 0x80 + 0x800) >> 12);
            state[8] = (int)(((long long)state[8] * (0x1000 - t) + 0x800) >> 12);
            rest -= 0x88;
        }
        return;
    }
    ScaleVec3Fx12(-0x1000, (VecFx32 *)(*(int *)(*state + 0x4c8) + 0x2c), (VecFx32 *)(state + 3));
    ScaleVec3Fx12(-0x1800, (VecFx32 *)(*(int *)(*state + 0x430) + 0x3bc), &push);
    VEC_Add((VecFx32 *)(state + 3), &push, (VecFx32 *)(state + 3));
    if (*(int *)(*(int *)(*state + 0x430) + 0x38c) != 0) {
        return;
    }
    Ov245_ResetMode(*state);
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov245_AiStep_FlagAndQueueAction2AfterGate);
}
