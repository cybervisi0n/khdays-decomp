/* Sweep step of the ov169 enemy (x2: ov169/170): while the +0x44 timer stays within 0xbb0 it
 * runs the attack sweep (Ov170_AttackSweep) with a query built from the +8 position raised by
 * 0xc00, the up axis, a 0x3000 range and a 0xc00 radius; afterwards sub-state 0 is requested. */

#include "nitro/fx_types.h"

struct Ov169SweepQuery {
    VecFx32 vPos;
    VecFx32 vAxis;
    int nRange;
    int nRadius;
};

extern const VecFx32 data_02042264;
extern void Ov170_AttackSweep(int *state, int kind, struct Ov169SweepQuery *query);
extern void SetIndexedSlot(int *node, int slot, void *cb);

void Ov170_SweepStep(int *node)
{
    int *state = (int *)node[1];
    struct Ov169SweepQuery query;

    state[0x11] += *(int *)(*node + 0x2c);
    if (state[0x11] <= 0xbb0) {
        query.vAxis = data_02042264;
        query.nRadius = 0xc00;
        query.nRange = 0x3000;
        query.vPos = *(VecFx32 *)state[2];
        query.vPos.y += query.nRadius;
        Ov170_AttackSweep(state, 0, &query);
        return;
    }
    *(unsigned char *)(*state + 0x1c7) = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}
