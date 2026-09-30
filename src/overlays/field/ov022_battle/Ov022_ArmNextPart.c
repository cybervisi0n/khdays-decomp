/* Ov022_ArmNextPart -- arm the next part of a slot and say whether one was free.
 *
 * Kind 8 takes the part at nIndex; every other kind scans for the first part
 * in state 0 and gives up when there is none. The part goes to state 2 with
 * its timer cleared, takes the slot's back angle and raises its 0x20 flag.
 *
 * Kind 6 chains along the facing: the back point is pulled half a unit back
 * along the facing, a cast of nAdvance plus that half unit is settled on the
 * ground, and the settled point becomes the part's position and the new back
 * point (keeping the old height). Kind 7 scatters: one of sixteen flat offsets
 * chosen by nIndex is scaled by nScatter, turned by the back angle, added to
 * the back point and settled on the ground with no direction. Kind 8 keeps
 * whatever position the part already had.
 *
 * The part's target is then its position, and the notifier is told about it
 * with the reaction the tail's state maps to.
 */

#include "nitro/types.h"
#include "nitro/fx_types.h"

struct MtxFx33 {
    int m[3][3];
};

/* Ov022SlotTail: the tail block of a slot, the part's owner */
struct SlotTail {
    u8 nState;                       /* 0x00 */
};

/* Ov022SlotPart */
struct SlotPart {
    int nTimer;                      /* 0x000 */
    VecFx32 vecPos;           /* 0x004 */
    VecFx32 vecVel;           /* 0x010 */
    u16 nSlotFlags;                  /* 0x01c */
    u8 pad01e[0x7a];
    u16 nAngle;                      /* 0x098 */
    u8 pad09a[0x26];
    VecFx32 vecAt;            /* 0x0c0 */
    u8 pad0cc[0x7c];
    struct SlotTail *pOwner;         /* 0x148 */
    u8 nState;                       /* 0x14c */
    u8 pad14d[3];
};

/* Ov022ActorSlot */
struct ActorSlot {
    u8 nKind;                        /* 0x000 */
    u8 nParts;                       /* 0x001 */
    u8 pad002[0x10e];
    s8 nSlotIndex;                   /* 0x110 */
    u8 pad111[0xb];
    int nHalfBound;                  /* 0x11c */
    u8 pad120[0x48];
    struct SlotPart *pParts;         /* 0x168 */
    u8 pad16c[0x14];
    int nAdvance;                    /* 0x180 */
    int nField184;                   /* 0x184 */
    int nScatter;                    /* 0x188 */
    u16 nBackAngle;                  /* 0x18c */
    u8 pad18e[2];
    VecFx32 vecBack;          /* 0x190 */
};

struct ReactionCtx;

#define KIND_CHAIN 6
#define KIND_SCATTER 7
#define KIND_INDEXED 8
#define STATE_IDLE 0
#define STATE_LIVE 2
#define PART_FLAG_ARMED 0x20
#define FX32_HALF 0x800
#define REACTION_KIND 1
#define FX32_CONST(x) ((int)((x) > 0 ? (x) * 4096 + 0.5 : (x) * 4096 - 0.5))

extern const s16 data_0203d210[];            /* sin, cos pairs by angle >> 4 */
extern const VecFx32 data_02041dc8;   /* kVecZero */

extern void VEC_MultAdd(int nScale, VecFx32 *pA, VecFx32 *pB,
                        VecFx32 *pOut);
extern void ScaleVec3Fx12(int nFactor, VecFx32 *pSrc,
                          VecFx32 *pDst);
extern void VEC_Add(VecFx32 *a, VecFx32 *b, VecFx32 *pOut);
extern void MTX_RotY33_(struct MtxFx33 *pMtx, int nSin, int nCos);
extern void MTX_MultVec33(VecFx32 *pIn, struct MtxFx33 *pMtx,
                          VecFx32 *pOut);
extern void Ov022_SettlePointOnGround(VecFx32 *pOut, struct ReactionCtx *pCtx,
                                VecFx32 *pOrigin, VecFx32 *pDir,
                                int nRadius, unsigned int nGroup);
extern int Ov022_ClampReactionForKind10(int nState, int nReaction);
extern void func_ov022_0208acdc(struct ReactionCtx *pCtx, VecFx32 *pAim,
                                int nReaction);

