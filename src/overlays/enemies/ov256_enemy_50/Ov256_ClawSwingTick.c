/* Swing tick of an ov256 claw: the +0x60 timer accumulates the frame rate, the +0x10 velocity is the
 * +0x390 part's +0x2c vector turned by the claw's heading (020d1900) and the claw moves (020d1400 1, 2).
 * Each time the part's animation ends the swing count (+0x64) grows: on the second swing the timer
 * clears, the +0x1c spin reverses, the part takes motion 2 and the node moves on to 020d2368;
 * otherwise the part restarts motion 1. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern void Ov256_RotateByOwnerHeading(int *out, int param_2, int *vec);
extern void Ov256_AttackHitTestB(int *node, int a, int b);
extern void ScaleVec3Fx12(int scale, VecFx32 *v, VecFx32 *d);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov256_ClawReturnTick(void);

void Ov256_ClawSwingTick(int *node)
{
    int *state = (int *)node[1];
    VecFx32 v;

    state[0x18] += *(int *)(node[0] + 0x2c);
    Ov256_RotateByOwnerHeading((int *)&v, (int)node, (int *)(*(int *)(*state + 0x390) + 0x2c));
    *(VecFx32 *)(state + 4) = v;
    Ov256_AttackHitTestB(node, 1, 2);
    if (*(u8 *)(*(int *)(*(int *)(*state + 0x390) + 0x3c) + 0xad) != 0) {
        return;
    }
    if (++state[0x19] == 2) {
        state[0x18] = 0;
        ScaleVec3Fx12(-0x1000, (VecFx32 *)(state + 7), (VecFx32 *)(state + 7));
        Ov107_StartAnim(*(int *)(*state + 0x390), 2, 0);
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov256_ClawReturnTick);
        return;
    }
    Ov107_StartAnim(*(int *)(*state + 0x390), 1, 0);
}
