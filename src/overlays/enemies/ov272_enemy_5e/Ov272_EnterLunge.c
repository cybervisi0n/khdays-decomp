/* Lunge entry of the ov272 enemy: effect 1 spawns at the +0x4c point,
 * reaction 0x167 mode 4 fires there, animation 1 plays (looping), bit 0 of +0x1ae is raised, bit
 * 0 of the +0x388 part's flag byte clears, bits 1 and 2 of the +0x60 high byte are raised, the
 * +0x50 timer restarts and the tick hands over to Ov272_RepositionTick. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

typedef struct { unsigned int lo : 8, rest : 24; } Byte8;

extern void func_ov107_020c0b90(int actor, int a, VecFx32 v, int b);
extern void Ov107_BuildAndSendUpdate(int owner, int id, int mode, void *at);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov272_RepositionTick(int *node);

void Ov272_EnterLunge(int *node)
{
    int *state = (int *)node[1];
    unsigned short v;

    func_ov107_020c0b90(*state, 1, *(VecFx32 *)state[0x13], 0);
    Ov107_BuildAndSendUpdate(*state, 0x167, 4, (void *)state[0x13]);
    Ov107_PostTagUpdate((Actor *)(*state), 1, 1);
    *(unsigned short *)(*state + 0x100 + 0xae) |= 1;
    ((Byte8 *)(*(int *)(*state + 0x388) + 8))->lo &= ~1;
    v = *(unsigned short *)(*state + 0x60);
    *(unsigned short *)(*state + 0x60) = (unsigned short)((v & ~0xff00) | (((((unsigned int)v << 0x10) >> 0x18 | 6) << 0x18) >> 0x10));
    state[0x14] = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov272_RepositionTick);
}
