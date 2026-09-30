/* Enter the lunge of the ov237 actor: the +0x3c aim point is taken from the +0x3d8 partner's +0x2c
 * point (020cdb50); once the +4 rig is idle pose 8 plays, the partner takes pose 4, the +0x30 / +0x34
 * timers and the +0x57 flag clear and the brain waits on 020ce980. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern VecFx32 Ov237_RotateByActorHeading(int *node, VecFx32 *target);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov237_DoubleSlamTick(void);

void Ov237_EnterLunge(int *node)
{
    int *state = (int *)node[1];

    *(VecFx32 *)(state + 0xf) = Ov237_RotateByActorHeading(node, (VecFx32 *)(*(int *)(*state + 0x3d8) + 0x2c));
    if (*(u8 *)(state[1] + 0xad) != 0) {
        return;
    }
    Ov107_PostTagUpdate((Actor *)(*state), 8, 0);
    Ov107_StartAnim(*(int *)(*state + 0x3d8), 4, 0);
    state[0xc] = 0;
    state[0xd] = 0;
    *((u8 *)state + 0x57) = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov237_DoubleSlamTick);
}
