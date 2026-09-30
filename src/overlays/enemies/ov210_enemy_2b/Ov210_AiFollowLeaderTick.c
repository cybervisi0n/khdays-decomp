/* Binds to the leader once, then follows its position. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern void Ov210_BindOwnerAndAttach(int a, int b, int c);
extern long long FX_DivFx64c(int num, int denom);
extern void SetIndexedSlot(int self, int idx, int cb);
extern void Ov210_BurstTick(void);

void Ov210_AiFollowLeaderTick(int *self) {
    int *state = (int *)self[1];
    int leaderVal;
    long long q;
    int delta;
    VecFx32 v;

    if (*(unsigned char *)((char *)state + 0x64) == 0) {
        leaderVal = *(int *)(*(int *)(*(int *)(*state + 0x3d4)) + 0x194);
        Ov210_BindOwnerAndAttach(leaderVal, 0, 0);
        *(unsigned char *)((char *)state + 0x64) = 1;
    }
    state[0xb] += *(int *)(self[0] + 0x2c);
    state[0xc] += *(int *)(self[0] + 0x2c);
    q = FX_DivFx64c(state[0xc], 0x555);
    if (q > 0x100000000LL) {
        q = 0x100000000LL;
    }
    delta = (int)(((q * (long long)0x1666) + 0x80000000LL) >> 32);
    v = *(VecFx32 *)(state + 0xd);
    v.y = v.y + (delta - 0x1000);
    Ov107_MoveNodeAndRelayout((Actor *)state[0], &v);
    if (q == 0x100000000LL) {
        state[0xb] = 0;
        SetIndexedSlot((int)self, *(signed char *)((int)self + 0x20), (int)Ov210_BurstTick);
    }
}
