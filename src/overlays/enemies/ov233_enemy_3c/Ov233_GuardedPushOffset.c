/* c634 handler: run the guard Ov233_MeasureTargetGap; if it fails (<0), dispatch null. Else,
 * only when the parent's flag byte obj[1]+0xad is clear: notify Ov107_PostTagUpdate(owner,
 * 0x11,0), push the owner's local-offset vec (owner+0x494) via func_ov107_020c0b90 mode 4,
 * clear obj+0x61 and obj[2], and dispatch via SetIndexedSlot. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern int Ov233_MeasureTargetGap(int self);
extern void SetIndexedSlot(int self, int index, void *cb);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 v, int flag);
extern void Ov233_AiBurstWindup(void);
void Ov233_GuardedPushOffset(int self) {
    int *obj = *(int **)(self + 4);
    if (Ov233_MeasureTargetGap(self) < 0) {
        SetIndexedSlot(self, *(signed char *)(self + 0x20), 0);
        return;
    }
    if (*(unsigned char *)(obj[1] + 0xad) != 0) {
        return;
    }
    Ov107_PostTagUpdate((Actor *)(*obj), 0x11, 0);
    func_ov107_020c0b90(*obj, 4, *(VecFx32 *)(*obj + 0x494), 1);
    *(char *)((char *)obj + 0x61) = 0;
    obj[2] = 0;
    SetIndexedSlot(self, *(signed char *)(self + 0x20), &Ov233_AiBurstWindup);
}
