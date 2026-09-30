/* Does the party entry for this roster slot have ground under it?
 *
 * The signed halfword at entry+0x66 is the collision handle; a negative value means the entry has
 * none, so the answer is no. Otherwise a ray is cast from the entry's vector raised by 0x1000,
 * straight down by 0x32000, filtered by the mask at entry+0x20, and the answer is whether it hit.
 * EntityMgr_RunRayCast wraps the collision ray cast.
 *
 * Ov002_UpdatePartyEntries calls this per slot and only refills that slot's defaults through
 * Ov002_FillRosterSlotDefaults when it answers yes.
 *
 * Codegen notes. The answer is a materialised bool: set to 1 up front, cleared on both failure
 * paths, one shared exit. Writing either failure as an early return costs four bytes because mwcc
 * predicates it inline instead of branching. The failure block is written LAST, as the else of a
 * positive test, so it lands out of line the way the ROM has it. The two stack vectors are laid
 * out start-above-direction, which is this declaration order, and the direction's z component is
 * assigned before its x.
 *
 * Ghidra carries this as Ov002_TestRosterSlotGroundRay over VecFx32 and Ov022Ent.
 */

#include "nitro/fx_types.h"
#include "game/engine.h"

extern VecFx32 *func_ov022_020881f8(int index);
extern int EntityMgr_RunRayCast(int handle, VecFx32 *from, VecFx32 *dir, int mask);

int Ov002_TestRosterSlotGroundRay(int index) {
    VecFx32 from;
    VecFx32 dir;
    int hit = 1;
    short handle = *(short *)(GetEntryField20ByIndex(index) + 0x66);

    if (handle >= 0) {
        VecFx32 *entryVec = func_ov022_020881f8(index);
        int entry = GetEntryField20ByIndex(index);

        from.x = entryVec->x;
        from.y = entryVec->y + 0x1000;
        from.z = entryVec->z;
        dir.z = 0;
        dir.x = 0;
        dir.y = -0x32000;
        if (EntityMgr_RunRayCast((unsigned short)handle, &from, &dir,
                          *(int *)(entry + 0x20)) == 0) {
            hit = 0;
        }
    } else {
        hit = 0;
    }
    return hit;
}
