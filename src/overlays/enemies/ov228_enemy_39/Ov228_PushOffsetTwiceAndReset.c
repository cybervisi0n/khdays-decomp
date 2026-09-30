/* c634 handler: push the owner's local offset vector (owner+0x494) to it twice via
 * func_ov107_020c0b90 (modes 9 then 3), clear obj[0x13] and the two flag bytes at
 * obj+0x61/+0x62, and dispatch via SetIndexedSlot. */

#include "nitro/fx_types.h"

extern void func_ov107_020c0b90(int owner, int mode, VecFx32 v, int flag);
extern void SetIndexedSlot(int self, int index, void *cb);
extern void Ov228_SpinAttackTick(void);
void Ov228_PushOffsetTwiceAndReset(int self) {
    int *obj = *(int **)(self + 4);
    func_ov107_020c0b90(*obj, 9, *(VecFx32 *)(*obj + 0x494), 0);
    func_ov107_020c0b90(*obj, 3, *(VecFx32 *)(*obj + 0x494), 0);
    obj[0x13] = 0;
    *(char *)((char *)obj + 0x61) = 0;
    *(char *)((char *)obj + 0x62) = 0;
    SetIndexedSlot(self, *(signed char *)(self + 0x20), &Ov228_SpinAttackTick);
}
