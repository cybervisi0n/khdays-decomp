/* Ov231_ProbeSpawnPoint -- probe a direction: gather the candidates, skip the ones already handled,
 * and report the first that answers. Returns 1 on a hit, 0 if nothing answered.
 *
 * `kind` picks the gather: non-zero goes through Ov107_CollectSphereOverlaps with it, zero uses
 * Ov107_CollectSegmentOverlaps with the query block instead. Either fills a 4-entry list on the stack and
 * returns how many are in it.
 *
 * Each candidate carries a small id at +2; `1 << id` (kept to a byte) is its bit in the mask at
 * ctx+0x4d, so a candidate already marked there is skipped. A candidate that passes
 * Ov107_InvokeHitCallback gets marked, and effect 0 fires at its position raised by 0x2000 -- then
 * the probe stops and reports the hit.
 *
 * The position vector is passed BY VALUE to func_ov107_020c0b90, which is what the `sub r3,sp,#8`
 * is: mwcc straddles the struct across sp so two words load into r2/r3 and the third is already
 * in the outgoing stack slot. The hit kind is a byte in Ov107_InvokeHitCallback's prototype: that is
 * what makes mwcc load the stack parameter at entry and keep its `& 0xff` in a callee-saved
 * register across the gather call. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern int Ov107_CollectSphereOverlaps(int owner, int kind, void **list);
/* Defined taking flags as int: declared narrower here, which is what makes mwcc truncate the
 * argument at the call as the ROM does (declared as defined, the code comes out different). */
extern int Ov107_InvokeHitCallback(void *cand, int owner, int owner2, unsigned char flags, void *pt, int a);
extern void func_ov107_020c0b90(int owner, int effect, VecFx32 v, int a);

int Ov231_ProbeSpawnPoint(int self, int kind, void *query, void *pt, int flags) {
    int *ctx = *(int **)(self + 4);
    /* Reverse of their stack order: pos at sp+8, list at sp+0x14 (sp+0/+4 are outgoing args). */
    void *list[4];
    VecFx32 pos;
    int n;
    int i;
    int bit;

    if (kind != 0) {
        n = Ov107_CollectSphereOverlaps(ctx[0], kind, list);
    } else {
        n = Ov107_CollectSegmentOverlaps((Actor *)ctx[0], query, list);
    }

    for (i = 0; i < n; i++) {
        bit = (1 << *(unsigned short *)((char *)list[i] + 2)) & 0xff;
        if (*(unsigned char *)((char *)ctx + 0x4d) & bit) {
            continue;
        }
        if (Ov107_InvokeHitCallback(list[i], ctx[0], ctx[0], flags, pt, 0) == 0) {
            continue;
        }

        pos = *(VecFx32 *)((char *)list[i] + 0x190);
        pos.y += 0x2000;
        *(unsigned char *)((char *)ctx + 0x4d) |= bit;
        func_ov107_020c0b90(ctx[0], 0, pos, 0);
        return 1;
    }

    return 0;
}
