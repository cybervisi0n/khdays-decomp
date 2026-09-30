/* Ov023_CmdSetCamera -- Ov023_CmdSetCamera: script command that sets the current camera
 * (+0x488 of the event block, a 0x104-byte block from +0x30) directly.  Operands 1..3 are the
 * position (+0x94), operand 4 the distance (+0xdc), operands 5 / 6 the yaw / pitch in degrees
 * (x 182, a pitch above 360 mirrored) when given as numbers; operand 0 picks the tracked actor
 * (+0xfc; -1 leaves it, 0x40 none) or, as a string, a placement (0202c3c4) whose position is
 * added to the camera's.  Returns 1. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct Ov023Operand {
    s16  nType;               /* 0x00 */
    u8   pad_02[2];
    int  nValue;              /* 0x04 */
} Ov023Operand;               /* 0x08 */

typedef struct Ov023Camera {
    u8   pad_000[0x94];
    VecFx32 vPos;             /* 0x94 */
    VecFx32 vAngle;           /* 0xa0 */
    u8   pad_0ac[0xdc - 0xac];
    int  nDistance;           /* 0xdc */
    u8   pad_0e0[0xfc - 0xe0];
    int  nTargetActor;        /* 0xfc */
    int  nField100;           /* 0x100 */
} Ov023Camera;                /* 0x104 */

typedef struct Ov023EventBlock {
    u8   pad_000[0x30];
    Ov023Camera aCamera[4];   /* 0x030 */
    u8   pad_440[0x488 - 0x440];
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
extern Ov023Placement *EntityMgr_FindCollEntry(int nSlot, const char *pszName); /* find a placement */
extern void  VEC_Add(const VecFx32 *pA, const VecFx32 *pB, VecFx32 *pOut);

int Ov023_CmdSetCamera(Ov023ScriptCtx *pCtx, Ov023Operand *pOperand)
{
    VecFx32 vPos;
    int nDistance;
    Ov023Camera *pCamera;
    Ov023Operand *pOp;
    int nAngle;
    Ov023Placement *pPlacement;

    nDistance = ScriptVm_ReadOperandFx32(pCtx, pOperand + 4);
    pCamera = &pCtx->pEvent->aCamera[pCtx->pEvent->nCamera];
    vPos.x = ScriptVm_ReadOperandFx32(pCtx, pOperand + 1);
    vPos.y = ScriptVm_ReadOperandFx32(pCtx, pOperand + 2);
    vPos.z = ScriptVm_ReadOperandFx32(pCtx, pOperand + 3);
    pCamera->vPos = vPos;
    pCamera->nDistance = nDistance;
    pOp = ScriptVm_ResolveOperand(pCtx, pOperand + 5);
    if (pOp->nType == 1) {
        pCamera->vAngle.x = ScriptVm_ReadOperandInt(pCtx, pOp) * 0xb6;
    }
    pOp = ScriptVm_ResolveOperand(pCtx, pOperand + 6);
    if (pOp->nType == 1) {
        nAngle = ScriptVm_ReadOperandInt(pCtx, pOp);
        if (nAngle > 360) {
            nAngle = -(nAngle - 360);
        }
        pCamera->vAngle.y = nAngle * 0xb6;
    }
    pOp = ScriptVm_ResolveOperand(pCtx, pOperand);
    switch (pOp->nType) {
    case 1:
        if (pOp->nValue != -1) {
            if (pOp->nValue != 0x40) {
                pCamera->nTargetActor = pOp->nValue;
            } else {
                pCamera->nTargetActor = 0x40;
            }
        }
        break;
    case 2:
        pPlacement = EntityMgr_FindCollEntry(0, ByteCode_ResolveOperand(pCtx, pOp));
        pCamera->nTargetActor = 0x40;
        VEC_Add(&pCamera->vPos, &pPlacement->vPos, &pCamera->vPos);
        break;
    }
    return 1;
}
