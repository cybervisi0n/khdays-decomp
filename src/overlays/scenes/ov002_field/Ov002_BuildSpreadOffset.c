
#include "nitro/fx_types.h"

typedef struct {
    int m[9];
} Mtx33;

/* The stored yaw the whole scene is drawn at, as a sine and cosine pair. */
extern short data_0203e210[2];

/* The engine's sine and cosine table: entry n is at [n * 2] and [n * 2 + 1]. */
extern short data_0203d210[];

extern int VEC_Normalize(const VecFx32 *v, VecFx32 *pUnit);
extern void MTX_RotY33_(Mtx33 *pMtx, short nSin, short nCos);
extern void MTX_MultVec33(const VecFx32 *v, const Mtx33 *pMtx, VecFx32 *pOut);
extern void func_01ff9044(Mtx33 *pMtx, const VecFx32 *pAxis, int nSin, int nCos);
extern void ScaleVec3Fx12(int nFactor, const VecFx32 *pSrc, VecFx32 *pDst);

/* Turn a direction into a spread offset of a given length.
 *
 * The direction is flattened to the ground plane and normalised, rotated into
 * the scene's yaw frame, then tilted around that frame's own axis by the angle
 * whose index is the caller's step shifted right by four, and finally
 * normalised again and scaled out to the requested length.
 */
void Ov002_BuildSpreadOffset(const VecFx32 *pDir, int nLength, int nStep, VecFx32 *pOut)
{
    Mtx33 mtx;
    VecFx32 vAxis;
    VecFx32 vDir;
    int nAngle;

    vDir = *pDir;
    vDir.y = 0;
    VEC_Normalize(&vDir, &vDir);

    MTX_RotY33_(&mtx, data_0203e210[0], data_0203e210[1]);
    MTX_MultVec33(&vDir, &mtx, &vAxis);

    nAngle = nStep >> 4;
    func_01ff9044(&mtx, &vAxis, data_0203d210[nAngle * 2], data_0203d210[nAngle * 2 + 1]);
    MTX_MultVec33(&vDir, &mtx, &vDir);

    VEC_Normalize(&vDir, &vDir);
    ScaleVec3Fx12(nLength, &vDir, &vDir);

    *pOut = vDir;
}
