/* Attack sweep of the ov219 enemy: sweeps the actor's +0x74 sphere and, for every entity whose
 * id bit (1 << its +2 id, as a byte) is clear in the +0x3c mask, pushes it away on the ground
 * plane by 0x800 (falling back to the shared forward vector) through the ov107 checker with the
 * given kind. On acceptance the contact point (the actor's position plus the radius along the
 * unflattened direction plus the push) is published with mode 0 and the id bit is set. Reaction
 * 0x136 mode 5 fires at the +8 position when anything was hit. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

struct Sphere { VecFx32 pos; int radius; };

extern int Ov107_CollectSphereOverlaps(int owner, struct Sphere *sphere, int *out);
extern void VEC_Subtract(void *a, void *b, void *d);
extern int VEC_Normalize(void *a, void *d);
extern void ScaleVec3Fx12(int scale, void *v, void *d);
/* Defined taking kind as int: declared narrower here, which is what makes mwcc truncate the
 * argument at the call as the ROM does (declared as defined, the code comes out different). */
extern int Ov107_InvokeHitCallback(int hit, int actor, int item, u8 kind, void *push, int z);
extern void VEC_Add(void *a, void *b, void *d);
extern void func_ov107_020c0b90(int actor, int a, VecFx32 v, int b);
extern void Ov107_BuildAndSendUpdate(int actor, int a, int id, void *at);
extern const VecFx32 data_02042258;

void Ov219_AttackSweep(int *state, int kind)
{
    struct Sphere sphere;
    int hits[4];
    VecFx32 push;
    VecFx32 out;
    VecFx32 fwd;
    int hit;
    long i;
    long n;
    u8 mask;

    sphere.pos = *(VecFx32 *)(*state + 0x74);
    sphere.radius = *(int *)(*state + 0x80);
    hit = 0;
    n = Ov107_CollectSphereOverlaps(*state, &sphere, hits);
    i = 0;
    if (n > 0) {
        fwd = data_02042258;
        do {
            mask = (u8)(1 << *(u16 *)(hits[i] + 2));
            if ((*(u8 *)((char *)state + 0x3c) & mask) == 0) {
                VEC_Subtract((void *)(hits[i] + 0x74), (void *)(*state + 0x74), &push);
                VEC_Normalize(&push, &out);
                push.y = 0;
                if (VEC_Normalize(&push, &push) == 0) {
                    push = fwd;
                }
                ScaleVec3Fx12(0x800, &push, &push);
                if (Ov107_InvokeHitCallback(hits[i], *state, *state, kind, &push, 0) != 0) {
                    ScaleVec3Fx12(sphere.radius, &out, &out);
                    VEC_Add(&out, &push, &out);
                    VEC_Add(&out, &sphere.pos, &out);
                    func_ov107_020c0b90(*state, 0, out, 0);
                    *(u8 *)((char *)state + 0x3c) |= mask;
                    hit = 1;
                }
            }
        } while (++i < n);
    }
    if (hit != 0) {
        Ov107_BuildAndSendUpdate(*state, 0x136, 5, (void *)state[2]);
    }
}
