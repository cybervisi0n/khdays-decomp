/* Ov022_StepHomingPart -- the step a homing slot part runs while it falls.
 *
 * Like the plain flight step, nothing happens unless the slot is open, and the
 * part moves by its own velocity each frame with gravity pulling the vertical
 * down -- by a different amount in each of the two modes.
 *
 * What it adds is steering. While the owner still has turns, the actor's mark
 * is valid and the part is already on its way down, the horizontal heading is
 * blended toward the mark: both the heading and the direction to the mark are
 * flattened and normalised, the mark's share is the owner's turn count out of
 * one, and the result is rescaled by the owner's rate -- half as much again in
 * one of the two modes. Only the horizontal is replaced; the fall is untouched.
 *
 * A part still in flight ends its run once the owner's window has closed.
 */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/engine.h"

#define SLOT_OPEN 0xffff
#define ONE 0x1000

/* Ov022AnimBlock */
struct AnimBlock {
    u16 nFlags;                  /* 0x00 */
    u8 pad02[0xa2];
    VecFx32 vecAt;        /* 0xa4 */
    int aEntryFlags[3];          /* 0xb0 */
};

/* Ov022SlotTail */
struct SlotTail {
    u8 nState;                   /* 0x00 */
    u8 pad01;
    short nTag;                  /* 0x02 */
    u8 pad04[8];
    int nRate;                   /* 0x0c */
    u8 pad10[8];
    int nWindow;                 /* 0x18 */
};

/* Ov022SlotPart */
struct SlotPart {
    int nTimer;                  /* 0x000 */
    VecFx32 vecPos;       /* 0x004 */
    VecFx32 vecVel;       /* 0x010 */
    struct AnimBlock anim;       /* 0x01c */
    u8 pad0d8[0x70];
    struct SlotTail *pOwner;     /* 0x148 */
    u8 nState;                   /* 0x14c */
    u8 pad14d[3];
};

/* Ov022ActorSlot */
struct ActorSlot {
    u8 pad000[0x114];
    u16 nOpen;                   /* 0x114 */
};

struct Actor;

/* Ov022ReactionCtx */
struct ReactionCtx {
    u8 pad00[0xc];
    int nSlot;                   /* 0x0c */
    u8 pad10[8];
    struct ActorSlot *aSlots[11];/* 0x18 */
    u8 pad44[0x14];
    struct Actor *pActor;        /* 0x58 */
};

extern int Ov022_ValidateTargetRef(struct Actor *pActor);
extern VecFx32 *func_ov022_020ad0c0(struct Actor *pActor);
extern void VEC_Subtract(VecFx32 *pA, VecFx32 *pB,
                         VecFx32 *pOut);
extern void VEC_Add(VecFx32 *pA, VecFx32 *pB,
                    VecFx32 *pOut);
extern void VEC_MultAdd(int nScale, VecFx32 *pA, VecFx32 *pB,
                        VecFx32 *pOut);
extern int VEC_Normalize(VecFx32 *pOut, VecFx32 *pIn);
extern void ScaleVec3Fx12(int nScale, VecFx32 *pIn, VecFx32 *pOut);
extern void Ov022_MovePartTo(struct ReactionCtx *pCtx, struct SlotPart *pPart,
                                VecFx32 *pAt, VecFx32 *pDir);
extern int Ov022_ClampReactionForKind10(int nKind, int nMode);
extern void Ov022_EndPartRun(struct ReactionCtx *pCtx, struct SlotPart *pPart,
                                int nReaction);
extern unsigned short Sequence_UpdateTracks(u16 *pFlags, int nDelta);

int Ov022_StepHomingPart_2(struct ReactionCtx *pCtx, struct SlotPart *pPart,
                        int nDelta)
{
    VecFx32 vecAt;
    VecFx32 vecFlat;
    VecFx32 vecDir;
    VecFx32 vecStep;
    VecFx32 vecHeading;
    VecFx32 vecMark;
    struct Actor *pActor;
    struct SlotTail *pOwner;
    struct ActorSlot *pSlot;
    int nFall;
    int nScale;
    int nFallStep;

    pSlot = pCtx->aSlots[pCtx->nSlot];
    pActor = pCtx->pActor;
    pOwner = pPart->pOwner;
    if (pSlot->nOpen == SLOT_OPEN) {
        pPart->nTimer = pPart->nTimer + nDelta;
        vecAt = pPart->anim.vecAt;
        nFall = pPart->vecVel.y;
        if (pOwner->nTag != 0 && Ov022_ValidateTargetRef(pActor) != 0
            && pPart->vecVel.y < 0) {
            vecFlat = vecAt;
            vecMark = *func_ov022_020ad0c0(pActor);
            vecHeading = pPart->vecVel;
            vecHeading.y = 0;
            vecFlat.y = 0;
            vecMark.y = 0;
            VEC_Subtract(&vecMark, &vecFlat, &vecDir);
            VEC_Normalize(&vecDir, &vecDir);
            VEC_Normalize(&vecHeading, &vecHeading);
            ScaleVec3Fx12(pOwner->nTag, &vecDir, &vecDir);
            VEC_MultAdd(ONE - pOwner->nTag, &vecHeading, &vecDir, &vecHeading);
            VEC_Normalize(&vecHeading, &vecHeading);
            if (GetFrameRateMode() == 1) {
                nScale = pOwner->nRate * 3 / 2;
            } else {
                nScale = pOwner->nRate;
            }
            ScaleVec3Fx12(nScale, &vecHeading, &vecHeading);
            pPart->vecVel.x = vecHeading.x;
            pPart->vecVel.z = vecHeading.z;
        }
        vecStep = pPart->vecVel;
        vecStep.y = nFall;
        pPart->vecVel.y = nFall;
        VEC_Add(&vecAt, &vecStep, &vecAt);
        pPart->anim.vecAt = vecAt;
        if (GetFrameRateMode() == 1) {
            nFallStep = 0x48;
        } else {
            nFallStep = 0x30;
        }
        pPart->vecVel.y = pPart->vecVel.y - nFallStep;
        Ov022_MovePartTo(pCtx, pPart, &vecAt, &pPart->vecVel);
        if (pPart->nState == 2 && pPart->nTimer >= pOwner->nWindow) {
            Ov022_EndPartRun(pCtx, pPart,
                                Ov022_ClampReactionForKind10(pOwner->nState, 2));
        }
    }
    Sequence_UpdateTracks(&pPart->anim.nFlags, nDelta);
    return 0;
}
