/* Target finder of the ov241 enemy (x3: ov241/242/243): from the actor's +0x3bc heading builds
 * the forward vector (sine/cosine table) and an origin 1.5 units behind the +0x74 position;
 * walks the world's +0xa8 actor list for live actors (bit 1 of +0x40, bit 0 of +0x60) within
 * 30.0 of height, keeping the closest by flattened distance that is either inside 1.5 or, up
 * to 15.0, whose flattened direction from the origin (world Z when degenerate) lies within
 * the 0xf74 cosine of the heading, and that Ov241_IsPathToTargetClear accepts with its position and
 * +0x80 radius. Returns the best actor or 0. */

#include "nitro/fx_types.h"

struct flags40 { int bit0 : 1, bit1 : 1; };
struct hw60 { unsigned short lo : 8, hi : 8; };

extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int *List_First(void *list);
extern int *List_Next(void *list);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern int VEC_DotProduct(const VecFx32 *a, const VecFx32 *b);
extern int Ov241_IsPathToTargetClear(int *state, VecFx32 pos, int radius);
extern const short data_0203d210[];
extern const VecFx32 data_02042258;

int Ov241_FindTarget(int node)
{
    int *state = *(int **)(node + 4);
    int heading = *(int *)(*state + 0x3bc);
    int best = 0;
    int bestDist = 0x7fffffff;
    int dist;
    int actor;
    int owner = *(int *)(*state + 4);
    int idx;
    VecFx32 fwd;
    VecFx32 origin;
    VecFx32 d;
    VecFx32 pos;
    int *pNode;
    int dy;

    idx = (unsigned short)((0x28BE60DB9391LL * heading + 0x80000000000LL) >> 44);   /* FX_RAD_TO_IDX */
    fwd.x = data_0203d210[(idx >> 4) << 1];                                          /* FX_SinIdx */
    fwd.z = data_0203d210[((idx >> 4) << 1) + 1];                                    /* FX_CosIdx */
    fwd.y = 0;
    ScaleVec3Fx12(-0x1800, &fwd, &origin);
    VEC_Add(&origin, (VecFx32 *)(*state + 0x74), &origin);
    pNode = List_First((void *)(owner + 0xa8));
    actor = pNode == 0 ? 0 : *pNode;
    while (actor != 0) {
        if (((struct flags40 *)(actor + 0x40))->bit1 && (((struct hw60 *)(actor + 0x60))->lo & 1) != 0) {
            pos = *(VecFx32 *)(actor + 0x74);
            VEC_Subtract(&pos, (VecFx32 *)(*state + 0x74), &d);
            dy = d.y;
            if (dy < 0) {
                dy = -dy;
            }
            if (dy <= 0x1e000) {
                d.y = 0;
                dist = VEC_Normalize(&d, &d);
                if (dist < bestDist) {
                    VEC_Subtract(&pos, &origin, &d);
                    d.y = 0;
                    if (VEC_Normalize(&d, &d) == 0) {
                        d = data_02042258;
                    }
                    if (dist <= 0x1800 || (dist <= 0xf000 && VEC_DotProduct(&d, &fwd) >= 0xf74)) {
                        if (Ov241_IsPathToTargetClear(state, pos, *(int *)(actor + 0x80)) != 0) {
                            bestDist = dist;
                            best = actor;
                        }
                    }
                }
            }
        }
        pNode = List_Next((void *)(owner + 0xa8));
        actor = pNode == 0 ? 0 : *pNode;
    }
    return best;
}
