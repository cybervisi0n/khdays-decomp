/* Copy the value into the first free slot of the owner's four-slot park queue and mark that slot in
 * use. No-op when the owner at data_ov002_0207f628 is null or all four slots are taken. Queue lives
 * at owner +0x1434 (values) and +0x1464 (flags). */

#include "nitro/fx_types.h"

typedef struct {
    char pad0000[0x1434];
    VecFx32 aParked[4];
    int aInUse[4];
} Ov002ParkOwner;

extern int data_ov002_0207f628;

void Ov002_ParkValue(const VecFx32 *pValue)
{
    int i;
    Ov002ParkOwner *pOwner;

    pOwner = *(Ov002ParkOwner **)&data_ov002_0207f628;
    if (pOwner == 0) return;
    for (i = 0; i < 4; i++) {
        if (pOwner->aInUse[i] == 0) {
            pOwner->aParked[i] = *pValue;
            pOwner->aInUse[i] = 1;
            return;
        }
    }
}
