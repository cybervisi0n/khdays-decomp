/* Posts update 0x16a/4 once, lowers the actor on an eased curve and queues action 6 when the
 * animation ends. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern int Ov107_BuildAndSendUpdate(int a, int b, int c, int d);
extern long long FX_DivFx64c(int num, int denom);
extern void SetIndexedSlot(int self, int idx, int cb);

void Ov282_AiSinkTick(int *self) {
    int *state = (int *)self[1];
    long long q;
    VecFx32 v;

    if (*(unsigned char *)((char *)state + 0x66) == 0) {
        state[0x18] += *(int *)(*self + 0x2c);
        if (state[0x18] >= 0x999) {
            *(unsigned char *)((char *)state + 0x66) = 1;
            Ov107_BuildAndSendUpdate(state[0], 0x16a, 4, state[1]);
        }
    }
    state[0xb] += *(int *)(*self + 0x2c);
    if (state[0xb] > 0xaaa) {
        state[0xc] += *(int *)(*self + 0x2c);
        q = FX_DivFx64c(state[0xc], 0x888);
        if (q > 0x100000000LL) {
            q = 0x100000000LL;
        }
        v = *(VecFx32 *)(state + 0xd);
        v.y -= (int)(((q * (long long)0x6000) + 0x80000000LL) >> 32);
        Ov107_MoveNodeAndRelayout((Actor *)state[0], &v);
    }
    if (*(unsigned char *)state[3] == 0) {
        *(char *)(state[0] + 0x1c7) = 6;
        SetIndexedSlot((int)self, *(signed char *)((int)self + 0x20), 0);
    }
}
