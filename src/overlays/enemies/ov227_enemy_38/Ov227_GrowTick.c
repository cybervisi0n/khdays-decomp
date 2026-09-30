/* Grow tick of an ov227 part: the +0x24 timer accumulates the owner's rate; past 0x660 the owner's
 * sub-state is reset to 0 and the tick ends. Otherwise the part is placed at its +0x18 point with a
 * scale growing from 0 to 2.67 over the first 0x440 of the timer (Ov227_HitSweep). */

#include "nitro/fx_types.h"

typedef struct { VecFx32 pos; int scale; } Placement;

extern void Ov227_HitSweep(int *part, Placement *placement);
extern void SetIndexedSlot(int *node, int slot, void *cb);

void Ov227_GrowTick(int *node)
{
    int *state = (int *)node[1];
    Placement place;
    int t;

    state[9] += *(int *)(*node + 0x2c);
    if (state[9] <= 0x660) {
        place.pos = *(VecFx32 *)(state + 6);
        t = state[9];
        if (t > 0x440) {
            t = 0x440;
        } else if (t < 0) {
            t = 0;
        }
        place.scale = t * 0x2aaa / 0x440;
        Ov227_HitSweep(state, &place);
        return;
    }
    *(unsigned char *)(*state + 0x1c7) = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}
