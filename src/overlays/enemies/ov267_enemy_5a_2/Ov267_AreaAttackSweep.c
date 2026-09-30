/*
 * Ov267_AreaAttackSweep -- x3 (ov212/266/267). Area attack: for each collision candidate around
 * targetPos, spawn a hit and a directed effect.
 * Get the candidate node list (020c8eb8). For each not already flagged (bit 1<<node[2] in self+0x5b):
 * dir = target - node(+0x74); unit = normalise(dir) (keeps the length); flatten dir.y=0 and
 * re-normalise, falling back to the const vec data_02042258 if degenerate; scale dir by the mode
 * (1 -> 0xc00, 2 -> 0x1000) with dir.y = 0x800. Try to spawn the hit (020ca918); on success, restore
 * unit's length, offset it by targetPos, emit the effect (020c0b90, vec by value) and set the flag.
 * If anything spawned, fire the follow-up: mode 2 -> 0x4e, else 0x51 (020c5af8).
 * Codegen: the mode dispatch and the follow-up are switches; the hit mode is a byte in 020ca918's
 * prototype (the `& 0xff` is hoisted to a stack slot); count is declared before spawned, i starts
 * at 0 before the count test and the flag is set before spawned.
 */

#include "nitro/fx_types.h"

extern int  Ov107_CollectSphereOverlaps(int obj, int kind, int *list);
extern void VEC_Subtract(void *a, void *b, void *c);
extern int  VEC_Normalize(void *a, void *b);
extern void ScaleVec3Fx12(int scale, void *in, void *out);
/* Defined taking mode as int: declared narrower here, which is what makes mwcc truncate the
 * argument at the call as the ROM does (declared as defined, the code comes out different). */
extern int  Ov107_InvokeHitCallback(int node, int a, int b, unsigned char mode, void *pt, int z);
extern void VEC_Add(void *a, void *b, void *c);
extern void func_ov107_020c0b90(int a, int b, VecFx32 v, int c);
extern void Ov107_BuildAndSendUpdate(int a, int b, int c, int d);
extern int  data_02042258;

void Ov267_AreaAttackSweep(int *self, int mode, int *targetPos) {
    int nodes[4];
    VecFx32 dir;
    VecFx32 unit;
    VecFx32 fallback;
    int count;
    int spawned = 0;
    int i;

    count = Ov107_CollectSphereOverlaps(*self, (int)targetPos, nodes);
    i = 0;
    if (count > 0) {
        fallback = *(VecFx32 *)&data_02042258;
        do {
            int bit = (1 << *(unsigned short *)(nodes[i] + 2)) & 0xff;
            if ((*(unsigned char *)((int)self + 0x5b) & bit) == 0) {
                int len;
                VEC_Subtract((void *)(nodes[i] + 0x74), targetPos, &dir);
                len = VEC_Normalize(&dir, &unit);
                dir.y = 0;
                if (VEC_Normalize(&dir, &dir) == 0) {
                    dir = fallback;
                }
                switch (mode) {
                case 1:
                    ScaleVec3Fx12(0xc00, &dir, &dir);
                    dir.y = 0x800;
                    break;
                case 2:
                    ScaleVec3Fx12(0x1000, &dir, &dir);
                    dir.y = 0x800;
                    break;
                }
                if (Ov107_InvokeHitCallback(nodes[i], *self, *self, mode, &dir, 0) != 0) {
                    ScaleVec3Fx12(len, &unit, &unit);
                    VEC_Add(&unit, targetPos, &unit);
                    func_ov107_020c0b90(*self, 3, unit, 0);
                    *(unsigned char *)((int)self + 0x5b) |= bit;
                    spawned = 1;
                }
            }
            i++;
        } while (i < count);
    }
    if (!spawned) {
        return;
    }
    switch (mode) {
    case 2:
        Ov107_BuildAndSendUpdate(*self, 0, 0x4e, self[2]);
        break;
    default:
        Ov107_BuildAndSendUpdate(*self, 0, 0x51, self[2]);
        break;
    }
}
