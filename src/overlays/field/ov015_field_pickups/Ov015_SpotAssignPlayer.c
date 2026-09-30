/* Ov015_SpotAssignPlayer -- Ov015_SpotAssignPlayer: the spot's assign message.  When the
 * party holds item 0xc (Ov022_IsHoldingItem, for the player's actor, 01fffde0 of the message
 * byte) the item is dropped (Ov022_SetHeldItem), the player is recorded in the class table
 * (+0x180), the owner pickup dropped (+0x54) and the nearest free entry of the spot's kind
 * table searched from the target (+0x30) within 0x400000 (02080884).  Without one the
 * rejected callback (+0x50) fires; otherwise the entry becomes current (+0x179), the
 * assigned callback (+0x48) fires with it and the live bit (bit 1 of +0x40) is dropped.
 * Always returns 0. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct Ov015PlayerActor {
    u8  pad_000[0x4ec];
    void *pSub;               /* 0x4ec */
} Ov015PlayerActor;

typedef struct Ov015SpotDef {
    u8  pad_000[0x179];
    s8  nCurrent;             /* 0x179 */
    u8  pad_17a[6];
    s8  nPlayer;              /* 0x180 */
} Ov015SpotDef;

typedef struct Ov015Spot {
    u8  pad_00[8];
    Ov015SpotDef *pDef;       /* 0x08 */
    u8  pad_0c[4];
    u8  nKind;                /* 0x10 */
    u8  pad_11[0x30 - 0x11];
    VecFx32 target;           /* 0x30 */
    void *pActor;             /* 0x3c */
    u8  nSpotFlags;           /* 0x40: bit 1 live */
    u8  pad_41[7];
    void (*pfnAssigned)(void *pActor, void *pSub, u8 nEntry); /* 0x48 */
    u8  pad_4c[4];
    void (*pfnRejected)(void *pActor, void *pSub);            /* 0x50 */
    void *pOwner;             /* 0x54 */
} Ov015Spot;

extern int  Ov022_IsHoldingItem(void *pActor, int nState);                 /* the party holds the item */
extern void Ov022_SetHeldItem(void *pActor, u16 nState, int bOn);        /* hold / drop an item */
extern int  Ov015_SpotDefFindNearestFreeEntry(Ov015Spot *pSpot, Ov015SpotDef *pDef, VecFx32 *pFrom, u32 nTable, int nRange, u16 *pVisited, int nDepth); /* nearest free entry */

int Ov015_SpotAssignPlayer(Ov015Spot *pSpot, u8 *pMessage, int nArg2, int nArg3)
{
    Ov015SpotDef *pDef;
    Ov015PlayerActor *pPlayer;
    u16 nVisited;
    int nEntry;

    pDef = pSpot->pDef;
    pPlayer = (Ov015PlayerActor *)GetEntryField20ByIndex(*pMessage);
    if (Ov022_IsHoldingItem(pPlayer, 0xc) != 0) {
        Ov022_SetHeldItem(pPlayer, 0xc, 0);
        pDef->nPlayer = *pMessage;
        pSpot->pOwner = 0;
        nVisited = 0;
        nEntry = Ov015_SpotDefFindNearestFreeEntry(pSpot, pDef, &pSpot->target, pSpot->nKind, 0x400000, &nVisited, 0);
        if (nEntry != -1) {
            pDef->nCurrent = nEntry;
            pSpot->pfnAssigned(pSpot->pActor, pPlayer->pSub, nEntry);
            pSpot->nSpotFlags &= ~2;
        } else {
            pSpot->pfnRejected(pSpot->pActor, pPlayer->pSub);
        }
    }
    return 0;
}
