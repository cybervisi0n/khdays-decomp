/* Rebound hit test: sweeps the actor list with the given sphere or segment; every entity whose +2 id
 * bit is clear in the +0x71 mask is pushed 2.0 horizontally away from the actor (the +z axis of
 * data_02042258 when directly above; kind 3 lifts it by 1.0 instead of scaling). On acceptance the
 * actor spawns effect 0 at the rebound point (the volume's surface or segment end, pushed out) and the
 * entity's bit joins the result. The hits are added to the mask and, if any, reaction 0 mode 0x50
 * fires at the +8 point. Returns the new hit bits. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { VecFx32 p0; VecFx32 dir; int nLength; int nRadius; } Segment;

extern const VecFx32 data_02042258;
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
extern void Ov107_BuildAndSendUpdate(int owner, int id, int mode, void *at);

u8 Ov254_ReboundHitTest(int *state, int kind, VecFx32 *sphere, Segment *seg)
{
    int n = 0;
    u8 hitMask = 0;
    int hits[4];
    VecFx32 push;
    VecFx32 dir;
    VecFx32 end;
    int i;

    if (sphere != 0) {
        n = Ov107_CollectSphereOverlaps(*state, sphere, hits);
    } else if (seg != 0) {
        n = Ov107_CollectSegmentOverlaps(*state, seg, hits);
    }
    for (i = 0; i < n; i++) {
        u8 bit = 1 << *(u16 *)(hits[i] + 2);

        if ((*((u8 *)state + 0x71) & bit) != 0) {
            continue;
        }
        VEC_Subtract((void *)(hits[i] + 0x74), (void *)(*state + 0x74), &push);
        VEC_Normalize(&push, &dir);
        push.y = 0;
        if (VEC_Normalize(&push, &push) == 0) {
            push = data_02042258;
        }
        if (kind == 3) {
            push.y = 0x1000;
        } else {
            ScaleVec3Fx12(0x2000, &push, &push);
        }
        if (Ov107_InvokeHitCallback(hits[i], *state, *state, kind, &push, 0) == 0) {
            continue;
        }
        if (sphere != 0) {
            ScaleVec3Fx12(*(int *)((u8 *)sphere + 0xc), &dir, &dir);
            VEC_Add(&dir, sphere, &dir);
        } else {
            ScaleVec3Fx12(seg->nLength, &seg->dir, &end);
            VEC_Add(&end, seg, &end);
            VEC_Add(&dir, &end, &dir);
        }
        VEC_Add(&dir, &push, &dir);
        func_ov107_020c0b90(*state, 0, dir, 0);
        hitMask |= bit;
    }
    *((u8 *)state + 0x71) |= hitMask;
    if (hitMask != 0) {
        Ov107_BuildAndSendUpdate(*state, 0, 0x50, (void *)state[2]);
    }
    return hitMask;
}
