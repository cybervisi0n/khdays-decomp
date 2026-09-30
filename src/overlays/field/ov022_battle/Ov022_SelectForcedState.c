/* ov022: pick the state the actor is forced into this frame.
 *
 * The per-frame tick calls this before running the state machine and stores a
 * non-zero result straight into the actor's state slot, so what it returns is
 * a state function, not a number.
 *
 * In order it decides: whether the actor has dropped below the floor for its
 * slot, accumulating a timer and killing it past 0x3000; whether a pending
 * forced state should be entered; the whole death path once hit points reach
 * zero, which differs between the two game modes; the guard-break state; and
 * finally the knockdown table selected by the byte at 0x2770.
 *
 * Note the flag test before the guard-break call reads only the low half of
 * the 64-bit word, unlike every other test here, so it is spelled as a cast.
 */

#include "nitro/types.h"
#include "nitro/fx_types.h"

struct Actor {
    unsigned long long nFlags;   /* 0x000 */
    u8 nOwner;                   /* 0x008 */
    u8 pad009[9];
    u16 nHp;                     /* 0x012 */
    u8 pad014[0x52];
    short nSlotIndex;            /* 0x066 */
    u8 pad068[0x3f8];
    void *pfnState;              /* 0x460 */
    unsigned long long nFlags2;  /* 0x464 */
    u8 pad46c[8];
    int nFallTimer;              /* 0x474 */
    u8 pad478[0x14];
    VecFx32 vecPos;          /* 0x48c */
    u8 pad498[0x1f8];
    int nPendingState;           /* 0x690 */
    u8 bSuppressDraw : 1;        /* 0x694 bit 0 */
    u8 nPad694 : 2;
    u8 bDeathHandled : 1;        /* bit 3 */
    u8 nRest694 : 4;
    u8 pad695[0x27];
    u32 nField6bc;               /* 0x6bc */
    u8 pad6c0[0x1c38];
    u8 stateBlk;                 /* 0x22f8 */
    u8 pad22f9[0x477];
    signed char nKnockdownKind;  /* 0x2770 */
};

extern u8 data_0204c240;

extern int Ov002_GetSlotFloor(int nSlot);
extern int Session_GetLocalPlayerIndex(void);
extern int Ov022_GetGlobal34(void);
extern void Ov022_ActorSetHp(struct Actor *pActor, int nValue);
extern void *Ov022_ActorSetState(struct Actor *pActor, int nState);
extern void *Ov022_StepChargeSequence(struct Actor *pActor);
extern void Ov022_EnterState0E(struct Actor *pActor);
extern void Ov022_StepDownedState(void);
extern void PauseMenu_SetMode(int nArg);
extern void PauseMenu_SetAllowed(int nArg);
extern void Ov022_CopyBlock2c00(struct Actor *pActor);
extern void *Ov022_ResolveGuardBreakState(struct Actor *pActor);
extern int Ov022_IsType8AndBit7Set(u8 *pBlk);

void *Ov022_SelectForcedState(struct Actor *pActor)
{
    void *pNext;
    void *pGuard;
    int nFloor;
    int bAlive;

    pNext = 0;
    if ((pActor->nFlags & (1ULL << 24)) != 0) {
        return pNext;
    }
    nFloor = Ov002_GetSlotFloor(pActor->nSlotIndex);
    if (pActor->vecPos.y < nFloor) {
        if (Session_GetLocalPlayerIndex() == 0) {
            pActor->nFlags2 |= (1ULL << 34);
        }
        pActor->nFallTimer += Ov022_GetGlobal34();
        if (pActor->nFallTimer >= 0x3000) {
            pActor->nFlags &= ~(1ULL << 19);
            Ov022_ActorSetHp(pActor, 0);
        }
    } else {
        pActor->nFallTimer = 0;
    }

    if ((pActor->nFlags & (1ULL << 28)) != 0) {
        bAlive = 1;
        if (pActor->nHp == 0) {
            bAlive = 0;
        }
        if ((pActor->nFlags & (1ULL << 13)) != 0) {
            bAlive = 0;
        }
        if (bAlive != 0) {
            if ((pActor->nFlags & (1ULL << 36)) != 0
                && (pActor->nField6bc == 0xe || pActor->nField6bc == 0xf)
                && Session_GetLocalPlayerIndex() == 0) {
                pActor->nFlags2 |= (1ULL << 7);
            }
            pNext = Ov022_ActorSetState(pActor, pActor->nPendingState);
        } else {
            pActor->nFlags &= ~(1ULL << 28);
        }
    }

    if ((pActor->nFlags & (1ULL << 19)) != 0) {
        if ((pActor->nFlags & (1ULL << 8)) == 0
            && (pActor->nFlags & (1ULL << 17)) == 0 && pNext == 0) {
            switch (pActor->nKnockdownKind) {
            case 0:
            case 4:
                break;
            case 1:
            case 2:
            case 3:
            case 5:
                pNext = Ov022_StepChargeSequence(pActor);
                break;
            default:
                break;
            }
        }
        return pNext;
    }

    if (pActor->nHp == 0) {
        Ov022_EnterState0E(pActor);
        if ((data_0204c240 & 4) != 0) {
            if (pActor->pfnState != (void *)Ov022_StepDownedState
                && (pActor->nFlags & (1ULL << 8)) == 0
                && pActor->bDeathHandled == 0) {
                pNext = Ov022_ActorSetState(pActor, 0x10);
                if (pActor->nOwner == Session_GetLocalPlayerIndex()) {
                    PauseMenu_SetMode(0);
                    PauseMenu_SetAllowed(0);
                }
            }
        } else {
            if (Session_GetLocalPlayerIndex() == 0) {
                pActor->nFlags2 |= (1ULL << 27);
            }
            if ((pActor->nFlags & (1ULL << 8)) == 0) {
                Ov022_CopyBlock2c00(pActor);
                pActor->nFlags |= (1ULL << 8);
                if (((u32)pActor->nFlags & 0x10000) == 0) {
                    PauseMenu_SetAllowed(0);
                    pNext = Ov022_ResolveGuardBreakState(pActor);
                } else {
                    pNext = Ov022_ActorSetState(pActor, 0x1c);
                }
            }
        }
    }

    if ((pActor->nFlags & (1ULL << 23)) != 0) {
        if (pNext == 0) {
            pNext = Ov022_ActorSetState(pActor, 0x1c);
        } else {
            pActor->nFlags &= ~(1ULL << 23);
        }
    }

    if ((pActor->nFlags & (1ULL << 8)) == 0
        && (pActor->nFlags & (1ULL << 17)) == 0
        && (pActor->nFlags & (1ULL << 13)) == 0
        && (pActor->nFlags & (1ULL << 7)) == 0) {
        pGuard = Ov022_StepChargeSequence(pActor);
        if (pNext == 0 && pGuard != 0) {
            pNext = pGuard;
        }
    }
    if (pNext == 0 && Ov022_IsType8AndBit7Set(&pActor->stateBlk) != 0) {
        pNext = Ov022_ActorSetState(pActor, 0x17);
    }
    return pNext;
}
