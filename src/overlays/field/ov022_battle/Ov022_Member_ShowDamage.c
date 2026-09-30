/* Reports a hit on member index (locally) and spawns the damage number above it. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct Ov022SeatEntry {
    char padding000[0x09];
    u8 nActorIndex09;
    char padding00a[0x5c];
    s16 nSlotId66;
} Ov022SeatEntry;

typedef struct Ov022EntryOwner {
    char padding000[0x20];
    Ov022SeatEntry *actor20;
} Ov022EntryOwner;

typedef struct Ov022RosterRow {
    Ov022EntryOwner *owner00;
    char padding004[0x06];
    u8 playerIndex0a;
    char padding00b;
} Ov022RosterRow;

typedef struct Ov022EntryRoot {
    char padding000[0x04];
    Ov022RosterRow rows04[1];
} Ov022EntryRoot;

typedef struct Ov022EntrySystem {
    int callback00;
    Ov022EntryRoot *root04;
} Ov022EntrySystem;

extern Ov022EntrySystem data_ov022_020b2e78;

extern void Ov022_DriveOwnedSound(Ov022SeatEntry *entry, int value, int id, int delta);
extern short Ov002_FindNamedValue(int id);
extern VecFx32 *func_ov022_020881f8(int index);
extern void Ov002_ClearSeatBitByKey(int context, int key, int value, u16 id);
extern int Ov002_GetSlotTableByte(int group);
extern void *Ov002_SpawnSpot(int index, int value, int id, int group, const VecFx32 *position,
                             int unused, int level);

void Ov022_Member_ShowDamage(int index, int id, unsigned int value, unsigned int level)
{
    Ov022SeatEntry *entry;
    int mappedValue;
    VecFx32 position;

    entry = (Ov022SeatEntry *)GetEntryField20ByIndex(index);
    if (entry == 0) {
        return;
    }

    if (Session_GetLocalPlayerIndex() == 0 ||
        data_ov022_020b2e78.root04->rows04[index].playerIndex0a == Session_GetLocalPlayerIndex()) {
        Ov022_DriveOwnedSound(entry, value, id, -1);
    }

    mappedValue = Ov002_FindNamedValue((s16)id);
    position = *func_ov022_020881f8(index);
    position.y += 0x800;

    Ov002_ClearSeatBitByKey(entry->nActorIndex09, value & 0xff, mappedValue & 0xff, id);
    Ov002_SpawnSpot(value, mappedValue, (u16)id, (u16)Ov002_GetSlotTableByte(entry->nSlotId66),
                    &position, 0, level);
}
