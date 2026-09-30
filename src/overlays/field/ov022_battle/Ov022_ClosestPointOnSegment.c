/* ov022: drop a point onto a segment and report whatever the caller asked for.
 *
 * The answer is the position along the segment, as a fraction: the point's
 * offset from the start projected on the segment, divided by the segment's own
 * squared length. A segment of no length gives zero rather than a division.
 *
 * The fraction is not clamped, so a point beyond either end gives a value
 * outside zero to one and the foot of the perpendicular lands off the segment.
 *
 * Three outputs are all optional and each is skipped when its pointer is null:
 * the foot itself, the distance from the point to it, and the unit direction
 * between them. The last argument picks which way round that direction runs.
 */

#include "nitro/types.h"
#include "nitro/fx_types.h"

extern void VEC_Subtract(const VecFx32 *pA, const VecFx32 *pB,
                         VecFx32 *pOut);
extern int VEC_DotProduct(const VecFx32 *pA, const VecFx32 *pB);
extern int VEC_Mag(const VecFx32 *pA);
extern void VEC_MultAdd(int nFactor, const VecFx32 *pStep,
                        const VecFx32 *pFrom, VecFx32 *pOut);
/* the tree's name for the fixed-point divide; it takes the numerator and the
 * denominator, not a single value to invert */
extern int FX_Div(int nNum, int nDen);
/* VEC_Normalize */
extern int VEC_Normalize(const VecFx32 *pSrc, VecFx32 *pDst);

int Ov022_ClosestPointOnSegment(VecFx32 *pOutFoot, int *pOutDist,
                        VecFx32 *pOutDir, const VecFx32 *pPoint,
                        const VecFx32 *pFrom, const VecFx32 *pTo,
                        int bFromPoint)
{
    VecFx32 vecSeg;
    VecFx32 vecFoot;
    VecFx32 vecDelta;
    int nAlong;
    int nLen2;

    VEC_Subtract(pTo, pFrom, &vecSeg);
    nAlong = VEC_DotProduct(pPoint, &vecSeg) - VEC_DotProduct(pFrom, &vecSeg);
    nLen2 = VEC_DotProduct(&vecSeg, &vecSeg);
    if (nLen2 == 0) {
        nAlong = 0;
    } else {
        nAlong = FX_Div(nAlong, nLen2);
    }
    VEC_MultAdd(nAlong, &vecSeg, pFrom, &vecFoot);
    if (bFromPoint != 0) {
        VEC_Subtract(pPoint, &vecFoot, &vecDelta);
    } else {
        VEC_Subtract(&vecFoot, pPoint, &vecDelta);
    }
    if (pOutFoot != 0) {
        *pOutFoot = vecFoot;
    }
    if (pOutDist != 0) {
        *pOutDist = VEC_Mag(&vecDelta);
    }
    if (pOutDir != 0) {
        VEC_Normalize(&vecDelta, pOutDir);
    }
    return nAlong;
}
