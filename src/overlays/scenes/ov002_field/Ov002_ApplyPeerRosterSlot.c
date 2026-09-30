
#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct Ov002RosterSlot {
    VecFx32 vecBase;                   /* +0x00 */
    int nBaseValue;                 /* +0x0c */
    int nSeatId;                    /* +0x10 */
    char aSeatData[32];             /* +0x14 */
    VecFx32 vecTarget;                 /* +0x34 */
    int nTargetValue;               /* +0x40 */
} Ov002RosterSlot;                  /* 0x44 */

typedef struct Ov002SeatRow {
    void *pInstance;                /* +0x00 */
    Ov002RosterSlot *pSlots;        /* +0x04 */
    char pad0008[0x24];
} Ov002SeatRow;                     /* 0x2c */

typedef struct Ov002RosterBlock {
    Ov002SeatRow aSeats[4];         /* +0x00 */
    char pad00b0[8];
    u8 bSeatMask;                   /* +0xb8 */
} Ov002RosterBlock;

typedef struct Ov002RootContext {
    char pad0000[0x8bcc];
    Ov002RosterBlock roster;        /* +0x8bcc */
} Ov002RootContext;

extern Ov002RootContext *data_ov002_0207fa00;

extern void strcpy(unsigned int *pDst, unsigned int *pSrc);

/* Apply a roster slot a peer sent us.  The host ignores it -- only a machine
 * whose player index is non-zero takes the update -- then the message's seat
 * byte picks the slot, its next byte is the seat id, the three words after
 * that are the base position, one more word is the base value, and the tail is
 * copied wholesale.  The seat is finally marked present in the mask. */
void Ov002_ApplyPeerRosterSlot(char *pMsg)
{
    Ov002RosterBlock *pRoster;
    Ov002RosterSlot *pSlot;

    pRoster = &data_ov002_0207fa00->roster;
    if (Session_GetLocalPlayerIndex() == 0) {
        return;
    }

    pSlot = pRoster->aSeats[0].pSlots + pMsg[1];
    pSlot->nSeatId = pMsg[2];
    pSlot->vecBase = *(VecFx32 *)(pMsg + 4);
    pSlot->nBaseValue = *(int *)(pMsg + 0x10);
    strcpy((unsigned int *)pSlot->aSeatData, (unsigned int *)(pMsg + 0x14));

    pRoster->bSeatMask |= 1 << pMsg[1];
}
