/* Plan a jump from the +0x18 point to `target`: starting 0.75 up with a rising step of 0.75 that
 * shrinks by 0xd0 per frame, count the frames until the arc falls back to the target's height;
 * the +0x28 horizontal speed is the flat distance over that count (64-bit, 20 fraction bits), the +8 heading faces
 * the target, +0x48 is set and the next move is 2. */

#include "nitro/fx_types.h"

extern void VEC_Subtract(const void *a, const void *b, VecFx32 *out);
extern int VEC_Normalize(VecFx32 *v, VecFx32 *d);
extern long long func_020201b8(long long num, long long den);
extern void Quat_FromTwoVectors(void *rotation, const VecFx32 *from, const VecFx32 *to);
extern const VecFx32 data_02042258;

void Ov254_PlanJump(int *state, VecFx32 *target)
{
    VecFx32 p;
    VecFx32 d;
    int step;
    int n;

    p = *(VecFx32 *)state[6];
    p.y += 0xc00;
    step = 0xc00;
    n = 1;
    while (step > 0 || p.y > target->y) {
        step -= 0xd0;
        p.y += step;
        n++;
    }
    VEC_Subtract(target, (VecFx32 *)state[6], &d);
    d.y = 0;
    *(long long *)(state + 10) = VEC_Normalize(&d, &d);
    *(long long *)(state + 10) = func_020201b8(*(long long *)(state + 10) << 20, n);
    Quat_FromTwoVectors(state + 2, &data_02042258, &d);
    state[0x12] = 1;
    *(signed char *)(*state + 0x1c7) = 2;
}
