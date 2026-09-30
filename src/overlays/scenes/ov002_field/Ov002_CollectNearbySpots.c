/* Collect nearby spots for one actor and context. Placed spots dispatch a
 * replicated command; free spots begin homing toward their action owner.
 * Keep the declaration order: it preserves the target's register reuse. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct Ov002SpotActionOwner Ov002SpotActionOwner;
struct Ov002SpotActionOwner {
    char pad000[0x70];
    int nAttractHeight;
    char pad074[0x11c];
    VecFx32 vAttractPosition;
    char pad19c[0x20];
    void (*pfnActivate)(Ov002SpotActionOwner *, int, int, u16);
    char pad1c0[0x1c];
    int nPickupRadius;
};
typedef struct Ov002Spawned {
    struct Ov002Spawned *pNext;
    char pad004[0x24];
    Ov002SpotActionOwner *pActionOwner;
    u8 nKind, nSpotId;
    u16 nTier;
    u8 bResting : 1;
    u8 bActivationPending : 1;
    u8 bPlaced : 1;
    u8 bOtherFlags : 5;
    s8 nSpinDirection;
    char pad032[2];
    s16 nSpin;
    s16 nHealth;
    s16 nPickupDelay;
    char pad03a[2];
    int nRadius;
    VecFx32 vPos;
} Ov002Spawned;
typedef struct Ov002SpawnCtx {
    char pad000[0xc];
    Ov002Spawned *pHead;
    char pad010[0x174];
} Ov002SpawnCtx;
typedef struct Ov002Rng { int nSeed, nMult, nInc; } Ov002Rng;
typedef struct Ov002SpotStage {
    char pad000[0xb8];
    Ov002SpawnCtx aCtx[24];
    Ov002Rng rng;
} Ov002SpotStage;
typedef struct Ov002SpotHolder { char pad000[4]; Ov002SpotStage *pStage; } Ov002SpotHolder;

extern Ov002SpotHolder data_ov002_0207fa28;
extern int Ov002_RunShutdownHook(void);
extern int Ov002_GetCtxTableByte(int);
extern u32 func_ov022_020886f8(int);
extern int VEC_Distance(const VecFx32 *, const VecFx32 *);
extern void Ov022_Member_ShowSpotMessage(int, u32, int);
extern int Ov002_PostSpotCommand(Ov002Spawned *, int, u8, int);
extern void Ov002_PlaySlotActionSound(Ov002SpotActionOwner *, int);

static inline u16 RandRange(Ov002Rng *pRng, u16 nLimit)
{
    pRng->nSeed = pRng->nMult * pRng->nSeed + pRng->nInc;
    return (u16)((((u32)pRng->nSeed >> 16) * nLimit) >> 16);
}
static inline void Activate(Ov002SpotActionOwner *pOwner, int nSpot, int nKind, u16 nTier)
{
    if (pOwner->pfnActivate != 0) pOwner->pfnActivate(pOwner, nSpot, nKind, nTier);
}

static inline int ChooseDirection(void)
{
    int bPositive = Session_IsActive() != 0;
    if (!bPositive) bPositive = RandRange(&data_ov002_0207fa28.pStage->rng, 2) != 0;
    return bPositive;
}

void Ov002_CollectNearbySpots(int nCtxIndex, Ov002SpotActionOwner *pOwner, u32 nPlayerIndex)
{
    int nSlot;
    Ov002Spawned *pRow;
    int bLocalPlayer;
    u32 nPlayerFlags;
    int nOwnerRadius;
    int bNotifiedFull;
    VecFx32 vAttractPosition;
    int nRadius;

    bNotifiedFull = 0;
    if (Ov002_RunShutdownHook() != 0) return;
    nSlot = Ov002_GetCtxTableByte(nCtxIndex);
    bLocalPlayer = nPlayerIndex == (u16)Session_GetLocalPlayerIndex();
    nPlayerFlags = func_ov022_020886f8(nPlayerIndex);
    if (nSlot < 0) return;
    nOwnerRadius = pOwner->nPickupRadius;
    pRow = data_ov002_0207fa28.pStage->aCtx[nCtxIndex].pHead;
    for (; pRow != 0; pRow = pRow->pNext) {
        if (pRow->pActionOwner != 0 || pRow->nPickupDelay > 0 || pRow->bActivationPending) continue;
        if (nPlayerFlags != 0 && pRow->nKind != 2 && pRow->nKind != 5) continue;
        if (pRow->bPlaced && !bLocalPlayer) continue;
        vAttractPosition = pOwner->vAttractPosition;
        vAttractPosition.y += pOwner->nAttractHeight;
        nRadius = pRow->nRadius + nOwnerRadius;
        if (pRow->nKind >= 6) nRadius /= 2;
        else if (pRow->nKind == 0) nRadius /= 2;
        if (VEC_Distance(&pRow->vPos, &vAttractPosition) > nRadius) continue;
        if (pRow->bPlaced) {
            if (pRow->nKind != 13 && pRow->nKind != 15 && Slot_AllEntriesFilled(nPlayerIndex)) {
                if (!bNotifiedFull) {
                    bNotifiedFull = 1;
                    Ov022_Member_ShowSpotMessage(nPlayerIndex, pRow->nTier, pRow->nSpotId);
                }
            } else {
                Ov002_PostSpotCommand(pRow, nPlayerIndex, (u8)nCtxIndex, 0);
            }
        } else {
            pRow->pActionOwner = pOwner;
            pRow->nSpin = 0;
            pRow->nSpinDirection = ChooseDirection() ? 1 : -1;
            Activate(pOwner, pRow->nSpotId, pRow->nKind, pRow->nTier);
            Ov002_PlaySlotActionSound(pOwner, pRow->nKind);
        }
    }
}
