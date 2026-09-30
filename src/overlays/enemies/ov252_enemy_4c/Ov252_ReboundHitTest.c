/* Rebound hit test: sweeps the actor list with the given box, cylinder or sphere (in that order of
 * preference); kind 6 uses the +0x85 hit mask, others +0x86. Every entity whose +2 id bit is clear
 * in the mask is pushed horizontally away from the actor (the +z axis when directly above): kind 3 by
 * 0.94, kind 6 by 1.0 at a 2.5 lift, others by 0.5 (kind 5 lifted by 3.3, kind 1 by 2.0). On
 * acceptance the actor spawns effect 0 at the rebound point (the entity pushed out, or the sphere's
 * surface) and the entity's bit joins the result. The hits are added to the mask; returns the new
 * hit bits. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

extern const VecFx32 data_02042258;
extern int Ov107_CollectCapsuleOverlaps(int owner, void *box, int *hits);
extern int Ov107_CollectEntitiesTouchingDisc(int owner, void *cyl, int *hits);
extern int Ov107_CollectSphereOverlaps(int owner, void *sphere, int *hits);
extern void VEC_Subtract(const void *a, const void *b, VecFx32 *out);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void VEC_Add(const void *a, const void *b, void *out);
/* Defined taking kind as int: declared narrower here, which is what makes mwcc truncate the
 * argument at the call as the ROM does (declared as defined, the code comes out different). */
extern int Ov107_InvokeHitCallback(int hit, int owner, int item, u8 kind, VecFx32 *push, int z);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);

u8 Ov252_ReboundHitTest(int *state, int kind, VecFx32 *sphere, void *cyl, void *box)
{
    int n = 0;
    u8 *mask = kind == 6 ? (u8 *)state + 0x85 : (u8 *)state + 0x86;
    u8 hitMask = 0;
    int hits[4];
    VecFx32 push;
    VecFx32 dir;
    int i;

    if (box != 0) {
        n = Ov107_CollectCapsuleOverlaps(*state, box, hits);
    } else if (cyl != 0) {
        n = Ov107_CollectEntitiesTouchingDisc(*state, cyl, hits);
    } else if (sphere != 0) {
        n = Ov107_CollectSphereOverlaps(*state, sphere, hits);
    }
    for (i = 0; i < n; i++) {
        u8 bit = 1 << *(u16 *)(hits[i] + 2);

        if ((*mask & bit) != 0) {
            continue;
        }
        VEC_Subtract((void *)(hits[i] + 0x74), (void *)(*state + 0x74), &push);
        VEC_Normalize(&push, &dir);
        push.y = 0;
        if (VEC_Normalize(&push, &push) == 0) {
            push = data_02042258;
        }
        if (kind == 5) {
            push.y += 0x3500;
        }
        if (kind == 1) {
            push.y += 0x2000;
        }
        if (kind == 3) {
            ScaleVec3Fx12(0xf00, &push, &push);
        } else if (kind == 6) {
            push.y = 0x2800;
            ScaleVec3Fx12(0x1000, &push, &push);
        } else {
            ScaleVec3Fx12(0x800, &push, &push);
        }
        if (Ov107_InvokeHitCallback(hits[i], *state, *state, kind, &push, 0) == 0) {
            continue;
        }
        if (box != 0 || cyl != 0) {
            VEC_Add((void *)(hits[i] + 0x74), &push, &dir);
            func_ov107_020c0b90(*state, 0, dir, 0);
        } else if (sphere != 0) {
            ScaleVec3Fx12(*(int *)((u8 *)sphere + 0xc), &dir, &dir);
            VEC_Add(&dir, &push, &dir);
            VEC_Add(&dir, sphere, &dir);
            func_ov107_020c0b90(*state, 0, dir, 0);
        }
        hitMask |= bit;
    }
    *mask |= hitMask;
    return hitMask;
}
