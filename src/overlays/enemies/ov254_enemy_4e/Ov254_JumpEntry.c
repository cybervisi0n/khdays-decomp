/* Jump entry: the actor's +0x390 latch is set, bits 1-3, 6 and 7 of its +0x60 high byte and bit 0
 * of +0x1ae are set, pose 0 plays and reaction 0x16d/9 fires at the +0x18 point; the +0x1c / +0x24
 * velocity is the +8 heading's forward direction times the 64-bit +0x28 speed, the +0x20 rise is
 * 0.75 and the node moves to 020d3890. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"
#include "game/engine.h"

extern void Ov107_BuildAndSendUpdate(int owner, int id, int mode, void *at);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const VecFx32 data_02042258;
extern void Ov254_TurnBackEntry(void);

void Ov254_JumpEntry(int *node)
{
    int *state = (int *)node[1];
    VecFx32 dir;

    *(int *)(*state + 0x390) = 1;
    {
        u16 hw = *(u16 *)(*state + 0x60);
        *(u16 *)(*state + 0x60) = (hw & ~0xff00) |
            ((((((unsigned int)hw << 0x10) >> 0x18) | 0xce) << 0x18) >> 0x10);
    }
    *(u16 *)(*state + 0x100 + 0xae) |= 1;
    Ov107_PostTagUpdate((Actor *)(*state), 0, 0);
    Ov107_BuildAndSendUpdate(*state, 0x16d, 9, (void *)state[6]);
    Vec3TransformViaTempMtx(&dir, state + 2, &data_02042258);
    state[7] = (int)((*(long long *)(state + 10) * dir.x + 0x80000000LL) >> 32);
    state[9] = (int)((*(long long *)(state + 10) * dir.z + 0x80000000LL) >> 32);
    state[8] = 0xc00;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov254_TurnBackEntry);
}
