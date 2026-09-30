/* Turn `vec` by the fixed-point heading `angle` (Y rotation from the shared trig table), in place,
 * and return it. Twin of ov256 020cd054. */

#include "nitro/fx_types.h"

typedef struct { int m[9]; } Mtx33;

extern void MTX_RotY33_(Mtx33 *pMtx, int nSin, int nCos);
extern void MTX_MultVec33(const VecFx32 *pIn, const Mtx33 *pMtx, VecFx32 *pOut);
extern const short data_0203d210[];

#define ANG2IDX(a) ((unsigned short)(((long long)(a) * 0x28be60db9391LL + 0x80000000000LL) >> 44) >> 4)

VecFx32 Ov252_TurnVecY(int angle, VecFx32 *vec)
{
    Mtx33 rot;

    {
        int idx = ANG2IDX(angle) * 2;

        MTX_RotY33_(&rot, data_0203d210[idx], data_0203d210[idx + 1]);
    }
    MTX_MultVec33(vec, &rot, vec);
    return *vec;
}
