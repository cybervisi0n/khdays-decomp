/* Landing tick of an ov255 state: the +0x50 timer accumulates the owner's rate and the +0x5c path
 * point is resolved (Ov255_SteerToTarget) into the +0x10 step. Once the +0xc idle byte clears, the
 * +0x54 cooldown is re-rolled in [+0x224, +0x228], +0x58 clears and sub-state 2 is requested. */

#include "nitro/fx_types.h"
#include "game/engine.h"

extern void Ov255_SteerToTarget(int *state, int point, VecFx32 *dir, int *speed);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void SetIndexedSlot(int *node, int slot, void *cb);

static inline int RandRange(int lo, int hi)
{
    int d = hi - lo;

    if (d < 0) {
        d = -d;
    }
    return lo + RandNextScaled(d + 1);
}

void Ov255_LandingTick(int *node)
{
    int *state = (int *)node[1];
    VecFx32 dir;
    int speed;

    state[0x14] += *(int *)(node[0] + 0x2c);
    Ov255_SteerToTarget(state, state[0x17], &dir, &speed);
    ScaleVec3Fx12(speed, &dir, (VecFx32 *)(state + 4));
    if (*(unsigned char *)state[3] != 0) {
        return;
    }
    state[0x15] = RandRange(*(int *)(*state + 0x224), *(int *)(*state + 0x228));
    state[0x16] = 0;
    *(unsigned char *)(*state + 0x1c7) = 2;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}
