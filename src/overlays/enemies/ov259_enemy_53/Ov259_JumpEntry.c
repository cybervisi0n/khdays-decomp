/* Jump entry of the ov259 actor: the timer, step and cue flags clear, +0x94 = 15 and +0x80 = 0x1078;
 * sound 0x172/0x21 fires at the +0x10 point, pose 0xe plays on the actor and its partner, the shot
 * is armed (020cd628: pose 0xc after 0xff0), the body sweeps 0x1078-0x1430 flat (+0x420 = 1) and the
 * node moves on to 020cf5d8. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern void Ov259_PlaySound(int actor, int id, int variant, void *at);
extern void Ov259_MirrorPartnerPose(int *node, int pose, int mode);
extern void Ov259_ArmPartnerCue(int *node, int pose, int delay);
extern void Ov259_ForwardSweep(int body, int a, int b, VecFx32 lift);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov259_LaunchTick(void);
extern const VecFx32 data_02041dc8;

void Ov259_JumpEntry(int *node)
{
    int *state = (int *)node[1];

    state[0x1a] = 0;
    state[0x26] = 0;
    *((u8 *)state + 0xac) = 0;
    state[0x25] = 0xf;
    state[0x20] = 0x1078;
    Ov259_PlaySound(*state, 0x172, 0x21, (void *)state[4]);
    Ov107_PostTagUpdate((Actor *)(*state), 0xe, 0);
    Ov259_MirrorPartnerPose(node, 0xe, 0);
    Ov259_ArmPartnerCue(node, 0xc, 0xff0);
    Ov259_ForwardSweep(*(int *)(*state + 0x384), 0x1078, 0x1430, data_02041dc8);
    *(int *)(*state + 0x420) = 1;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov259_LaunchTick);
}
