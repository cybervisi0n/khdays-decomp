
/* The 0x2c byte path-task record this command builds on the stack. */

#include "nitro/fx_types.h"

typedef struct Ov002TaskArgsPath {
    int n00;                            /* +0x00 */
    int n04;                            /* +0x04 */
    int n08;                            /* +0x08 */
    VecFx32 vPlace;                   /* +0x0c */
    int n18;                            /* +0x18 */
    int n1c;                            /* +0x1c */
    int n20;                            /* +0x20 */
    int nResolved;                      /* +0x24 */
    int n28;                            /* +0x28 */
} Ov002TaskArgsPath;

extern int ScriptVm_ReadOperandInt(int pCtx, unsigned short *pOperand);
extern int ScriptVm_ReadOperandFx32(int pCtx, unsigned short *pOperand);
extern int ByteCode_ResolveOperand(int pCtx, unsigned short *pOperand);
extern void Ov002_SubmitTaskNode(int bGate, int nSlot, int nKind,
                                Ov002TaskArgsPath *pArgs);

/* Script command: submit a path task.
 *
 * The first four operands give a gate flag, the slot and two plain values.
 * The fifth operand's tag then chooses how the place is filled: tag zero
 * reads three fixed-point operands into it, anything else resolves that
 * operand into the record's resolved field and leaves the place at zero.
 * Four more plain values, three of them fixed-point, close the record, which
 * goes in with kind two. Always returns one.
 */
int Ov002_ScriptSubmitPathTask(int pCtx, unsigned short *pArgs)
{
    VecFx32 vPlace;
    Ov002TaskArgsPath args;
    int nSlot;
    int nA;
    int nB;
    int nGate;
    int nC;
    int n18;
    int n1c;
    int n20;
    int nResolved;
    int n28;

    nResolved = 0;
    nGate = ScriptVm_ReadOperandInt(pCtx, pArgs);
    nSlot = ScriptVm_ReadOperandInt(pCtx, pArgs + 4);
    nA = ScriptVm_ReadOperandInt(pCtx, pArgs + 8);
    nB = ScriptVm_ReadOperandInt(pCtx, pArgs + 0xc);

    if (*(short *)(pArgs + 0x10) == 0) {
        vPlace.x = ScriptVm_ReadOperandFx32(pCtx, pArgs + 0x14);
        vPlace.y = ScriptVm_ReadOperandFx32(pCtx, pArgs + 0x18);
        vPlace.z = ScriptVm_ReadOperandFx32(pCtx, pArgs + 0x1c);
    } else {
        nResolved = ByteCode_ResolveOperand(pCtx, pArgs + 0x10);
        vPlace.x = 0;
        vPlace.y = 0;
        vPlace.z = 0;
    }

    nC = ScriptVm_ReadOperandInt(pCtx, pArgs + 0x20);
    n18 = ScriptVm_ReadOperandFx32(pCtx, pArgs + 0x24);
    n1c = ScriptVm_ReadOperandFx32(pCtx, pArgs + 0x28);
    n20 = ScriptVm_ReadOperandFx32(pCtx, pArgs + 0x2c);
    n28 = ScriptVm_ReadOperandInt(pCtx, pArgs + 0x30);

    args.n00 = nA;
    args.n04 = nC;
    args.n08 = nB;
    args.vPlace = vPlace;
    args.n18 = n18;
    args.n1c = n1c;
    args.n20 = n20;
    args.nResolved = nResolved;
    args.n28 = n28;

    Ov002_SubmitTaskNode(nGate == 0, nSlot, 2, &args);
    return 1;
}
