/* Ov023_CmdMoveCamera -- Ov023_CmdMoveCamera: script command that sets a camera's new target.
 * The camera is the current one (+0x488 of the event block): 0 / 1 pick the live cameras at
 * +0x30, 2 / 3 the queued ones at +0x238.  Operand 1 is the frame count, 2 the distance,
 * 3..5 the position (its y raised by 0xccd), 6..8 the yaw / pitch (a pitch above 360 mirrored)
 * / zoom and 9 the roll, each in degrees when given as numbers; the target mode (+0xf8) is
 * raised to at least 2.  The camera's "from" state is taken from the motion in flight when one
 * is (02021418 into the from copies) or from its current state; operand 0 then picks the
 * tracked actor (+0xfc; -3 the local player, -1 leaves it, 0x40 none) or, as a string, adds
 * an anchor's position ("AnchorPos<n>", +0x444) or a placement's (0202c3c4) to the position.
 * A live camera with no frames or a motion in flight is updated at once (020217d4); a queued
 * camera is finished (02021418 with no outputs), linked from its live twin (+0) and the
 * current camera index dropped by 2.  Returns 1. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct Ov023Operand {
    s16  nType;               /* 0x00 */
    u8   pad_02[2];
    int  nValue;              /* 0x04 */
} Ov023Operand;               /* 0x08 */

typedef struct Ov023Camera {
    struct Ov023Camera *pQueued; /* 0x00 */
    u8   pad_004[0x18 - 0x04];
    VecFx32 vField18;         /* 0x18 */
    VecFx32 vField24;         /* 0x24 */
    u8   pad_030[0x94 - 0x30];
    VecFx32 vPos;             /* 0x94 */
    VecFx32 vAngle;           /* 0xa0 */
    VecFx32 vPosFrom;         /* 0xac */
    VecFx32 vAngleFrom;       /* 0xb8 */
    VecFx32 vFieldc4;         /* 0xc4 */
    VecFx32 vFieldd0;         /* 0xd0 */
    int  nDistance;           /* 0xdc */
    int  nRoll;               /* 0xe0 */
    int  nDistanceFrom;       /* 0xe4 */
    int  nRollFrom;           /* 0xe8 */
    int  nFieldec;            /* 0xec */
    int  nDuration;           /* 0xf0 */
    int  nRemaining;          /* 0xf4 */
    int  nTargetMode;         /* 0xf8 */
    int  nTargetActor;        /* 0xfc */
    int  nField100;           /* 0x100 */
} Ov023Camera;                /* 0x104 */

typedef struct Ov023EventBlock {
    u8   pad_000[0x30];
    Ov023Camera aCamera[2];   /* 0x030 */
    Ov023Camera aCameraQueued[2]; /* 0x238 */
    u8   pad_440[4];
    VecFx32 aAnchorPos[4];    /* 0x444 */
    u8   pad_474[0x488 - 0x474];
    int  nCamera;             /* 0x488 */
} Ov023EventBlock;

typedef struct Ov023ScriptCtx {
    u8   pad_000[0x128];
    Ov023EventBlock *pEvent;  /* 0x128 */
} Ov023ScriptCtx;

typedef struct Ov023Placement {
    u8   pad_00[8];
    VecFx32 vPos;             /* 0x08 */
} Ov023Placement;

extern int   ScriptVm_ReadOperandInt(Ov023ScriptCtx *pCtx, Ov023Operand *pOperand);   /* ScriptVm_ReadOperandInt */
extern int   ScriptVm_ReadOperandFx32(Ov023ScriptCtx *pCtx, Ov023Operand *pOperand);   /* ScriptVm_ReadOperandFx32 */
extern char *ByteCode_ResolveOperand(Ov023ScriptCtx *pCtx, Ov023Operand *pOperand);   /* ScriptVm_ReadOperandString */
extern Ov023Operand *ScriptVm_ResolveOperand(Ov023ScriptCtx *pCtx, Ov023Operand *pOperand); /* ScriptVm_ResolveOperand */
extern void  Obj_StepMotionTransform(Ov023Camera *pCamera, VecFx32 *pPos, VecFx32 *pAngle, int *pDistance, int *pRoll); /* Camera_SampleMotion */
extern void  Game_UpdateObjectMotion(Ov023Camera *pCamera);                   /* Camera_Update */
extern int   Session_GetLocalPlayerIndex(void);                                   /* Session_GetLocalPlayerIndex */
extern int   strncmp(const char *pA, const char *pB, int nCount);
extern void  VEC_Add(const VecFx32 *pA, const VecFx32 *pB, VecFx32 *pOut);
extern Ov023Placement *EntityMgr_FindCollEntry(int nSlot, const char *pszName); /* find a placement */
extern char  data_ov023_0208a5cc[];                                 /* "AnchorPos" */

