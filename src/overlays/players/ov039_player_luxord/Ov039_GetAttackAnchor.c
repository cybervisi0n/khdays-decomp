/* Compute one of this enemy's attack anchor points into `out`: the plain offset {0, 0x2000,
 * 0xfae} while the shared rig's +0x2cd4 flag is clear, otherwise the left/right offset picked
 * by `side` ({-0xc00, 0x2000, 0xfae} / {0xc00, 0x2000, 0xfd7}); the offset is turned by the
 * enemy's facing (angle at +0x80 of the +0x20 node, biased by 0x8000, /16 into the sin/cos
 * table, both components negated) and added to the enemy's origin at +0x48c. */

#include "nitro/fx_types.h"

typedef struct { int m[9]; } Mtx33;
typedef struct { VecFx32 v[2]; } Ov039SidePair;

extern void MTX_RotY33_(Mtx33 *m, int nSin, int nCos);
extern void MTX_MultVec33(const VecFx32 *v, const Mtx33 *m, VecFx32 *out);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern char *data_ov039_020b5600;
extern const VecFx32 data_ov039_020b5390;               /* the plain anchor offset */
extern const Ov039SidePair data_ov039_020b53b0;      /* the left/right anchor offsets */
extern short data_0203d210[];

void Ov039_GetAttackAnchor(char *self, int side, VecFx32 *out)
{
    Mtx33 mFacing;
    VecFx32 vDefault;
    Ov039SidePair sides;
    int nIndex;

    vDefault = data_ov039_020b5390;
    sides = data_ov039_020b53b0;

    if (*(int *)(data_ov039_020b5600 + 0x2000 + 0xcd4) == 0) {
        out->x = vDefault.x;
        out->y = vDefault.y;
        out->z = vDefault.z;
    } else {
        out->x = sides.v[side].x;
        out->y = sides.v[side].y;
        out->z = sides.v[side].z;
    }
    nIndex = (unsigned short)(*(unsigned short *)(*(char **)(self + 0x20) + 0x80) - 0x8000) >> 4;
    MTX_RotY33_(&mFacing, -data_0203d210[nIndex * 2], -data_0203d210[nIndex * 2 + 1]);
    MTX_MultVec33(out, &mFacing, out);
    VEC_Add((VecFx32 *)(self + 0x8c + 0x400), out, out);
}
