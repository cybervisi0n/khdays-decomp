/* Ov253_SpawnPosChild -- spawn the 0x20-byte sub-object of kind 0x64 (tick 020d3210, finish
 * 020d3388) linked to the actor and its +0x3a0 item, seeded with the given position. */

#include "nitro/fx_types.h"

struct Ov253Child { int pOwner; int pItem; VecFx32 pos; };

extern int CreateRegistryEntry(int scene, int kind, int size, void *cb, void *cb2, struct Ov253Child **out);
extern void Ov253_RingSeed(void);
extern void Ov253_ClearPendingRecords(void);

int Ov253_SpawnPosChild(int self, const VecFx32 *pos) {
    struct Ov253Child *out;
    int rc = CreateRegistryEntry(*(int *)(self + 0x3c), 100, 0x20, Ov253_RingSeed, Ov253_ClearPendingRecords, &out);
    out->pOwner = self;
    out->pItem = *(int *)(self + 0x3a0);
    out->pos = *pos;
    return rc;
}
