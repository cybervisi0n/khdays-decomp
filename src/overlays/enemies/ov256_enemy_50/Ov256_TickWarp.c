/* Warp tick of the ov256 actor: on the first tick (+0x4c clear) it is placed (020c5c54) 5.0 above
 * the +0x430 partner's +0x190 point, or, with a leash request (+0x78), 5.0 above a new perch
 * stored in +0x1c: one of five random spots (+0x7c) or the arena corner nearest to the +0xc
 * track. After 0x1a90 of the timer the +0x428 shape loses bit 1, pose 0x18 / partner motion 0xa
 * play, the actor is knocked back at the track (mode 6), +0x69 is set and the node moves on to
 * 020cec64. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"
#include "game/engine.h"

typedef struct { VecFx32 v[5]; } Spots5;
typedef struct { VecFx32 v[4]; } Corners4;
typedef struct { unsigned f : 8; } B8;

extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int VEC_Normalize(VecFx32 *v, VecFx32 *out);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov256_LeapTick(void);
extern const Spots5 data_ov256_020d2600;
extern const Corners4 data_ov256_020d25d0;

static inline void VecSet(VecFx32 *v, int x, int y, int z)
{
    v->x = x;
    v->y = y;
    v->z = z;
}

void Ov256_TickWarp(int *node)
{
    int *state = (int *)node[1];
    VecFx32 at;
    signed char i;
    int best;
    int bestDist;
    int dist;

    if (state[0x13] == 0) {
        VecSet(&at, *(int *)(*(int *)(*state + 0x430) + 0x190), *(int *)(*(int *)(*state + 0x430) + 0x194) + 0x5000,
               *(int *)(*(int *)(*state + 0x430) + 0x198));
        if (state[0x1e] != 0) {
            if (state[0x1f] != 0) {
                Spots5 spots = data_ov256_020d2600;

                at = spots.v[RandNextScaled(5)];
                *(VecFx32 *)(state + 7) = at;
                at.y += 0x5000;
            } else {
                Corners4 corners = data_ov256_020d25d0;
                VecFx32 d;

                for (i = 0; i < 4; i++) {
                    VEC_Subtract(&corners.v[i], (VecFx32 *)state[3], &d);
                    dist = VEC_Normalize(&d, &d);
                    if (i != 0) {
                        if (bestDist > dist) {
                            best = i;
                            bestDist = dist;
                        }
                    } else {
                        best = 0;
                        bestDist = dist;
                    }
                }
                *(VecFx32 *)(state + 7) = corners.v[best];
                at = *(VecFx32 *)(state + 7);
                at.y += 0x5000;
            }
        }
        Ov107_MoveNodeAndRelayout((Actor *)(*state), &at);
    }
    state[0x13] += *(int *)(node[0] + 0x2c);
    if (!(state[0x13] < 0x1a90)) {
        ((B8 *)(*(int *)(*state + 0x428) + 8))->f &= ~2;
        Ov107_PostTagUpdate((Actor *)(*state), 0x18, 0);
        Ov107_StartAnim(*(int *)(*state + 0x450), 0xa, 0);
        func_ov107_020c0b90(*state, 6, *(VecFx32 *)state[3], 0);
        state[0x13] = 0;
        *((u8 *)state + 0x69) = 1;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov256_LeapTick);
        return;
    }
}
