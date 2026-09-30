/* Ov002_CollectObjectPositionsOfKind -- collect the world position of every live actor that belongs to the
 * current party member; returns how many were written.
 * Walks all 0x40 object slots, keeps the ones whose class byte (+0x10) maps to the same kind as
 * the active member, and appends each one's Vec3 from its own vtable getter (+0x2c of *(obj+8)).
 *
 * Parked as "class A -- register permutation": the ROM colours count->r6 and i->r5 and mwcc the
 * other way round, with four declaration orders already ruled out. Two separate things were wrong
 * and the declaration order only pays once the first is fixed:
 *  - the two leading calls are CHAINED. Ov022_GetEntryField66 was declared `(void)` and is defined
 *    with one parameter, and the ROM's `bl ; bl` back to back with nothing in between is it being
 *    called on QueryActiveStateOrDelegate's return value. Written as two statements, mwcc has a free slot before
 *    the calls and sinks the counter's zero into it, which is what shifts everything.
 *  - with that fixed, the declaration order below (i, count, kind, obj) is the one that colours
 *    count->r6 and i->r5. tools/declperm.py finds it by brute force; 4 of the 24 orders match.
 * The ROM's `mov r5,r6` is just mwcc reusing the counter's zero for the loop index, not an
 * artefact to reproduce by hand.
 */

#include "nitro/fx_types.h"
#include "game/engine.h"

extern int Ov002_GetCtxTableByte(int slot);
extern int Ov022_GetEntryField66(int a);
extern char *Ov002_GetListEntry(int index);

/* Collects the world position of every live actor that belongs to the current party member.
 * Returns how many were written. */
int Ov002_CollectObjectPositionsOfKind(char *out) {
    int i;
    int count;
    int kind;
    char *obj;
    kind = Ov022_GetEntryField66(QueryActiveStateOrDelegate()) & 0xff;
    count = 0;
    for (i = 0; i < 0x40; i++) {
        obj = Ov002_GetListEntry(i);
        if (obj != 0 && kind == Ov002_GetCtxTableByte((unsigned char)obj[0x10])) {
            *(VecFx32 *)out =
                *(*(VecFx32 *(**)(void *))(*(char **)(obj + 8) + 0x2c))(obj);
            out += 0xc;
            count++;
        }
    }
    return count;
}

