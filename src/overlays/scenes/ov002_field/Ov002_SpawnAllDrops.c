
#include "nitro/types.h"
#include "nitro/fx_types.h"

extern u8 data_0204c240;                /* boot-mode flags */

extern void Ov002_SpawnTieredDrop(int nKind, u8 nAmount, int nCtxIndex,
                                const VecFx32 *pPlace);
extern void *Ov002_SpawnKindIntoFreeSpot(int nKind, int nTier, int nCtxIndex, const VecFx32 *pPlace,
                                         int nMode);

/* Spawns the four kinds of drop listed in aAmount, one byte of value each.

   Each kind is broken down greedily into three tiers.  Kind 0 is the cheap one
   and goes 30 / 8 / 1; the other three go 100 / 10 / 1, and kind 1 skips its
   middle tier entirely, so it only ever pays out hundreds and ones.

   In the boot mode that raises bit 2, kinds 2 and 3 go through the tiered
   splitter instead, which spawns only half of what the amount affords. */
void Ov002_SpawnAllDrops(const u8 *aAmount, int nCtxIndex,
                         const VecFx32 *pPlace)
{
    int bHalfRate;
    int nKind;
    u8 nAmount;
    u8 nBig;
    u8 nMid;
    int nTier;

    bHalfRate = data_0204c240 & 4;
    nKind = 0;
    do {
        nAmount = aAmount[nKind];
        if (nKind == 0) {
            nBig = 0x1e;
            nMid = 8;
        } else {
            nBig = 0x64;
            nMid = 0xa;
        }

        if (bHalfRate != 0 && (nKind == 2 || nKind == 3)) {
            Ov002_SpawnTieredDrop(nKind, nAmount, nCtxIndex, pPlace);
        } else {
            while (nAmount != 0) {
                if (nAmount >= nBig) {
                    nTier = 2;
                    nAmount = (u8)(nAmount - nBig);
                } else if (nKind != 1 && nAmount >= nMid) {
                    nTier = 1;
                    nAmount = (u8)(nAmount - nMid);
                } else {
                    nTier = 0;
                    nAmount = (u8)(nAmount - 1);
                }
                Ov002_SpawnKindIntoFreeSpot(nKind, (u16)nTier, nCtxIndex, pPlace, 0);
            }
        }
        nKind++;
    } while (nKind <= 3);
}
