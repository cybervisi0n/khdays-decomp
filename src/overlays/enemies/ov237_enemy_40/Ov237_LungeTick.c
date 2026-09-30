/* Lunge tick of the ov237 actor: the +0x3c aim point follows the +0x3d8 partner's +0x2c point
 * (020cdb50); once the +4 rig finishes the next pose is picked by the +0x3dc target's height: above
 * 3.0 bit 6 of the +0x60 high byte is set with poses 0x16 / partner 9, else poses 10 / partner 6; the
 * +0x30 timer and the +0x57 flag clear, +0x34 = 1 and the brain waits on 020ced4c. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern VecFx32 Ov237_RotateByActorHeading(int *node, VecFx32 *target);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov237_SlamTick(void);

void Ov237_LungeTick(int *node)
{
    int *state = (int *)node[1];

    *(VecFx32 *)(state + 0xf) = Ov237_RotateByActorHeading(node, (VecFx32 *)(*(int *)(*state + 0x3d8) + 0x2c));
    if (*(u8 *)(state[1] + 0xad) == 0) {
        return;
    }
    if (*(int *)(*(int *)(*state + 0x3dc) + 0x194) > 0x3000) {
        u16 hw = *(u16 *)(state + 0x18);

        *(u16 *)(state + 0x18) = (hw & ~0xff00) |
            ((((((unsigned int)hw << 0x10) >> 0x18) | 0x40) << 0x18) >> 0x10);
        Ov107_PostTagUpdate((Actor *)(*state), 0x16, 0);
        Ov107_StartAnim(*(int *)(*state + 0x3d8), 9, 0);
    } else {
        Ov107_PostTagUpdate((Actor *)(*state), 10, 0);
        Ov107_StartAnim(*(int *)(*state + 0x3d8), 6, 0);
    }
    state[0xc] = 0;
    state[0xd] = 1;
    *((u8 *)state + 0x57) = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov237_SlamTick);
}
