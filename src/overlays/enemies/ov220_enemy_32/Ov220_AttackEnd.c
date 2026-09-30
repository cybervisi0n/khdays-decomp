/* Attack end of the ov220 enemy: the +0x24 velocity copies the +0x30 heading, which then
 * shrinks to 0xb00 of itself. Once the +4 item is idle the actor plays animation 8 (looped),
 * publishes a zero vector with mode 2, the +0x14 timer becomes 0x4b000 scaled by 1.5 per +0x1c
 * phase, the +0x3e flag is cleared and the tick hands off to the guard tick. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern void ScaleVec3Fx12(int scale, void *v, void *d);
extern void func_ov107_020c0b90(int actor, int a, VecFx32 v, int b);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const VecFx32 data_02041dc8;
extern void Ov220_GuardTimerTick(int *node);

static inline int FX_Mul(int a, int b)
{
    return (int)(((long long)a * b + 0x800) >> 12);
}

void Ov220_AttackEnd(int *node)
{
    int *state = (int *)node[1];
    int i;

    *(VecFx32 *)(state + 9) = *(VecFx32 *)(state + 0xc);
    ScaleVec3Fx12(0xb00, state + 0xc, state + 0xc);
    if (*(u8 *)(state[1] + 0xad) != 0) {
        return;
    }
    Ov107_PostTagUpdate((Actor *)(*state), 8, 1);
    func_ov107_020c0b90(*state, 2, data_02041dc8, 0);
    state[5] = 0x4b000;
    for (i = 0; i < state[7]; i++) {
        state[5] = FX_Mul(state[5], 0x1800);
    }
    *(u8 *)((char *)state + 0x3e) = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov220_GuardTimerTick);
}
