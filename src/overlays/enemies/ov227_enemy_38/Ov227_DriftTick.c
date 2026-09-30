/* Drift tick of an ov227 part: the +0x24 timer accumulates the owner's rate, bit 7 of the owner's
 * +0x60 high byte clears and the +0xc velocity decays to 0.97. When the owner's +0x74 sphere hits
 * something (Ov227_HitSweep) effect 0 spawns there, the owner's sub-state resets to 0 and
 * the tick ends. Every 2.0 of the timer the velocity is re-aimed at 0.75 towards a point jittered
 * by up to 4.0 around the +8 target's +0x190, the timer and the +0x28 hit mask clear and the tick
 * hands over to Ov227_RockFlightTick. */

#include "nitro/fx_types.h"
#include "game/engine.h"

struct hw60 { unsigned short lo : 8, hi : 8; };

extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern int Ov227_HitSweep(int *part, void *sphere);
extern void func_ov107_020c0b90(int actor, int a, VecFx32 v, int b);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void VEC_Subtract(const void *a, const void *b, VecFx32 *out);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern void Ov227_RockFlightTick(int *node);

static inline int RandRange(int lo, int hi)
{
    int d = hi - lo;

    if (d < 0) {
        d = -d;
    }
    return lo + RandNextScaled(d + 1);
}

void Ov227_DriftTick(int *node)
{
    int *state = (int *)node[1];
    VecFx32 goal;

    state[9] += *(int *)(*node + 0x2c);
    ((struct hw60 *)(*state + 0x60))->hi &= ~0x80;
    ScaleVec3Fx12(0xf85, (VecFx32 *)(state + 3), (VecFx32 *)(state + 3));
    if (Ov227_HitSweep(state, (void *)(*state + 0x74)) != 0) {
        func_ov107_020c0b90(*state, 0, *(VecFx32 *)(*state + 0x74), 1);
        *(unsigned char *)(*state + 0x1c7) = 0;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
        return;
    }
    if (state[9] < 0x2000) {
        return;
    }
    goal = *(VecFx32 *)(state[2] + 0x190);
    goal.x += RandRange(-0x4000, 0x4000);
    goal.z += RandRange(-0x4000, 0x4000);
    VEC_Subtract(&goal, (void *)state[1], (VecFx32 *)(state + 3));
    VEC_Normalize((VecFx32 *)(state + 3), (VecFx32 *)(state + 3));
    ScaleVec3Fx12(0xc00, (VecFx32 *)(state + 3), (VecFx32 *)(state + 3));
    state[9] = 0;
    *(unsigned char *)(state + 10) = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov227_RockFlightTick);
}
