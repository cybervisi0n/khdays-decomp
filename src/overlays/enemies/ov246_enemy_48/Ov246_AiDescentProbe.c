/* Ov246_AiDescentProbe -- AI drop step: the state's hit target (+0x2c) is
 * refreshed from the actor (020cab14); with none the task ends (0203c640).
 * A probe straight down (020cdf9c with (0, -0x32000, 0)) that hits aims the
 * second transform (+0x8) along the hit normal (kVecUp -> normal quaternion,
 * 0202ed60 / 0203c9d0), lifts the target y (+0x20) by 0x100 and moves it
 * there.  A second probe by half the frame step (-(delta * 30) / 2) that
 * hits aims the third transform (+0xc) the same way and snaps the position
 * (+0x10) to the target (+0x1c); otherwise the position becomes the query
 * origin, the step normalised (01ff8d18) as the direction with speed =
 * length and range 0x400 asks the actor for hits (020c8f44): without any
 * the position slides to the target x / z and down by the step on the
 * first transform (+0x4); with hits the nearest 64-bit distance (02036304
 * at each result + 0x74) scales the direction (rounded high word) and is
 * added to the origin on the third transform, and the timer (+0x28)
 * clears.  Either hit path chains the follow-up 020ce68c on the node's
 * slot (signed byte +0x20).  Codegen: the state pointer is initialised
 * before the zeroed step vector (the aggregate initialiser's zero is the
 * 020cab14 argument); the stack aggregates are declared in reverse frame
 * order (step, normal, the two quaternions, results, query, distance); the
 * step is kept in a callee-saved register from the top; the half step is a
 * rounded 64-bit FX_Mul by 0x800; the query origin is a struct copy; the
 * nearest distance is a signed 64-bit compare with the min in a register
 * pair (counter declared before the count); the hit branches come first.
 */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct Quat {
    fx32 q[4];
} Quat;

typedef struct HitQuery {
    VecFx32 vOrigin;             /* 0x00 */
    VecFx32 vDir;                /* 0x0c */
    fx32 nSpeed;              /* 0x18 */
    fx32 nRange;              /* 0x1c */
} HitQuery;

typedef struct DropState {
    int   pSelf;              /* 0x00: actor address */
    char *pSrtA;              /* 0x04 */
    char *pSrtB;              /* 0x08 */
    char *pSrtC;              /* 0x0c */
    VecFx32  vPos;               /* 0x10 */
    VecFx32  vTarget;            /* 0x1c */
    int   nTimer;             /* 0x28 */
    int   nHit;               /* 0x2c */
} DropState;

typedef struct DropNode {
    int  *pClock;             /* 0x00: +0x2c frame delta */
    DropState *pState;        /* 0x04 */
    char  pad_08[0x20 - 8];
    signed char nSlot;        /* 0x20 */
} DropNode;

extern const VecFx32 data_02042264;                                       /* kVecUp */
extern int   Ov107_FindNearestObject(int nActor, int nArg);
extern int   Ov246_ProbeGround(DropState *pState, VecFx32 *pStep, VecFx32 *pNormal, fx32 nY);
extern void  Quat_FromTwoVectors(Quat *pOut, const VecFx32 *pFrom, const VecFx32 *pTo);
extern void  Srt_SetRotationQuat(char *pSrt, const Quat *pRot);              /* Srt_SetRotationQuat */
extern void  Srt_SetTranslation(char *pSrt, const VecFx32 *pPos);              /* Srt_SetTranslation */
extern fx32  VEC_Normalize(const VecFx32 *pIn, VecFx32 *pOut);                /* VEC_Normalize */
extern int   Ov107_CollectSegmentOverlaps(int nActor, HitQuery *pQuery, int *pResults);
extern void  Segment_ClosestPoint(void *pVolume, HitQuery *pQuery, fx64 *pDist);
extern void  VEC_Add(const VecFx32 *pA, const VecFx32 *pB, VecFx32 *pOut);
extern void  SetIndexedSlot(DropNode *pNode, int nSlot, void (*pfnCallback)(void)); /* SetIndexedSlot */
extern void  Ov246_EnterGroundDrop(void);

static inline fx32 FX_Mul(fx32 a, fx32 b)
{
    return (int)(((long long)a * b + 0x800) >> 12);
}

static inline fx32 FX_MulHigh64(fx64 a, fx32 b)
{
    return (int)((a * b + 0x80000000LL) >> 32);
}

void Ov246_AiDescentProbe(DropNode *pNode)
{
    DropState *pState = pNode->pState;
    VecFx32 vStep = {0, 0, 0};
    VecFx32 vNormal;
    Quat rotA;
    Quat rotB;
    int aResult[4];
    HitQuery query;
    fx64 nDist;
    int nStep;
    int i;
    int nCount;
    fx64 nMin;

    nStep = pNode->pClock[0x2c / 4] * 30;
    pState->nHit = Ov107_FindNearestObject(pState->pSelf, 0);
    if (pState->nHit == 0) {
        Task_MarkFinished(pNode);
        return;
    }
    vStep.y = -0x32000;
    if (Ov246_ProbeGround(pState, &vStep, &vNormal, vStep.y) != 0) {
        Quat_FromTwoVectors(&rotA, &data_02042264, &vNormal);
        Srt_SetRotationQuat(pState->pSrtB + 4, &rotA);
        pState->vTarget.y += 0x100;
        Srt_SetTranslation(pState->pSrtB + 4, &pState->vTarget);
    }
    vStep.y = -FX_Mul(nStep, 0x800);
    if (Ov246_ProbeGround(pState, &vStep, &vNormal, vStep.y) != 0) {
        Quat_FromTwoVectors(&rotB, &data_02042264, &vNormal);
        Srt_SetRotationQuat(pState->pSrtC + 4, &rotB);
        pState->vPos = pState->vTarget;
        Srt_SetTranslation(pState->pSrtC + 4, &pState->vPos);
        SetIndexedSlot(pNode, pNode->nSlot, Ov246_EnterGroundDrop);
    } else {
        query.vOrigin = pState->vPos;
        query.nSpeed = VEC_Normalize(&vStep, &query.vDir);
        query.nRange = 0x400;
        nCount = Ov107_CollectSegmentOverlaps(pState->pSelf, &query, aResult);
        if (nCount != 0) {
            nMin = 0x7fffffffffffffffLL;
            for (i = 0; i < nCount; i++) {
                Segment_ClosestPoint((char *)aResult[i] + 0x74, &query, &nDist);
                if (nDist < nMin) {
                    nMin = nDist;
                }
            }
            pState->vPos.x = FX_MulHigh64(nMin, query.vDir.x);
            pState->vPos.y = FX_MulHigh64(nMin, query.vDir.y);
            pState->vPos.z = FX_MulHigh64(nMin, query.vDir.z);
            VEC_Add(&query.vOrigin, &pState->vPos, &pState->vPos);
            Srt_SetTranslation(pState->pSrtC + 4, &pState->vPos);
            pState->nTimer = 0;
            SetIndexedSlot(pNode, pNode->nSlot, Ov246_EnterGroundDrop);
        } else {
            pState->vPos.x = pState->vTarget.x;
            pState->vPos.y += vStep.y;
            pState->vPos.z = pState->vTarget.z;
            Srt_SetTranslation(pState->pSrtA + 4, &pState->vPos);
        }
    }
}
