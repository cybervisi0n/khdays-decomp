/* Acquire a target; on failure mark the owner state 2 and dispatch with no callback.
 * On success re-tag the owner, clear the two progress fields and re-issue the move
 * command with the stored vector, then dispatch with the continuation. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern int  Ov107_FindNearestObject(int obj, int flag);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 v, int flag);
extern void SetIndexedSlot(int self, int index, void *cb);
extern void Ov169_HoverHoldTick(void);

void Ov169_AcquireTargetOrGiveUp(int self) {
    int *obj = *(int **)(self + 4);
    int target = Ov107_FindNearestObject(*obj, 0);
    obj[3] = target;
    if (target == 0) {
        *(signed char *)(*obj + 0x1c7) = 2;
        SetIndexedSlot(self, *(signed char *)(self + 0x20), 0);
        return;
    }
    Ov107_PostTagUpdate((Actor *)(*obj), 2, 0);
    obj[0x12] = 0;
    *(signed char *)((int)obj + 0x84) = 0;
    func_ov107_020c0b90(*obj, 0, *(VecFx32 *)obj[2], 0);
    SetIndexedSlot(self, *(signed char *)(self + 0x20), &Ov169_HoverHoldTick);
}
