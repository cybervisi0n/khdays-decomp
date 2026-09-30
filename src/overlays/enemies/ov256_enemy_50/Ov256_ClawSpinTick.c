/* Spin tick of an ov256 claw: the +0x60 timer accumulates the frame rate, the +0x10 velocity is the
 * +0x1c spin vector scaled by 1 + the owner's +0x3ac part's +0x45c boost (x 1/8), the claw moves
 * (020d1400 1, 2). Once the +0x390 part's animation (+0x3c -> +0xad) ends the timers clear, the +0x6d
 * flag is set, the part takes motion 1 and the node moves on to 020d227c. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern void ScaleVec3Fx12(int scale, VecFx32 *v, VecFx32 *d);
extern void Ov256_AttackHitTestB(int *node, int a, int b);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov256_ClawSwingTick(void);

void Ov256_ClawSpinTick(int *node)
{
    int *state = (int *)node[1];

    state[0x18] += *(int *)(node[0] + 0x2c);
    *(VecFx32 *)(state + 4) = *(VecFx32 *)(state + 7);
    ScaleVec3Fx12((*(int *)(*(int *)(*state + 0x3ac) + 0x45c) << 9) + 0x1000, (VecFx32 *)(state + 4), (VecFx32 *)(state + 4));
    Ov256_AttackHitTestB(node, 1, 2);
    if (*(u8 *)(*(int *)(*(int *)(*state + 0x390) + 0x3c) + 0xad) != 0) {
        return;
    }
    state[0x18] = 0;
    state[0x19] = 0;
    *((u8 *)state + 0x6d) = 1;
    Ov107_StartAnim(*(int *)(*state + 0x390), 1, 0);
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov256_ClawSwingTick);
}
