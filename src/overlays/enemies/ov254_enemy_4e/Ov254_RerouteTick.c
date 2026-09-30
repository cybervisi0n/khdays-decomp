/* Re-route tick: the +0x44 timer accumulates the frame rate; the +0x10 climb follows a quarter
 * sine of the +0x54 drop over the timer (clamped to 0..0x2a80) relative to the +8 track's height
 * above the +0x50 start, and the +0xc / +0x14 velocity is the +0x30 yaw's direction at speed
 * 0.125. The first tick knocks the actor back at its feet (020cdbbc, side -1). At 0x2a80, with a
 * route (+0x454 points on the +0x434 list), the flattened offsets from the track to the current
 * (+0x6c) and previous route point are compared and the farther one is chosen, unless the
 * actor's +0x3dc target (020cab14) is visible from it (ray through the +4 item's world, radius
 * 0x3d98), in which case the other; +0x18 becomes that point. The node then moves on to
 * 020cf3a4. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/engine.h"

extern void Ov254_KnockbackAtFeet(int actor, int side);
extern VecFx32 *List_First(void *list);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int VEC_Mag(const VecFx32 *v);
extern int Ov107_FindNearestObject(int obj, int kind);
extern int Collision_CastSphere(void *world, const VecFx32 *from, const VecFx32 *step, int radius);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov254_ApproachTick(void);
extern const short data_0203d210[];
extern const VecFx32 data_02041dc8;

#define ANG2IDX(a) ((unsigned short)(((long long)(a) * 0x28be60db9391LL + 0x80000000000LL) >> 44) >> 4)

static inline int FX_Mul(int a, int b) {
    return (int)(((long long)a * b + 0x800) >> 12);
}

void Ov254_RerouteTick(int *node)
{
    int *state = (int *)node[1];
    VecFx32 pts[2];
    VecFx32 d;
    VecFx32 *p;
    int t;
    int prev;
    int i;
    int pick;
    int item;

    t = state[0x11] += *(int *)(node[0] + 0x2c);
    if (t > 0x2a80) {
        t = 0x2a80;
    } else if (t < 0) {
        t = 0;
    }
    state[4] = FX_Mul(data_0203d210[ANG2IDX(t * 0x1922 / 0x2a80) * 2], state[0x15]) -
               (*(int *)(state[2] + 4) - state[0x14]);
    state[3] = FX_Mul(data_0203d210[ANG2IDX(state[0xc]) * 2], 0x200);
    state[5] = FX_Mul(data_0203d210[ANG2IDX(state[0xc]) * 2 + 1], 0x200);
    if ((*((u8 *)state + 0x70) & 1) == 0 && state[0x11] >= 0) {
        *((u8 *)state + 0x70) |= 1;
        Ov254_KnockbackAtFeet(*state, -1);
    }
    if (state[0x11] < 0x2a80) {
        return;
    }
    if (*(int *)(*state + 0x454) > 0) {
        prev = (state[0x1b] + *(int *)(*state + 0x454) - 1) % *(int *)(*state + 0x454);
        pts[1] = data_02041dc8;
        pts[0] = pts[1];
        for (p = List_First((void *)(*state + 0x434)), i = 0; p != 0;
             p = (VecFx32 *)List_Next((void *)(*state + 0x434)), i++) {
            if (i == state[0x1b]) {
                pts[0] = *p;
            }
            if (i == prev) {
                pts[1] = *p;
            }
        }
        for (i = 0; i < 2; i++) {
            VEC_Subtract(&pts[i], (VecFx32 *)state[2], &pts[i]);
            pts[i].y = 0;
        }
        pick = VEC_Mag(&pts[0]) <= VEC_Mag(&pts[1]) ? 1 : 0;
        *(int *)(*state + 0x3dc) = Ov107_FindNearestObject(*state, 0);
        if (*(int *)(*state + 0x3dc) != 0) {
            item = *(int *)(*state + 4);
            VEC_Subtract((VecFx32 *)(*(int *)(*state + 0x3dc) + 0x190), &pts[pick], &d);
            if (Collision_CastSphere(*(void **)(item + 0x7c), &pts[pick], &d, 0x3d98) != 0) {
                pick ^= 1;
            }
        }
        VEC_Add(&pts[pick], (VecFx32 *)state[2], (VecFx32 *)(state + 6));
    }
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov254_ApproachTick);
}