int Ov023_CmdMoveCamera(Ov023ScriptCtx *pCtx, Ov023Operand *pOperand)
{
    VecFx32 vPos;
    int nFrames;
    int nDistance;
    int bInFlight;
    int nCamera;
    Ov023Camera *pCamera;
    Ov023Operand *pOp;
    int nAngle;
    char *pszName;
    Ov023Placement *pPlacement;

    nFrames = ScriptVm_ReadOperandInt(pCtx, pOperand + 1);
    nDistance = ScriptVm_ReadOperandFx32(pCtx, pOperand + 2);
    nCamera = pCtx->pEvent->nCamera;
    bInFlight = 0;
    if (nCamera < 2) {
        pCamera = &pCtx->pEvent->aCamera[nCamera];
    } else {
        pCamera = &pCtx->pEvent->aCameraQueued[nCamera - 2];
    }
    if (pCamera->nTargetMode < 2) {
        pCamera->nTargetMode = 2;
    }
    vPos.x = ScriptVm_ReadOperandFx32(pCtx, pOperand + 3);
    vPos.y = ScriptVm_ReadOperandFx32(pCtx, pOperand + 4);
    vPos.z = ScriptVm_ReadOperandFx32(pCtx, pOperand + 5);
    pCamera->vFieldd0 = pCamera->vField24;
    pCamera->vFieldc4 = pCamera->vField18;
    if (pCamera->nRemaining > 0 && pCamera->nDuration > 0) {
        Obj_StepMotionTransform(pCamera, &pCamera->vPosFrom, &pCamera->vAngleFrom, &pCamera->nDistanceFrom, &pCamera->nRollFrom);
        bInFlight = 1;
    } else {
        pCamera->vPosFrom = pCamera->vPos;
        pCamera->nDistanceFrom = pCamera->nDistance;
        pCamera->vAngleFrom = pCamera->vAngle;
        pCamera->nRollFrom = pCamera->nRoll;
    }
    pCamera->vPos = vPos;
    pCamera->vPos.y += 0xccd;
    pCamera->nDistance = nDistance;
    pOp = ScriptVm_ResolveOperand(pCtx, pOperand + 6);
    if (pOp->nType == 1) {
        pCamera->vAngle.x = ScriptVm_ReadOperandInt(pCtx, pOp) * 0xb6;
    }
    pOp = ScriptVm_ResolveOperand(pCtx, pOperand + 7);
    if (pOp->nType == 1) {
        nAngle = ScriptVm_ReadOperandInt(pCtx, pOp);
        if (nAngle > 360) {
            nAngle = -(nAngle - 360);
        }
        pCamera->vAngle.y = nAngle * 0xb6;
    }
    pOp = ScriptVm_ResolveOperand(pCtx, pOperand + 8);
    if (pOp->nType == 1) {
        pCamera->vAngle.z = ScriptVm_ReadOperandInt(pCtx, pOp) * 0x79;
    }
    pOp = ScriptVm_ResolveOperand(pCtx, pOperand + 9);
    if (pOp->nType == 1) {
        pCamera->nRoll = ScriptVm_ReadOperandInt(pCtx, pOp) * 0xb6;
    }
    pCamera->nDuration = nFrames;
    pCamera->nRemaining = pCamera->nDuration;
    pOp = ScriptVm_ResolveOperand(pCtx, pOperand);
    switch (pOp->nType) {
    case 1:
        switch (pOp->nValue) {
        case -3:
            pOp->nValue = Session_GetLocalPlayerIndex();
            break;
        case -1:
            goto done;
        }
        if (pOp->nValue != 0x40) {
            pCamera->nTargetActor = pOp->nValue;
        } else {
            pCamera->nTargetActor = 0x40;
        }
        break;
    case 2:
        pszName = ByteCode_ResolveOperand(pCtx, pOp);
        if (strncmp(pszName, data_ov023_0208a5cc, 9) == 0) {
            VEC_Add(&pCamera->vPos, &pCtx->pEvent->aAnchorPos[pszName[9]], &pCamera->vPos);
            pCamera->nTargetActor = 0x40;
        } else {
            pPlacement = EntityMgr_FindCollEntry(0, pszName);
            pCamera->nTargetActor = 0x40;
            VEC_Add(&pCamera->vPos, &pPlacement->vPos, &pCamera->vPos);
        }
        break;
    }
done:
    pCamera->pQueued = 0;
    if (nCamera < 2) {
        if (nFrames == 0 || bInFlight) {
            Game_UpdateObjectMotion(pCamera);
        }
    } else {
        pCamera->nRemaining = 0;
        Obj_StepMotionTransform(pCamera, 0, 0, 0, 0);
        pCtx->pEvent->aCamera[nCamera - 2].pQueued = pCamera;
        pCtx->pEvent->nCamera -= 2;
    }
    return 1;
}
