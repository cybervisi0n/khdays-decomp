/* LineBox_Face -- Face() of Eberly's (Magic Software) line-box distance, MAIN. Called by
 * CaseNoZeros (Shape_ProjectDominantAxis) once the face the line crosses is known: i0 is the axis of that
 * face, i1/i2 the other two. pnt is the line point (in box coordinates, overwritten with the
 * closest box point when pParam is not null), dir the line direction, box the box (extents at
 * +0x30), pmE = pnt - extent. The squared distance is accumulated into *pSqrDist and the line
 * parameter of the closest point returned through pParam. Fixed point: products are FX_Mul
 * (rounded 20.12), quotients FX_Div (FX_Inv two-argument form), 1/dir through FX_InvFx64c.
 * The axis indices are long and ppE is a vector reached through a component pointer, as the
 * original's Vector3 operator[] did. */

#include "nitro/fx_types.h"

typedef struct Box3 {
    char pad00[0x30];
    fx32 extent[3];                     /* +0x30 */
} Box3;

extern fx32 FX_Div(fx32 numer, fx32 denom);          /* FX_Div */
extern fx64c FX_InvFx64c(fx32 v);                  /* FX_InvFx64c */

/* FX_MulInline of the NitroSDK: rounded 20.12 product. */
static inline fx32 FX_Mul(fx32 v1, fx32 v2)
{
    return (fx32)(((fx64)v1 * v2 + 0x800LL) >> 12);
}

static inline fx32 FX_Mul32x64c(fx32 v32, fx64c v64c)
{
    fx64c tmp = v64c * v32 + 0x80000000LL;

    return (fx32)(tmp >> 32);
}

