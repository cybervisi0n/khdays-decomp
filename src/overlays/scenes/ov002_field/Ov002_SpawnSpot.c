
/* The stage's linear congruential generator, seeded here from the level. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct Ov002Rng {
    int nSeed;
    int nMult;
    int nInc;
} Ov002Rng;

typedef struct Ov002Spawned {
    char pad000[0x30];
    u8 bFlags;
} Ov002Spawned;

typedef struct Ov002SpotStage {
    char pad000[0x4e];
    u16 nRowStride;
    char pad050[4];
    char *pRows;
    char pad058[4];
    void *pFreeBits;        /* one bit per spot id, set while the id is free */
    char pad060[0x24c8];
    VecFx32 aSpots[1];    /* where each spot was last spawned */
} Ov002SpotStage;

typedef struct Ov002SpotHolder {
    char pad000[4];
    Ov002SpotStage *pStage;
} Ov002SpotHolder;

extern Ov002SpotHolder data_ov002_0207fa28;

extern void Ov002_ReleaseSlotOwner(char *pRow);   /* release the row's owner */
extern Ov002Spawned *Ov002_BuildSpawnRow(int nIndex, int a1, int a2, int a3,
                                         const VecFx32 *pPlace, int a5,
                                         Ov002Rng *pRng);

/* Spawns the object for one spot and remembers where it went.  If the spot's
   bit is clear it first releases whatever owns its row, so the spot is always
   free before the spawn.  The spawn is handed a three-word seed built on the
   stack: the caller's level shifted up by 14 as the seed, then the fixed
   multiplier and increment. */
void *Ov002_SpawnSpot(int nIndex, int a1, int a2, int a3,
                          const VecFx32 *pPlace, int a5, u8 bLevel)
{
    Ov002SpotStage *pStage;
    Ov002Spawned *pSpawned;
    Ov002Rng rng;

    pStage = data_ov002_0207fa28.pStage;
    if (BitArray_TestBit(pStage->pFreeBits, nIndex) == 0) {
        pStage = data_ov002_0207fa28.pStage;
        Ov002_ReleaseSlotOwner(pStage->pRows + pStage->nRowStride * nIndex);
    }

    rng.nSeed = bLevel << 14;
    rng.nMult = 0x5d588b65;
    rng.nInc = 0x269ec3;
    pSpawned = Ov002_BuildSpawnRow(nIndex, a1, a2, a3, pPlace, a5, &rng);
    pSpawned->bFlags = (u8)(pSpawned->bFlags | 4);
    data_ov002_0207fa28.pStage->aSpots[nIndex] = *pPlace;
    return pSpawned;
}