int Ov022_ArmNextPart(struct ReactionCtx *pCtx, struct ActorSlot *pSlot,
                        int nIndex)
{
    VecFx32 vecBase;
    VecFx32 vecDir;
    struct MtxFx33 mtx;
    struct SlotPart *pPart;
    int nPart;
    int nTrig;
    int nKeepY;

    pPart = 0;
    if (pSlot->nKind != KIND_INDEXED) {
        for (nPart = 0; nPart < pSlot->nParts; nPart++) {
            if (pSlot->pParts[nPart].nState == STATE_IDLE) {
                pPart = &pSlot->pParts[nPart];
                break;
            }
        }
        if (pPart == 0) {
            return 0;
        }
    } else {
        pPart = &pSlot->pParts[nIndex];
    }
    pPart->nState = STATE_LIVE;
    pPart->nTimer = 0;
    pPart->nAngle = pSlot->nBackAngle;
    pPart->nSlotFlags |= PART_FLAG_ARMED;
    switch (pSlot->nKind) {
    case KIND_CHAIN: {
        VecFx32 vecSettled;
        vecBase = pSlot->vecBack;
        nKeepY = vecBase.y;
        nTrig = pSlot->nBackAngle >> 4;
        vecDir.x = data_0203d210[nTrig * 2];
        vecDir.z = data_0203d210[nTrig * 2 + 1];
        vecDir.y = 0;
        VEC_MultAdd(-FX32_HALF, &vecDir, &vecBase, &vecBase);
        ScaleVec3Fx12(pSlot->nAdvance + FX32_HALF, &vecDir, &vecDir);
        Ov022_SettlePointOnGround(&vecSettled, pCtx, &vecBase, &vecDir,
                            pSlot->nHalfBound, pSlot->nSlotIndex);
        pPart->vecPos = vecSettled;
        pSlot->vecBack = pPart->vecPos;
        pSlot->vecBack.y = nKeepY;
        break;
    }
    case KIND_SCATTER: {
        VecFx32 aOffset[16] = {
            { FX32_CONST(0.0), 0, FX32_CONST(0.0) },
            { FX32_CONST(-0.5), 0, FX32_CONST(-0.8) },
            { FX32_CONST(1.1), 0, FX32_CONST(-0.7) },
            { FX32_CONST(-0.2), 0, FX32_CONST(0.5) },
            { FX32_CONST(0.0), 0, FX32_CONST(-1.1) },
            { FX32_CONST(0.4), 0, FX32_CONST(1.2) },
            { FX32_CONST(-1.2), 0, FX32_CONST(0.2) },
            { FX32_CONST(0.5), 0, FX32_CONST(-0.4) },
            { FX32_CONST(0.7), 0, FX32_CONST(0.5) },
            { FX32_CONST(-0.6), 0, FX32_CONST(-1.5) },
            { FX32_CONST(-0.3), 0, FX32_CONST(1.2) },
            { FX32_CONST(1.2), 0, FX32_CONST(0.6) },
            { FX32_CONST(-0.8), 0, FX32_CONST(-0.2) },
            { FX32_CONST(0.8), 0, FX32_CONST(-1.4) },
            { FX32_CONST(-0.8), 0, FX32_CONST(1.2) },
            { FX32_CONST(-1.0), 0, FX32_CONST(-0.9) },
        };
        VecFx32 vecSettled;
        ScaleVec3Fx12(pSlot->nScatter, &aOffset[nIndex & 0xf], &vecBase);
        nTrig = pSlot->nBackAngle >> 4;
        MTX_RotY33_(&mtx, data_0203d210[nTrig * 2], data_0203d210[nTrig * 2 + 1]);
        MTX_MultVec33(&vecBase, &mtx, &vecBase);
        VEC_Add(&pSlot->vecBack, &vecBase, &vecBase);
        Ov022_SettlePointOnGround(&vecSettled, pCtx, &vecBase,
                            (VecFx32 *)&data_02041dc8,
                            pSlot->nHalfBound, pSlot->nSlotIndex);
        pPart->vecPos = vecSettled;
        break;
    }
    case KIND_INDEXED:
        break;
    }
    pPart->vecAt = pPart->vecPos;
    func_ov022_0208acdc(pCtx, &pPart->vecPos,
                        Ov022_ClampReactionForKind10(pPart->pOwner->nState, REACTION_KIND));
    return 1;
}
