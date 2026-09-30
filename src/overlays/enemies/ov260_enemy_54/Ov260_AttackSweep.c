/* Ov260_AttackSweep -- attack sweep of the ov260 actor: collects the hits of the given cylinder,
 * sphere or segment (or, with none, a segment from the owner's +0x424 joint along its heading,
 * 0x1800 or 0x3000 long by the +0x470 flag, radius 0x400). Each hit not yet struck (bit of +0x79)
 * is pushed away flat at half strength (020ca918 with `kind`); an accepted hit spawns the impact
 * effect (7 for cylinder/sphere, 0 otherwise) at the sphere's scaled contact or the victim, and is
 * marked struck. Any hit sounds the attack (020cd148 mode 4). */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct { VecFx32 p0; VecFx32 dir; int nLength; int nRadius; } Segment;

extern const VecFx32 data_02042258;
extern int Ov107_CollectEntitiesTouchingDisc(int owner, void *cyl, int *hits);
extern int Ov107_CollectSphereOverlaps(int owner, void *sphere, int *hits);
extern int Ov107_CollectSegmentOverlaps(int owner, void *seg, int *hits);
extern void VEC_Subtract(const void *a, const void *b, VecFx32 *out);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void VEC_Add(const void *a, const void *b, void *out);
/* Defined taking kind as int: declared narrower here, which is what makes mwcc truncate the
 * argument at the call as the ROM does (declared as defined, the code comes out different). */
extern int Ov107_InvokeHitCallback(int hit, int owner, int item, u8 kind, VecFx32 *push, int z);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern void Ov260_PlaySound(int owner, int mode, int arg);

void Ov260_AttackSweep(int *state, int kind, VecFx32 *sphere, void *cyl, void *seg)
{
    int hits[4];
    Segment sweep;
    VecFx32 push;
    VecFx32 dir;
    long i;
    int effect = 7;
    long n;
    int hit = 0;
    u8 bit;

    if (cyl != 0) {
        n = Ov107_CollectEntitiesTouchingDisc(*state, cyl, hits);
    } else if (sphere != 0) {
        n = Ov107_CollectSphereOverlaps(*state, sphere, hits);
    } else if (seg != 0) {
        n = Ov107_CollectSegmentOverlaps(*state, seg, hits);
        effect = 0;     /* cleared after the query in each branch, as the ROM hoists it */
    } else {
        sweep.p0 = *(VecFx32 *)(*(int *)(*state + 0x424) + 0x14);
        sweep.nLength = *(int *)(*state + 0x470) == 0 ? 0x1800 : 0x3000;
        sweep.nRadius = 0x400;
        Vec3TransformViaTempMtx(&sweep.dir, (void *)(*(int *)(*state + 0x424) + 4), &data_02042258);
        n = Ov107_CollectSegmentOverlaps(*state, &sweep, hits);
        effect = 0;
    }
    for (i = 0; i < n; i++) {
        bit = 1 << *(u16 *)(hits[i] + 2);

        if ((*((u8 *)state + 0x79) & bit) != 0) {
            continue;
        }
        VEC_Subtract((void *)(hits[i] + 0x74), (void *)(*state + 0x74), &push);
        VEC_Normalize(&push, &dir);
        push.y = 0;
        if (VEC_Normalize(&push, &push) == 0) {
            push = data_02042258;
        }
        ScaleVec3Fx12(0x800, &push, &push);
        if (Ov107_InvokeHitCallback(hits[i], *state, *state, kind, &push, 0) == 0) {
            continue;
        }
        if (sphere != 0) {
            ScaleVec3Fx12(*(int *)((u8 *)sphere + 0xc), &dir, &dir);
            VEC_Add(&dir, &push, &dir);
            VEC_Add(&dir, sphere, &dir);
            func_ov107_020c0b90(*state, effect, dir, 0);
        } else {
            VEC_Add((void *)(hits[i] + 0x74), &push, &dir);
            func_ov107_020c0b90(*state, effect, dir, 0);
        }
        *((u8 *)state + 0x79) |= bit;
        hit = 1;
    }
    if (hit == 0) {
        return;
    }
    Ov260_PlaySound(*state, 4, state[4]);
}
