/* Publish the landing pose: only while bit 0 of the hw60 low byte is set, send the anchor point
 * -- the vector at state[0x12] with y raised by 0x1200 -- to the render hook and latch the pending
 * action byte from +0x1c9 into +0x1c7.
 *
 * Matched byte-exact 2026-07-23, closing an old park. The anchor is built with the SDK's
 * `static inline VEC_Set`, not with three field assignments: the three arguments are three
 * adjacent loads that mwcc groups into one ldm, and the inline body is three separate stores.
 * Written as `v.x = p[0]; v.y = p[1] + 0x1200; v.z = p[2];` the loads stay split and nine
 * instructions come out different.
 *
 * One of three byte-identical siblings. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

struct hw60 { unsigned short lo : 8, hi : 8; };
static inline void VEC_Set(VecFx32 *v, int x, int y, int z) {
    v->x = x;
    v->y = y;
    v->z = z;
}

extern void SetIndexedSlot(int self, int idx, int cb);

void Ov119_AiStep_ResumeStoredActionRaised(int *self) {
    int *state = (int *)self[1];
    VecFx32 v;

    if ((((struct hw60 *)(*state + 0x60))->lo & 1) == 0) {
        return;
    }
    { VecFx32 *p = (VecFx32 *)state[0x12];
      VEC_Set(&v, p->x, p->y + 0x1200, p->z); }
    Ov107_MoveNodeAndRelayout((Actor *)(*state), &v);
    *(char *)(*state + 0x1c7) = *(signed char *)(*state + 0x1c9);
    SetIndexedSlot((int)self, *(signed char *)((int)self + 0x20), 0);
}