void LineBox_Face(long i0, long i1, long i2, fx32 *pnt, const fx32 *dir, const Box3 *box, const fx32 *pmE,
                   fx32 *pParam, fx32 *pSqrDist)
{
    VecFx32 ppE;                        /* Eberly's kPpE, indexed through its components */
    fx32 *pPpE = (fx32 *)&ppE;
    fx32 lSqr;
    fx64c inv;
    fx32 tmp;
    fx32 param;
    fx32 t;
    fx32 delta;

    pPpE[i1] = pnt[i1] + box->extent[i1];
    pPpE[i2] = pnt[i2] + box->extent[i2];
    if (FX_Mul(dir[i0], pPpE[i1]) >= FX_Mul(dir[i1], pPpE[i0])) {
        if (FX_Mul(dir[i0], pPpE[i2]) >= FX_Mul(dir[i2], pPpE[i0])) {
            /* v[i1] >= -e[i1], v[i2] >= -e[i2] (distance = 0) */
            if (pParam != 0) {
                pnt[i0] = box->extent[i0];
                inv = FX_InvFx64c(dir[i0]);
                pnt[i1] -= FX_Mul32x64c(FX_Mul(dir[i1], pmE[i0]), inv);
                pnt[i2] -= FX_Mul32x64c(FX_Mul(dir[i2], pmE[i0]), inv);
                *pParam = -FX_Mul32x64c(pmE[i0], inv);
            }
        } else {
            /* v[i1] >= -e[i1], v[i2] < -e[i2] */
            lSqr = FX_Mul(dir[i0], dir[i0]) + FX_Mul(dir[i2], dir[i2]);
            tmp = FX_Mul(lSqr, pPpE[i1]) - FX_Mul(dir[i1], FX_Mul(dir[i0], pmE[i0]) + FX_Mul(dir[i2], pPpE[i2]));
            if (tmp <= FX_Mul(2 * lSqr, box->extent[i1])) {
                t = FX_Div(tmp, lSqr);
                lSqr += FX_Mul(dir[i1], dir[i1]);
                tmp = pPpE[i1] - t;
                delta = FX_Mul(dir[i0], pmE[i0]) + FX_Mul(dir[i1], tmp) + FX_Mul(dir[i2], pPpE[i2]);
                param = -FX_Div(delta, lSqr);
                *pSqrDist += FX_Mul(pmE[i0], pmE[i0]) + FX_Mul(tmp, tmp) + FX_Mul(pPpE[i2], pPpE[i2]) + FX_Mul(delta, param);
                if (pParam != 0) {
                    *pParam = param;
                    pnt[i0] = box->extent[i0];
                    pnt[i1] = t - box->extent[i1];
                    pnt[i2] = -box->extent[i2];
                }
            } else {
                lSqr += FX_Mul(dir[i1], dir[i1]);
                delta = FX_Mul(dir[i0], pmE[i0]) + FX_Mul(dir[i1], pmE[i1]) + FX_Mul(dir[i2], pPpE[i2]);
                param = -FX_Div(delta, lSqr);
                *pSqrDist += FX_Mul(pmE[i0], pmE[i0]) + FX_Mul(pmE[i1], pmE[i1]) + FX_Mul(pPpE[i2], pPpE[i2]) + FX_Mul(delta, param);
                if (pParam != 0) {
                    *pParam = param;
                    pnt[i0] = box->extent[i0];
                    pnt[i1] = box->extent[i1];
                    pnt[i2] = -box->extent[i2];
                }
            }
        }
    } else {
        if (FX_Mul(dir[i0], pPpE[i2]) >= FX_Mul(dir[i2], pmE[i0])) {
            /* v[i1] < -e[i1], v[i2] >= -e[i2] */
            lSqr = FX_Mul(dir[i0], dir[i0]) + FX_Mul(dir[i1], dir[i1]);
            tmp = FX_Mul(lSqr, pPpE[i2]) - FX_Mul(dir[i2], FX_Mul(dir[i0], pmE[i0]) + FX_Mul(dir[i1], pPpE[i1]));
            if (tmp <= FX_Mul(2 * lSqr, box->extent[i2])) {
                t = FX_Div(tmp, lSqr);
                lSqr += FX_Mul(dir[i2], dir[i2]);
                tmp = pPpE[i2] - t;
                delta = FX_Mul(dir[i0], pmE[i0]) + FX_Mul(dir[i1], pPpE[i1]) + FX_Mul(dir[i2], tmp);
                param = -FX_Div(delta, lSqr);
                *pSqrDist += FX_Mul(pmE[i0], pmE[i0]) + FX_Mul(pPpE[i1], pPpE[i1]) + FX_Mul(tmp, tmp) + FX_Mul(delta, param);
                if (pParam != 0) {
                    *pParam = param;
                    pnt[i0] = box->extent[i0];
                    pnt[i1] = -box->extent[i1];
                    pnt[i2] = t - box->extent[i2];
                }
            } else {
                lSqr += FX_Mul(dir[i2], dir[i2]);
                delta = FX_Mul(dir[i0], pmE[i0]) + FX_Mul(dir[i1], pPpE[i1]) + FX_Mul(dir[i2], pmE[i2]);
                param = -FX_Div(delta, lSqr);
                *pSqrDist += FX_Mul(pmE[i0], pmE[i0]) + FX_Mul(pPpE[i1], pPpE[i1]) + FX_Mul(pmE[i2], pmE[i2]) + FX_Mul(delta, param);
                if (pParam != 0) {
                    *pParam = param;
                    pnt[i0] = box->extent[i0];
                    pnt[i1] = -box->extent[i1];
                    pnt[i2] = box->extent[i2];
                }
            }
        } else {
            /* v[i1] < -e[i1], v[i2] < -e[i2] */
            lSqr = FX_Mul(dir[i0], dir[i0]) + FX_Mul(dir[i2], dir[i2]);
            tmp = FX_Mul(lSqr, pPpE[i1]) - FX_Mul(dir[i1], FX_Mul(dir[i0], pmE[i0]) + FX_Mul(dir[i2], pPpE[i2]));
            if (tmp >= 0) {
                /* v[i1]-edge is closest */
                if (tmp <= FX_Mul(2 * lSqr, box->extent[i1])) {
                    t = FX_Div(tmp, lSqr);
                    lSqr += FX_Mul(dir[i1], dir[i1]);
                    tmp = pPpE[i1] - t;
                    delta = FX_Mul(dir[i0], pmE[i0]) + FX_Mul(dir[i1], tmp) + FX_Mul(dir[i2], pPpE[i2]);
                    param = -FX_Div(delta, lSqr);
                    *pSqrDist += FX_Mul(pmE[i0], pmE[i0]) + FX_Mul(tmp, tmp) + FX_Mul(pPpE[i2], pPpE[i2]) + FX_Mul(delta, param);
                    if (pParam != 0) {
                        *pParam = param;
                        pnt[i0] = box->extent[i0];
                        pnt[i1] = t - box->extent[i1];
                        pnt[i2] = -box->extent[i2];
                    }
                } else {
                    lSqr += FX_Mul(dir[i1], dir[i1]);
                    delta = FX_Mul(dir[i0], pmE[i0]) + FX_Mul(dir[i1], pmE[i1]) + FX_Mul(dir[i2], pPpE[i2]);
                    param = -FX_Div(delta, lSqr);
                    *pSqrDist += FX_Mul(pmE[i0], pmE[i0]) + FX_Mul(pmE[i1], pmE[i1]) + FX_Mul(pPpE[i2], pPpE[i2]) + FX_Mul(delta, param);
                    if (pParam != 0) {
                        *pParam = param;
                        pnt[i0] = box->extent[i0];
                        pnt[i1] = box->extent[i1];
                        pnt[i2] = -box->extent[i2];
                    }
                }
                return;
            }

            lSqr = FX_Mul(dir[i0], dir[i0]) + FX_Mul(dir[i1], dir[i1]);
            tmp = FX_Mul(lSqr, pPpE[i2]) - FX_Mul(dir[i2], FX_Mul(dir[i0], pmE[i0]) + FX_Mul(dir[i1], pPpE[i1]));
            if (tmp >= 0) {
                /* v[i2]-edge is closest */
                if (tmp <= FX_Mul(2 * lSqr, box->extent[i2])) {
                    t = FX_Div(tmp, lSqr);
                    lSqr += FX_Mul(dir[i2], dir[i2]);
                    tmp = pPpE[i2] - t;
                    delta = FX_Mul(dir[i0], pmE[i0]) + FX_Mul(dir[i1], pPpE[i1]) + FX_Mul(dir[i2], tmp);
                    param = -FX_Div(delta, lSqr);
                    *pSqrDist += FX_Mul(pmE[i0], pmE[i0]) + FX_Mul(pPpE[i1], pPpE[i1]) + FX_Mul(tmp, tmp) + FX_Mul(delta, param);
                    if (pParam != 0) {
                        *pParam = param;
                        pnt[i0] = box->extent[i0];
                        pnt[i1] = -box->extent[i1];
                        pnt[i2] = t - box->extent[i2];
                    }
                } else {
                    lSqr += FX_Mul(dir[i2], dir[i2]);
                    delta = FX_Mul(dir[i0], pmE[i0]) + FX_Mul(dir[i1], pPpE[i1]) + FX_Mul(dir[i2], pmE[i2]);
                    param = -FX_Div(delta, lSqr);
                    *pSqrDist += FX_Mul(pmE[i0], pmE[i0]) + FX_Mul(pPpE[i1], pPpE[i1]) + FX_Mul(pmE[i2], pmE[i2]) + FX_Mul(delta, param);
                    if (pParam != 0) {
                        *pParam = param;
                        pnt[i0] = box->extent[i0];
                        pnt[i1] = -box->extent[i1];
                        pnt[i2] = box->extent[i2];
                    }
                }
                return;
            }

            /* (v[i1], v[i2])-corner is closest */
            lSqr += FX_Mul(dir[i2], dir[i2]);
            delta = FX_Mul(dir[i0], pmE[i0]) + FX_Mul(dir[i1], pPpE[i1]) + FX_Mul(dir[i2], pPpE[i2]);
            param = -FX_Div(delta, lSqr);
            *pSqrDist += FX_Mul(pmE[i0], pmE[i0]) + FX_Mul(pPpE[i1], pPpE[i1]) + FX_Mul(pPpE[i2], pPpE[i2]) + FX_Mul(delta, param);
            if (pParam != 0) {
                *pParam = param;
                pnt[i0] = box->extent[i0];
                pnt[i1] = -box->extent[i1];
                pnt[i2] = -box->extent[i2];
            }
        }
    }
}
