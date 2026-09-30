/* Slam entry of the ov259 actor: the timers, step and cue flags clear, +0xa8 and +0x70 clear and
 * +0xae becomes 0x11; sound 0x172/0x22 fires at the +0x10 point, pose 0x12 plays on the actor and
 * its partner (020cd524), the body sweeps 0x908-0xb28 flat (020d1700, +0x420 = 5), +0x94 = 50 and
 * the node moves on to 020d0400. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern void Ov259_PlaySound(int actor, int id, int variant, void *at);
extern void Ov259_MirrorPartnerPose(int *node, int pose, int mode);
extern void Ov259_ForwardSweep(int body, int a, int b, VecFx32 lift);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov259_SlamChargeTick(void);
extern const VecFx32 data_02041dc8;

void Ov259_SlamEntry(int *node)
{
    int *state = (int *)node[1];

    state[0x1a] = 0;
    state[0x26] = 0;
    *((u8 *)state + 0xac) = 0;
    state[0x2a] = 0;
    *((u8 *)state + 0xae) = 0x11;
    state[0x1c] = 0;
    Ov259_PlaySound(*state, 0x172, 0x22, (void *)state[4]);
    Ov107_PostTagUpdate((Actor *)(*state), 0x12, 0);
    Ov259_MirrorPartnerPose(node, 0x12, 0);
    Ov259_ForwardSweep(*(int *)(*state + 0x384), 0x908, 0xb28, data_02041dc8);
    *(int *)(*state + 0x420) = 5;
    state[0x25] = 0x32;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov259_SlamChargeTick);
}
