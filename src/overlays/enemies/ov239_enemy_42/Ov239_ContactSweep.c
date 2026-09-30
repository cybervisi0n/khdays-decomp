/* Contact sweep of the ov239 enemy: collects the entities in the given sphere (else the actor's
 * own +0x74/+0x80 sphere) and, for each whose id bit is clear in the +0x30 mask, offers a hit of
 * the given kind pushed 0x800 away from the actor on the ground plane (forward when on top of
 * it); on acceptance effect 3 spawns at the sphere surface along the push and the id bit is set.
 * When anything was hit, reaction 0x138 mode 6 fires at the +8 point. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { VecFx32 pos; int radius; } Sphere;

extern int Ov107_CollectSphereOverlaps(int actor, Sphere *sphere, int *out);
extern void VEC_Subtract(void *a, void *b, VecFx32 *d);
extern int VEC_Normalize(VecFx32 *v, VecFx32 *d);
extern void ScaleVec3Fx12(int scale, VecFx32 *v, VecFx32 *d);
/* Defined taking kind as int: declared narrower here, which is what makes mwcc truncate the
 * argument at the call as the ROM does (declared as defined, the code comes out different). */
extern int Ov107_InvokeHitCallback(int hit, int a, int b, u8 kind, VecFx32 *push, int z);
extern void VEC_Add(void *a, void *b, VecFx32 *d);
extern void func_ov107_020c0b90(int actor, int a, VecFx32 v, int b);
extern void Ov107_BuildAndSendUpdate(int actor, int a, int id, void *at);
extern const VecFx32 data_02042258;

void Ov239_ContactSweep(int *state, int kind, Sphere *sphere)
{
    int hit;
    Sphere probe;
    int hits[4];
    VecFx32 push;
    VecFx32 dir;
    VecFx32 fwd;
    long i;
    long n;
    unsigned int mask;

    hit = 0;
    if (sphere != 0) {
        probe = *sphere;
    } else {
        probe.pos = *(VecFx32 *)(*state + 0x74);
        probe.radius = *(int *)(*state + 0x80);
    }
    n = Ov107_CollectSphereOverlaps(*state, &probe, hits);
    i = 0;
    if (n > 0) {
        fwd = data_02042258;
        do {
            mask = (1 << *(unsigned short *)(hits[i] + 2)) & 0xff;
            if ((*(u8 *)(state + 0xc) & mask) == 0) {
                VEC_Subtract((void *)(hits[i] + 0x74), (void *)(*state + 0x74), &push);
                VEC_Normalize(&push, &dir);
                push.y = 0;
                if (VEC_Normalize(&push, &push) == 0) {
                    push = fwd;
                }
                ScaleVec3Fx12(0x800, &push, &push);
                if (Ov107_InvokeHitCallback(hits[i], *state, *state, kind, &push, 0) != 0) {
                    ScaleVec3Fx12(probe.radius, &dir, &dir);
                    VEC_Add(&dir, &push, &dir);
                    VEC_Add(&dir, &probe, &dir);
                    func_ov107_020c0b90(*state, 3, dir, 0);
                    *(u8 *)(state + 0xc) |= mask;
                    hit = 1;
                }
            }
        } while (++i < n);
    }
    if (hit != 0) {
        Ov107_BuildAndSendUpdate(*state, 0x138, 6, (void *)state[2]);
    }
}
