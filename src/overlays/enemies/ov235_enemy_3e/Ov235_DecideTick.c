/* Decision tick of an ov235 state: the +0x40 timer clears and the +0x5c path point is resolved
 * (Ov235_SteerToTarget) into the +0x10 step. Once the +0xc idle byte clears a d101 picks the next
 * sub-state -- 0xc below 40, 9 below 80, else 2 -- and the tick ends. */

#include "nitro/fx_types.h"
#include "game/engine.h"

extern void Ov235_SteerToTarget(int *state, int point, VecFx32 *dir, int *speed);
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

void Ov235_DecideTick(int *node)
{
    int *state = (int *)node[1];
    VecFx32 dir;
    int speed;
    int roll;

    state[0x10] = 0;
    Ov235_SteerToTarget(state, state[0x17], &dir, &speed);
    ScaleVec3Fx12(speed, &dir, (VecFx32 *)(state + 4));
    if (*(unsigned char *)state[3] != 0) {
        return;
    }
    roll = RandRange(0, 100);
    if (roll < 0x28) {
        *(unsigned char *)(*state + 0x1c7) = 0xc;
    } else if (roll < 0x50) {
        *(unsigned char *)(*state + 0x1c7) = 9;
    } else {
        *(unsigned char *)(*state + 0x1c7) = 2;
    }
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}
