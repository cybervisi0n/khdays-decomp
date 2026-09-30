/* Fires one part of the ov037 enemy (x4: ov037/055/075/092) unless actor flag bit 27 forbids
 * it: rotates the part's muzzle offset (+0x1c) by the actor's heading and adds the actor origin
 * (+0x48c) for the spawn position, rotates the part's aim (+0x28) the same way and normalises
 * it, takes the kind from the part's +0x19 byte and the speed from its first word (record 2),
 * and marks the request 2 when the caller asks for the alternate shot and 1 for bit 1 of the
 * part's +0x14 byte. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { int m[9]; } Mtx33;
typedef struct { u8 b0 : 1, b1 : 1; } PartBits;
typedef struct {
    VecFx32 pos;
    short f0c, f0e, f10, f12;
    int f14, f18, f1c, f20, f24, f28;
} Params;

extern void MTX_RotY33_(Mtx33 *m, int s, int c);
extern void MTX_MultVec33(const VecFx32 *v, const Mtx33 *m, VecFx32 *out);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int VEC_Mag(const VecFx32 *v);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *unit);
extern void Ov022_SendPlacementMessage(char *self, Params *p);
extern short data_0203d210[];
extern VecFx32 data_ov093_020bc2a4;

int Ov093_FirePartShot(char *self, int unused, char *pPart, int bAlt)
{
    Params p;
    VecFx32 v;
    Mtx33 m;
    int idx;

    if ((*(unsigned long long *)self & 0x8000000ULL) == 0) {
        v = data_ov093_020bc2a4;
        idx = (unsigned short)(*(unsigned short *)(*(char **)(self + 0x20) + 0x80) - 0x8000) >> 4;
        MTX_RotY33_(&m, -data_0203d210[idx * 2], -data_0203d210[idx * 2 + 1]);
        MTX_MultVec33((VecFx32 *)(pPart + 0x1c), &m, &v);
        p.f12 = (short)*(int *)pPart;
        VEC_Add((VecFx32 *)(self + 0x8c + 0x400), &v, &p.pos);
        MTX_MultVec33((VecFx32 *)(pPart + 0x28), &m, &v);
        if (VEC_Mag(&v) != 0) {
            VEC_Normalize(&v, &v);
        }
        p.f0c = (short)v.x;
        p.f0e = (short)v.y;
        p.f10 = (short)v.z;
        p.f14 = *(signed char *)(pPart + 0x19);
        p.f18 = 2;
        p.f1c = 0;
        p.f20 = 0;
        p.f28 = 0;
        p.f24 = 0;
        if (bAlt != 0) {
            p.f20 |= 2;
        }
        if (((PartBits *)(pPart + 0x14))->b1) {
            p.f20 |= 1;
        }
        Ov022_SendPlacementMessage(self, &p);
    }
    return 0;
}
