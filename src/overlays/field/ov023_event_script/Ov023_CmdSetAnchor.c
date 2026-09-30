/* Ov023_CmdSetAnchor -- Ov023_CmdSetAnchor: script command that defines one of the event's
 * four anchors (event block +0x444 positions, +0x474 angles).  Operand 0 is the anchor, 6 its
 * angle in degrees (x 182, kept as a u16); the position is operands 3..5, or, when operand 2
 * names a spot, that spot on the model (0202c3e4 with the model id 0202bf84) of the actor in
 * operand 1.  Returns 1. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct Ov023Operand {
    s16  nType;               /* 0x00 */
    u8   pad_02[6];
} Ov023Operand;               /* 0x08 */

typedef struct Ov023EventBlock {
    u8   pad_000[0x444];
    VecFx32 aAnchorPos[4];    /* 0x444 */
    int  aAnchorAngle[4];     /* 0x474 */
} Ov023EventBlock;

typedef struct Ov023ScriptCtx {
    u8   pad_000[0x128];
    Ov023EventBlock *pEvent;  /* 0x128 */
} Ov023ScriptCtx;

extern int   ScriptVm_ReadOperandInt(Ov023ScriptCtx *pCtx, Ov023Operand *pOperand);   /* ScriptVm_ReadOperandInt */
extern int   ScriptVm_ReadOperandFx32(Ov023ScriptCtx *pCtx, Ov023Operand *pOperand);   /* ScriptVm_ReadOperandFx32 */
extern char *ByteCode_ResolveOperand(Ov023ScriptCtx *pCtx, Ov023Operand *pOperand);   /* ScriptVm_ReadOperandString */
extern int   LoadArrayU8At0ce(int nEntity);                            /* Entity_GetModelId */
extern void  EntityMgr_ProbeGround(int nModel, char *pszSpot, VecFx32 *pOut); /* Model_GetSpotPosition */

int Ov023_CmdSetAnchor(Ov023ScriptCtx *pCtx, Ov023Operand *pOperand)
{
    VecFx32 vPos;
    int nAnchor;
    int nAngle;
    int nActor;
    char *pszSpot;

    nAnchor = ScriptVm_ReadOperandInt(pCtx, pOperand);
    nAngle = ScriptVm_ReadOperandInt(pCtx, pOperand + 6);
    if (pOperand[2].nType == 0) {
        VecFx32 vRead;

        vRead.x = ScriptVm_ReadOperandFx32(pCtx, pOperand + 3);
        vRead.y = ScriptVm_ReadOperandFx32(pCtx, pOperand + 4);
        vRead.z = ScriptVm_ReadOperandFx32(pCtx, pOperand + 5);
        vPos = vRead;
    } else {
        nActor = ScriptVm_ReadOperandInt(pCtx, pOperand + 1);
        pszSpot = ByteCode_ResolveOperand(pCtx, pOperand + 2);
        EntityMgr_ProbeGround((u16)(LoadArrayU8At0ce((u16)nActor)), pszSpot, &vPos);
    }
    pCtx->pEvent->aAnchorPos[nAnchor] = vPos;
    pCtx->pEvent->aAnchorAngle[nAnchor] = (u16)(nAngle * 0xb6);
    return 1;
}
