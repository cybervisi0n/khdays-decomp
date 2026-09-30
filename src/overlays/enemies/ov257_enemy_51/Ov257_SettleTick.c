/* Settle tick of an ov257 state: the +0x40 rate clears and the +0x5c path point is resolved
 * (Ov257_SteerToTarget) into the +0x10 step. Once the +0xc idle byte clears a d101 picks the next
 * sub-state -- 0xc below 40; 9 below 80 when the +0x400 partner is active (bit 1 of its +0x40
 * object's +0x5c) and the path point is closer than 8.0; else 2 -- and the tick ends. */

#include "nitro/fx_types.h"
#include "game/engine.h"

struct Bits5c { int b0 : 1, b1 : 1; };

extern int Ov257_SteerToTarget(int *state, int point, VecFx32 *dir, int *speed);
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

void Ov257_SettleTick(int *node)
{
    int *state = (int *)node[1];
    VecFx32 dir;
    int speed;
    int dist;
    int roll;

    state[0x10] = 0;
    dist = Ov257_SteerToTarget(state, state[0x18], &dir, &speed);
    ScaleVec3Fx12(speed, &dir, (VecFx32 *)(state + 4));
    if (*(unsigned char *)state[3] != 0) {
        return;
    }
    roll = RandRange(0, 100);
    if (roll < 0x28) {
        *(unsigned char *)(*state + 0x1c7) = 0xc;
    } else if (roll < 0x50
               && ((struct Bits5c *)(*(int *)(*(int *)(*state + 0x400) + 0x40) + 0x5c))->b1
               && dist < 0x4000) {
        *(unsigned char *)(*state + 0x1c7) = 9;
    } else {
        *(unsigned char *)(*state + 0x1c7) = 2;
    }
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}
