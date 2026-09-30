/* Stomp tick of the ov256 actor: the +0x4c timer accumulates the frame rate; until 0x1d38 it keeps
 * tracking the target (020ccd54) into +0x1c. At 0x2530, once per charge (+0x69), the target's +0x190
 * point is reset and the actor is knocked back at the tracked point (mode 8). Once the partner holds
 * no queued move the stomp count +0x54 grows: at 4 the next move is the +0x74 mode + 2 and the node
 * ends, otherwise the timer restarts with a new charge and pose 0x19. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern int Ov256_PickTarget(int *node);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const VecFx32 data_02041dc8;

void Ov256_StompTick(int *node)
{
    int *state = (int *)node[1];

    state[0x13] += *(int *)(node[0] + 0x2c);
    if (state[0x13] < 0x1d38) {
        Ov256_PickTarget(node);
        *(VecFx32 *)(state + 7) = *(VecFx32 *)(*(int *)(*state + 0x430) + 0x190);
    }
    if (state[0x13] >= 0x2530 && *((u8 *)state + 0x69) != 0) {
        *((u8 *)state + 0x69) = 0;
        *(VecFx32 *)(*(int *)(*state + 0x430) + 0x190) = data_02041dc8;
        func_ov107_020c0b90(*state, 8, *(VecFx32 *)(state + 7), 0);
    }
    if (*(u8 *)(state[1] + 0xad) != 0) {
        return;
    }
    if (++state[0x15] == 4) {
        *(signed char *)(*state + 0x1c7) = state[0x1d] + 2;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
        return;
    }
    state[0x13] = 0;
    *((u8 *)state + 0x69) = 1;
    Ov107_PostTagUpdate((Actor *)(*state), 0x19, 0);
}
