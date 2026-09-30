/* Ov022_StepWaitingPart -- the step a slot part runs while it waits its turn.
 *
 * The part's timer runs until it reaches the limit its owner sets. Once there,
 * the part whose tag matches the owner's is the one that acts: it is aimed at
 * the actor's position, raised a little, and pushed back along the way the
 * actor is facing. Then, if it is still waiting, it moves on to the running
 * state with its timer reset.
 *
 * The tag it compares is the one in the NEXT part, not its own -- the read is
 * a whole part past the end of this one.
 *
 * The sequence tracks are updated whether or not any of that happened, and the
 * step always reports that it has not finished.
 */

#include "nitro/types.h"
#include "nitro/fx_types.h"

#define RAISE 0xc00

/* Ov022ActorNode */
struct ActorNode {
    u8 pad000[0x80];
    u16 nAngle;                  /* 0x0080 */
};

/* Ov022Actor */
struct Actor {
    u8 pad000[0x20];
    struct ActorNode *pNode;     /* 0x0020 */
    u8 pad024[0x468];
    VecFx32 vecPos;       /* 0x048c */
};

/* Ov022AnimBlock */
struct AnimBlock {
    u16 nFlags;                  /* 0x00 */
    u8 pad02[6];
    signed char nTag;            /* 0x08 */
    u8 pad09[0xa7];
    int aEntryFlags[3];          /* 0xb0 */
};

/* the slot's own part, from 0x118 on */
struct SlotTail {
    u8 pad00[2];
    short nTag;                  /* 0x02 */
    u8 pad04[0x20];
    int nLimit;                  /* 0x24 */
};

/* Ov022SlotPart */
struct SlotPart {
    int nTimer;                  /* 0x000 */
    u8 pad004[0x18];
    struct AnimBlock anim;       /* 0x01c */
    u8 pad0d8[0x70];
    struct SlotTail *pOwner;     /* 0x148 */
    u8 nState;                   /* 0x14c */
    u8 pad14d[3];
};

/* Ov022ReactionCtx */
struct ReactionCtx {
    u8 pad00[0x58];
    struct Actor *pActor;        /* 0x58 */
};

extern const short data_0203d210[];

extern void Ov022_MovePartTo(struct ReactionCtx *pCtx, struct SlotPart *pPart,
                                VecFx32 *pAt, VecFx32 *pDir);
extern unsigned short Sequence_UpdateTracks(u16 *pFlags, int nDelta);

int Ov022_StepWaitingPart(struct ReactionCtx *pCtx, struct SlotPart *pPart,
                        int nDelta)
{
    VecFx32 vecAt;
    VecFx32 vecDir;
    struct Actor *pActor;
    struct SlotTail *pOwner;
    int nTimer;
    int k;

    pOwner = pPart->pOwner;
    nTimer = pPart->nTimer + nDelta;
    pPart->nTimer = nTimer;
    if (nTimer >= pOwner->nLimit) {
        if (pOwner->nTag == pPart[1].anim.nTag) {
            pActor = pCtx->pActor;
            k = (u16)(pActor->pNode->nAngle - 0x8000) >> 4;
            vecAt = pActor->vecPos;
            vecAt.y = vecAt.y + RAISE;
            vecDir.x = -data_0203d210[k * 2];
            vecDir.y = 0;
            vecDir.z = -data_0203d210[k * 2 + 1];
            Ov022_MovePartTo(pCtx, pPart, &vecAt, &vecDir);
        }
        if (pPart->nState == 1) {
            pPart->nTimer = 0;
            pPart->nState = 2;
        }
    }
    Sequence_UpdateTracks(&pPart->anim.nFlags, nDelta);
    return 0;
}
