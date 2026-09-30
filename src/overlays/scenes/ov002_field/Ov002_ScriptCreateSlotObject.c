
#include "nitro/fx_types.h"

extern int ScriptVm_ReadOperandInt(int pCtx, unsigned short *pOperand);
extern int ScriptVm_ReadOperandFx32(int pCtx, unsigned short *pOperand);
/* The setup creates the object slot, stamps the five values into the object it
 * makes, and starts its animation. The names below are what it does with each
 * argument, not guesses. */
extern void Ov002_CreateSlotObjectAndStart(int nEntry, int nKind, int nMask, int nMode,
                                VecFx32 *pObjectPos, int nObjA, int nObjTag,
                                int nObjB, int nObjC, VecFx32 *pAnimAt,
                                int nAnimMode, int nAnimParam);

/* Script command: create a slot object and start it.
 *
 * Sixteen operands feed the setup at 02073a10. The first four give the entry,
 * the kind, a mask and a mode; the fifth is a gate that goes in last as a
 * boolean. Three fixed-point operands build the position stamped into the new
 * object, four single values follow for its other fields, three more build the
 * place the animation starts from, and one closes with the animation mode.
 * Always returns one.
 */
int Ov002_ScriptCreateSlotObject(int pCtx, unsigned short *pArgs)
{
    VecFx32 vObjectPos;
    VecFx32 vAnimAt;
    int nEntry;
    int nKind;
    int nMask;
    int nMode;
    int nGate;
    int nObjA;
    int nObjTag;
    int nObjB;
    int nObjC;
    int nAnimMode;

    nEntry = ScriptVm_ReadOperandInt(pCtx, pArgs);
    nKind = ScriptVm_ReadOperandInt(pCtx, pArgs + 4);
    nMask = ScriptVm_ReadOperandInt(pCtx, pArgs + 8);
    nMode = ScriptVm_ReadOperandInt(pCtx, pArgs + 0xc);
    nGate = ScriptVm_ReadOperandInt(pCtx, pArgs + 0x10);
    vObjectPos.x = ScriptVm_ReadOperandFx32(pCtx, pArgs + 0x14);
    vObjectPos.y = ScriptVm_ReadOperandFx32(pCtx, pArgs + 0x18);
    vObjectPos.z = ScriptVm_ReadOperandFx32(pCtx, pArgs + 0x1c);
    nObjA = ScriptVm_ReadOperandFx32(pCtx, pArgs + 0x20);
    nObjTag = ScriptVm_ReadOperandInt(pCtx, pArgs + 0x24);
    nObjB = ScriptVm_ReadOperandFx32(pCtx, pArgs + 0x28);
    nObjC = ScriptVm_ReadOperandFx32(pCtx, pArgs + 0x2c);
    vAnimAt.x = ScriptVm_ReadOperandFx32(pCtx, pArgs + 0x30);
    vAnimAt.y = ScriptVm_ReadOperandFx32(pCtx, pArgs + 0x34);
    vAnimAt.z = ScriptVm_ReadOperandFx32(pCtx, pArgs + 0x38);
    nAnimMode = ScriptVm_ReadOperandFx32(pCtx, pArgs + 0x3c);

    Ov002_CreateSlotObjectAndStart(nEntry, nKind, nMask, nMode, &vObjectPos, nObjA,
                        nObjTag, nObjB, nObjC, &vAnimAt, nAnimMode,
                        nGate != 0);
    return 1;
}
