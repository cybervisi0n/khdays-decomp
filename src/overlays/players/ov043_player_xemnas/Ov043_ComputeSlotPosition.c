/* Computes a world position for the mission enemy's request slot: picks the local offset of
 * data_ov043_020b578c (two VecFx32, indexed by slot % 2), rotates it about Y by the model's
 * heading (+0x80, flipped by 0x8000, negated sine/cosine) and adds the actor's +0x48c origin
 * into out. */

#include "nitro/fx_types.h"

typedef struct { int m[9]; } MtxFx33;
typedef struct { VecFx32 v[2]; } OffsetPair;

extern void MTX_RotY33_(MtxFx33 *m, int s, int c);
extern void MTX_MultVec33(const VecFx32 *v, const MtxFx33 *m, VecFx32 *out);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern const OffsetPair data_ov043_020b578c;
extern short data_0203d210[];

void Ov043_ComputeSlotPosition(char *self, int slot, VecFx32 *out)
{
    VecFx32 v;
    VecFx32 origin;
    MtxFx33 m;
    OffsetPair offsets;
    int i;

    offsets = data_ov043_020b578c;
    i = (unsigned short)(*(unsigned short *)(*(char **)(self + 0x20) + 0x80) - 0x8000) >> 4;
    MTX_RotY33_(&m, -data_0203d210[i * 2], -data_0203d210[i * 2 + 1]);
    MTX_MultVec33(&offsets.v[slot % 2], &m, &v);
    origin = *(VecFx32 *)(self + 0x8c + 0x400);
    VEC_Add(&v, &origin, out);
}
