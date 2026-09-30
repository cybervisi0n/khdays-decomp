/* Contact sweep of the ov248 enemy: collects the entities in the given box (or
 * else sphere) and, for each whose id bit is clear in the +0x62 mask, offers a hit of the given
 * kind pushed 0x800 away from the +0x494 point on the ground plane (forward when on top of it);
 * on acceptance effect 8 spawns at the entity pushed out of the box, or effect 1 at the sphere
 * surface along the push, and the id bit is set. When anything was hit, reaction 0x146 mode 5
 * (kinds 0/5), mode 6 (kind 1) or reaction 0 mode 0x53 (kind 2) fires at the +0xc position. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { VecFx32 pos; int radius; } Sphere;

extern int Ov107_CollectEntitiesTouchingDisc(int actor, void *box, int *out);
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

void Ov248_ContactSweep(int *state, int kind, Sphere *sphere, void *box)
{
    int hits[4];
    VecFx32 push;
    VecFx32 dir;
    VecFx32 fwd;
    int n;
    int hit;
    int i;
    unsigned int mask;

    n = 0;
    hit = 0;
    if (box != 0) {
        n = Ov107_CollectEntitiesTouchingDisc(*state, box, hits);
    } else if (sphere != 0) {
        n = Ov107_CollectSphereOverlaps(*state, sphere, hits);
    }
    i = 0;
    if (n > 0) {
        fwd = data_02042258;
        do {
            mask = (1 << *(unsigned short *)(hits[i] + 2)) & 0xff;
            if ((*(u8 *)((char *)state + 0x62) & mask) == 0) {
                VEC_Subtract((void *)(hits[i] + 0x74), (void *)(*state + 0x494), &push);
                VEC_Normalize(&push, &dir);
                push.y = 0;
                if (VEC_Normalize(&push, &push) == 0) {
                    push = fwd;
                }
                ScaleVec3Fx12(0x800, &push, &push);
                if (Ov107_InvokeHitCallback(hits[i], *state, *state, kind, &push, 0) != 0) {
                    if (box != 0) {
                        VEC_Add((void *)(hits[i] + 0x74), &push, &dir);
                        func_ov107_020c0b90(*state, 8, dir, 0);
                    } else if (sphere != 0) {
                        ScaleVec3Fx12(sphere->radius, &dir, &dir);
                        VEC_Add(&dir, &push, &dir);
                        VEC_Add(&dir, sphere, &dir);
                        func_ov107_020c0b90(*state, 1, dir, 0);
                    }
                    *(u8 *)((char *)state + 0x62) |= mask;
                    hit = 1;
                }
            }
        } while (++i < n);
    }
    if (hit == 0) {
        return;
    }
    switch (kind) {
    case 0:
    case 5:
        Ov107_BuildAndSendUpdate(*state, 0x146, 5, (void *)state[3]);
        break;
    case 1:
        Ov107_BuildAndSendUpdate(*state, 0x146, 6, (void *)state[3]);
        break;
    case 2:
        Ov107_BuildAndSendUpdate(*state, 0, 0x53, (void *)state[3]);
        break;
    case 3:
    case 4:
        break;
    }
}
