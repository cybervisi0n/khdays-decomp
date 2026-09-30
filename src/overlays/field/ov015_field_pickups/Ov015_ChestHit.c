/* Ov015_ChestHit -- Ov015_ChestHit: hit handler of a treasure chest.  A chest that is
 * not collidable (bit 3 of +0x12) or already opening (bit 2 of +0x464) answers 8.  A hit
 * with bit 2 of its flags (+0x14) first pulls the touch position towards its party member
 * (ov002 0207c948, radius 0x400); the touch position is then spread (ov002 0207c9d8 with
 * 0x666 / 0xeaab), both helpers writing into the local copy of it.  A class 0x1b chest hit with bit 7 of the second flag word
 * (+0x18), or a class 0x1c chest hit with bit 3, queues a type-5 notify message (kind 6)
 * with the toucher's player byte (+0xc) and answers 0 once accepted; any other hit queues
 * a type-1 open message (kind 0x14) carrying the position and the player, and once accepted
 * clears the ack mask (+0x725), raises the opening flag and answers 0.  Otherwise 8. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct Ov015HitInfo {
    VecFx32 position;         /* 0x00 */
    u8   nPlayer;             /* 0x0c */
    u8   pad_0d[7];
    int  nFlags;              /* 0x14: bit 2 pull towards the member */
    int  nFlags2;             /* 0x18: bit 7 / bit 3 notify */
} Ov015HitInfo;

typedef struct Ov015ChestNotify {
    u8   nType;               /* 0x00: 5 */
    u8   pad_01[3];
    u8   nPlayer;             /* 0x04 */
    u8   pad_05[3];
} Ov015ChestNotify;

typedef struct Ov015ChestOpen {
    u8   nType;               /* 0x00: 1 */
    u8   pad_01[3];
    VecFx32 position;         /* 0x04 */
    u8   nPlayer;             /* 0x10 */
    u8   pad_11[3];
} Ov015ChestOpen;

typedef struct Ov015ChestDef {
    u8   pad_00[0x4c];
    u16  nClass;              /* 0x4c: 0x1b / 0x1c */
} Ov015ChestDef;

typedef struct Ov015Chest {
    u8   pad_000[8];
    Ov015ChestDef *pDef;      /* 0x008 */
    u8   pad_00c[6];
    u16  nFlags;              /* 0x012: bit 3 collidable */
    u8   pad_014[0x464 - 0x14];
    u16  nChestFlags;         /* 0x464: bit 2 opening */
    u8   pad_466[0x725 - 0x466];
    u8   nAckMask;            /* 0x725 */
} Ov015Chest;

extern void Ov002_PushSpotOutToRange(void *pPiece, Ov015HitInfo *pSpot, int nRange, VecFx32 *pOut); /* pull a spot towards its member */
extern void Ov002_BuildSpreadOffset(VecFx32 *pDir, int nLength, int nStep, VecFx32 *pOut);      /* spread offset */
extern int  Ov002_RecordElementHit(void *pPiece, void *pMessage, int nKind);        /* queue a message on the piece */

int Ov015_ChestHit(Ov015Chest *pChest, Ov015HitInfo *pHit)
{
    VecFx32 position;
    VecFx32 spread;
    Ov015ChestOpen open;
    Ov015ChestNotify notify;
    Ov015ChestDef *pDef;
    u16 nClass;
    int bNotify;

    pDef = pChest->pDef;
    position = pHit->position;
    bNotify = 0;
    if ((pChest->nFlags & 8) == 0) {
        return 8;
    }
    if ((pChest->nChestFlags & 4) != 0) {
        return 8;
    }
    if ((pHit->nFlags & 4) != 0) {
        Ov002_PushSpotOutToRange(pChest, pHit, 0x400, &position);
    }
    spread = position;
    Ov002_BuildSpreadOffset(&spread, 0x666, 0xeaab, &position);
    nClass = pDef->nClass;
    if (nClass == 0x1b && (pHit->nFlags2 & 0x80) != 0) {
        bNotify = 1;
    } else if (nClass == 0x1c && (pHit->nFlags2 & 8) != 0) {
        bNotify = 1;
    }
    if (bNotify) {
        notify.nType = 5;
        notify.nPlayer = pHit->nPlayer;
        if (Ov002_RecordElementHit(pChest, &notify, 6) != 0) {
            return 0;
        }
    } else {
        open.nType = 1;
        open.nPlayer = pHit->nPlayer;
        open.position = position;
        if (Ov002_RecordElementHit(pChest, &open, 0x14) != 0) {
            pChest->nAckMask = 0;
            pChest->nChestFlags |= 4;
            return 0;
        }
    }
    return 8;
}
