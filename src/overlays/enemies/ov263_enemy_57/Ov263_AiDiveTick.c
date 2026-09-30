/* Ov263_AiDiveTick -- AI hover-dive step (byte-identical in ov232 /
 * ov263 / ov265 / ov280 with their own helpers).  The base heading
 * (data_020d368c) rotated by the owner's yaw (020d06b4) becomes the
 * velocity (+0x30) and the timer (+0x28) advances by the frame delta.
 * Phase (+0x4a) 0: the heading updates (020d0620), the angle (+0x1c, also
 * copied to +0x18) turns by the turn byte (+0x4f) times pi / 2, a height
 * (+0x20) under 0x2000 moves to phase 2, 0x2800 of timer ends.  Phase 1:
 * the velocity is the axis (+0x3c) scaled by 0xc00; every 0xc00 of timer
 * the pass count (+0x48) rises, the heading updates, and a height under
 * 0x5000 or three passes end.  Phase 2: the heading updates, the actor's
 * forward (0202f384 on its transform +0xa0 with kVecForward 02042258) and the
 * facing (VEC_DotProduct fills it) give the side sign (x * z - z * x); a
 * turn byte on the other side turns the angle by pi / 2; height above
 * 0x4800 returns to phase 0, 0x2800 of timer ends.  Phase 3: 0x200 of
 * timer ends.  The heading scaled to 0x10000 probes for a hit (020d0748):
 * a hit counts (+0x4b), the first adding pi times the turn to the angle,
 * the ninth copying it to +0x18, the eleventh ending (tested before the
 * timer clears); phase 3 follows; no hit clears the count.  Contact bit 1 (+0x17a)
 * ends too: sub-state 2 (+0x1c7) and the slot (signed byte +0x20) released
 * (0203c634 with 0).  Codegen: the done flag's zero is the timer reset in
 * phase 1; the side sign is a signed char compared with the turn byte;
 * the hit count is a u8 incremented with `and #0xff`; the four vectors
 * are declared heading, axis copy, forward, facing.
 */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/engine.h"

#define FX32_PI      0x3244
#define FX32_PI_HALF 0x1922

typedef struct DiveActor {
    u8   pad_000[0xa0];
    u8   srt[0x17a - 0xa0];   /* 0x0a0 */
    u8   bGrounded : 1;       /* 0x17a */
    u8   bBlocked : 1;
    u8   pad_17b[0x1c7 - 0x17b];
    u8   nSubState;           /* 0x1c7 */
} DiveActor;

typedef struct DiveState {
    DiveActor *pSelf;         /* 0x00 */
    u8   pad_04[0x18 - 4];
    fx32 nAngleB;             /* 0x18 */
    fx32 nAngle;              /* 0x1c */
    fx32 nHeight;             /* 0x20 */
    u8   pad_24[4];
    int  nTimer;              /* 0x28 */
    u8   pad_2c[4];
    VecFx32 vVel;                /* 0x30 */
    VecFx32 vAxis;               /* 0x3c */
    u8   nPasses;             /* 0x48 */
    u8   pad_49;
    u8   nPhase;              /* 0x4a */
    u8   nHits;               /* 0x4b */
    u8   pad_4c[3];
    signed char nTurn;        /* 0x4f */
} DiveState;

typedef struct DiveNode {
    int  *pClock;             /* 0x00: +0x2c frame delta */
    DiveState *pState;        /* 0x04 */
    u8    pad_08[0x20 - 8];
    signed char nSlot;        /* 0x20 */
} DiveNode;

extern const VecFx32 data_ov263_020d36b0;                                 /* base heading */
extern const VecFx32 data_02042258;                                       /* kVecForward */
extern void  Ov263_rotateVecByOwnerYaw(VecFx32 *pOut, DiveNode *pNode, const VecFx32 *pIn); /* rotate by the owner yaw */
extern void  Ov263_AcquireTarget(DiveNode *pNode);                      /* update the heading */
extern void  ScaleVec3Fx12(int nScale, const VecFx32 *pSrc, VecFx32 *pDst);   /* ScaleVec3Fx12 */
extern void  VEC_DotProduct(VecFx32 *pOut, const VecFx32 *pIn);
extern int   Ov263_ProbeGround(DiveNode *pNode, const VecFx32 *pDir, int nArg); /* probe for a hit */
extern void  SetIndexedSlot(DiveNode *pNode, int nSlot, void *pValue);   /* SetIndexedSlot */

static inline fx32 FX_Mul(fx32 a, fx32 b)
{
    return (int)(((long long)a * b + 0x800) >> 12);
}

void Ov263_AiDiveTick(DiveNode *pNode)
{
    VecFx32 vDir;
    VecFx32 vAxis;
    VecFx32 vForward;
    VecFx32 vFacing;
    DiveState *pState;
    int bDone;
    signed char nSide;

    pState = pNode->pState;
    vDir = data_ov263_020d36b0;
    bDone = 0;
    Ov263_rotateVecByOwnerYaw(&vDir, pNode, &vDir);
    pState->vVel = vDir;
    pState->nTimer += pNode->pClock[0x2c / 4];
    switch (pState->nPhase) {
    case 0:
        Ov263_AcquireTarget(pNode);
        pState->nAngle -= pState->nTurn * FX32_PI_HALF;
        pState->nAngleB = pState->nAngle;
        if (pState->nHeight < 0x2000) {
            pState->nPhase = 2;
        }
        if (pState->nTimer >= 0x2800) {
            bDone = 1;
        }
        break;
    case 1:
        vAxis = pState->vAxis;
        ScaleVec3Fx12(0xc00, &vAxis, &pState->vVel);
        if (pState->nTimer >= 0xc00) {
            pState->nTimer = 0;
            pState->nPasses++;
            Ov263_AcquireTarget(pNode);
            if (pState->nHeight < 0x5000 || pState->nPasses >= 3) {
                bDone = 1;
            }
        }
        break;
    case 2:
        Ov263_AcquireTarget(pNode);
        Vec3TransformViaTempMtx(&vForward, pState->pSelf->srt, &data_02042258);
        VEC_DotProduct(&vFacing, &vForward);
        nSide = FX_Mul(vFacing.x, vForward.z) - FX_Mul(vFacing.z, vForward.x) < 0 ? 1 : -1;
        if (pState->nTurn != nSide) {
            pState->nAngle -= pState->nTurn * FX32_PI_HALF;
        }
        if (pState->nHeight > 0x4800) {
            pState->nPhase = 0;
        }
        if (pState->nTimer >= 0x2800) {
            bDone = 1;
        }
        break;
    case 3:
        if (pState->nTimer >= 0x200) {
            bDone = 1;
        }
        break;
    }
    ScaleVec3Fx12(0x10000, &vDir, &vDir);
    if (Ov263_ProbeGround(pNode, &vDir, 0) != 0) {
        pState->nHits++;
        if (pState->nHits == 1) {
            pState->nAngle += pState->nTurn * FX32_PI;
        }
        if (pState->nHits >= 9) {
            pState->nAngleB = pState->nAngle;
        }
        if (pState->nHits >= 11) {
            bDone = 1;
        }
        pState->nTimer = 0;
        pState->nPhase = 3;
    } else {
        pState->nHits = 0;
    }
    if (pState->pSelf->bBlocked) {
        bDone = 1;
    }
    if (bDone) {
        pState->pSelf->nSubState = 2;
        SetIndexedSlot(pNode, pNode->nSlot, 0);
    }
}
