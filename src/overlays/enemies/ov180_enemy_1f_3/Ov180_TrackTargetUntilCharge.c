/* Three form choices here are load-bearing:
 *  - `target = obj[3] = f(...)` in THAT order: the ROM stores the raw call result
 *    (`str r0`) and keeps the copy for the test. The other order stores the copy.
 *  - the 16-byte copy uses int-pointer indices (`obj + 0x19` / `obj + 0x1d`), not byte
 *    offsets: the byte-offset spelling lets mwcc CSE the two `obj + 0x74` derivations
 *    into a callee-saved register (+r7 in the push list); the ROM recomputes each time.
 *  - `obj` is declared LAST. Declared first it takes r4 and target r5; the ROM has it
 *    the other way round.
 * tmp[9], not tmp[8], is what makes the frame 0x2c. */

#include "nitro/fx_types.h"

struct quat { int x, y, z, w; };
extern int  Ov107_FindNearestObject(int obj, int flag);
extern void SetIndexedSlot(int self, int index, void *cb);
extern void Mtx33_LookAt();
extern void Quat_FromMtx33();
extern void VEC_Subtract();
extern void VEC_Normalize();
extern void ScaleVec3Fx12();
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 v, int flag);
extern char data_02042264[];
extern VecFx32 data_02041dc8;
extern void Ov180_DashTick(void);

void Ov180_TrackTargetUntilCharge(int self) {
    int tmp[9];
    int t;
    int target;
    int *obj = *(int **)(self + 4);

    target = obj[3] = Ov107_FindNearestObject(*obj, 0);
    if (target == 0) {
        *(signed char *)(*obj + 0x1c7) = 2;
        SetIndexedSlot(self, *(signed char *)(self + 0x20), 0);
        return;
    }
    Mtx33_LookAt(tmp, target + 0x74, obj[2], data_02042264);
    Quat_FromMtx33((int)obj + 0x74, tmp);
    *(struct quat *)(obj + 0x19) = *(struct quat *)(obj + 0x1d);
    t = obj[0x12] + *(int *)(*(int *)self + 0x2c);
    obj[0x12] = t;
    if (t < 0x219a) {
        return;
    }
    VEC_Subtract(target + 0x74, obj[2], (int)obj + 0x38);
    VEC_Normalize((int)obj + 0x38, (int)obj + 0x38);
    ScaleVec3Fx12(0x300, (int)obj + 0x38, (int)obj + 0x38);
    func_ov107_020c0b90(*obj, 8, data_02041dc8, 1);
    *(signed char *)((int)obj + 0x86) = 0;
    SetIndexedSlot(self, *(signed char *)(self + 0x20), &Ov180_DashTick);
}
