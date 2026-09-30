/*
 * Ov002_IsWithinRangeHorizontal - test whether a target point is within an object's horizontal range (ARM).
 *
 * A negative range (param_1[0x20] sign bit set) is a wildcard that always passes. Otherwise it takes
 * the object position (VecFx32 at param_1+0x2c) and the target param_2, flattens both to the ground
 * plane by zeroing their Y components, and returns whether their distance (VEC_Distance) is within
 * the range param_1[0x20].
 */

#include "nitro/fx_types.h"

extern int VEC_Distance(void *a, void *b, int c);

int Ov002_IsWithinRangeHorizontal(int param_1, void *param_2)
{
    int radius = *(int *)(param_1 + 0x20);
    VecFx32 a, b;
    if (radius & 0x80000000) return 1;
    a = *(VecFx32 *)(param_1 + 0x2c);
    b = *(VecFx32 *)param_2;
    a.y = 0;
    b.y = 0;
    return VEC_Distance(&a, &b, 0) <= *(int *)(param_1 + 0x20);
}
