/* Landing tick of an ov235 state: once bit 0 of the owner's +0x60 low byte is set, the owner is
 * placed 1.0 above the +8 point, the +0x4c delay is re-rolled in [+0x224, +0x228], the +0x1c9
 * return sub-state is requested and the tick ends. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"
#include "game/engine.h"

struct hw60 { unsigned short lo : 8, hi : 8; };

extern void SetIndexedSlot(int *node, int slot, void *cb);

static inline void VEC_Set(VecFx32 *v, int x, int y, int z)
{
    v->x = x;
    v->y = y;
    v->z = z;
}

static inline int RandRange(int lo, int hi)
{
    int d = hi - lo;

    if (d < 0) {
        d = -d;
    }
    return lo + RandNextScaled(d + 1);
}

void Ov235_LandingTick(int *node)
{
    int *state = (int *)node[1];
    VecFx32 pos;

    if ((((struct hw60 *)(*state + 0x60))->lo & 1) == 0) {
        return;
    }
    VEC_Set(&pos, ((VecFx32 *)state[2])->x, ((VecFx32 *)state[2])->y + 0x1000, ((VecFx32 *)state[2])->z);
    Ov107_MoveNodeAndRelayout((Actor *)(*state), &pos);
    state[0x13] = RandRange(*(int *)(*state + 0x224), *(int *)(*state + 0x228));
    *(signed char *)(*state + 0x1c7) = *(signed char *)(*state + 0x100 + 0xc9);
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}
