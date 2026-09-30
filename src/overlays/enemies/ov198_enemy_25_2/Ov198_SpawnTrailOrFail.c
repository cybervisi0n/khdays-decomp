/* State step: acquires the nearest target, or queues action 2 and ends the step without one; posts
 * pose 6, fires the trail effects at the tracked position and installs the steer-trail step. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern int Ov107_FindNearestObject(int a, int b);
extern void SetIndexedSlot(int *self, int idx, void *cb);
extern void Ov198_SteerTrailThenAdvance(void);

extern VecFx32 data_02041dc8;
extern void func_ov107_020c0b90(int a, int b, VecFx32 v, int d);

void Ov198_SpawnTrailOrFail(int *self) {
    int *s = (int *)self[1];
    *(int *)(*s + 0x394) = Ov107_FindNearestObject(*s, 0);
    if (*(int *)(*s + 0x394) == 0) {
        *(signed char *)(*s + 0x1c7) = 2;
        SetIndexedSlot(self, *(signed char *)((char *)self + 0x20), 0);
        return;
    }
    Ov107_PostTagUpdate((Actor *)(*s), 6, 0);
    func_ov107_020c0b90(*s, 0, *(VecFx32 *)(*s + 0x3d8), 0);
    func_ov107_020c0b90(*s, 2, data_02041dc8, 0);
    s[0x10] = 0;
    SetIndexedSlot(self, *(signed char *)((char *)self + 0x20), (void *)&Ov198_SteerTrailThenAdvance);
}
