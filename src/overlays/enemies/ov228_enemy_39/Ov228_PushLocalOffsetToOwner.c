/* c634 handler: add the owner's local offset (owner+0x494) to its parent's position
 * (*(owner+0x490)+0x2c) into a vec, push it to the owner via func_ov107_020c0b90, arm
 * Ov107_BuildAndSendUpdate(owner, 0, 0x48, &vec), and dispatch via SetIndexedSlot. */

#include "nitro/fx_types.h"

extern void VEC_Add(int *a, int *b, VecFx32 *out);
extern void func_ov107_020c0b90(int owner, int a, VecFx32 v, int flag);
extern void Ov107_BuildAndSendUpdate(int owner, int a, int b, VecFx32 *v);
extern void SetIndexedSlot(int self, int index, void *cb);
extern void Ov228_TickTimerReleaseAtThreshold(void);
void Ov228_PushLocalOffsetToOwner(int self) {
    int *obj = *(int **)(self + 4);
    VecFx32 buf;
    VEC_Add((int *)(*obj + 0x494), (int *)(*(int *)(*obj + 0x490) + 0x2c), &buf);
    func_ov107_020c0b90(*obj, 0, buf, 1);
    Ov107_BuildAndSendUpdate(*obj, 0, 0x48, &buf);
    SetIndexedSlot(self, *(signed char *)(self + 0x20), &Ov228_TickTimerReleaseAtThreshold);
}
