/* Ov022_ProjectPointToScreen -- project a world point to screen pixels,
 * refusing points behind the camera.
 *
 * The point is projected (the y answer is raised by eight lines); a point the
 * projector accepts is still refused when the direction from the camera eye
 * to it points away from the eye-to-target direction. Both answers are handed
 * back as fx32, and the projector's own answer (or -1) is the result.
 */

/* Ov022ScreenPos: an fx32 screen position */

#include "nitro/fx_types.h"

struct ScreenPos {
    int x;
    int y;
};

#define SCREEN_Y_LIFT 8
#define PROJECT_FAILED (-1)

extern VecFx32 data_020475ac;     /* cached camera eye */
extern VecFx32 data_020475c4;     /* cached camera target */

extern int func_0201653c(VecFx32 *pWorld, int *pScreenX, int *pScreenY);
extern void VEC_Subtract(VecFx32 *pA, VecFx32 *pB, VecFx32 *pOut);
extern void VEC_Normalize(VecFx32 *pSrc, VecFx32 *pDst);   /* VEC_Normalize */
extern int VEC_DotProduct(VecFx32 *pA, VecFx32 *pB);

int Ov022_ProjectPointToScreen(VecFx32 *pWorld, struct ScreenPos *pScreen)
{
    VecFx32 vecToPoint;
    VecFx32 vecView;
    int nScreenX;
    int nScreenY;
    int nResult;
    VecFx32 *pEye;
    VecFx32 *pTarget;

    pEye = &data_020475ac;
    pTarget = &data_020475c4;
    nResult = func_0201653c(pWorld, &nScreenX, &nScreenY);
    nScreenY -= SCREEN_Y_LIFT;
    if (nResult != PROJECT_FAILED) {
        VEC_Subtract(pWorld, pEye, &vecToPoint);
        VEC_Subtract(pTarget, pEye, &vecView);
        VEC_Normalize(&vecToPoint, &vecToPoint);
        VEC_Normalize(&vecView, &vecView);
        if (VEC_DotProduct(&vecView, &vecToPoint) < 0) {
            nResult = PROJECT_FAILED;
        }
    }
    pScreen->x = nScreenX << 12;
    pScreen->y = nScreenY << 12;
    return nResult;
}
