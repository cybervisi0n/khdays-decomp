/* Wander tick of an ov256 part: the +0x4c timer accumulates the frame rate and the +0x10 velocity is
 * the +0x450 owner's +0x2c vector turned by its heading (020cd054). After 5.0 of wandering, once the
 * partner holds no queued move, pose 0x1d plays, the +0x450 part takes motion 0xe and the node moves
 * on to 020d02a0; before that each idle partner restarts pose 0x1c / motion 0xd. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern void Ov256_RotateByActorHeading(int *out, int param_2, int *vec);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov256_PartHoverTick(void);

void Ov256_PartWanderTick(int *node)
{
    int *state = (int *)node[1];
    VecFx32 v;

    state[0x13] += *(int *)(node[0] + 0x2c);
    Ov256_RotateByActorHeading((int *)&v, (int)node, (int *)(*(int *)(*state + 0x450) + 0x2c));
    *(VecFx32 *)(state + 4) = v;
    if (state[0x13] >= 0x5000 && *(u8 *)(state[1] + 0xad) == 0) {
        Ov107_PostTagUpdate((Actor *)(*state), 0x1d, 0);
        Ov107_StartAnim(*(int *)(*state + 0x450), 0xe, 0);
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov256_PartHoverTick);
        return;
    }
    if (*(u8 *)(state[1] + 0xad) != 0) {
        return;
    }
    Ov107_PostTagUpdate((Actor *)(*state), 0x1c, 0);
    Ov107_StartAnim(*(int *)(*state + 0x450), 0xd, 0);
}
