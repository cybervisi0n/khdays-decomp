/* Ov015_ChestReset -- Ov015_ChestReset: put the chest back into its live state: the
 * render node's transform (+0x4a4) is synced to the home position (+0x474), the lid
 * position (+0x54c) becomes the current position (+0x488), the pushed / notified masks
 * (+0x72c / +0x730 / +0x734), the chest flags (+0x464), the timer (+0x728) and the ack
 * mask (+0x725) are cleared, the handler block (+0xc) points at the chest update
 * (02081ca8), the piece flags get 0x48 and the refresh bit (bit 0 of +0x724) is set. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/engine.h"

extern void Ov015_ChestUpdate(void);                        /* Ov015_ChestUpdate */

typedef struct Ov015Chest {
    u8   pad_000[0xc];
    void *pHandlers;          /* 0x00c */
    u8   pad_010[2];
    u16  nFlags;              /* 0x012 */
    u8   pad_014[0x464 - 0x14];
    u16  nChestFlags;         /* 0x464 */
    u8   pad_466[0x474 - 0x466];
    VecFx32 homePos;          /* 0x474 */
    u8   pad_480[0x488 - 0x480];
    VecFx32 position;         /* 0x488 */
    u8   pad_494[0x4a4 - 0x494];
    u8   transform[0x54c - 0x4a4]; /* 0x4a4: render node transform */
    VecFx32 lidPos;           /* 0x54c */
    u8   pad_558[0x724 - 0x558];
    u8   nStateBits;          /* 0x724 */
    u8   nAckMask;            /* 0x725 */
    u8   pad_726[2];
    int  nTimer;              /* 0x728 */
    u8   nPushedMask;         /* 0x72c */
    u8   pad_72d[3];
    int  nPushedMask2;        /* 0x730 */
    int  nNotifiedMask;       /* 0x734 */
} Ov015Chest;

void Ov015_ChestReset(Ov015Chest *pChest)
{
    Actor_SetVecAndSyncChild(pChest->transform, &pChest->homePos);
    pChest->position = pChest->lidPos;
    pChest->nPushedMask = 0;
    pChest->nPushedMask2 = 0;
    pChest->nNotifiedMask = 0;
    pChest->nChestFlags = 0;
    pChest->pHandlers = (void *)Ov015_ChestUpdate;
    pChest->nFlags |= 0x48;
    pChest->nTimer = 0;
    pChest->nAckMask = 0;
    pChest->nStateBits |= 1;
}
