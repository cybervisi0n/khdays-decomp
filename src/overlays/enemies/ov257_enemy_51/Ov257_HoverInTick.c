/* Glide-in tick of an ov257 state: the +0x50 timer accumulates the owner's rate and the +0x5c path
 * point is resolved (Ov257_SteerToTarget) into the +0x10 step. Once the +0xc idle byte clears, the
 * owner's +0x24 hook receives note 2 of data_ov257_020d325c, animation 0x16 plays looped, +0x44,
 * +0x48 and +0x65 clear, +0x68 takes the owner's hit points (+0x21a) as a fixed-point value and the
 * tick hands over to Ov257_HealBurstTick. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

typedef struct { u16 lo; u16 hi; } Cmd4;

extern void Ov257_SteerToTarget(int *state, int point, VecFx32 *dir, int *speed);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const Cmd4 data_ov257_020d325c[];
extern void Ov257_HealBurstTick(int *node);

void Ov257_HoverInTick(int *node)
{
    int *state = (int *)node[1];
    VecFx32 dir;
    int speed;

    state[0x15] += *(int *)(node[0] + 0x2c);
    Ov257_SteerToTarget(state, state[0x18], &dir, &speed);
    ScaleVec3Fx12(speed, &dir, (VecFx32 *)(state + 4));
    if (*(unsigned char *)state[3] != 0) {
        return;
    }
    {
        Cmd4 note;
        Cmd4 *p = &note;

        p->hi = data_ov257_020d325c[2].hi;
        p->lo = data_ov257_020d325c[2].lo;
        if (*(void (**)(int, Cmd4 *, int))(*state + 0x24) != 0) {
            (*(void (**)(int, Cmd4 *, int))(*state + 0x24))(*state, p, 4);
        }
    }
    Ov107_PostTagUpdate((Actor *)(*state), 0x16, 1);
    state[0x11] = 0;
    state[0x12] = 0;
    *((unsigned char *)state + 0x76) = 0;
    state[0x17] = *(short *)(*state + 0x200 + 0x1a) << 12;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov257_HealBurstTick);
}
