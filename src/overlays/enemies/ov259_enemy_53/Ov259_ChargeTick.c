/* Charge tick of the ov259 actor: it keeps facing its target (020cd5d4) while the +0x68 timer
 * accumulates the frame rate; the cue pulses once 0x330 before the end (020cd2c8 2, +0xac bit 0).
 * Past 0x27d8 the timer, cue flags and +0x60 clear, +0x38 resets, +0x2c takes the target's +0x190
 * point, pose 0x18 loops on the actor and its partner and the node moves on to 020d105c. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern void Ov259_FaceTarget(int *node);
extern void Ov259_MirrorPartnerPose(int *node, int pose, int mode);
extern void Ov259_MapHeldItemKindToAnim(int actor, int flag);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov259_TickDive(void);
extern const VecFx32 data_02041dc8;

void Ov259_ChargeTick(int *node)
{
    int *state = (int *)node[1];

    Ov259_FaceTarget(node);
    state[0x1a] += *(int *)(node[0] + 0x2c);
    if (state[0x1a] > 0x27d8) {
        state[0x1a] = 0;
        *((u8 *)state + 0xac) = 0;
        state[0x18] = 0;
        *(VecFx32 *)(state + 0xe) = data_02041dc8;
        *(VecFx32 *)(state + 0xb) = *(VecFx32 *)(state[2] + 0x190);
        Ov107_PostTagUpdate((Actor *)(*state), 0x18, 1);
        Ov259_MirrorPartnerPose(node, 0x18, 1);
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov259_TickDive);
        return;
    }
    if ((*((u8 *)state + 0xac) & 1) == 0 && state[0x1a] >= 0x27d8 - 0x330) {
        *((u8 *)state + 0xac) |= 1;
        Ov259_MapHeldItemKindToAnim(*state, 2);
    }
}
