/* Ov022_StepShot -- one frame of a shot in flight.
 *
 * The shot's clock takes the frame, its own step gives it a delta, and the delta
 * moves it. The hit pass then runs against where it has just arrived.
 *
 * A shot dies when it has flown further than its kind allows, and, unless its
 * kind holds it, when its clock passes the kind's life; a held kind runs its own
 * marshalling first and only expires once it has been released. Anything but a
 * live shot is finished off here: the state goes to spent, the clock is set to
 * the retire mark, the eight already-struck ids are cleared and the rig slots go
 * back.
 */

/* Ov022ShotDesc */

#include "nitro/types.h"
#include "nitro/fx_types.h"

struct ShotDesc {
    unsigned int nFlags;         /* 0x00 */
    u8 pad04[0x10];
    int nMaxDist;                /* 0x14 */
    int nMaxAge;                 /* 0x18 */
    u8 pad1c[0x20];
    int nField3c;                /* 0x3c */
};

/* Ov022Shot */
struct Shot {
    u8 nFlags;                   /* 0x000 */
    u8 pad001;
    s8 nState;                   /* 0x002 */
    u8 pad003;
    int nAge;                    /* 0x004 */
    u8 pad008[8];
    VecFx32 vecStart;     /* 0x010 */
    u8 pad01c[0xc];
    u16 nSlotFlags;              /* 0x028 */
    u8 pad02a[0xa2];
    VecFx32 vecPos;       /* 0x0cc */
    u8 pad0d8[0x60];
    struct ShotDesc *pDesc;      /* 0x138 */
    short aHitIds[8];            /* 0x13c */
};

struct ReactionCtx;

extern void Ov022_ComputeShotStep(VecFx32 *pOut, struct ReactionCtx *pCtx,
                                struct Shot *pShot, int nDelta);
extern void VEC_Add(VecFx32 *pA, VecFx32 *pB,
                    VecFx32 *pOut);
extern void func_ov022_02091540(u16 *pFlags, int nDelta);
extern void Ov022_ResolveShotHit(struct ReactionCtx *pCtx, struct Shot *pShot,
                                VecFx32 *pAt, VecFx32 *pDelta);
extern int VEC_Distance(VecFx32 *pA, VecFx32 *pB);
extern void Ov022_MarshalStateByte9(struct ReactionCtx *pCtx, struct Shot *pShot);
extern void Ov022_ReleaseRigSlots(struct Shot *pShot, int nRig);

#define SHOT_HELD 0x10
#define SHOT_RELEASED 1
#define SHOT_LIVE 2
#define SHOT_LANDED 3
#define SHOT_SPENT 4
#define RETIRE_MARK 0x3000
#define HIT_SLOTS 8

int Ov022_StepShot_2(struct ReactionCtx *pCtx, struct Shot *pShot, int nDelta)
{
    VecFx32 vecAt;
    VecFx32 vecDelta;
    struct ShotDesc *pDesc;
    int nSlot;

    pDesc = pShot->pDesc;
    pShot->nAge = pShot->nAge + nDelta;
    vecAt = pShot->vecPos;
    Ov022_ComputeShotStep(&vecDelta, pCtx, pShot, nDelta);
    VEC_Add(&vecAt, &vecDelta, &vecAt);
    pShot->vecPos = vecAt;
    func_ov022_02091540(&pShot->nSlotFlags, nDelta);
    Ov022_ResolveShotHit(pCtx, pShot, &vecAt, &vecDelta);
    if (pShot->nState != SHOT_LANDED
        && VEC_Distance(&pShot->vecStart, &vecAt) > pDesc->nMaxDist) {
        pShot->nState = SHOT_SPENT;
    }
    if ((pDesc->nFlags & SHOT_HELD) != 0) {
        Ov022_MarshalStateByte9(pCtx, pShot);
        if ((pShot->nFlags & SHOT_RELEASED) != 0
            && pShot->nAge >= pDesc->nMaxAge) {
            pShot->nState = SHOT_SPENT;
        }
    } else {
        if (pShot->nAge >= pDesc->nMaxAge) {
            pShot->nState = SHOT_SPENT;
        }
    }
    if (pShot->nState != SHOT_LIVE) {
        pShot->nState = SHOT_SPENT;
        pShot->nAge = RETIRE_MARK;
        nSlot = 0;
        do {
            pShot->aHitIds[nSlot] = -1;
            nSlot++;
        } while (nSlot < HIT_SLOTS);
        Ov022_ReleaseRigSlots(pShot, pDesc->nField3c);
    }
    return 0;
}
