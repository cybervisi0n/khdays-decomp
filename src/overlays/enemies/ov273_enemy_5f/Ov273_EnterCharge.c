/* Charge entry: re-acquires the lock-on target into +0x24; with none, pose request 2 is queued
 * and the node dispatches null. Otherwise the +0x38 orientation is built to look from the actor
 * (+0x74) at the target (+0x74) with the data_02042264 up vector, the +0x48 rate becomes 30/10
 * of the frame step, pose 8 plays on the actor and pose 0 on its +0x3dc partner, effect 9 is
 * spawned at the zero vector (data_02041dc8) and the node moves to 020ce88c. */

#include "nitro/fx_types.h"

extern int  Ov107_FindNearestObject(int obj, int flag);
extern void Mtx33_LookAt(void *out, int a, int b, void *c);
extern void Quat_FromMtx33(void *quat, void *mtx);
extern void Ov107_PostTagUpdate(int a, int b, int c);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 v, int flag);
extern void SetIndexedSlot(int self, int idx, void *cb);
extern void Ov273_LockOnEntry(void);
extern int  data_02042264;
extern VecFx32 data_02041dc8;

void Ov273_EnterCharge(int *self) {
    int *state = (int *)self[1];
    int mtx[9];
    int target;

    target = state[9] = Ov107_FindNearestObject(*state, 0);
    if (target == 0) {
        *(unsigned char *)(*state + 0x1c7) = 2;
        SetIndexedSlot((int)self, *(signed char *)((int)self + 0x20), 0);
        return;
    }
    Mtx33_LookAt(mtx, target + 0x74, *state + 0x74, &data_02042264);
    Quat_FromMtx33((void *)(state + 0xe), mtx);
    state[0x12] = *(int *)(self[0] + 0x2c) * 30 / 10;
    Ov107_PostTagUpdate(*state, 8, 0);
    Ov107_PostTagUpdate(*(int *)(*state + 0x3dc), 0, 0);
    func_ov107_020c0b90(*state, 9, data_02041dc8, 0);
    SetIndexedSlot((int)self, *(signed char *)((int)self + 0x20), &Ov273_LockOnEntry);
}
