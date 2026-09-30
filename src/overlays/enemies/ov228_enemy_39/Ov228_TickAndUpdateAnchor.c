/* Refreshes the child selector and runs the tick, then refreshes the anchor point (raised 0x2000
 * while airborne) and its 0x2000 radius. */

#include "game/actor.h"
#include "nitro/fx_types.h"

extern int Ov107_RefreshAndSelectChild();
extern int Ov107_ProcessObjectTick();

struct S {
    Actor base;                  /* 0x000 */
    u8 pad38c[0x64];
    VecFx32 v3f0;
    char pad4[0x490 - 0x3fc];
    void *p490;
    VecFx32 v494;
    int n4a0;
};

void Ov228_TickAndUpdateAnchor(struct S *r4, int r5) {
    Ov107_RefreshAndSelectChild(r4->p490, r5);
    Ov107_ProcessObjectTick(r4, r5);
    if (r4->base.flags60.bits.lo & 0x80) {
        r4->v494 = r4->base.srt.translation;
        *(int *)((char *)r4 + 0x498) += 0x2000;
    } else {
        r4->v494 = r4->v3f0;
    }
    r4->n4a0 = 0x2000;
}
