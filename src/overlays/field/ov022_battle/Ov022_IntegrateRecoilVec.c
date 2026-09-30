/* Applies an actor's recoil: adds its horizontal velocity to the position, halves the velocity, and
 * flags the actor while it is still strong (clearing it when it becomes negligible). */

#include "nitro/fx_types.h"

extern void VEC_Add(int *a, int *b, int *c);
extern void ScaleVec3Fx12(int scale, int *src, int *dst);
extern int VEC_Mag(int *v);

void Ov022_IntegrateRecoilVec(int obj) {
    VecFx32 stack;
    int m;
    *(unsigned long long *)obj &= ~0x80000000000LL;
    if (*(int *)(obj + 0x4a4) == 0 && *(int *)(obj + 0x4ac) == 0) return;
    stack = *(VecFx32 *)(obj + 0x4a4);
    stack.y = 0;
    VEC_Add((int *)(obj + 0x498), (int *)&stack, (int *)(obj + 0x498));
    ScaleVec3Fx12(0x800, (int *)(obj + 0x4a4), (int *)(obj + 0x4a4));
    m = VEC_Mag((int *)(obj + 0x4a4));
    if (m > 0x19a) {
        *(unsigned long long *)obj |= 0x80000000000LL;
        return;
    }
    if (m < 0x29) {
        *(int *)(obj + 0x4ac) = 0;
        *(int *)(obj + 0x4a8) = 0;
        *(int *)(obj + 0x4a4) = 0;
    }
}
