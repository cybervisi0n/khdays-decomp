/* Rise entry of the ov218 actor: bit 7 of the +0x60 high byte clears and bit 0 is set, the +0x388
 * model's shape shows and +0x394 is set; effect 1 fires at the origin, pose 0 loops, the +0x28
 * velocity starts at the +0x3ac point and half of it goes to +0x10, the +0x38/+0x14 timers start at
 * 0.125, the rest clears and the node moves on to 020ce9b0. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

typedef struct { unsigned f : 8; } B8;

extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov218_ShotBounceTick(void);
extern const VecFx32 data_02041dc8;

void Ov218_RiseEntry(int *node)
{
    int *state = (int *)node[1];

    {
        u16 hw = *(u16 *)(*state + 0x60);
        *(u16 *)(*state + 0x60) = (hw & ~0xff00) |
            (((unsigned int)(u16)((((unsigned int)hw << 0x10) >> 0x18) & ~0x80) << 0x18) >> 0x10);
    }
    {
        u16 hw = *(u16 *)(*state + 0x60);
        *(u16 *)(*state + 0x60) = (hw & ~0xff00) |
            ((((((unsigned int)hw << 0x10) >> 0x18) | 1) << 0x18) >> 0x10);
    }
    ((B8 *)(*(int *)(*state + 0x388) + 8))->f |= 1;
    *(int *)(*state + 0x394) = 1;
    func_ov107_020c0b90(*state, 1, data_02041dc8, 0);
    Ov107_PostTagUpdate((Actor *)(*state), 0, 1);
    *(VecFx32 *)(state + 0xa) = *(VecFx32 *)(*state + 0x3ac);
    ScaleVec3Fx12(0x800, (VecFx32 *)(state + 0xa), (VecFx32 *)(state + 4));
    state[0xe] = 0x200;
    state[5] = 0x200;
    state[0xf] = 0;
    state[0x10] = 0;
    state[3] = 0;
    state[0x11] = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov218_ShotBounceTick);
}
