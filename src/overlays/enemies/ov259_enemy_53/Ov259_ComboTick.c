/* Combo tick of the ov259 actor: the +0x68 timer accumulates the frame rate, the ground test
 * (020cdc20) and aim refresh run and the drift decays to a quarter while within the body radius
 * (+0x80). When the step time (+0x80) runs out, the partner holds no queued move or the combo has not
 * started (+0x98 == 0), the next swing plays on the actor and its partner: 0 pose 9 sweeping
 * 0x440-0x660 for 0x7f8, 1 pose 0xa sweeping 0x880-0xbb0 for 0xcc0, 2 pose 0xb sweeping 0x220-0x440
 * for 0x13a8 with +0x424 set (+0x420 = 0 each time); the timer restarts and the step count grows.
 * After the third swing the node moves on to 020cef48. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern int Ov259_FaceTargetGap(int *node);
extern void Ov259_RefreshAim(int *node);
extern void ScaleVec3Fx12(int scale, VecFx32 *v, VecFx32 *d);
extern void Ov259_MirrorPartnerPose(int *node, int pose, int mode);
extern void Ov259_ForwardSweep(int body, int a, int b, VecFx32 lift);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov259_EnterFlinch(void);
extern const VecFx32 data_02041dc8;

void Ov259_ComboTick(int *node)
{
    int *state = (int *)node[1];
    int next = 0;
    int ground;

    state[0x1a] += *(int *)(node[0] + 0x2c);
    ground = Ov259_FaceTargetGap(node);
    Ov259_RefreshAim(node);
    if (ground <= *(int *)(*state + 0x80)) {
        ScaleVec3Fx12(0x400, (VecFx32 *)(state + 5), (VecFx32 *)(state + 5));
    }
    if (state[0x1a] > state[0x20] || *(u8 *)(state[1] + 0xad) == 0 || state[0x26] == 0) {
        next = 1;
    }
    if (next == 0) {
        return;
    }
    switch (state[0x26]) {
    case 0:
        Ov107_PostTagUpdate((Actor *)(*state), 9, 0);
        Ov259_MirrorPartnerPose(node, 9, 0);
        Ov259_ForwardSweep(*(int *)(*state + 0x384), 0x440, 0x660, data_02041dc8);
        *(int *)(*state + 0x420) = 0;
        state[0x20] = 0x7f8;
        break;
    case 1:
        Ov107_PostTagUpdate((Actor *)(*state), 0xa, 0);
        Ov259_MirrorPartnerPose(node, 0xa, 0);
        Ov259_ForwardSweep(*(int *)(*state + 0x384), 0x880, 0xbb0, data_02041dc8);
        *(int *)(*state + 0x420) = 0;
        state[0x20] = 0xcc0;
        break;
    case 2:
        Ov107_PostTagUpdate((Actor *)(*state), 0xb, 0);
        Ov259_MirrorPartnerPose(node, 0xb, 0);
        Ov259_ForwardSweep(*(int *)(*state + 0x384), 0x220, 0x440, data_02041dc8);
        *(int *)(*state + 0x420) = 0;
        state[0x20] = 0x13a8;
        *(int *)(*state + 0x424) = 1;
        break;
    default:
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov259_EnterFlinch);
        return;
    }
    state[0x1a] = 0;
    state[0x26]++;
}
