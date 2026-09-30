/* Finds the ground under the point with a long downward ray cast from above it: returns the hit
 * point slightly raised, or the point lowered by the search height when nothing is hit. */

#include "nitro/fx_types.h"
#include "game/engine.h"

extern void *EntityMgr_RunRayCast(int a, void *b, void *c, int d);
extern int data_ov104_020bc2a0;

void Ov104_ResolveHitPositionRaised(VecFx32 *src, int *out) {
    VecFx32 a;
    int q[3];
    void *r;
    int *ctx;
    a = *src;
    *(VecFx32 *)out = a;
    a.y += 0x25000;
    q[0] = 0;
    q[1] = -0x4a000;
    q[2] = 0;
    ctx = *(int **)&data_ov104_020bc2a0;
    r = EntityMgr_RunRayCast(*(unsigned short *)((char *)ctx + 0x66), &a, q, ctx[8]);
    if (r == 0) {
        out[1] -= 0x25000;
        return;
    }
    Vec3ScaleAddQ27(*(int *)((char *)r + 0xc), (const VecFx32 *)q, &a, (VecFx32 *)out);
    out[1] += 0x333;
}
