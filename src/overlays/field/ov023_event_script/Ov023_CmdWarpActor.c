/* Ov023_CmdWarpActor -- Ov023_CmdWarpActor: script command that moves an actor to a position
 * at once.  Operand 0 is the entity (0202bfcc) and, resolved (02020d10), the actor; an actor
 * with a model resource (+0x15e0) is first detached (02088f90) and reset (02089174).  Operands
 * 3..5 give the position.  Without operand 2 it is applied (0202b450) with the facing of
 * operand 6 in degrees; as "AnchorPos<n>" the position is rotated by the anchor's angle (event
 * block +0x474, FX_SinCosTable_) and offset by its position (+0x444) and the anchor's angle is
 * the facing; any other name is a spot of the actor's group (0202bfa0, 0202b0b8) whose
 * position is added, its angle (0202b150) being the facing.  The facing goes to the model
 * (02088e78) when there is one, else onto the entity (+0x80, flag bit 5 of +4) unless its
 * bit 5 at +0 is set.  The entity is then shown (0202beb8 1).  Returns 1. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct Ov023Operand {
    s16  nType;               /* 0x00 */
    u8   pad_02[6];
} Ov023Operand;               /* 0x08 */

typedef struct Ov023Actor {
    u8   pad_0000[0x15e0];
    void *pResource;          /* 0x15e0 */
    u8   pad_15e4[0x1a64 - 0x15e4];
} Ov023Actor;

typedef struct Ov023EventBlock {
    u8   pad_000[0x440];
    Ov023Actor *pActors;      /* 0x440 */
    VecFx32 aAnchorPos[4];    /* 0x444 */
    int  aAnchorAngle[4];     /* 0x474 */
} Ov023EventBlock;

typedef struct Ov023ScriptCtx {
    u8   pad_000[0x128];
    Ov023EventBlock *pEvent;  /* 0x128 */
} Ov023ScriptCtx;

typedef struct Ov023Entity {
    int  nFlags;              /* 0x00 */
    u16  wFlags;              /* 0x04 */
    u8   pad_06[0x80 - 0x06];
    u16  nAngle;              /* 0x80 */
} Ov023Entity;

extern int   ScriptVm_ReadOperandInt(Ov023ScriptCtx *pCtx, Ov023Operand *pOperand);   /* ScriptVm_ReadOperandInt */
extern int   ScriptVm_ReadOperandFx32(Ov023ScriptCtx *pCtx, Ov023Operand *pOperand);   /* ScriptVm_ReadOperandFx32 */
extern char *ByteCode_ResolveOperand(Ov023ScriptCtx *pCtx, Ov023Operand *pOperand);   /* ScriptVm_ReadOperandString */
extern int   ScriptVm_ResolveActorIndex(Ov023ScriptCtx *pCtx, int nIndex);      /* resolve an actor index */
extern Ov023Entity *ArrayEntryPtrD0(int nEntity);                     /* Entity_Get */
/* The quotient is the low half of the helper's long long return; writing `/` emits _s32_div_f,
 * which is not linkable here. */
extern long long func_02020400(int nNumerator, int nDenominator);
extern void  Actor_SetVecAndSyncChild(Ov023Entity *pEntity, VecFx32 *pPos);   /* Entity_SetPositionNow */
extern void  Entity_SetVisible(int nEntity, int bVisible);              /* Entity_SetVisible */
extern int   strncmp(const char *pA, const char *pB, int nCount);
extern int   func_020200b4(char *pszNumber);                        /* parse a number */
extern int   FX_Mul(int nA, int nB);                         /* FX_Mul */
/* Defined taking nGroup as int: declared narrower here, which is what makes mwcc truncate the
 * argument at the call as the ROM does (declared as defined, the code comes out different). */
extern void *GetTrackEntryBase(u16 nGroup);                             /* the actor group by id */
extern int   Collision_ProbeGround(void *pGroup, char *pszSpot, VecFx32 *pOut); /* Group_GetSpotPosition */
extern int   CollModel_GetEntryField14(void *pGroup, char *pszSpot);            /* Group_GetSpotAngle */
extern void  VEC_Add(const VecFx32 *pA, const VecFx32 *pB, VecFx32 *pOut);
extern void  Ov023_DetachActorModel(Ov023Actor *pActor);               /* Ov023_DetachActorModel */
extern void  Ov023_ResetActorModel(Ov023Actor *pActor);               /* Ov023_ResetActorModel */
extern void  Ov023_SetScrollAndMarkDirty(Ov023Actor *pActor, int nAngle);   /* Ov023_SetActorAngle */
extern const short data_0203d210[];                                 /* FX_SinCosTable_: sin, cos pairs */
extern char  data_ov023_0208a5cc[];                                 /* "AnchorPos" */

