/* AI step: follows the part's motion and, on ground contact, posts pose 8, starts animation 2 and
 * continues. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern void SetIndexedSlot(void *node, int idx, void *next);
extern void Ov293_AiFollowPartUntilAnimEnd(void);

void Ov293_CopyVecThenSetupSubActionAndAdvance(int *node) {
    int *state = (int *)node[1];
    state[0xb] -= 0x100;
    *(VecFx32 *)(state + 7) = *(VecFx32 *)(state + 10);
    if (((unsigned int)(*(unsigned char *)(*state + 0x17a) << 0x1f) >> 0x1f) == 0) return;
    Ov107_PostTagUpdate((Actor *)(*state), 8, 0);
    Ov107_StartAnim(*(int *)(*state + 0x39c), 2, 0);
    SetIndexedSlot(node, *(signed char *)(node + 8), Ov293_AiFollowPartUntilAnimEnd);
}
