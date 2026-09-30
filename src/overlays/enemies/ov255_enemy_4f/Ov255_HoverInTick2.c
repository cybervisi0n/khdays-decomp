/* Hover-in tick of an ov255 state: the +0x50 timer accumulates the owner's rate and the +0x5c path
 * point is resolved (Ov255_SteerToTarget) into the +0x10 step. Once the +0xc idle byte clears, the
 * owner's +0x24 hook receives note 7 of data_ov255_020d2b20, animation 0x19 plays, +0x44 and +0x65
 * clear and the tick hands over to Ov255_RingTick. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

typedef struct { u16 lo; u16 hi; } Cmd4;

extern void Ov255_SteerToTarget(int *state, int point, VecFx32 *dir, int *speed);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const Cmd4 data_ov255_020d2b20[];
extern void Ov255_RingTick(int *node);

void Ov255_HoverInTick2(int *node)
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
    {
        Cmd4 note;
        Cmd4 *p = &note;

        p->hi = data_ov255_020d2b20[7].hi;
        p->lo = data_ov255_020d2b20[7].lo;
        if (*(void (**)(int, Cmd4 *, int))(*state + 0x24) != 0) {
            (*(void (**)(int, Cmd4 *, int))(*state + 0x24))(*state, p, 4);
        }
    }
    Ov107_PostTagUpdate((Actor *)(*state), 0x19, 0);
    state[0x11] = 0;
    *((unsigned char *)state + 0x65) = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov255_RingTick);
}