/* Give an entity a heading unless it is locked (bit 5 of its flags). */
static inline void Ov023_EntitySetAngle(Ov023Entity *pEntity, int nAngle)
{
    if (!(pEntity->nFlags & 0x20)) {
        pEntity->nAngle = nAngle;
        pEntity->wFlags |= 0x20;
    }
}

int Ov023_CmdWarpActor(Ov023ScriptCtx *pCtx, Ov023Operand *pOperand)
{
    VecFx32 vPos;
    VecFx32 vSpot;
    int nAngle;
    int nActor;
    Ov023Entity *pEntity;
    char *pszName;
    int nAnchor;
    int nX;
    s16 nSin;
    int nCos;
    int nZ;
    int nIdx;

    nActor = ScriptVm_ReadOperandInt(pCtx, pOperand);
    pEntity = ArrayEntryPtrD0((u16)((u16)nActor));
    nActor = ScriptVm_ResolveActorIndex(pCtx, nActor);
    if (pCtx->pEvent->pActors != 0) {
        if (pCtx->pEvent->pActors[nActor].pResource != 0) {
            Ov023_DetachActorModel(&pCtx->pEvent->pActors[nActor]);
            Ov023_ResetActorModel(&pCtx->pEvent->pActors[nActor]);
        }
    }
    vPos.x = ScriptVm_ReadOperandFx32(pCtx, pOperand + 3);
    vPos.y = ScriptVm_ReadOperandFx32(pCtx, pOperand + 4);
    vPos.z = ScriptVm_ReadOperandFx32(pCtx, pOperand + 5);
    if (pOperand[2].nType == 0) {
        u16 nFacing;

        nFacing = (u16)func_02020400(ScriptVm_ReadOperandInt(pCtx, pOperand + 6) << 16, 360);
        Actor_SetVecAndSyncChild(pEntity, &vPos);
        if (pCtx->pEvent->pActors != 0 && pCtx->pEvent->pActors[nActor].pResource != 0) {
            Ov023_SetScrollAndMarkDirty(&pCtx->pEvent->pActors[nActor], nFacing);
        } else {
            Ov023_EntitySetAngle(pEntity, nFacing);
        }
    } else {
        pszName = ByteCode_ResolveOperand(pCtx, pOperand + 2);
        if (strncmp(pszName, data_ov023_0208a5cc, 9) == 0) {
            pszName += 9;
            nAnchor = func_020200b4(pszName);
            nIdx = (pCtx->pEvent->aAnchorAngle[nAnchor] >> 4) * 2;
            nSin = data_0203d210[nIdx];
            nCos = data_0203d210[nIdx + 1];
            nCos = (s16)nCos;
            nX = pCtx->pEvent->aAnchorPos[nAnchor].x + FX_Mul(nCos, vPos.x) + FX_Mul(nSin, vPos.z);
            /* A no-op re-assignment of the short sine between its two uses: it keeps the
             * s16 variable itself (not an int promotion temporary) in the register, which is
             * what orders its spill store right after the muls in the original. */
            nSin = (s16)nSin;
            nZ = pCtx->pEvent->aAnchorPos[nAnchor].z + FX_Mul(-nSin, vPos.x) + FX_Mul(nCos, vPos.z);
            vPos.x = nX;
            vPos.z = nZ;
            vPos.y = vPos.y + pCtx->pEvent->aAnchorPos[nAnchor].y;
            Actor_SetVecAndSyncChild(pEntity, &vPos);
            if (pCtx->pEvent->pActors != 0 && pCtx->pEvent->pActors[nActor].pResource != 0) {
                Ov023_SetScrollAndMarkDirty(&pCtx->pEvent->pActors[nActor], pCtx->pEvent->aAnchorAngle[nAnchor]);
            } else {
                Ov023_EntitySetAngle(pEntity, pCtx->pEvent->aAnchorAngle[nAnchor]);
            }
        } else {
            if (Collision_ProbeGround(GetTrackEntryBase((u16)nActor), pszName, &vSpot) != 0) {
                VEC_Add(&vSpot, &vPos, &vSpot);
                Actor_SetVecAndSyncChild(pEntity, &vSpot);
                nAngle = CollModel_GetEntryField14(GetTrackEntryBase((u16)nActor), pszName);
            } else {
                Actor_SetVecAndSyncChild(pEntity, &vPos);
            }
            if (pCtx->pEvent->pActors != 0 && pCtx->pEvent->pActors[nActor].pResource != 0) {
                Ov023_SetScrollAndMarkDirty(&pCtx->pEvent->pActors[nActor], nAngle);
            } else {
                Ov023_EntitySetAngle(pEntity, nAngle);
            }
        }
    }
    Entity_SetVisible((u16)((u16)nActor), 1);
    return 1;
}
