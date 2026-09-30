/* Approach tick: the +0xc / +0x14 velocity points from the +0x18 point towards the +8 track
 * (speed 0.25, level); a fresh waypoint (020cd128) copies +0x34 to +0x30; the +0x10 climb is the
 * height difference to route point 0xb, clamped to +-0x7fff. Within 10.0 pose 0x1a plays, the
 * +0x44 timer clears, the +0x50 start takes the track's +4 and +0x54 the distance to route point
 * 5; the +0x70 flag clears and the node moves to 020cf4e8. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern void VEC_Subtract(const void *a, const void *b, VecFx32 *out);
extern int VEC_Normalize(VecFx32 *v, VecFx32 *d);
extern int Ov254_TrackTargetFlatDistance(int *node);
extern int Ov254_PanelYForPhase(int *state, int a);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov254_DropTick(void);

static inline int FX_Mul(int a, int b) {
    return (int)(((long long)a * b + 0x800) >> 12);
}

void Ov254_ApproachTick(int *node)
{
    int *state = (int *)node[1];
    VecFx32 d;
    int dist;

    VEC_Subtract(state + 6, (void *)state[2], &d);
    d.y = 0;
    dist = VEC_Normalize(&d, &d);
    if (Ov254_TrackTargetFlatDistance(node) != 0) {
        state[0xc] = state[0xd];
    }
    state[4] = Ov254_PanelYForPhase(state, 0xb) - *(int *)(state[2] + 4);
    state[4] = state[4] > 0x7fff ? 0x7fff : (state[4] < -0x7fff ? -0x7fff : state[4]);
    state[3] = FX_Mul(d.x, 0x400);
    state[5] = FX_Mul(d.z, 0x400);
    if (dist > 0xa000) {
        return;
    }
    Ov107_PostTagUpdate((Actor *)(*state), 0x1a, 0);
    state[0x11] = 0;
    state[0x14] = *(int *)(state[2] + 4);
    state[0x15] = Ov254_PanelYForPhase(state, 5) - state[0x14];
    *((u8 *)state + 0x70) = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov254_DropTick);
}
