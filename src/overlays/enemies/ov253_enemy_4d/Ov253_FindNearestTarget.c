/* Ov253_FindNearestTarget -- pick the nearest visible target: walks the state's +0xa8 list and, for
 * each node with bit 1 of +0x40 and bit 0 of its +0x60 low byte set, takes the distance from
 * `pos` to its +0x74 position minus its +0x80 radius; a node whose direction dots against `dir`
 * above `minDot` and whose distance is within `maxDist` and below the best so far becomes the
 * pick. Returns the picked node (0 when none). Codegen: `state` is an `int *` and the list address is
 * spelled `(int)state + 0xa8`; an `int state` (or `(int)(state + 0x2a)`) colours state last (r8)
 * instead of the ROM's r4. */

#include "nitro/fx_types.h"

struct hw60 { unsigned short lo : 8, hi : 8; };
struct w40 { int b0 : 1, b1 : 1; };

extern int List_First(int list);
extern int List_Next(int list);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern int VEC_DotProduct(const VecFx32 *a, const VecFx32 *b);

int Ov253_FindNearestTarget(int *self, const VecFx32 *pos, const VecFx32 *dir, int minDot, int maxDist) {
    int *state = (int *)self[1];
    int bestnode = 0;
    int best = 0x7fffffff;
    VecFx32 v;
    int it;
    int node;

    it = List_First((int)state + 0xa8);
    node = it == 0 ? 0 : *(int *)it;
    while (node != 0) {
        if (((struct w40 *)(node + 0x40))->b1 != 0 && (((struct hw60 *)(node + 0x60))->lo & 1) != 0) {
            int dist;
            int radius = *(int *)(node + 0x80);
            VEC_Subtract((VecFx32 *)(node + 0x74), pos, &v);
            dist = VEC_Normalize(&v, &v) - radius;
            if (VEC_DotProduct(dir, &v) > minDot && dist < best && dist <= maxDist) {
                best = dist;
                bestnode = node;
            }
        }
        it = List_Next((int)state + 0xa8);
        node = it == 0 ? 0 : *(int *)it;
    }
    return bestnode;
}
