/* Chase tick of the ov220 enemy: the +0x24 velocity is the +0x10 yaw's (sin, 0, cos) scaled by
 * 0x800, the attack sweep runs (kind 0) and the +0x14 clock grows by 0x800 per tick. Once the
 * clock reaches 0x10000, bit 1 of the actor's +0x17a flags is set or the sweep hit something,
 * the actor plays animation 5, starts sub-animation 0, publishes a zero vector with mode 4
 * (flag 1) and hands off to the next chase state. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

struct Flags17a { u8 b0 : 1, b1 : 1; };

extern void ScaleVec3Fx12(int scale, void *v, void *d);
extern int Ov220_AttackSweep(int *state, int kind);
extern void Ov220_startAnim(int actor, int anim);
extern void func_ov107_020c0b90(int actor, int a, VecFx32 v, int b);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const short data_0203d210[];
extern const VecFx32 data_02041dc8;
extern void Ov220_stateFixedAngleMatrix(int *node);

void Ov220_ChaseTick(int *node)
{
    int *state = (int *)node[1];
    int idx;
    int hit;

    idx = (unsigned short)((0x28BE60DB9391LL * state[4] + 0x80000000000LL) >> 44);   /* FX_RAD_TO_IDX */
    state[9] = data_0203d210[(idx >> 4) << 1];                                        /* FX_SinIdx */
    state[10] = 0;
    state[11] = data_0203d210[((idx >> 4) << 1) + 1];                                 /* FX_CosIdx */
    ScaleVec3Fx12(0x800, state + 9, state + 9);
    hit = Ov220_AttackSweep(state, 0);
    state[5] += 0x800;
    if (state[5] >= 0x10000 || ((struct Flags17a *)(*state + 0x17a))->b1 || hit != 0) {
        Ov107_PostTagUpdate((Actor *)(*state), 5, 0);
        Ov220_startAnim(*state, 0);
        func_ov107_020c0b90(*state, 4, data_02041dc8, 1);
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov220_stateFixedAngleMatrix);
        return;
    }
}
