/* Publish the landing pose: only while bit 0 of the hw60 low byte is set, send the anchor point
 * -- the vector at state[8] with y doubled from (y + 0xe00) -- to the render hook and latch the
 * pending action byte from +0x1c9 into +0x1c7.
 *
 * The anchor is built with the SDK's
 * `static inline VEC_Set`, not with three field assignments: the three arguments are three
 * adjacent loads that mwcc groups into one ldm, and the inline body is three separate stores.
 * Written as three field assignments the loads stay split. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

struct hw60 { unsigned short lo : 8, hi : 8; };
static inline void VEC_Set(VecFx32 *v, int x, int y, int z) {
    v->x = x;
    v->y = y;
    v->z = z;
}

extern void SetIndexedSlot(int self, int idx, int cb);

void Ov125_PublishLandingPose(int *self) {
    int *state = (int *)self[1];
    VecFx32 v;

    if ((((struct hw60 *)(*state + 0x60))->lo & 1) == 0) {
        return;
    }
    { VecFx32 *p = (VecFx32 *)state[8];
      VEC_Set(&v, p->x, (p->y + 0xe00) * 2, p->z); }
    Ov107_MoveNodeAndRelayout((Actor *)(*state), &v);
    *(char *)(*state + 0x1c7) = *(signed char *)(*state + 0x1c9);
    SetIndexedSlot((int)self, *(signed char *)((int)self + 0x20), 0);
}
