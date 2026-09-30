/* Resets save record `slot` (PartyMember_Reset), then stamps `kind` into byte 1 of its 8-byte header
 * (copied out and back as a whole); records 1 and 2 also clear their 0x48-byte extra block
 * (data_0204c500).
 * Codegen: built with `opt_common_subs off` (push/pop scoped); with CSE on mwcc swaps the record
 * base and element pointer registers of the 8-byte header copy. */
#pragma thumb on

#include "nitro/types.h"
#include "nitro/mi.h"
#include "game/engine.h"

typedef struct {
    u8 id;
    u8 kind;
    u16 w1;
    u16 w2;
    u16 w3;
} SlotHeader;

typedef struct {
    SlotHeader header;
    u8 pad[0x104 - 8];
} SlotRecord;

typedef struct {
    u8 data[0x48];
} SlotExtra;

extern SlotRecord data_0204c678[];
extern SlotExtra data_0204c500[];

#pragma push
#pragma opt_common_subs off
void PartyMember_ResetWithKind(int slot, u8 kind, int a, int b)
{
    SlotHeader h;

    PartyMember_Reset(slot, a, b);
    h = data_0204c678[slot].header;
    h.kind = kind;
    data_0204c678[slot].header = h;
    if (slot > 0 && slot - 1 < 2) {
        MI_CpuFill8(&data_0204c500[slot - 1], 0, sizeof(SlotExtra));
    }
}
#pragma pop
