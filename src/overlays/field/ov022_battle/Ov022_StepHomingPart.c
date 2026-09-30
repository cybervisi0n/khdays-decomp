/* Steers a launched part towards its target: when the target is valid and ahead, turns its velocity
 * towards it within its turn rate, moves it and plays its reaction on reach. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/engine.h"

struct Actor;

struct ActorSlot {
    u8 pad000[0x114];
    u16 nOpen;                       /* 0x114 */
};

struct ReactionCtx {
    u8 pad00[0xc];
    int nSlot;                       /* 0x0c */
    u8 pad10[8];
    struct ActorSlot *aSlots[16];   /* 0x18 */
    void *pActor;                    /* 0x58 */
};

struct SlotTail {
    u8 nState;                       /* 0x00 */
    u8 pad01[1];
    short nTag;                      /* 0x02 */
    u8 pad04[8];
    int nRate;                       /* 0x0c */
    short nField10;                  /* 0x10 */
    u8 pad12[2];
    int nReach;                      /* 0x14 */
    int nWindow;                     /* 0x18 */
    u8 pad1c[0xc];
    int nMark;                       /* 0x28 */
};

struct SlotPart {
    int nTimer;                      /* 0x000 */
    VecFx32 vecPos;           /* 0x004 */
    VecFx32 vecVel;           /* 0x010 */
    u16 nSlotFlags;                  /* 0x01c */
    u8 pad01e[0xa2];
    VecFx32 vecAt;            /* 0x0c0 */
    u8 pad0cc[0x7c];
    struct SlotTail *pOwner;         /* 0x148 */
    u8 nState;                       /* 0x14c */
};

#define SLOT_OPEN 0xffff
#define DOT_LIMIT 0xa00
#define FX32_ONE 0x1000
#define FRAME_RATE_20FPS 1   /* GetFrameRateMode(): 0 = 30 fps, 1 = 20 fps, 2 = 60 fps */
#define STATE_LIVE 2
#define REACTION_KIND 2

extern int Ov022_ValidateTargetRef(void *pActor);
extern VecFx32 *func_ov022_020ad0c0(void *pActor);
extern void VEC_Subtract(VecFx32 *a, VecFx32 *b,
                         VecFx32 *pOut);
extern int VEC_DotProduct(VecFx32 *a, VecFx32 *b);
extern int VEC_Normalize(VecFx32 *pSrc, VecFx32 *pDst);
extern void ScaleVec3Fx12(int nFactor, VecFx32 *pSrc,
                          VecFx32 *pDst);
extern void VEC_MultAdd(int nScale, VecFx32 *pVec,
                        VecFx32 *pAdd, VecFx32 *pDst);
extern int func_ov022_0208a9ac(int nSpeed, int nDecay, int nTimer);
extern void VEC_Add(VecFx32 *a, VecFx32 *b, VecFx32 *pOut);
extern void Ov022_MovePartTo(struct ReactionCtx *pCtx,
                                struct SlotPart *pPart,
                                VecFx32 *pAt, VecFx32 *pDir);
extern int VEC_Distance(VecFx32 *a, VecFx32 *b);
extern int Ov022_ClampReactionForKind10(int nKind, int nReaction);
extern void Ov022_EndPartRun(struct ReactionCtx *pCtx,
                                struct SlotPart *pPart, int nReaction);
extern unsigned short Sequence_UpdateTracks(u16 *pFlags, int nDelta);

int Ov022_StepHomingPart(struct ReactionCtx *pCtx, struct SlotPart *pPart,
                        int nDelta)
{
    VecFx32 vecAt;
    VecFx32 vecToTarget;
    VecFx32 vecStep;
    void *pActor;
    struct SlotTail *pOwner;
    int nSpeed;

    pActor = pCtx->pActor;
    pOwner = pPart->pOwner;
    if (pCtx->aSlots[pCtx->nSlot]->nOpen == SLOT_OPEN) {
        pPart->nTimer = pPart->nTimer + nDelta;
        vecAt = pPart->vecAt;
        if (Ov022_ValidateTargetRef(pActor) != 0
            && pPart->nTimer >= pOwner->nMark) {
            VEC_Subtract(func_ov022_020ad0c0(pActor), &vecAt, &vecToTarget);
            if (VEC_DotProduct(&vecToTarget, &pPart->vecVel) >= -DOT_LIMIT) {
                VEC_Normalize(&vecToTarget, &vecToTarget);
                VEC_Normalize(&pPart->vecVel, &pPart->vecVel);
                ScaleVec3Fx12(pOwner->nTag, &vecToTarget, &vecToTarget);
                VEC_MultAdd(FX32_ONE - pOwner->nTag, &pPart->vecVel,
                            &vecToTarget, &pPart->vecVel);
                VEC_Normalize(&pPart->vecVel, &pPart->vecVel);
                if (GetFrameRateMode() == FRAME_RATE_20FPS) {
                    nSpeed = pOwner->nRate * 3 / 2;
                } else {
                    nSpeed = pOwner->nRate;
                }
                ScaleVec3Fx12(nSpeed, &pPart->vecVel, &pPart->vecVel);
            }
        }
        vecStep = pPart->vecVel;
        if (pOwner->nField10 != 0) {
            ScaleVec3Fx12(func_ov022_0208a9ac(pOwner->nRate, pOwner->nField10,
                                              pPart->nTimer),
                          &pPart->vecVel, &vecStep);
        }
        VEC_Add(&vecAt, &vecStep, &vecAt);
        pPart->vecAt = vecAt;
        Ov022_MovePartTo(pCtx, pPart, &vecAt, &pPart->vecVel);
        if (pPart->nState == STATE_LIVE
            && (VEC_Distance(&pPart->vecPos, &vecAt) > pOwner->nReach
                || pPart->nTimer >= pOwner->nWindow)) {
            Ov022_EndPartRun(pCtx, pPart,
                                Ov022_ClampReactionForKind10(pOwner->nState,
                                                    REACTION_KIND));
        }
    }
    Sequence_UpdateTracks(&pPart->nSlotFlags, nDelta);
    return 0;
}
