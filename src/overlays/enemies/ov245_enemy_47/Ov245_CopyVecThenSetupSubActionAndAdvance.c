/* Copy the working VecFx32 after decrementing the node timer, guard on the actor bit, run
 * pose/subaction setup, and advance to the next state callback. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern void SetIndexedSlot(void *node, int idx, void *next);
extern void Ov245_LaunchTick2(void);

void Ov245_CopyVecThenSetupSubActionAndAdvance(int *node) {
    int *state = (int *)node[1];
    state[0xb] -= 0x100;
    *(VecFx32 *)(state + 7) = *(VecFx32 *)(state + 10);
    if (((unsigned int)(*(unsigned char *)(*state + 0x17a) << 0x1f) >> 0x1f) == 0) return;
    Ov107_PostTagUpdate((Actor *)(*state), 8, 0);
    Ov107_StartAnim(*(int *)(*state + 0x3a0), 2, 0);
    SetIndexedSlot(node, *(signed char *)(node + 8), Ov245_LaunchTick2);
}
