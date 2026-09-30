/* Ring charge tick of the ov223 enemy (variant 1): the +0x38c item's +0x3ac pool entry's +0x20
 * point is announced to the owner (ov107 c5c54) with bit 7 of the +0x60 high byte cleared, the
 * +0x20 target is data_02042258 turned by the item's +0xa0 pose, and the strike sweep (ov223
 * 442c, mode 0) runs with a segment from that point to the target of length 16.0 and radius
 * 0.5. The +0x3c timer accumulates the owner's rate; past 0x1200 it clears and the tick hands
 * over to Ov223_AiFastCountdownQueue0. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"
#include "game/engine.h"

struct hw60 { unsigned short lo : 8, hi : 8; };
struct Ov223Segment { VecFx32 p0; VecFx32 p1; int nLength; int nRadius; };

extern int Ov223_StrikeSweep(int *node, int mode, struct Ov223Segment *seg);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const VecFx32 data_02042258;
extern void Ov223_AiFastCountdownQueue0(int *node);

void Ov223_RingChargeTick(int *node)
{
    int *state = (int *)node[1];
    struct Ov223Segment seg;
    VecFx32 at;

    at = *(VecFx32 *)(*(int *)(*(int *)(*(int *)(*state + 0x38c) + 0x3ac)) + 0x20);
    ((struct hw60 *)(*state + 0x60))->hi &= ~0x80;
    Ov107_MoveNodeAndRelayout((Actor *)(*state), &at);
    Vec3TransformViaTempMtx((VecFx32 *)(state + 8), (const void *)(*(int *)(*state + 0x38c) + 0xa0), &data_02042258);
    seg.p0 = at;
    seg.p1 = *(VecFx32 *)(state + 8);
    seg.nLength = 0x10000;
    seg.nRadius = 0x800;
    Ov223_StrikeSweep(node, 0, &seg);
    state[0xf] += *(int *)(*node + 0x2c);
    if (state[0xf] <= 0x1200) {
        return;
    }
    state[0xf] = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov223_AiFastCountdownQueue0);
}
