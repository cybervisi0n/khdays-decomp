/* c634 handler: copy obj[8..10] into obj[5..7], charge the timer obj[0x17] by self->f0->f2c.
 * While still below obj[0x18]+0x800, re-arm via Ov221_StrikeSweepEntities with mode 2/4 (by the
 * obj[0x1e] flag). Once past it, notify Ov107_PostTagUpdate with mode 0xa/0xe, and when the
 * flag is clear push a shared constant vec (data_02041dc8), then dispatch via SetIndexedSlot. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern void func_ov107_020c0b90(int owner, int mode, VecFx32 v, int flag);
extern void SetIndexedSlot(int self, int index, void *cb);
extern void Ov221_StrikeSweepEntities(int *obj, int mode, int b);
extern VecFx32 data_02041dc8;
extern void Ov221_DashTick(void);
void Ov221_ChargeTimerThenFire(int self) {
    int *obj = *(int **)(self + 4);
    int t, flag;
    *(VecFx32 *)(obj + 5) = *(VecFx32 *)(obj + 8);
    t = (obj[0x17] += *(int *)(*(int *)self + 0x2c));
    flag = obj[0x1e];
    if (t >= obj[0x18] + 0x800) {
        Ov107_PostTagUpdate((Actor *)(*obj), flag != 0 ? 0xa : 0xe, 0);
        if (obj[0x1e] == 0) {
            func_ov107_020c0b90(*obj, 4, data_02041dc8, 1);
        }
        SetIndexedSlot(self, *(signed char *)(self + 0x20), &Ov221_DashTick);
        return;
    }
    Ov221_StrikeSweepEntities(obj, flag != 0 ? 2 : 4, 0);
}
