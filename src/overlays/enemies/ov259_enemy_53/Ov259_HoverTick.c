/* Hover tick of the ov259 actor after a lift: the +0x68 timer accumulates the frame rate, the aim
 * refreshes (020cdcac) and the +0x14 drift decays by 0x1d00. Once the partner holds no queued move
 * (+4 -> +0xad == 0) the actor is knocked back at the +0x10 point (mode 9), the drift clears, it is
 * placed three radii above the target's +0xb0 anchor (020c5c54), the timer restarts, it is knocked
 * back again (mode 0xc) and the node moves on to 020cf1d4. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern void Ov259_RefreshAim(int *node);
extern void ScaleVec3Fx12(int scale, VecFx32 *v, VecFx32 *d);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov259_LungeEntry(void);
extern const VecFx32 data_02041dc8;

void Ov259_HoverTick(int *node)
{
    int *state = (int *)node[1];
    VecFx32 at;
    VecFx32 lift;

    state[0x1a] += *(int *)(node[0] + 0x2c);
    Ov259_RefreshAim(node);
    ScaleVec3Fx12(0x1d00, (VecFx32 *)(state + 5), (VecFx32 *)(state + 5));
    if (*(u8 *)(state[1] + 0xad) != 0) {
        return;
    }
    func_ov107_020c0b90(*state, 9, *(VecFx32 *)state[4], 0);
    *(VecFx32 *)(state + 5) = data_02041dc8;
    at = *(VecFx32 *)(state[2] + 0xb0);
    lift.y = *(int *)(*state + 0x80) * 3;
    lift.x = 0;
    lift.z = 0;
    VEC_Add(&at, &lift, &at);
    Ov107_MoveNodeAndRelayout((Actor *)(*state), &at);
    state[0x1a] = 0;
    func_ov107_020c0b90(*state, 0xc, *(VecFx32 *)state[4], 0);
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov259_LungeEntry);
}
