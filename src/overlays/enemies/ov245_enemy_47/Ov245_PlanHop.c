/* Ov245_PlanHop -- plan a hop towards the target: from the state's +8 origin raises the
 * landing height by `step` per hop (each hop 0.75 shorter) while the step is still rising or the
 * landing height stays above the target's +4 height, aims the +0x18 direction at the target (normalised, its length kept at +0x30 as a
 * 64-bit value) and divides that length by the number of hops (64-bit divide). */

#include "nitro/types.h"
#include "nitro/fx_types.h"

extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern s64 func_020201b8(s64 num, s64 den);

void Ov245_PlanHop(int *state, VecFx32 *target, int step) {
    VecFx32 goal;
    int hops;

    goal = *(VecFx32 *)state[2];
    goal.y += step;
    hops = 1;
    while (step > 0 || goal.y > target->y) {
        step -= 0xc0;
        goal.y += step;
        hops++;
    }
    VEC_Subtract(target, (VecFx32 *)state[2], (VecFx32 *)(state + 6));
    *(s64 *)(state + 0xc) = VEC_Normalize((VecFx32 *)(state + 6), (VecFx32 *)(state + 6));
    *(s64 *)(state + 0xc) = func_020201b8(*(s64 *)(state + 0xc) << 20, hops);
}
