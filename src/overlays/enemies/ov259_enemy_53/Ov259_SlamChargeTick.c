/* Slam charge tick of the ov259 actor: the ground test (020cdc20) and aim refresh run; within three
 * body radii (+0x80) the x/z drift decays to a quarter. The +0x68 timer accumulates the frame rate
 * and the cue pulses at 0x330 (bit 0, 2) and at 0xaa0 on the first step / 0xee0 on the second
 * (bit 1, 3). Once the partner holds no queued move: a target beyond 12 radii queues move 0x12,
 * beyond 5 move 0x10, and the node ends; otherwise the first step (+0x98 == 0) plays pose 0x13
 * sweeping 0x550-0xee0 and restarts, and the second plays pose 0x14 sweeping 0x330-0x550, resets the
 * step and moves on to 020d06b0 (+0x420 = 5 both times). */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern int Ov259_FaceTargetGap(int *node);
extern void Ov259_RefreshAim(int *node);
extern void ScaleVec3Fx12(int scale, VecFx32 *v, VecFx32 *d);
extern void Ov259_MapHeldItemKindToAnim(int actor, int flag);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov259_MirrorPartnerPose(int *node, int pose, int mode);
extern void Ov259_ForwardSweep(int body, int a, int b, VecFx32 lift);
extern void Ov259_LungeSequenceTick(void);
extern const VecFx32 data_02041dc8;

void Ov259_SlamChargeTick(int *node)
{
    int *state = (int *)node[1];
    int ground = Ov259_FaceTargetGap(node);
    VecFx32 v;

    Ov259_RefreshAim(node);
    if (ground <= *(int *)(*state + 0x80) * 3) {
        v = *(VecFx32 *)(state + 5);
        ScaleVec3Fx12(0x400, &v, &v);
        state[5] = v.x;
        state[7] = v.z;
    }
    state[0x1a] += *(int *)(node[0] + 0x2c);
    switch (state[0x26]) {
    case 0:
        if ((*((u8 *)state + 0xac) & 1) == 0 && state[0x1a] >= 0x330) {
            *((u8 *)state + 0xac) |= 1;
            Ov259_MapHeldItemKindToAnim(*state, 2);
        }
        if ((*((u8 *)state + 0xac) & 2) == 0 && state[0x1a] >= 0xaa0) {
            *((u8 *)state + 0xac) |= 2;
            Ov259_MapHeldItemKindToAnim(*state, 3);
        }
        break;
    case 1:
        if ((*((u8 *)state + 0xac) & 1) == 0 && state[0x1a] >= 0x330) {
            *((u8 *)state + 0xac) |= 1;
            Ov259_MapHeldItemKindToAnim(*state, 2);
        }
        if ((*((u8 *)state + 0xac) & 2) == 0 && state[0x1a] >= 0xee0) {
            *((u8 *)state + 0xac) |= 2;
            Ov259_MapHeldItemKindToAnim(*state, 3);
        }
        break;
    }
    if (*(u8 *)(state[1] + 0xad) != 0) {
        return;
    }
    if (ground > *(int *)(*state + 0x80) * 12) {
        *(signed char *)(*state + 0x1c7) = 0x12;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
        return;
    }
    if (ground > *(int *)(*state + 0x80) * 5) {
        *(signed char *)(*state + 0x1c7) = 0x10;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
        return;
    }
    if (state[0x26] == 0) {
        Ov107_PostTagUpdate((Actor *)(*state), 0x13, 0);
        Ov259_MirrorPartnerPose(node, 0x13, 0);
        Ov259_ForwardSweep(*(int *)(*state + 0x384), 0x550, 0xee0, data_02041dc8);
        *(int *)(*state + 0x420) = 5;
        state[0x26]++;
        *((u8 *)state + 0xac) = 0;
        state[0x1a] = 0;
        return;
    }
    Ov107_PostTagUpdate((Actor *)(*state), 0x14, 0);
    Ov259_MirrorPartnerPose(node, 0x14, 0);
    Ov259_ForwardSweep(*(int *)(*state + 0x384), 0x330, 0x550, data_02041dc8);
    *(int *)(*state + 0x420) = 5;
    state[0x26] = 0;
    *((u8 *)state + 0xac) = 0;
    state[0x1a] = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov259_LungeSequenceTick);
}
