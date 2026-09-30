
#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct {
    u8 bCommandKind;
    u8 bMode;
    u8 bReserved;
    u8 bMoveKind;
    VecFx32 vPosition;
} Ov002PathPointCommand;

typedef struct {
    char pad0000[0x8db0];
    s8 *pPathOrder;
    u16 wRosterToken;
    s8 nSessionCommandProgress;
} Ov002RootContext;

extern Ov002RootContext *data_ov002_0207fa00;
extern u8 data_0204c240;

extern void *NNSi_FndAllocFromDefaultExpHeap(u32 nSize);
extern void NNSi_FndFreeFromDefaultHeap(void *pBlock);
extern u32 Ov002_BuildSessionCommand(int nCommand,
                               Ov002PathPointCommand *pCommand);

/* Submit a shuffled prefix of the script path as session commands.
 *
 * When the command queue is full, the shuffled order and progress cursor stay
 * live so the script VM can call again and resume at the same point.
 */
int Ov002_SendShuffledPathPoints(int nSendCount, u8 bMoveKind, int nMode,
                        u32 nPathCount, VecFx32 *pPath)
{
    Ov002PathPointCommand command;
    Ov002RootContext *pRoot;
    s8 *pOrder;
    s8 bSwapIndex;
    u32 nFirstIndex;
    u32 nSecondIndex;
    int i;

    pRoot = data_ov002_0207fa00;
    if ((data_0204c240 & 4) == 0) {
        return 1;
    }
    if (Session_GetLocalPlayerIndex() != 0) {
        return 1;
    }

    if (pRoot->pPathOrder == 0) {
        pRoot->pPathOrder =
            (s8 *)NNSi_FndAllocFromDefaultExpHeap(nPathCount);
        pOrder = pRoot->pPathOrder;
        i = 0;
        if (i < (int)nPathCount) {
            do {
                pOrder[i] = (s8)i;
                i++;
            } while (i < (int)nPathCount);
        }

        i = 0;
        if (i < (int)(nPathCount * 2)) {
            do {
                nFirstIndex = Rand16NextScaled((u16)nPathCount);
                nSecondIndex = Rand16NextScaled((u16)nPathCount);
                bSwapIndex = pOrder[nFirstIndex];
                pOrder[nFirstIndex] = pOrder[nSecondIndex];
                pOrder[nSecondIndex] = bSwapIndex;
                i++;
            } while (i < (int)(nPathCount * 2));
        }
    }

    pOrder = pRoot->pPathOrder;
    if (pRoot->nSessionCommandProgress < nSendCount) {
        do {
            command.bMode = (u8)nMode;
            command.bReserved = 0;
            command.bMoveKind = bMoveKind;
            command.vPosition =
                pPath[pOrder[pRoot->nSessionCommandProgress]];
            if (Ov002_BuildSessionCommand(0x11, &command) == 0xffff) {
                return 0;
            }
            pRoot->nSessionCommandProgress++;
        } while (pRoot->nSessionCommandProgress < nSendCount);
    }

    if (pRoot->pPathOrder != 0) {
        NNSi_FndFreeFromDefaultHeap(pRoot->pPathOrder);
    }
    pRoot->pPathOrder = 0;
    pRoot->nSessionCommandProgress = 0;
    return 1;
}
