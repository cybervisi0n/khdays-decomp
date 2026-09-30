/* Throw tick of the ov218 actor: the +0x28 velocity takes the +0x34 drift, which decays to 0.69; while
 * fewer than two throws are out (+0x24) the +0x14 timer runs down (to 0) and at 0 the partner's +0xa8
 * hold clears (once, +0x40); after the partner holds no queued move pose 7 plays, effect 3 fires at the
 * origin and the node moves on to 020cdb68. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov218_GuardEnd(void);
extern const VecFx32 data_02041dc8;

void Ov218_ThrowTick(int *node)
{
    int *state = (int *)node[1];

    *(VecFx32 *)(state + 0xa) = *(VecFx32 *)(state + 0xd);
    ScaleVec3Fx12(0xb00, (VecFx32 *)(state + 0xd), (VecFx32 *)(state + 0xd));
    if (state[9] >= 2) {
        return;
    }
    state[5] -= *(int *)(node[0] + 0x2c);
    if (state[5] < 0) {
        state[5] = 0;
    }
    if (*((unsigned char *)state + 0x40) == 0 && state[5] == 0) {
        *(unsigned char *)(state[1] + 0xa8) = 0;
        *((unsigned char *)state + 0x40) = 1;
    }
    if (*(unsigned char *)(state[1] + 0xad) != 0) {
        return;
    }
    Ov107_PostTagUpdate((Actor *)(*state), 7, 0);
    func_ov107_020c0b90(*state, 3, data_02041dc8, 1);
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov218_GuardEnd);
}
