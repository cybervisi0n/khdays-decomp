/* Sequence tick of the ov259 actor: the +0x68 timer accumulates the frame rate; with more than one
 * target in range (020cdc20) the aim is refreshed (020cdcac) and the +0x14 velocity doubles. The
 * +0x98 step fires the rig's four sweeps (020d1700 on the +0x384 body) at 0xb28 / 0x1298 / 0x18f8
 * / 0x1ed0, the last one lifted 1.25 (data_ov259_020d2f78), queueing +0x420 = 4 each time (the last
 * also +0x94 = 200 and +0x424); step 4 moves the node on to 020d02b8. Otherwise +0x94 grades the
 * target count against the actor's +0x80 range (100 / 10 / 2), and the six +0xac flags pulse the
 * cue (020cd2c8, alternating 0 / 1) at 0x550, 0xaa0, 0xff0, 0x1430, 0x17e8 and 0x1ed0. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

extern int Ov259_FaceTargetGap(int *node);
extern void Ov259_RefreshAim(int *node);
extern void ScaleVec3Fx12(int scale, VecFx32 *v, VecFx32 *d);
extern void Ov259_ForwardSweep(int body, int a, int b, VecFx32 lift);
extern void Ov259_MapHeldItemKindToAnim(int actor, int flag);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov259_SequenceTailTick(void);
extern const VecFx32 data_02041dc8;
extern const VecFx32 data_ov259_020d2f78;

void Ov259_SweepSequenceTick(int *node)
{
    int *state = (int *)node[1];
    int n;

    state[0x1a] += *(int *)(node[0] + 0x2c);
    n = Ov259_FaceTargetGap(node);
    if (n > 1) {
        Ov259_RefreshAim(node);
        ScaleVec3Fx12(0x2000, (VecFx32 *)(state + 5), (VecFx32 *)(state + 5));
    }
    switch (state[0x26]) {
    case 0:
        if (state[0x1a] > 0xb28) {
            Ov259_ForwardSweep(*(int *)(*state + 0x384), 0xb28 - 0x660, 0x770, data_02041dc8);
            *(int *)(*state + 0x420) = 4;
            state[0x26]++;
        }
        break;
    case 1:
        if (state[0x1a] > 0x1298) {
            Ov259_ForwardSweep(*(int *)(*state + 0x384), 0x3b8, 0x660, data_02041dc8);
            *(int *)(*state + 0x420) = 4;
            state[0x26]++;
        }
        break;
    case 2:
        if (state[0x1a] > 0x18f8) {
            Ov259_ForwardSweep(*(int *)(*state + 0x384), 0x3b8, 0x5d8, data_02041dc8);
            *(int *)(*state + 0x420) = 4;
            state[0x26]++;
        }
        break;
    case 3:
        if (state[0x1a] > 0x1ed0) {
            Ov259_ForwardSweep(*(int *)(*state + 0x384), 0x3b8, 0xe58, data_ov259_020d2f78);
            *(int *)(*state + 0x420) = 4;
            state[0x25] = 200;
            *(int *)(*state + 0x424) = 1;
            state[0x26]++;
        }
        break;
    case 4:
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov259_SequenceTailTick);
        return;
    }
    if (n < *(int *)(*state + 0x80) * 4) {
        state[0x25] = 100;
    } else if (n < *(int *)(*state + 0x80) * 6) {
        state[0x25] = 10;
    } else {
        state[0x25] = 2;
    }
    if ((*((u8 *)state + 0xac) & 1) == 0 && state[0x1a] >= 0x550) {
        *((u8 *)state + 0xac) |= 1;
        Ov259_MapHeldItemKindToAnim(*state, 0);
    }
    if ((*((u8 *)state + 0xac) & 2) == 0 && state[0x1a] >= 0xaa0) {
        *((u8 *)state + 0xac) |= 2;
        Ov259_MapHeldItemKindToAnim(*state, 1);
    }
    if ((*((u8 *)state + 0xac) & 4) == 0 && state[0x1a] >= 0xff0) {
        *((u8 *)state + 0xac) |= 4;
        Ov259_MapHeldItemKindToAnim(*state, 0);
    }
    if ((*((u8 *)state + 0xac) & 8) == 0 && state[0x1a] >= 0x1430) {
        *((u8 *)state + 0xac) |= 8;
        Ov259_MapHeldItemKindToAnim(*state, 1);
    }
    if ((*((u8 *)state + 0xac) & 0x10) == 0 && state[0x1a] >= 0x17e8) {
        *((u8 *)state + 0xac) |= 0x10;
        Ov259_MapHeldItemKindToAnim(*state, 0);
    }
    if ((*((u8 *)state + 0xac) & 0x20) == 0 && state[0x1a] >= 0x1ed0) {
        *((u8 *)state + 0xac) |= 0x20;
        Ov259_MapHeldItemKindToAnim(*state, 1);
    }
}
