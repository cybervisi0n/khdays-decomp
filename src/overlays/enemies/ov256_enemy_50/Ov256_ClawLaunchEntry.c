/* Launch entry of an ov256 claw: the owner is marked (+0x3a0), +0x54 takes the +0x50 speed, the +0x34
 * start point is the owner's +0x3ac part's hand bone (+0x418, or +0x424 for the second claw, +0x394)
 * position, bits 1-4 of the +0x60 high byte are set, the +0x390 part takes motion 3, the timers and
 * the +0x6c flag clear and the node moves on to 020d1c50. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov256_ClawOrbitEntryTick(void);

static inline void VEC_Set(VecFx32 *vec, int x, int y, int z)
{
    int *components = (int *)vec;
    components[0] = x;
    components[1] = y;
    components[2] = z;
}

void Ov256_ClawLaunchEntry(int *node)
{
    int *state = (int *)node[1];
    VecFx32 zero;
    VecFx32 start;

    VEC_Set(&zero, 0, 0, 0);
    *(int *)(*state + 0x3a0) = 1;
    state[0x15] = state[0x14];
    {
        int owner = *state;
        VecFx32 *p = &start;

        *p = *(VecFx32 *)((*(u8 *)(owner + 0x394) == 0 ? *(int *)(*(int *)(owner + 0x3ac) + 0x418)
                                                     : *(int *)(*(int *)(owner + 0x3ac) + 0x424)) + 0x14);
    }
    *(VecFx32 *)(state + 0xd) = start;
    {
        u16 hw = *(u16 *)(*state + 0x60);
        *(u16 *)(*state + 0x60) = (hw & ~0xff00) |
            ((((((unsigned int)hw << 0x10) >> 0x18) | 0x1e) << 0x18) >> 0x10);
    }
    Ov107_StartAnim(*(int *)(*state + 0x390), 3, 0);
    state[0x18] = 0;
    state[0x19] = 0;
    *((u8 *)state + 0x6c) = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov256_ClawOrbitEntryTick);
}
