/* d0e14 */

#include "nitro/types.h"
#include "nitro/fx_types.h"

extern int Ov107_CollectEntitiesTouchingDisc(int owner, void *cyl, int *hits);
extern int Ov107_CollectSphereOverlaps(int owner, void *sphere, int *hits);
extern void VEC_Subtract(const void *a, const void *b, VecFx32 *out);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern int Ov107_InvokeHitCallback(int hit, int owner, int item, int kind, VecFx32 *push, int z);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);

int Ov260_AttackHitTest(int *state, void *sphere, void *cyl)
{
    int hit = 0;
    int hits[4];
    VecFx32 push;
    long i;
    long n;
    u8 bit;

    if (cyl != 0) {
        n = Ov107_CollectEntitiesTouchingDisc(*(int *)(*state + 0x390), cyl, hits);
    } else {
        if (sphere == 0) {
            return 0;
        }
        n = Ov107_CollectSphereOverlaps(*(int *)(*state + 0x390), sphere, hits);
    }
    for (i = 0; i < n; i++) {
        bit = 1 << *(u16 *)(hits[i] + 2);

        if ((*((u8 *)state + 0x48) & bit) != 0) {
            continue;
        }
        if (cyl != 0) {
            VEC_Subtract((void *)(hits[i] + 0x74), cyl, &push);
        } else {
            VEC_Subtract((void *)(hits[i] + 0x74), sphere, &push);
        }
        push.y = 0;
        VEC_Normalize(&push, &push);
        ScaleVec3Fx12(0x800, &push, &push);
        push.y = 0x1000;
        if (Ov107_InvokeHitCallback(hits[i], *state, *(int *)(*state + 0x390), 6, &push, 0) == 0) {
            continue;
        }
        func_ov107_020c0b90(*(int *)(*state + 0x390), 7, *(VecFx32 *)state[6], 0);
        *((u8 *)state + 0x48) |= bit;
        hit = 1;
        *(int *)(*state + 0x38c) = 1;
    }
    return hit;
}
