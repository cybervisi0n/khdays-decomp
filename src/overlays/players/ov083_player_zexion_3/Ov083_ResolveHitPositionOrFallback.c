/* Finds the ground under the point with a downward cast: returns the hit point slightly raised, or
 * the point 0x5000 higher when nothing is hit. */

#include "nitro/fx_types.h"
#include "game/engine.h"

extern void *EntityMgr_RunCastSimple(int a, void *b, void *c, int d);
extern int data_ov083_020b9b00;

void Ov083_ResolveHitPositionOrFallback(VecFx32 *src, int *out) {
    VecFx32 a;
    int q[3];
    void *r;
    int *ctx;
    a = *src;
    *(VecFx32 *)out = a;
    q[0] = 0;
    q[1] = 0x5ccd;
    q[2] = 0;
    ctx = *(int **)&data_ov083_020b9b00;
    r = EntityMgr_RunCastSimple(*(unsigned short *)((char *)ctx + 0x66), &a, q, ctx[8]);
    if (r == 0) {
        out[1] += 0x5000;
        return;
    }
    Vec3ScaleAddQ27(*(int *)((char *)r + 0xc), (const VecFx32 *)q, &a, (VecFx32 *)out);
    out[1] -= 0xe67;
}
