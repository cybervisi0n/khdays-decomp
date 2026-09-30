/* ov022: turn the actor's drift into this frame's step, and decay it.
 *
 * Drift below a floor is not worth carrying, so it is dropped outright and the
 * step comes back as nothing.
 *
 * Otherwise the step is the drift with its vertical set aside: only the
 * horizontal part is capped, at a speed the actor cannot exceed however hard it
 * was pushed. One input bit halves both the step and the drift again, and the
 * vertical is put back untouched either way -- falling is never capped and
 * never slowed.
 *
 * The drift itself is decayed at the end, whatever happened to the step.
 */

#include "nitro/types.h"
#include "nitro/fx_types.h"

#define DRIFT_FLOOR 0x200
#define STEP_CAP 0x900
#define SLOW_SCALE 0x333
#define DRIFT_DECAY 0xc80
#define SLOW_BIT 2

/* Ov022Actor */
struct Actor {
    u8 pad0000[0x24];
    u32 nInputMask;              /* 0x0024 */
    u8 pad0028[0x454];
    VecFx32 vecDrift;     /* 0x047c */
};

extern int VEC_Mag(const VecFx32 *pA);
/* VEC_Normalize */
extern int VEC_Normalize(const VecFx32 *pSrc, VecFx32 *pDst);
/* scale a vector */
extern void ScaleVec3Fx12(int nScale, const VecFx32 *pSrc,
                          VecFx32 *pDst);

void Ov022_TakeDriftStep(struct Actor *pActor, VecFx32 *pOut)
{
    VecFx32 vecStep;
    int nFall;

    if (VEC_Mag(&pActor->vecDrift) <= DRIFT_FLOOR) {
        pActor->vecDrift.z = 0;
        pActor->vecDrift.y = 0;
        pActor->vecDrift.x = 0;
        pOut->z = 0;
        pOut->y = 0;
        pOut->x = 0;
        return;
    }
    vecStep = pActor->vecDrift;
    nFall = vecStep.y;
    vecStep.y = 0;
    if (VEC_Mag(&vecStep) > STEP_CAP) {
        VEC_Normalize(&vecStep, &vecStep);
        ScaleVec3Fx12(STEP_CAP, &vecStep, &vecStep);
    }
    if ((pActor->nInputMask & SLOW_BIT) != 0) {
        ScaleVec3Fx12(SLOW_SCALE, &vecStep, &vecStep);
        ScaleVec3Fx12(SLOW_SCALE, &pActor->vecDrift, &pActor->vecDrift);
    }
    vecStep.y = nFall;
    *pOut = vecStep;
    ScaleVec3Fx12(DRIFT_DECAY, &pActor->vecDrift, &pActor->vecDrift);
}
