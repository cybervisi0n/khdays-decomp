/* The two vec3s are whole-struct copies (ldm/stm), and the `.y` fixups then add to the
 * ALREADY COPIED value (`b.y = b.y + base`) rather than re-reading the source field --
 * the copy has already put it there. Re-reading costs 4 B.
 * Declaration order `b, a` puts a at sp+0 and b at sp+0xc, which is the ROM layout.
 * Also: no cached `owner` local -- the ROM re-reads *obj each time. And the guard is
 * `>= 0x990` (an ARM immediate), not `> 0x98f`. */

#include "nitro/fx_types.h"

extern int  Ov107_FindNearestObject(int obj, int flag);
extern void VEC_Subtract();
extern int  VEC_Normalize();
extern void Ov147_FireThreeWaySpread();
extern void SetIndexedSlot(int self, int index, void *cb);

void Ov147_AimAtTargetAfterWindup(int *self) {
    int *obj = (int *)self[1];
    VecFx32 b, a;

    obj[0x10] = obj[0x10] + *(int *)(self[0] + 0x2c);
    if (*(unsigned char *)((int)obj + 0x45) == 0 && obj[0x10] >= 0x990) {
        *(unsigned char *)((int)obj + 0x45) = 1;
        *(int *)(*obj + 0x394) = Ov107_FindNearestObject(*obj, 0);
        if (*(int *)(*obj + 0x394) != 0) {
            b = *(VecFx32 *)(*obj + 0x3d8);
            a = *(VecFx32 *)(*(int *)(*obj + 0x394) + 0x74);
            b.y = b.y + *(int *)(*(int *)(*obj + 0x398) + 0x70);
            a.y = a.y + *(int *)(*(int *)(*obj + 0x398) + 0x70);
            VEC_Subtract(&a, &b, &a);
            VEC_Normalize(&a, &a);
            Ov147_FireThreeWaySpread(obj, a.y, &b);
        }
    }
    if (*(unsigned char *)(obj[1] + 0xad) == 0) {
        *(signed char *)(*obj + 0x1c7) = 2;
        SetIndexedSlot((int)self, *(signed char *)((int)self + 0x20), 0);
    }
}
