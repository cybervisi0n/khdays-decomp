/* ov022: step one part of a reaction, aiming it before it is moved.
 *
 * Where the part is aimed depends on whether it has run at all yet. On its very
 * first step it is aimed at a point in front of the actor: the actor's facing is
 * turned into a table index, the sine and cosine there become a horizontal unit
 * step, and that step is walked one unit out from the actor's live position
 * raised by a fixed amount. On every step after that the part is aimed along its
 * own velocity, normalised, from where it already is.
 *
 * Either way the mover is handed the point and the aim and does the rest. One
 * owner state opts out of being aimed and moved at all, and only advances the
 * clock.
 *
 * The clock runs afterwards. Once it passes the owner's part delay, a part still
 * in the delayed state is restarted at zero in the running one. The animation is
 * stepped last, whatever happened.
 */

#include "nitro/types.h"
#include "nitro/fx_types.h"

#define KIND_UNAIMED 0x19
#define ANGLE_BIAS 0x8000
#define ANGLE_SHIFT 4
#define AIM_RAISE 0xc00
#define AIM_REACH 0x800
#define PART_DELAYED 1
#define PART_RUNNING 2

/* Ov022AnimBlock */
struct AnimBlock {
    u8 pad00[0xa4];
    VecFx32 vecAt;        /* 0x00a4 */
    int aEntryFlags[3];          /* 0x00b0 */
};

/* Ov022SlotTail -- the owning slot's tail block, at slot+0x118 */
struct SlotTail {
    u8 nState;                   /* 0x00 */
    u8 pad01[0x23];
    int nPartDelay;              /* 0x24 */
};

/* Ov022SlotPart */
struct SlotPart {
    int nTimer;                  /* 0x0000 */
    VecFx32 vecPos;       /* 0x0004 */
    VecFx32 vecVel;       /* 0x0010 */
    struct AnimBlock anim;       /* 0x001c */
    u8 pad0d8[0x70];
    struct SlotTail *pOwner;     /* 0x0148 */
    u8 nState;                   /* 0x014c */
    u8 pad14d[3];
};

/* Ov022ActorNode */
struct ActorNode {
    u8 pad00[0x80];
    u16 nAngle;                  /* 0x80 */
    u8 pad82[2];
};

/* Ov022Actor */
struct Actor {
    u8 pad000[0x20];
    struct ActorNode *pNode;     /* 0x0020 */
    u8 pad024[0x468];
    VecFx32 vecPos;       /* 0x048c */
};

/* Ov022ReactionCtx */
struct ReactionCtx {
    u8 pad00[0x58];
    struct Actor *pActor;        /* 0x58 */
};

/* kFxSinCosTable, 512 entries of a sine and a cosine; the ROM walks it as one
 * flat array of halfwords rather than as pairs, so the index is doubled here
 * the same way. */
extern s16 data_0203d210[];

extern int Ov022_GetSlotMoveMode(struct ReactionCtx *pCtx, int nKind);
extern void Ov022_MovePartTo(struct ReactionCtx *pCtx, struct SlotPart *pPart,
                                VecFx32 *pAt, VecFx32 *pDir);
extern int VEC_Normalize(VecFx32 *pSrc, VecFx32 *pDst);
extern void Sequence_UpdateTracks(struct AnimBlock *pAnim, int nDelta);
extern void VEC_MultAdd(int nFactor, VecFx32 *pStep,
                        VecFx32 *pFrom, VecFx32 *pOut);
extern void VEC_Subtract(VecFx32 *pA, VecFx32 *pB,
                         VecFx32 *pOut);

int Ov022_StepAimedPart(struct ReactionCtx *pCtx, struct SlotPart *pPart,
                        int nDelta)
{
    VecFx32 vecAt;
    VecFx32 vecAim;
    struct SlotTail *pOwner;
    struct Actor *pActor;
    int nIndex;
    int bLapsed;

    pOwner = pPart->pOwner;
    bLapsed = 0;
    if (Ov022_GetSlotMoveMode(pCtx, pOwner->nState) != KIND_UNAIMED) {
        if (pPart->nTimer == 0) {
            pActor = pCtx->pActor;
            nIndex = (u16)(pActor->pNode->nAngle - ANGLE_BIAS) >> ANGLE_SHIFT;
            vecAt = pActor->vecPos;
            vecAt.y = vecAt.y + AIM_RAISE;
            vecAim.x = data_0203d210[nIndex * 2];
            vecAim.y = 0;
            vecAim.z = data_0203d210[nIndex * 2 + 1];
            VEC_MultAdd(AIM_REACH, &vecAim, &vecAt, &vecAt);
            VEC_Subtract(&pPart->anim.vecAt, &vecAt, &vecAim);
        } else {
            vecAt = pPart->anim.vecAt;
            VEC_Normalize(&pPart->vecVel, &vecAim);
        }
        Ov022_MovePartTo(pCtx, pPart, &vecAt, &vecAim);
    }
    pPart->nTimer = pPart->nTimer + nDelta;
    if (pPart->nTimer >= pOwner->nPartDelay) {
        bLapsed = 1;
    }
    if (bLapsed != 0) {
        if (pPart->nState == PART_DELAYED) {
            pPart->nTimer = 0;
            pPart->nState = PART_RUNNING;
        }
    }
    Sequence_UpdateTracks(&pPart->anim, nDelta);
    return 0;
}
