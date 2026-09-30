/* Moves the point by the step (stopping at walls when asked), then drops it onto the ground below
 * (or clamps it to the floor limit). */

#include "nitro/fx_types.h"
#include "game/engine.h"

extern int EntityMgr_RunSphereCast();
extern void VEC_Add();
extern int EntityMgr_RunRayCast();

void Ov082_ProbeLandingPoint(VecFx32 *out, char *p1, char *p2, int p3, int p4, int p5)
{
    int *r4;
    int *obj;
    VecFx32 va;
    VecFx32 vb;

    r4 = *(int **)(p1 + 0xdb4);

    if (p5 != 0) {
        obj = (int *)EntityMgr_RunSphereCast(*(unsigned char *)(p2 + 0x15c), p3, p4, 0x800, r4[8]);
        if (obj != 0 && obj[2] == 0) {
            Vec3ScaleAddQ27(obj[3] - 0x800, (const VecFx32 *)p4, (const VecFx32 *)p3, &va);
            *(char *)(p2 + 0x12c) = 5;
        } else {
            VEC_Add(p3, p4, &va);
        }
    } else {
        VEC_Add(p3, p4, &va);
    }

    va.y += 0x1000;
    vb.x = 0;
    vb.z = 0;
    vb.y = -0x2000;

    obj = (int *)EntityMgr_RunRayCast(*(unsigned char *)(p2 + 0x15c), &va, &vb, r4[8]);
    if (obj != 0) {
        Vec3ScaleAddQ27(obj[3], &vb, &va, &va);
    } else {
        int lim = *(int *)(p2 + 0x138);
        if (va.y >= lim) {
            va.y = lim;
        } else {
            va.y = lim - 0x2000;
        }
    }

    *out = va;
}
