/* Write one roster slot's defaults.
 *
 * The slot array hangs off the session block at root context + 0x8bcc, whose entries pointer sits
 * at +4, so the array is reached through +0x8bd0; slots are 0x44 apart. This fills the slot named
 * by the index: the default triple at +0 comes from the party entry's vector (or the shared zero
 * vector when the entry is missing), the default value at +0xc from func_ov022_02088254, and the
 * seat payload at +0x10 is written by Ov002_ReadRosterSeat using the seat id from
 * Ov022_GetEntryField66.
 *
 * Those two defaults are exactly what Ov002_SetRosterSlotTargets copies into the live pair at +0x34 and
 * +0x40 when it is called with a null source and a value of -1. Slot 0 additionally kicks
 * Save_StoreSlotTables, but only while bit 2 of data_0204c240 is clear.
 *
 * Codegen note. func_ov022_020881f8 TAKES the index. That one fact decides the whole prologue:
 * because the argument is already in r0 on entry and nothing has to set it up, r0 stays pinned
 * across the address arithmetic, so the two pool loads land in r1 and r2 and the copy of the
 * index into its callee-saved home is scheduled after the first pool load instead of before it.
 * Declaring that callee as taking no argument compiles to the same instructions in a different
 * order and with every prologue register shifted down one, which is a ten-byte difference.
 *
 * Ghidra carries this as Ov002_FillRosterSlotDefaults over Ov002RosterSlot and VecFx32.
 */

#include "nitro/fx_types.h"
#include "game/engine.h"

extern char *data_ov002_0207fa00;
extern unsigned char data_0204c240;
extern VecFx32 *func_ov022_020881f8(int index);
extern unsigned short func_ov022_02088254(int index);
extern int Ov022_GetEntryField66(int index);
extern void Ov002_ReadRosterSeat(int seatId, int *unused, void *out);

void Ov002_FillRosterSlotDefaults(int index) {
    int *slot = (int *)(*(char **)(data_ov002_0207fa00 + 0x8bd0) + index * 0x44);
    VecFx32 *defaultVec = func_ov022_020881f8(index);

    *(VecFx32 *)slot = *defaultVec;
    slot[3] = func_ov022_02088254(index);
    Ov002_ReadRosterSeat(Ov022_GetEntryField66(index), 0, slot + 4);

    if ((data_0204c240 & 4) == 0 && index == 0) {
        Save_StoreSlotTables();
    }
}
