/* ov022: run the frame's other three command handlers, then the fallback.
 *
 * The same shape as the first pipeline: three handlers get the same arguments
 * and any yes is the answer, and only when all three pass does the fallback get
 * a turn. Where this one differs is what it hands the last handler: the command
 * carries two points and the handler wants the vector between them, so it is
 * worked out on the stack first.
 *
 * The hit slot is cleared up front so a handler that does not fill it cannot
 * leave the previous frame's answer behind.
 */

#include "nitro/types.h"
#include "nitro/fx_types.h"

struct Command {
    VecFx32 vecFrom;         /* 0x00 */
    VecFx32 vecTo;           /* 0x0c */
};

struct Actor {
    u8 pad0000[0x26bc];
    int nHit;                    /* 0x26bc */
    int nHitKind;                /* 0x26c0 */
    u8 bHitHeld;                 /* 0x26c4 */
};

extern int Ov022_SweepCapsuleOverGroup(struct Actor *pActor, struct Command *pCmd,
                               void *pCtx);
extern int Ov022_SweepCapsuleOverParts(struct Actor *pActor, struct Command *pCmd,
                               void *pCtx);
extern int Ov022_SweepCapsuleOverEntries(struct Actor *pActor, struct Command *pCmd,
                               void *pCtx);
extern int func_ov022_020a1c80(struct Actor *pActor, struct Command *pCmd,
                               int *pHit);
extern int func_ov022_0209e4a4(struct Actor *pActor, void *pCtx,
                               VecFx32 *pDelta);
extern void VEC_Subtract(const VecFx32 *pA, const VecFx32 *pB,
                         VecFx32 *pOut);

int Ov022_RunReachHandlers(struct Actor *pActor, struct Command *pCmd, void *pCtx)
{
    VecFx32 vDelta;
    int bTaken;

    bTaken = 0;
    pActor->nHit = 0;
    pActor->bHitHeld = 0;
    if (Ov022_SweepCapsuleOverGroup(pActor, pCmd, pCtx) != 0) {
        bTaken = 1;
    }
    if (Ov022_SweepCapsuleOverParts(pActor, pCmd, pCtx) != 0) {
        bTaken = 1;
    }
    if (Ov022_SweepCapsuleOverEntries(pActor, pCmd, pCtx) != 0) {
        bTaken = 1;
    }
    if (bTaken == 0) {
        if (func_ov022_020a1c80(pActor, pCmd, &pActor->nHit) != 0) {
            VEC_Subtract(&pCmd->vecTo, &pCmd->vecFrom, &vDelta);
            bTaken = func_ov022_0209e4a4(pActor, pCtx, &vDelta);
        }
    }
    return bTaken;
}
