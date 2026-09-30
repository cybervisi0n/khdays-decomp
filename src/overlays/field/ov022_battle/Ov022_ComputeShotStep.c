/* Ov022_ComputeShotStep -- how far a shot moves this frame.
 *
 * The shot's age takes the frame first. A shot whose kind homes, once it is
 * past the kind's delay and while the shooter still has a target, turns towards
 * that target: the direction to it and the shot's own direction are both
 * normalised, blended by the kind's weight, normalised again and scaled by the
 * kind's speed. A target far enough behind the shot is left alone.
 *
 * The step is that direction, and for a kind that decays it is scaled again by
 * the speed left after the kind's loss for every whole three frames of age,
 * floored at nothing. At 20 fps (frame-rate mode 1) both figures are taken
 * one and a half times, so the shot covers the same distance per second.
 *
 * Two shapes carry the original's codegen: the 20 fps figures are written as
 * an if/else over the whole load, not a load followed by a conditional scaling,
 * and the age is divided into steps before the mode is asked while the fixed
 * point shift stays at the two use sites.
 */

/* Ov022ShotDesc */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/engine.h"

struct ShotDesc {
    unsigned int nFlags;             /* 0x00 */
    u8 pad04[6];
    short nBlend;                    /* 0x0a */
    u8 pad0c[4];
    int nSpeed;                      /* 0x10 */
    u8 pad14[0xc];
    int nHomeDelay;                  /* 0x20 */
    int nDecayPerStep;               /* 0x24 */
};

/* Ov022Shot */
struct Shot {
    u8 pad00[4];
    int nAge;                        /* 0x004 */
    u8 pad08[0x14];
    VecFx32 vecDir;           /* 0x01c */
    u8 pad028[0xa4];
    VecFx32 vecPos;           /* 0x0cc */
    u8 pad0d8[0x60];
    struct ShotDesc *pDesc;          /* 0x138 */
};

/* Ov022ReactionCtx */
struct ReactionCtx {
    u8 pad00[8];
    void *pShooter;                  /* 0x08 */
};

/* Ov022_ValidateTargetRef */
extern int Ov022_ValidateTargetRef(void *pActor);
/* the actor's current target position */
extern VecFx32 *func_ov022_020ad0c0(void *pActor);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b,
                         VecFx32 *pOut);
extern int VEC_DotProduct(const VecFx32 *a, const VecFx32 *b);
/* VEC_Normalize */
extern void VEC_Normalize(const VecFx32 *pVec, VecFx32 *pOut);
/* ScaleVec3Fx12 */
extern void ScaleVec3Fx12(int nScale, const VecFx32 *pVec,
                          VecFx32 *pOut);
extern void VEC_MultAdd(int nScale, const VecFx32 *pVec,
                        const VecFx32 *pAdd, VecFx32 *pOut);

#define FX32_ONE 0x1000
#define DOT_LIMIT -0xa00
#define DESC_NO_HOME 0x80
#define DESC_DECAYS 4
#define FRAME_RATE_20FPS 1   /* GetFrameRateMode(): 0 = 30 fps, 1 = 20 fps, 2 = 60 fps */
#define STEP_FRAMES 3

void Ov022_ComputeShotStep(VecFx32 *pOut, struct ReactionCtx *pCtx,
                         struct Shot *pShot, int nDelta)
{
    void *pShooter;
    struct ShotDesc *pDesc;
    VecFx32 *pTarget;
    VecFx32 vecPos;
    VecFx32 vecToTarget;
    VecFx32 vecStep;
    int nScale;
    int nSteps;

    pShooter = pCtx->pShooter;
    pDesc = pShot->pDesc;
    pShot->nAge = pShot->nAge + nDelta;
    vecPos = pShot->vecPos;
    if (Ov022_ValidateTargetRef(pShooter) != 0 && pShot->nAge >= pDesc->nHomeDelay
        && (pDesc->nFlags & DESC_NO_HOME) == 0) {
        pTarget = func_ov022_020ad0c0(pShooter);
        VEC_Subtract(pTarget, &vecPos, &vecToTarget);
        if (VEC_DotProduct(&vecToTarget, &pShot->vecDir) >= DOT_LIMIT) {
            VEC_Normalize(&vecToTarget, &vecToTarget);
            VEC_Normalize(&pShot->vecDir, &pShot->vecDir);
            ScaleVec3Fx12(pDesc->nBlend, &vecToTarget, &vecToTarget);
            VEC_MultAdd(FX32_ONE - pDesc->nBlend, &pShot->vecDir, &vecToTarget,
                        &pShot->vecDir);
            VEC_Normalize(&pShot->vecDir, &pShot->vecDir);
            if (GetFrameRateMode() == FRAME_RATE_20FPS) {
                nScale = pDesc->nSpeed * 3 / 2;
            } else {
                nScale = pDesc->nSpeed;
            }
            ScaleVec3Fx12(nScale, &pShot->vecDir, &pShot->vecDir);
        }
    }
    vecStep = pShot->vecDir;
    if ((pDesc->nFlags & DESC_DECAYS) != 0) {
        nSteps = pShot->nAge / STEP_FRAMES * STEP_FRAMES;
        if (GetFrameRateMode() == FRAME_RATE_20FPS) {
            nScale = pDesc->nDecayPerStep * 3 / 2;
        } else {
            nScale = pDesc->nDecayPerStep;
        }
        if (pDesc->nSpeed - (nSteps >> 12) * nScale > 0) {
            if (GetFrameRateMode() == FRAME_RATE_20FPS) {
                nScale = pDesc->nDecayPerStep * 3 / 2;
            } else {
                nScale = pDesc->nDecayPerStep;
            }
            nScale = pDesc->nSpeed - (nSteps >> 12) * nScale;
        } else {
            nScale = 0;
        }
        ScaleVec3Fx12(nScale, &vecStep, &vecStep);
    }
    *pOut = vecStep;
}
